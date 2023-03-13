/**
 * @file ext4.c
 * @author Yangyang Zhu (1929772352@qq.com)
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2023
 * NOTE: The codes here do not support 64 bit.
 */
#include "ext4.h"
#include "tatakos.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
 * Read or Write super block and block group descriptor on disk.
 */
int ext4_rw_ondisk_super_bgd(int rw){
  int i;
  ext4_super_block_t *pes = &es;

  if(rw == EXT4_WRITE)
    memcpy(ext4_block_buff, pes, sizeof(ext4_super_block_t));

  /* The first 1024 bytes (block 0) is empty , 
    so we begin form the block 1 . */
  ext4_rw_ondisk_block(1, ext4_block_buff, rw);
  
  if(rw == EXT4_READ)
    memcpy(pes, ext4_block_buff, sizeof(ext4_super_block_t));

  assert(1<<(10 + es.s_log_block_size) == EXT4_BLOCK_SIZE);

  bg_cnts = pes->s_blocks_count_lo / pes->s_blocks_per_group + 1;
  assert(bg_cnts < EXT4_MAX_BG_CNT);
  assert(bg_cnts * sizeof(ext4_group_desc_t) <= sizeof(ext4_block_buff)); //assume all block group descriptors only in one block

  /* read or wirte block group descriptor, padding bytes + one super block bytes occupy block 0 and block 1, so 
    the block of "block group descriptor" begin with 2 */
  if(rw == EXT4_READ)
    ext4_rw_ondisk_block(2, ext4_block_buff, rw);

  int free_inode_cnt = 0, free_block_cnt = 0;
  for(i = 0; i < bg_cnts; i++){
    if(rw == EXT4_READ)
      memcpy(egd + i, ext4_block_buff + i*sizeof(ext4_group_desc_t), sizeof(ext4_group_desc_t));
    else if (rw == EXT4_WRITE)
      memcpy(ext4_block_buff + i*sizeof(ext4_group_desc_t), egd+i, sizeof(ext4_group_desc_t));
    else 
      panic("error");
    free_inode_cnt += egd[i].bg_free_inodes_count_lo;
    free_block_cnt += egd[i].bg_free_blocks_count_lo;
  }
  if(rw == EXT4_WRITE)
    ext4_rw_ondisk_block(2, ext4_block_buff, rw);

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
 * Read or write the inode with number inode_num 
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
    /* Confirm there is no content before */
    assert(*(uint64_t *)(ext4_block_buff + inode_block_off) == 0);
    memcpy(ext4_block_buff + inode_block_off, pinode, sizeof(ext4_inode_t));
    ext4_rw_ondisk_block(blockno, ext4_block_buff, rw);
  } else {
    panic("rw error");
  }
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
  /* The three struct has the same size (12 bytes) and the former two
    is at the next of ext4_extent_header */
  
  pextent_idx = (ext4_extent_idx_t *)peh + 1; 
  pextent = (ext4_extent_t *)peh + 1;

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
 * Used for "ls" command, "sys_getdents64" system call
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
 * find and set several empty bits on bitmaps of inode and block.
 */
int ext4_find_set_bitmap(){

}

#define UP_FR_IND 0
#define UP_FR_BLK 1

/**
 * Update the free inode count and the free block count.
 * use type to choose inode or block, use groupid to choose block group,
 * if cnt is negative, the free counts decrese, or it increse.
 */
static void ext4_update_free_ib_cnt(int type, int groupid, int cnt){
  if(type == UP_FR_IND){
    es.s_free_inodes_count += cnt;
    egd[groupid].bg_free_inodes_count_lo += cnt;
  } else if (type == UP_FR_BLK){
    es.s_free_blocks_count_lo += cnt;
    egd[groupid].bg_free_blocks_count_lo += cnt;
  } else {
    panic("no such update type");
  }
}

/**
 * Allocate and return an inode number.
 * Note that the inode number is begin from 1.
 */
int ext4_get_inodeno(){
  int i;
  void *imap_block_buff;
  void *itable_block_buff;
  int inode_no = 0, groupid;
  int bitmap_blockno;

  /* find a block group that has free inode */
  for(i = 0; i < bg_cnts; i ++){
    inode_no = i*es.s_inodes_per_group;
    if(egd[i].bg_free_inodes_count_lo){
      /* one group has only one inode bitmap */
      bitmap_blockno = egd[i].bg_inode_bitmap_lo;
      groupid = i;
      break;
    }
  } 

  if(i == bg_cnts)
    panic("no free inodes");
  
  imap_block_buff = kmalloc(EXT4_BLOCK_SIZE);
  ext4_rw_ondisk_block(bitmap_blockno, imap_block_buff, EXT4_READ);
  for(i = 0; i < es.s_inodes_per_group; i++){
    // char a = *(char *)(imap_block_buff + i/8);
    char *a = ((char *)(imap_block_buff) + i/8);
    if((*a & (1 << (i%8))) == 0){
      *a |= (1 << (i%8));
      inode_no += i;
      break;
    }
  }
  ext4_rw_ondisk_block(bitmap_blockno, imap_block_buff, EXT4_WRITE);
  ext4_update_free_ib_cnt(UP_FR_IND, groupid, -1);

  kfree(imap_block_buff);

  /* the inode number begin with 1, not 0, so we need to plus 1 when return */
  return inode_no + 1;
}

static void ext4_set_new_inode(ext4_inode_t *new_inode, int type){
  ext4_extent_header_t *peh;

  new_inode->i_mode = S_IRWXO | S_IRWXG | S_IRWXU | type;
  new_inode->i_size_lo = 0;
  new_inode->i_atime = new_inode->i_ctime = new_inode->i_mtime = 0xffffffff;
  new_inode->i_links_count = 1;
  new_inode->i_blocks_lo = 0;
  new_inode->i_flags |= EXT4_EXTENTS_FL; //use extent tree

  /* set extent header */
  peh = (ext4_extent_header_t *)new_inode->i_block;
  peh->eh_magic = EXT4_EH_MAGIC;
  peh->eh_entries = 0;
  peh->eh_max = sizeof(new_inode->i_block) / sizeof(ext4_extent_header_t) - 1;
  peh->eh_depth = 0;
  peh->eh_generation = 0;
}


/**
 * Create a new ext4_extent_t on the endest of the header.
 */
static ext4_extent_t *ext4_create_new_extent(ext4_extent_header_t *peh){
  ext4_extent_t *pextent;

  if(peh->eh_entries >= peh->eh_max){
    panic("extent full!");
  }
  pextent = (ext4_extent_t *)(peh) + 1 + peh->eh_entries;
  pextent->ee_block = 0;
  pextent->ee_len = 0;
  pextent->ee_start_hi = 0;
  pextent->ee_start_lo = 0;

  /* do not forget this! */
  peh->eh_entries++;
  return pextent;
}

/**
 * Set the block, len, start_lo fields in ture
 */
static void ext4_set_extent(ext4_extent_t *pextent, int block, int len, int start_lo){
  /* file logical block */
  pextent->ee_block = block;
  pextent->ee_len = len;
  pextent->ee_start_lo = start_lo;
}

/**
 * Find free location and set it in given bitmap.
 */
static int ext4_get_free_bit(void *bitmap, int upperbound){
  int i;
  char *a;

  for(i = 0; i < upperbound; i++){
    a = (char *)bitmap + i/8;

    if((*a & (1 << i%8)) == 0){
      *a |= (1 << i%8);
      return i;
    }
  }
}

/**
 * return ONE free block. 
 */
static int ext4_get_free_blockno(){
  int i, blockno = 0;
  void *blockbitmap_buff;

  blockbitmap_buff = kmalloc(EXT4_BLOCK_SIZE);
  /* find a block group that has free blocks */
  for(i = 0; i < bg_cnts; i++){
    blockno = i*es.s_blocks_per_group;
    if(egd[i].bg_free_blocks_count_lo > 0){
      ext4_rw_ondisk_block(egd[i].bg_block_bitmap_lo, blockbitmap_buff, EXT4_READ);
      blockno += ext4_get_free_bit(blockbitmap_buff, es.s_blocks_per_group);
      ext4_rw_ondisk_block(egd[i].bg_block_bitmap_lo, blockbitmap_buff, EXT4_WRITE);
      break;
    }
  }

  ext4_update_free_ib_cnt(UP_FR_BLK, i, -1);
  kfree(blockbitmap_buff);
  /* or return blockno + 1? */
  TODO();
  return blockno;
}

/**
 * Check if the bit on bitmap is already assigned.
 */
int ext4_check_bitmap(int num){
  int groupid = num / es.s_blocks_per_group;
  int off = num % es.s_blocks_per_group;
  void *bitmap_buff = kmalloc(EXT4_BLOCK_SIZE);

  ext4_rw_ondisk_block(egd[groupid].bg_block_bitmap_lo, bitmap_buff, EXT4_READ);
  char *a = (char *)bitmap_buff + off / 8;

  return (*a & 1 << (off % 8));
}

/**
 * Allocate and append block_cnt blocks to inode.
 * Find the last struct ext4_extent of pinode extent tree, if
 * 1. there are enough blocks behind the scope of ext4_extent indicates, we just need to modify it's ee_len field.
 * 2. there are not enough blocks, we should allocate the blocks as much as we can, and find one or more new contiguous
 *    areas on disk, than create several new ext4_extent to describe it, and assign these blocks to inode.
 */
void ext4_alloc_block(ext4_inode_t *pinode, int block_cnt){
  assert(block_cnt == 1);
  ext4_extent_header_t *peh;
  ext4_extent_t *pextent;
  int i, groupid, blockbitmap_blockno;
  int remain_cnt = block_cnt;
  void *blockbitmap_buff;

  peh = (ext4_extent_header_t *)(pinode->i_block);
  assert(peh->eh_magic == EXT4_EH_MAGIC);

  /* for simplicity, not support extent tree which depth > 0 now */
  assert(peh->eh_depth == 0);

  if(pinode->i_blocks_lo == 0){
    assert(peh->eh_entries == 0);
    goto create_new_extent_and_assign_blocks;
  } else {
    assert(peh->eh_entries > 0);
    /* get the last struct ext4_extent */
    pextent = (ext4_extent_t *)peh + peh->eh_entries;
    /* if there are free blocks behind the last ext4_extent */
    if(!ext4_check_bitmap(pextent->ee_start_lo + pextent->ee_len)){
      int a = ext4_get_free_blockno();
      assert(a == pextent->ee_start_lo + pextent->ee_len);
      pextent->ee_len++;
      goto success;
    }
  }


/* Only support create one extent now at once */
create_new_extent_and_assign_blocks:
  pextent = ext4_create_new_extent(peh);
  int blockno = ext4_get_free_blockno();
  ext4_set_extent(pextent, pinode->i_blocks_lo, block_cnt, blockno);

success:
  /* this field of inode refers to sectors on disk, not ext4 block, see reference 2, 4.1 */
  pinode->i_blocks_lo += (block_cnt * EXT4_BLOCK2SECTOR_CNT);
  return;
}

void ext4_set_dir_entry(ext4_dir_entry_2_t* dir_entry, int inodeno, int rec_len, int dir_type, char *name){
  dir_entry->inode = inodeno;
  dir_entry->rec_len = rec_len;
  dir_entry->name_len = strlen(name);
  dir_entry->file_type = dir_type;
  /* sizeof(name)-1: donot copy '\0' */
  memcpy(dir_entry->name, name, strlen(name));
}

/**
 * Convert new_inode to struct ext4_dir_entry_2 and write it to parent_inode's data block.
 */
void ext4_write_dir_entry(ext4_inode_t *parent_inode, int inodeno, int dir_type, char *name){
  int block_off = 0, blockno;
  ext4_dir_entry_2_t *p_dir_entry;
  void *data_buff;
  ext4_extent_header_t *peh;
  ext4_extent_t *pextent;

  // panic("modify the file when it is mounded to see if it work");
  peh = (ext4_extent_header_t *)(parent_inode->i_block);
  assert(peh->eh_depth == 0);
  assert(peh->eh_entries > 0);
  /* get the last extent's last block */
  pextent = (ext4_extent_t *)peh + peh->eh_entries;
  /* the last data block of parent_inode */
  blockno = pextent->ee_start_lo + pextent->ee_len - 1;
  
  data_buff = kmalloc(EXT4_BLOCK_SIZE);
  ext4_rw_ondisk_block(blockno, data_buff, EXT4_READ);
  while(1){
    p_dir_entry = (ext4_dir_entry_2_t *)(data_buff + block_off);
    /* The last dir entry in the block */
    if(p_dir_entry->rec_len + block_off == EXT4_BLOCK_SIZE - 0x0C){
      int len1 = ALIGN(4+2+1+1+p_dir_entry->name_len, 4);
      int len2 = ALIGN(4+2+1+1+strlen(name), 4);
      if(p_dir_entry->rec_len - len1 >= len2){
        int origin = p_dir_entry->rec_len;
        p_dir_entry->rec_len = len1;
        block_off += p_dir_entry->rec_len;
        p_dir_entry = (ext4_dir_entry_2_t *)(data_buff + block_off);
        ext4_set_dir_entry(p_dir_entry, inodeno, origin-len1, dir_type, name);
        break;
      } else {
        panic("the block is full!");
      }
    }
    block_off += p_dir_entry->rec_len;
  }
  ext4_rw_ondisk_block(blockno, data_buff, EXT4_WRITE);
  kfree(data_buff);

}

void ext4_generate_dot(){

}

/**
 * Create an inode of dir or file, we should do the following things:
 * 1. get an inode number from inode bitmap.
 * 2. write the inode to inode table.
 * 3. get a free block from block bitmap, allocate the block to it.
 * 4. write the dir entry of the inode into its parent's data block. 
 */
ext4_inode_t *ext4_create_inode(ext4_inode_t *parent_inode, int type){
  int new_inodeno;
  ext4_inode_t *new_inode;

  new_inodeno = ext4_get_inodeno();
  new_inode = kmalloc(sizeof(ext4_inode_t));
  
  ext4_set_new_inode(new_inode, type);


  ext4_alloc_block(new_inode, 1);

  /* the length not include '\0' */
  char *name = "zyy123";
  // printf("%d\n", strlen(name));

  int dir_type = type == S_IFDIR ? EXT4_FT_DIR : EXT4_FT_REG_FILE;
  ext4_write_dir_entry(parent_inode, new_inodeno, dir_type, name);
  if(type == T_DIR){
    ext4_generate_dot();
  }

  ext4_rw_ondisk_inode(new_inodeno, new_inode, EXT4_WRITE);
  kfree(new_inode);
  printf("%d\n", new_inodeno);
}