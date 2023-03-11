/**
 * @file tatakos.h
 * @author Yangyang Zhu (1929772352@qq.com)
 * @version 0.1
 * @date 2023-03-10
 * 
 * @copyright Copyright (c) 2023
 * the codes here is to make the ext4 protable to tatakos
 */
#ifndef _BIO_H
#define _BIO_H

#include <stdint.h>

#define SECTOR_SIZE BSIZE
/* to make it portable to tatakos */
#define kmalloc malloc
#define kfree	free

#define BSIZE 512

#define T_DIR 1  // Directory
#define T_FILE 2 // File
#define T_DEVICE  3   // Device
#define T_RAM        4 // ramfs

#define ALIGN(a, align) ((a + (align - 1)) & ~(align - 1))

struct buf {
  int valid;   // has data been read from disk?
  int dirty;
  int disk;    // does disk "own" buf?
  uint32_t dev;
  uint32_t blockno;
  // struct sleeplock lock;
  // uint refcnt;
  // struct buf *prev; // LRU cache list
  // struct buf *next;
  uint8_t data[SECTOR_SIZE];
  // void *data;
  // todo:
};


struct linux_dirent64 {
    uint64_t          d_ino;
    uint64_t          d_off;
    unsigned short    d_reclen;
    unsigned char     d_type;
    char              d_name[];
};


typedef struct buf buf_t;


buf_t* bread(uint32_t dev, uint32_t blockno);

#endif