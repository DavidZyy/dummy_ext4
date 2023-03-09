#include "ext4.h"
#include "bio.h"
#include <string.h>
#include <assert.h>

uint8_t ext4_block_buff[EXT4_BLOCK_SIZE];
ext4_super_block_t es;
/* The descriptors of block group.
  The number of block groups is the size of the device divided by the size of a block group. */
// ext4_group_desc_t egd[EXT4_MAX_BG_CNT];


/**
 * read the content of block no.num on disk into ext4_block_buff.
 */
void ext4_read_ondisk_block(int num){
  buf_t *b;
  for(int i = 0; i < EXT4_BLOCK2SECTOR_CNT; i++){
    b = bread(0, EXT4_BLOCKNUM2SECTORNUM(num) + i);
    memcpy(ext4_block_buff + i*SECTOR_SIZE, b->data, SECTOR_SIZE);
  }
}

/**
 * read super block and block group descriptor
 */
int ext4_fill_super(){
  // struct buf *b;
  ext4_super_block_t *pes = &es;
  ext4_group_desc_t egd[EXT4_MAX_BG_CNT];
  int bg_cnts;
  
  /**
   * The first 1024 bytes of extr is empty, so we begin form the no.2 sectors.
   */
  // b = bread(0, EXT4_GROUP0_PADDING_SECTOR_CNT);
  // memcpy(&es, b->data, SECTOR_SIZE);
  // b = bread(0, EXT4_GROUP0_PADDING_SECTOR_CNT + 1);
  // memcpy((char *)&es + SECTOR_SIZE, b->data, SECTOR_SIZE);
  ext4_read_ondisk_block(1);
  memcpy(pes, ext4_block_buff, sizeof(ext4_super_block_t));
  assert(1<<(10 + es.s_log_block_size) == EXT4_BLOCK_SIZE);

  bg_cnts = pes->s_blocks_count_lo / pes->s_blocks_per_group + 1;
  assert(bg_cnts < EXT4_MAX_BG_CNT);
  assert(bg_cnts * sizeof(ext4_group_desc_t) <= sizeof(ext4_block_buff));

  /* read block group descriptor, padding bytes + one super block bytes */
  // b = bread(0, (EXT4_GROUP0_PADDING_BYTES + EXT4_BLOCK_SIZE) / SECTOR_SIZE);
  ext4_read_ondisk_block(2);
  for(int i = 0; i < bg_cnts; i++){
    memcpy(egd + i, ext4_block_buff + i*sizeof(ext4_group_desc_t), 
          sizeof(ext4_group_desc_t));
  }
}


/**
 * use this function to find the block group that an inode lives in
 */
int ext4_bg_inode_livein(int inode_num){
  return (inode_num - 1) / es.s_inodes_per_group;
}

/**
 * use this func to get the index that the inode in a block group inode table
 */
int ext4_itable_idx(int inode_num){
  return (inode_num - 1) % es.s_inodes_per_group;
}

int ext4_itable_off(int inode_num){
  return es.s_inode_size * ext4_itable_idx(inode_num);
}

/**
 * read the inode of number no.num
 */
int ext4_read_ondisk_inode(int inode_num){

}

/**
 * used for ls
 */
int ext4_readdir(){

}