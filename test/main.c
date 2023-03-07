#include<stdio.h>
#include "bio.h"

int main(){
  struct buf * b = bread(0, 2);
  printf("%x\n", *(b->data + 1));
}