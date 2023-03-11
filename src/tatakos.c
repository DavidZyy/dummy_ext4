/**
 * @file bio.c
 * @author Yangyang Zhu (1929772352@qq.com)
 * @version 0.1
 * @date 2023-03-05
 * 
 * @copyright Copyright (c) 2023
 * 
 * read a formated file(e.g. formated by mkfs.vfat or mkfs) like read a disk device.
 * We can mount a formated file like mount a real device.
 * These codes is used to test the correctness of dummy filesystems like fat32 or ext4 we implemented.
 * NOTE: I define the struct like this to be compatible with our os interface.
 * The codes here is to make the ext4 portable to tatakos
 */

#include "tatakos.h"
#include <unistd.h>
#include <fcntl.h>

struct buf buffer;

const char *fs_img = "/home/zhuyangyang/project/OS/dummy_ext4/ext4_fs.img";

struct buf* bread(uint32_t dev, uint32_t blockno)
{
  struct buf *b = &buffer;

  int fd = open(fs_img, O_RDWR);

  lseek(fd, blockno*BSIZE, SEEK_SET);

  read(fd, (buffer.data), BSIZE);

  close(fd);

  return &buffer;
}