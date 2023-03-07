#ifndef _VFS_H
#define _VFS_H

struct super_block {
  const struct super_operations *s_op;

};

#endif
