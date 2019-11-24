//Name: Ziying Yu, Shuhua Zhan
//Email: annyu@g.ucla.edu, shuhuaucla@gmail.com
//ID: 105182320, 705190671

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
//#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "ext2_fs.h"

//#define SUPER_OFFSET 1024
//#define BLOCK_SIZE    1024
#define EXT2_MIN_BLOCK_SIZE    1024

//int block_size;
uint16_t buf_mode;

#define BASE_OFFSET 1024  /* location of the super-block in the first group */
//#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)


int imag_fd = 0;
struct ext2_super_block super;
struct ext2_group_desc group;
struct ext2_inode inode;
struct ext2_dir_entry dir_entry;

unsigned int block_size;

unsigned long block_pos(unsigned int block) {
    return BASE_OFFSET + (block - 1) * block_size;
}

/*A single new-line terminated line,
 comprised of eight comma-separated fields (with no white-space),
 summarizing the key file system parameters:*/
void summarized_superblock_content()
{
    if (pread(imag_fd, &super, sizeof(struct ext2_super_block), BASE_OFFSET) < 0)
    {
        fprintf(stderr, "Error on reading superblock.\n");
        exit(2);
    }
    
    if (super.s_magic != EXT2_SUPER_MAGIC)
    {
        fprintf(stderr, "Error on superblock file system.\n");
        exit(2);
    }
    
    //get the block size
    block_size = BASE_OFFSET << super.s_log_block_size;
    
    fprintf(stdout, "%s,%d,%d,%d,%d,%d,%d,%d\n",
            "SUPERBLOCK",
            super.s_blocks_count,
            super.s_inodes_count,
            /*BASE_OFFSET << super.s_log_block_size*/ block_size,
            super.s_inode_size,
            super.s_blocks_per_group,
            super.s_inodes_per_group,
            super.s_first_ino);
}

void get_time(time_t raw_time, char* buf) {
    time_t epoch = raw_time;
    struct tm ts = *gmtime(&epoch);
    strftime(buf, 80, "%m/%d/%y %H:%M:%S", &ts);
}

void get_dir_entry(int dir_inode, int block_no) {
    
    unsigned long pos = block_pos(block_no);
    unsigned int i = 0;
    while(i < block_size) {
        memset(dir_entry.name, 0, 256);
        pread(imag_fd, &dir_entry, sizeof(dir_entry), pos + i);
        if (dir_entry.inode != 0) { //entry is not empty
            memset(&dir_entry.name[dir_entry.name_len], 0, 256 - dir_entry.name_len);
            fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", dir_inode, i, dir_entry.inode, dir_entry.rec_len, dir_entry.name_len, dir_entry.name);
        }
        i += dir_entry.rec_len;
    }
}

void inode_processor(int bg_inode_table, int inode_idx, int inode_no)
{
    pread(imag_fd, &inode, sizeof(struct ext2_inode), (block_pos(bg_inode_table) + inode_idx * sizeof(inode)));
    if(inode.i_mode == 0 || inode.i_links_count == 0)
        return;
    
    char ftype = '?';
    buf_mode = (inode.i_mode >> 12) << 12;
    if(buf_mode == 0x8000)
        ftype = 'f';
    else if(buf_mode == 0x4000)
        ftype = 'd';
    else if(buf_mode == 0xA000)
        ftype = 's';

    int block_no = (inode.i_blocks / (2 << super.s_log_block_size)) * 2;
    fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,", inode_no, ftype, inode.i_mode & 0xFFF,
            inode.i_uid, inode.i_gid, inode.i_links_count);
    char c_buf[20], m_buf[20], a_buf[20];
    get_time(inode.i_ctime, c_buf);
    get_time(inode.i_mtime, m_buf);
    get_time(inode.i_atime, a_buf);
    fprintf(stdout, "%s,%s,%s", c_buf, m_buf, a_buf);
    fprintf(stdout, ",%d,%d", inode.i_size, block_no);
    
    unsigned int i = 0;
    for(; i < 15; i++)
        fprintf(stdout, ",%d", inode.i_block[i]);
    fprintf(stdout, "\n");
    

    for(i = 0; i < 12; i++)
    {
        if(ftype == 'd' && inode.i_block[i] != 0)
        {
            get_dir_entry(inode_no, inode.i_block[i]);
        }
    }

        uint32_t level1[block_size];
        uint32_t level2[block_size];
        uint32_t level3[block_size];
    
        if(inode.i_block[12] != 0)
        {

            pread(imag_fd, level1, block_size, block_pos(inode.i_block[12]));
            unsigned int j = 0;
            while(j < (block_size / sizeof(uint32_t)))
            {
                if(level1[j] != 0)
                {
                    if(ftype == 'd')
                        get_dir_entry(inode_no, level1[j]);
                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 1, 12 + j,
                            inode.i_block[12], level1[j]);
                }
                j++;
            }
        }
        
        if(inode.i_block[13] != 0)
        {
            pread(imag_fd, level2, block_size, block_pos(inode.i_block[13]));
            unsigned int j = 0;
            while(j < (block_size / sizeof(uint32_t)))
            {
                if(level2[j] != 0)
                {
                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 2, 256 + 12 + j, inode.i_block[13], level2[j]);
                    pread(imag_fd, level1, block_size, block_pos(level2[j]));
                    unsigned int k = 0;
                    while(k < (block_size / sizeof(uint32_t)))
                    {
                        if(level1[k] != 0)
                        {
                            if(ftype == 'd')
                                get_dir_entry(inode_no, level1[k]);
                            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 1, 256 + 12 + k, level2[j], level1[k]);
                        }
                        k++;
                    }
                }
                j++;

            }
        }
        
        if(inode.i_block[14] != 0)
        {
            pread(imag_fd, level3, block_size, block_pos(inode.i_block[14]));
            unsigned int j = 0;
            while(j < (block_size / sizeof(uint32_t)))
            {
                if(level3[j] != 0)
                {
                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 3, 256 * 256 + 256 + 12 + j, inode.i_block[14], level3[j]);
                    pread(imag_fd, level2, block_size, block_pos(level3[j]));
                    unsigned int k = 0;
                    
                    while(k < (block_size / sizeof(uint32_t)))
                    {
                        if(level2[k] != 0)
                        {
                            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 2, 256 * 256 + 256 + 12 + k, level3[j], level2[k]);
                            pread(imag_fd, level1, block_size, block_pos(level2[k]));
                            unsigned int l = 0;
                            while(l < (block_size / sizeof(uint32_t)))
                            {
                                if(level1[l] != 0)
                                {
                                    if(ftype == 'd')
                                        get_dir_entry(inode_no, level1[l]);
                                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_no, 1, 256 * 256 + 256 + 12 + l, level2[k], level1[l]);
                                }
                                l++;
                            }
                        }
                        k++;
                    }
                }
                j++;
            }
        }
}


void summerized_group_content(int group_count, int groups )
{
    uint32_t block_desc;
    unsigned long group_offset;
    unsigned int block_count;
    unsigned int inode_count;
    unsigned int block_bitmap_count;
    
    block_desc = 0;
    if( block_size == BASE_OFFSET)
    {
        block_desc = 2;
    }
    else
        block_desc = 1;
    
    group_offset = block_size * block_desc + 32 * group_count;
    
    if (pread(imag_fd, &group, sizeof(struct ext2_group_desc), group_offset) < 0)
    {
        fprintf(stderr, "Error on reading group descriptor.\n");
        exit(2);
    }
    
    block_count = super.s_blocks_per_group;
    inode_count = super.s_inodes_per_group;
    if( group_count == groups-1)
    {
        block_count = super.s_blocks_count - ( super.s_blocks_per_group * (groups - 1));
    }
    if( group_count == groups-1)
    {
        inode_count = super.s_inodes_count - ( super.s_inodes_per_group * (groups - 1));
    }
    
    /*
     GROUP
     group number (decimal, starting from zero)
     total number of blocks in this group (decimal)
     total number of i-nodes in this group (decimal)
     number of free blocks (decimal)
     number of free i-nodes (decimal)
     block number of free block bitmap for this group (decimal)
     block number of free i-node bitmap for this group (decimal)
     block number of first block of i-nodes in this group (decimal)*/
    fprintf(stdout, "%s,%d,%d,%d,%d,%d,%d,%d,%d\n",
            "GROUP",
            group_count,
            block_count,
            inode_count,
            group.bg_free_blocks_count,
            group.bg_free_inodes_count,
            group.bg_block_bitmap,
            group.bg_inode_bitmap,
            group.bg_inode_table);
    
    block_bitmap_count = group.bg_block_bitmap;
    //summerized_freeblock_content(group_count,block_bitmap_count);
    
    /*Scan the free block bitmap for each group.
     For each free block,
     produce a new-line terminated line,
     with two comma-separated fields (with no white space).*/
    char *block_size_bytes;
    unsigned long block_offsets;
    unsigned int curr_block;
    unsigned int i,j;
    
    block_size_bytes = (char*)malloc(block_size);
    if(block_size_bytes == NULL)
    {
        fprintf(stderr, "Cannot allocate memory for block_size_bytes pointer.\n");
        exit(2);
    }
    block_offsets = block_pos(block_bitmap_count);
    curr_block = super.s_first_data_block + group_count * super.s_blocks_per_group;
    
    if (pread(imag_fd, block_size_bytes, block_size, block_offsets) < 0)
    {
        fprintf(stderr, "Error on reading superblock.\n");
        exit(2);
    }
    
    i = 0;
    for(; i <block_size; i++)
    {
        char curr = block_size_bytes[i];
        j = 0;
        for(; j < 8;j++)
        {
            int not_free;
            not_free = 1 & curr;
            if ( !not_free)
            {
                fprintf(stdout, "%s,%d\n", "BFREE",curr_block);
            }
            curr >>= 1;
            curr_block++;
        }
    }
    free(block_size_bytes);
    
    /*Scan the free I-node bitmap for each group.
     For each free I-node,
     produce a new-line terminated line,
     with two comma-separated fields (with no white space).*/
    unsigned int inode_bitmap_count,inode_table_count;
    int byte_count;
    char* byte_count_ptr;
    unsigned long inode_bitmap_offset;
    unsigned int curr_inode;
    unsigned int begin;
    
    inode_bitmap_count = group.bg_inode_bitmap;
    inode_table_count = group.bg_inode_table;
    
    byte_count = super.s_inodes_per_group/8;
    byte_count_ptr = (char*)malloc(byte_count);
    if(byte_count_ptr == NULL)
    {
        fprintf(stderr, "Cannot allocate memory for byte_count_ptr.\n");
        exit(2);
    }
    
    //read_inode_bitmap(group, inode_bitmap, inode_table);
    //    group_count
    inode_bitmap_offset = block_pos(inode_bitmap_count);
    curr_inode = group_count * super.s_inodes_per_group + 1;
    begin = curr_inode;
    
    if (pread(imag_fd, byte_count_ptr, byte_count, inode_bitmap_offset) < 0)
    {
        fprintf(stderr, "Error on reading superblock.\n");
        exit(2);
    }
    
    int m,n;
    m = 0;
    for(;m<byte_count; m++)
    {
        char curr = byte_count_ptr[m];
        n = 0;
        for (; n<8; n++)
        {
            int not_free;
            not_free = 1 & curr;
            if (not_free)
            {
                inode_processor(inode_table_count, curr_inode - begin,curr_inode);
            }
            else
            {
                fprintf(stdout, "%s,%d\n", "IFREE", curr_inode);
            }
            curr = curr >> 1;
            curr_inode++;
            
        }
    }
    
    free(byte_count_ptr);
}

struct option options[] =
{
	{0, 0, 0, 0}
};

int main(int argc, char** argv)
{
    int i;
    i = 0;
    
    if (argc!=2)
    {
        fprintf(stderr, "Bad arguments--only one.\n");
        exit(1);
    }
	
	if (getopt_long(argc, argv, "", options, NULL) != -1)
	{
		fprintf(stderr, "Bad arguments--only a .img file.\n");
		exit(1);
	}
	
    if ((imag_fd = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "Could not open file imag_fd.\n");
        exit(2);
    }
    
    block_size = EXT2_MIN_BLOCK_SIZE << super.s_log_block_size;
    
    summarized_superblock_content();
    
    int groups_count;
    groups_count = super.s_blocks_count / super.s_blocks_per_group;
    
    if( (double) groups_count < (double) super.s_blocks_count / super.s_blocks_per_group)
    {
        groups_count ++;
    }
    
    for(; i<groups_count; i++)
    {
        summerized_group_content(i, groups_count);
    }
    
    return 0;
}









