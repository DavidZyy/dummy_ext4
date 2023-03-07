ext4_fs.img = ext4_fs.img
fat_fs.img = fat_fs.img

$(ext4_fs.img):
	@dd if=/dev/zero of=$@ bs=1M count=30
	@mkfs.ext4 -b 1024 $@

$(fat_fs.img):
	@dd if=/dev/zero of=$@ bs=1M count=30
	@mkfs.vfat -F 32 -s 8 $@

dump_ext4_fs:
	dd if=$(ext4_fs.img) bs=1k count=20 | hexdump > ext4_fs.txt

dump_fat_fs:
	dd if=$(fat_fs.img) bs=1k count=20 | hexdump > fat_fs.txt

clean:
	rm -rf *.txt *.img build/

TEST ?= 

TESTDIR = test
SRCDIR = src
BUILDDIR = build

SRC = $(SRCDIR)/bio.c
SRC += $(TESTDIR)/$(TEST).c

OBJ = $(BUILDDIR)/$(TEST)

CFLAGS = -Iinclude -g

run:
	mkdir -p $(BUILDDIR)
	gcc -o $(OBJ) $(CFLAGS) $(SRC)
	$(OBJ)