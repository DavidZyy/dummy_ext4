ext4_fs.img = ext4_fs.img

$(ext4_fs.img):
	@dd if=/dev/zero of=$@ bs=1M count=30
	@mkfs.ext4 -b 1024 $@

dump_ext4_fs:
	dd if=$(ext4_fs.img) bs=1M count=30 | hexdump > ext4_fs.txt

clean:
	rm -rf *.txt build/

TEST ?= 

TESTDIR = test
SRCDIR = src
BUILDDIR = build

SRC = $(SRCDIR)/tatakos.c $(SRCDIR)/ext4.c
SRC += $(TESTDIR)/$(TEST).c

OBJ = $(BUILDDIR)/$(TEST)

CFLAGS = -Iinclude -g -Wall

compile:
	mkdir -p $(BUILDDIR)
	gcc -o $(OBJ) $(CFLAGS) $(SRC)

run:compile
	$(OBJ)