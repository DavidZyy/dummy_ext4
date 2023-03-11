#include <stdio.h>
#include "tatakos.h"
#include "ext4.h"
#include "stdlib.h"

int main(){
  ext4_inode_t *proot_inode = kmalloc(sizeof(ext4_inode_t));

  ext4_fill_super();

  // ext4_rw_ondisk_inode(EXT4_ROOT_DIR_INODE_NUM, proot_inode, EXT4_READ);
  ext4_rw_ondisk_inode(12, proot_inode, EXT4_READ);

  void *buf = malloc(EXT4_BLOCK_SIZE);
  /* readdir */ 
  ext4_readdir(proot_inode, 0, buf, EXT4_BLOCK_SIZE);
  /* create an inode of dir or file on disk */
  ext4_create_inode(proot_inode, S_IFDIR);
  // ext4_create_inode(proot_inode, S_IFREG);
  
  kfree(proot_inode);
}