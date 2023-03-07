
#ifndef _BIO_H
#define _BIO_H

#include <stdint.h>

#define BSIZE 512
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
  uint8_t data[BSIZE];
  // void *data;
  // todo:
};

struct buf* bread(uint32_t dev, uint32_t blockno);
#endif