#include<stdio.h>
#include "tatakos.h"
#include "ext4.h"
#include "stdlib.h"

int main(){
  ext4_fill_super();
  ext4_inode_t *pinode;
  pinode = ext4_read_ondisk_inode(EXT4_ROOT_DIR_INODE_NUM);
  void *buf = malloc(EXT4_BLOCK_SIZE);
  /* readdir */ 
  ext4_readdir(pinode, 0, buf, EXT4_BLOCK_SIZE);
  /* create a file */
}