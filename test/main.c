#include<stdio.h>
#include "bio.h"
#include "ext4.h"

int main(){
  // struct buf * b = bread(0, 2);
  // printf("%x\n", *(b->data + 1));
  // printf("%d\n", sizeof(struct ext4_super_block));
  ext4_fill_super();
  ext4_read_ondisk_inode(EXT4_ROOT_DIR_INODE_NUM);
}