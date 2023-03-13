#include <stdio.h>
#include "tatakos.h"
#include "ext4.h"
#include "stdlib.h"

extern ext4_super_block_t es;
/* The descriptors of block group.
The number of block groups is the size of the device divided by the size of a block group. */
extern ext4_group_desc_t egd[EXT4_MAX_BG_CNT];
/* The counts of block group */
extern int bg_cnts;


int main(){
  ext4_inode_t *proot_inode = kmalloc(sizeof(ext4_inode_t));
  void *buf = kmalloc(EXT4_BLOCK_SIZE);

  // ext4_fill_super();
  ext4_rw_ondisk_super_bgd(EXT4_READ);

  // ext4_rw_ondisk_block(1, buf, EXT4_READ);
  // ext4_rw_ondisk_block(307200000, buf, EXT4_READ);

  ext4_rw_ondisk_inode(37, proot_inode, EXT4_READ);

  ext4_rw_ondisk_inode(EXT4_ROOT_DIR_INODE_NUM, proot_inode, EXT4_READ);
  // ext4_rw_ondisk_inode(12, proot_inode, EXT4_READ);

  /* readdir */ 
  ext4_readdir(proot_inode, 0, buf, EXT4_BLOCK_SIZE);
  /* create an inode of dir or file on disk */
  // ext4_create_inode(proot_inode, T_FILE);
  ext4_create_inode(proot_inode, S_IFREG);
  
  ext4_rw_ondisk_super_bgd(EXT4_WRITE);
  kfree(proot_inode);
  kfree(buf);
}