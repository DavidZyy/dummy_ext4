#include "ext4.h"
#include "tatakos.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

ext4_super_block_t es;
/* The descriptors of block group.
  The number of block groups is the size of the device divided by the size of a block group. */
ext4_group_desc_t egd[EXT4_MAX_BG_CNT];
/* The counts of block group */
int bg_cnts;


uint8_t ext4_block_buff[EXT4_BLOCK_SIZE];
ext4_inode_t inode;

extern struct buf buffer;
/**
 * read or write the content of block with the number of blockno on disk into buffer.
 */
void ext4_rw_ondisk_block(int blockno, void *buff, int rw){
  buf_t *b;
  int sectorno;

  for(int i = 0; i < EXT4_BLOCK2SECTOR_CNT; i++){
    sectorno = EXT4_BLOCKNO2SECTORNO(blockno) + i;

    if(rw == EXT4_READ){
      b = bread(0, sectorno);
      memcpy(buff + i*SECTOR_SIZE, b->data, SECTOR_SIZE);
    } else {
      b = &buffer;
      /* conver blockno to sectorno */
      b->blockno = sectorno;
      memcpy(b->data, buff + i*SECTOR_SIZE, SECTOR_SIZE);
      bwrite(b);
    }
  }

}

/**
 * read super block and block group descriptor
 */
int ext4_fill_super(){
  ext4_super_block_t *pes = &es;
  // ext4_group_desc_t egd[EXT4_MAX_BG_CNT];
  
  /* The first 1024 bytes (block 0) is empty , 
    so we begin form the block 1 . */
  ext4_rw_ondisk_block(1, ext4_block_buff, EXT4_READ);
  memcpy(pes, ext4_block_buff, sizeof(ext4_super_block_t));
  assert(1<<(10 + es.s_log_block_size) == EXT4_BLOCK_SIZE);

  bg_cnts = pes->s_blocks_count_lo / pes->s_blocks_per_group + 1;
  assert(bg_cnts < EXT4_MAX_BG_CNT);
  assert(bg_cnts * sizeof(ext4_group_desc_t) <= sizeof(ext4_block_buff));

  /* read block group descriptor, padding bytes + one super block bytes */
  ext4_rw_ondisk_block(2, ext4_block_buff, EXT4_READ);
  int free_inode_cnt = 0, free_block_cnt = 0;
  for(int i = 0; i < bg_cnts; i++){
    memcpy(egd + i, ext4_block_buff + i*sizeof(ext4_group_desc_t), 
          sizeof(ext4_group_desc_t));
    free_inode_cnt += egd[i].bg_free_inodes_count_lo;
    free_block_cnt += egd[i].bg_free_blocks_count_lo;
  }
  assert(free_inode_cnt == es.s_free_inodes_count);
  assert(free_block_cnt == es.s_free_blocks_count_lo);

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

/**
 * get the offset in a block group inode table
 */
int ext4_itable_off(int inode_num){
  /* NOTE: can not use sizeof(ext4_inode_t) here, they are not equal,
    es.s_inode_size usaully equals to 256. */
  return es.s_inode_size * ext4_itable_idx(inode_num);
}

/**
 * Read the inode with number inode_num 
 */
void ext4_rw_ondisk_inode(int inode_num, ext4_inode_t *pinode, int rw){
  // ext4_inode_t *pinode = &inode;
  // ext4_extent_header_t *peh;
  int bg_inode_livein = ext4_bg_inode_livein(inode_num);
  int itable_off = ext4_itable_off(inode_num);
  int inode_block_idx = itable_off / EXT4_BLOCK_SIZE;
  int inode_block_off = itable_off % EXT4_BLOCK_SIZE;
  int blockno = egd[bg_inode_livein].bg_inode_table_lo + inode_block_idx;

  ext4_rw_ondisk_block(blockno, ext4_block_buff, EXT4_READ);
  if(rw == EXT4_READ)
    memcpy(pinode, ext4_block_buff + inode_block_off, sizeof(ext4_inode_t));
  else if (rw == EXT4_WRITE) {
    memcpy(ext4_block_buff + inode_block_off, pinode, sizeof(ext4_inode_t));
    // ext4_write_
    // todo();
    ext4_rw_ondisk_block(blockno, ext4_block_buff, EXT4_WRITE);

  } else {
    panic("rw error");
  }
  // pinode = (ext4_inode_t *)(ext4_block_buff + inode_block_off);

  // peh = (ext4_extent_header_t *)(pinode->i_block);
  // for(int i = 0; i < peh->)
  // return pinode;
  assert(pinode->i_flags | EXT4_EXTENTS_FL);
}

/**
 * Convert the dir entry on disk to struct linux_dirent64. 
 * A phony struct ext4_dir_entry is placed at the end of block.
 * So we do not to worry about the case that a dir entry span two blocks.
 * NOTE there is no '\0' on the disk, and the name_len of ext4_dir_entry_2
 * do not contain it.
 */
void ext4_get_linux_dirent64(int offset, void *buf, int len, void *imap_block_buff){
  int remain_size = EXT4_BLOCK_SIZE;
  int block_off = 0;
  ext4_dir_entry_2_t *p_dir_entry;
  struct linux_dirent64* plinux_dirent64;
  // int a = sizeof(struct linux_dirent64);
  assert(offset < len);
  
  /*we need to look throu the hole block, becase when we rename a file
    with longer name, its direntry location maybe changed.*/
  while(1){
    p_dir_entry = (ext4_dir_entry_2_t *)(imap_block_buff + block_off);
    plinux_dirent64 = (struct linux_dirent64 *)(buf + offset);

    /* end of the block */
    if(p_dir_entry->name_len == 0){
      assert(p_dir_entry->file_type == EXT4_FT_DIR_CSUM);
      break;
    }
    plinux_dirent64->d_ino = p_dir_entry->inode;
    plinux_dirent64->d_off = offset;// or offset
    plinux_dirent64->d_reclen = ALIGN(sizeof(struct linux_dirent64)+p_dir_entry->name_len, 8);
    // plinux_dirent64->d_reclen = 8+8+2+1 + p_dir_entry->name_len + 1;// the last 1 is '\0'
    plinux_dirent64->d_type = (p_dir_entry->file_type == EXT4_FT_REG_FILE) ? T_FILE : T_DIR;
    /* do we need to append '\0' ?*/
    memcpy(plinux_dirent64->d_name, p_dir_entry->name, p_dir_entry->name_len);
    // plinux_dirent64->d_name[p_dir_entry->name_len] = '\0';

    block_off += p_dir_entry->rec_len;
    *(&offset) += plinux_dirent64->d_reclen;
    printf("%s\n", plinux_dirent64->d_name);
  }
}

/**
 * Tranverse the tree in preorder to get its data blocks
 * If this functiosn have multiple use, we can use function handler and __VA_ARGS__.
 * to do the following tasks:
 * 1. Read directory entries.
 */
void ext4_traverse_extent_tree_recursively(ext4_extent_header_t *peh, int offset, void *buf, int len){
  int i, j;
  // ext4_extent_header_t *peh_next_level;
  ext4_extent_idx_t *pextent_idx;
  ext4_extent_t *pextent, *pextent_temp;
  // void *data_block_buff;
  void *imap_block_buff = kmalloc(EXT4_BLOCK_SIZE);

  assert(peh->eh_magic = EXT4_EH_MAGIC);
  assert(sizeof(ext4_extent_header_t) == sizeof(ext4_extent_idx_t));
  assert(sizeof(ext4_extent_idx_t) == sizeof(ext4_extent_t));
  /* the three struct has the same size (12 bytes) and the former two
    is at the next of ext4_extent_header */
  pextent_idx = pextent = peh + 1;

  if(peh->eh_depth == 0){
    // data_block_buff = kmalloc(EXT4_BLOCK_SIZE);
    for(i = 0; i < peh->eh_entries; i++){
      /* get the ext4_extent_t structure */
      pextent_temp = pextent + i; 
      for(j = 0; j < pextent_temp->ee_len; j++){
        ext4_rw_ondisk_block(pextent_temp->ee_start_lo + j, imap_block_buff, EXT4_READ);
        // todo();
        ext4_get_linux_dirent64(offset, buf, len, imap_block_buff);
      }
    }
    // kfree(data_block_buff);
  } else {
    for(i = 0; i < peh->eh_entries; i++){
      /* read the extent header in the next level of the tree */
      // peh_next_level = kmalloc(EXT4_BLOCK_SIZE);
      ext4_rw_ondisk_block((pextent_idx + i)->ei_leaf_lo, imap_block_buff, EXT4_READ);
      ext4_traverse_extent_tree_recursively((ext4_extent_header_t *)imap_block_buff, offset, buf, len);
      // ext4_traverse_extent_tree_recursively(peh_next_level);
      // kfree(peh_next_level);
    }
  }
  kfree(imap_block_buff);
}

/**
 * Used for ls command, sys_getdents64 system call
 */
int ext4_readdir(ext4_inode_t *pinode, int offset, void *buf, int len){
  // int i, j;
  ext4_extent_header_t *peh;
  // ext4_extent_idx_t *pextent_idx;
  // ext4_extent_t *pextent, *pextent_temp;
  // void *data_block_buff;

  peh = (ext4_extent_header_t *)(pinode->i_block);
  /* the three struct has the same size (12 bytes) and the former two
    is at the next of ext4_extent_header */
  // pextent_idx = pextent = peh + 1;

  assert(peh->eh_magic = EXT4_EH_MAGIC);
  assert(pinode->i_flags & EXT4_EXTENTS_FL);
  assert(!(pinode->i_flags & EXT4_INDEX_FL));
  assert(offset == 0);

  ext4_traverse_extent_tree_recursively(peh, offset, buf, len);
  // if(peh->eh_depth == 0){
  //   data_block_buff = kmalloc(EXT4_BLOCK_SIZE);
  //   for(i = 0; i < peh->eh_entries; i++){
  //     pextent_temp = pextent + i; 
  //     for(j = 0; j < pextent_temp->ee_len; i++){
  //       ext4_read_ondisk_block(pextent_temp + j, data_block_buff);    
  //       todo();
  //     }
  //   }
  //   kfree(data_block_buff);
  // } else {
  //   for(i = 0; i < peh->eh_entries; i++){
  //     /* read the extent header in the next level of the tree */
  //     peh_next_level = kmalloc(EXT4_BLOCK_SIZE);
  //     ext4_read_ondisk_block((pextent_idx + i)->ei_leaf_lo, peh_next_level);
  //     ext4_traverse_extent_tree_recursively(peh_next_level);
  //     kfree(peh_next_level);
  //   }
  // }
}

/**
 * Allocate and return an inode number.
 * Note that the inode number is begin from 1.
 */
int ext4_get_inodeno(){
  int i, bg_no;
  void *imap_block_buff;
  void *itable_block_buff;
  int inode_no = 0;

  /* find a block group that has free inode */
  for(i = 0; i < bg_cnts; i ++){
    inode_no = i*es.s_inodes_per_group;
    if(egd[i].bg_free_inodes_count_lo){
      bg_no = i;
      break;
    }
  } 

  if(bg_no == bg_cnts)
    panic("no free inodes");
  
  imap_block_buff = kmalloc(EXT4_BLOCK_SIZE);
  ext4_rw_ondisk_block(egd[i].bg_inode_bitmap_lo, imap_block_buff, EXT4_READ);
  for(i = 0; i < es.s_inodes_per_group; i++){
    // char a = *(char *)(imap_block_buff + i/8);
    char *a = ((char *)(imap_block_buff) + i/8);
    if((*a & (1 << (i%8))) == 0){
      *a |= (1 << (i%8));
      inode_no += i;
      break;
    }
  }

  egd[i].bg_free_inodes_count_lo--;
  // ext4_write_ondisk_block(egd[i].bg_inode_bitmap_lo, imap_block_buff);
  kfree(imap_block_buff);

  /* the inode number begin with 1 */
  return inode_no + 1;
}

/**
 * Create an inode of dir or file, we should do the following things:
 * 1. get an inode number from inode bitmap.
 * 2. write the inode to inode table.
 * 3. get a free block from block bitmap, allocate the block to it.
 * 4. write the dir entry of the inode into its parent's data block. 
 */
ext4_inode_t *ext4_create_inode(ext4_inode_t *parent_inode, int type){
  int new_inodeno = ext4_get_inodeno();
  printf("%d\n", new_inodeno);
  ext4_inode_t *new_inode = kmalloc(sizeof(ext4_inode_t));
  
  new_inode->i_mode = S_IRWXO | S_IRWXG | S_IRWXU | type;
  new_inode->i_size_lo = 0;
  new_inode->i_atime = new_inode->i_ctime = new_inode->i_mtime = 0xffffffff;
  new_inode->i_links_count = 1;
  new_inode->i_blocks_lo = 1;
  new_inode->i_flags |= EXT4_EXTENTS_FL;

  ext4_rw_ondisk_inode(new_inodeno, new_inode, EXT4_WRITE);
  kfree(new_inode);
}