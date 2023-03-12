/**
 * @file ext4.h
 * @author Yangyang Zhu (1929772352@qq.com)
 * @version 0.1
 * @date 2023-03-05
 * 
 * @copyright Copyright (c) 2023
 * Codes stealed from linux fs/ext4/ext4.h
 * reference: 
 * [1] linux kernel codes
 * [2] https://www.kernel.org/doc/html/latest/filesystems/ext4/index.html
 */

#ifndef _EXT4_H
#define _EXT4_H

#include <stdint.h>
#include "tatakos.h"

/* in octal */
#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001


/* define number */
#define __le8 uint8_t
#define __le16 uint16_t
#define __le32 uint32_t
#define __le64 uint64_t

#define __u8 uint8_t
#define __u16 uint16_t
#define __u32 uint32_t
#define __u64 uint64_t

#define EXT4_EH_MAGIC	0xf30a

#define EXT4_READ 0
#define EXT4_WRITE 1

/* we can use mkfs.ext4 -b option to specify the block size, here I set it to 1024 bytes, see manual */
#define EXT4_BLOCK_SIZE			1024
/* the max counts of block group*/
#define EXT4_MAX_BG_CNT			10

/* convert block counts to sector counts, one ext4 block size equals to ? physical sector size */
#define EXT4_BLOCK2SECTOR_CNT		EXT4_BLOCK_SIZE / SECTOR_SIZE

#define EXT4_BLOCKNO2SECTORNO(num)	num*EXT4_BLOCK2SECTOR_CNT

#define EXT4_LABEL_MAX			16

#define EXT4_NAME_LEN				255

/* Padding is at the most beginning of the disk layout */
#define EXT4_GROUP0_PADDING_BYTES 	1024
#define EXT4_GROUP0_PADDING_SECTOR_CNT EXT4_GROUP0_PADDING_BYTES / SECTOR_SIZE

/////////////////////////////////////////////////////// inode related//////////////////////////////////////////////////////////////////
/*
 * Constants relative to the data blocks
 */
#define	EXT4_NDIR_BLOCKS		12
#define	EXT4_IND_BLOCK			EXT4_NDIR_BLOCKS
#define	EXT4_DIND_BLOCK			(EXT4_IND_BLOCK + 1)
#define	EXT4_TIND_BLOCK			(EXT4_DIND_BLOCK + 1)
#define	EXT4_N_BLOCKS				(EXT4_TIND_BLOCK + 1)

/* the inode number of root directory, the i_flags of root inode = 0x80000
	means it use extents, I'm not sure if it's a common case. */
#define EXT4_ROOT_DIR_INODE_NUM 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Inode flags
 */
#define	EXT4_SECRM_FL			0x00000001 /* Secure deletion */
#define	EXT4_UNRM_FL			0x00000002 /* Undelete */
#define	EXT4_COMPR_FL			0x00000004 /* Compress file */
#define EXT4_SYNC_FL			0x00000008 /* Synchronous updates */
#define EXT4_IMMUTABLE_FL		0x00000010 /* Immutable file */
#define EXT4_APPEND_FL			0x00000020 /* writes to file may only append */
#define EXT4_NODUMP_FL			0x00000040 /* do not dump file */
#define EXT4_NOATIME_FL			0x00000080 /* do not update atime */
/* Reserved for compression usage... */
#define EXT4_DIRTY_FL			0x00000100
#define EXT4_COMPRBLK_FL		0x00000200 /* One or more compressed clusters */
#define EXT4_NOCOMPR_FL			0x00000400 /* Don't compress */
	/* nb: was previously EXT2_ECOMPR_FL */
#define EXT4_ENCRYPT_FL			0x00000800 /* encrypted file */
/* End compression flags --- maybe not all used */
#define EXT4_INDEX_FL			0x00001000 /* hash-indexed directory */
#define EXT4_IMAGIC_FL			0x00002000 /* AFS directory */
#define EXT4_JOURNAL_DATA_FL		0x00004000 /* file data should be journaled */
#define EXT4_NOTAIL_FL			0x00008000 /* file tail should not be merged */
#define EXT4_DIRSYNC_FL			0x00010000 /* dirsync behaviour (directories only) */
#define EXT4_TOPDIR_FL			0x00020000 /* Top of directory hierarchies*/
#define EXT4_HUGE_FILE_FL               0x00040000 /* Set to each huge file */
#define EXT4_EXTENTS_FL			0x00080000 /* Inode uses extents */
#define EXT4_VERITY_FL			0x00100000 /* Verity protected inode */
#define EXT4_EA_INODE_FL	        0x00200000 /* Inode used for large EA */
/* 0x00400000 was formerly EXT4_EOFBLOCKS_FL */

#define EXT4_DAX_FL			0x02000000 /* Inode is DAX */

#define EXT4_INLINE_DATA_FL		0x10000000 /* Inode has inline data. */
#define EXT4_PROJINHERIT_FL		0x20000000 /* Create with parents projid */
#define EXT4_CASEFOLD_FL		0x40000000 /* Casefolded directory */
#define EXT4_RESERVED_FL		0x80000000 /* reserved for ext4 lib */


/*
 * Ext4 directory file types.  Only the low 3 bits are used.  The
 * other bits are reserved for now.
 */
#define EXT4_FT_UNKNOWN		0
#define EXT4_FT_REG_FILE	1
#define EXT4_FT_DIR		2
#define EXT4_FT_CHRDEV		3
#define EXT4_FT_BLKDEV		4
#define EXT4_FT_FIFO		5
#define EXT4_FT_SOCK		6
#define EXT4_FT_SYMLINK		7

#define EXT4_FT_MAX		8

#define EXT4_FT_DIR_CSUM	0xDE


/*
 * Structure of the super block (the struct is defined as the data on disk)
 */
struct ext4_super_block {
/*00*/	uint32_t	s_inodes_count;		/* Inodes count */
	uint32_t	s_blocks_count_lo;	/* Blocks count */
	uint32_t	s_r_blocks_count_lo;	/* Reserved blocks count */
	uint32_t	s_free_blocks_count_lo;	/* Free blocks count */
/*10*/	uint32_t	s_free_inodes_count;	/* Free inodes count */
	uint32_t	s_first_data_block;	/* First Data Block */
	uint32_t	s_log_block_size;	/* Block size */
	uint32_t	s_log_cluster_size;	/* Allocation cluster size */
/*20*/	uint32_t	s_blocks_per_group;	/* # Blocks per group */
	uint32_t	s_clusters_per_group;	/* # Clusters per group */
	uint32_t	s_inodes_per_group;	/* # Inodes per group */
	uint32_t	s_mtime;		/* Mount time */
/*30*/	uint32_t	s_wtime;		/* Write time */
	uint16_t	s_mnt_count;		/* Mount count */
	uint16_t	s_max_mnt_count;	/* Maximal mount count */
	uint16_t	s_magic;		/* Magic signature */
	uint16_t	s_state;		/* File system state */
	uint16_t	s_errors;		/* Behaviour when detecting errors */
	uint16_t	s_minor_rev_level;	/* minor revision level */
/*40*/	uint32_t	s_lastcheck;		/* time of last check */
	uint32_t	s_checkinterval;	/* max. time between checks */
	uint32_t	s_creator_os;		/* OS */
	uint32_t	s_rev_level;		/* Revision level */
/*50*/	uint16_t	s_def_resuid;		/* Default uid for reserved blocks */
	uint16_t	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT4_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	uint32_t	s_first_ino;		/* First non-reserved inode */
	uint16_t  s_inode_size;		/* size of inode structure */
	uint16_t	s_block_group_nr;	/* block group # of this superblock，不是总块组数，是当前超级块所在的块组  */
	uint32_t	s_feature_compat;	/* compatible feature set */
/*60*/	uint32_t	s_feature_incompat;	/* incompatible feature set */
	uint32_t	s_feature_ro_compat;	/* readonly-compatible feature set */
/*68*/	uint8_t	s_uuid[16];		/* 128-bit uuid for volume */
/*78*/	char	s_volume_name[EXT4_LABEL_MAX];	/* volume name */
/*88*/	char	s_last_mounted[64];	/* directory where last mounted */
/*C8*/	uint32_t	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	uint8_t	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	uint8_t	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	uint16_t	s_reserved_gdt_blocks;	/* Per group desc for online growth */
	/*
	 * Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
/*D0*/	uint8_t	s_journal_uuid[16];	/* uuid of journal superblock */
/*E0*/	uint32_t	s_journal_inum;		/* inode number of journal file */
	uint32_t	s_journal_dev;		/* device number of journal file */
	uint32_t	s_last_orphan;		/* start of list of inodes to delete */
	uint32_t	s_hash_seed[4];		/* HTREE hash seed */
	uint8_t	s_def_hash_version;	/* Default hash version to use */
	uint8_t	s_jnl_backup_type;
	uint16_t  s_desc_size;		/* size of group descriptor */
/*100*/	uint32_t	s_default_mount_opts;
	uint32_t	s_first_meta_bg;	/* First metablock block group */
	uint32_t	s_mkfs_time;		/* When the filesystem was created */
	uint32_t	s_jnl_blocks[17];	/* Backup of the journal inode */
	/* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/	uint32_t	s_blocks_count_hi;	/* Blocks count */
	uint32_t	s_r_blocks_count_hi;	/* Reserved blocks count */
	uint32_t	s_free_blocks_count_hi;	/* Free blocks count */
	uint16_t	s_min_extra_isize;	/* All inodes have at least # bytes */
	uint16_t	s_want_extra_isize; 	/* New inodes should reserve # bytes */
	uint32_t	s_flags;		/* Miscellaneous flags */
	uint16_t  s_raid_stride;		/* RAID stride */
	uint16_t  s_mmp_update_interval;  /* # seconds to wait in MMP checking */
	uint64_t  s_mmp_block;            /* Block for multi-mount protection */
	uint32_t  s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	uint8_t	s_log_groups_per_flex;  /* FLEX_BG group size */
	uint8_t	s_checksum_type;	/* metadata checksum algorithm used */
	uint8_t	s_encryption_level;	/* versioning level for encryption */
	uint8_t	s_reserved_pad;		/* Padding to next 32bits */
	uint64_t	s_kbytes_written;	/* nr of lifetime kilobytes written */
	uint32_t	s_snapshot_inum;	/* Inode number of active snapshot */
	uint32_t	s_snapshot_id;		/* sequential ID of active snapshot */
	uint64_t	s_snapshot_r_blocks_count; /* reserved blocks for active
					      snapshot's future use */
	uint32_t	s_snapshot_list;	/* inode number of the head of the
					   on-disk snapshot list */
#define EXT4_S_ERR_START offsetof(struct ext4_super_block, s_error_count)
	uint32_t	s_error_count;		/* number of fs errors */
	uint32_t	s_first_error_time;	/* first time an error happened */
	uint32_t	s_first_error_ino;	/* inode involved in first error */
	uint64_t	s_first_error_block;	/* block involved of first error */
	uint8_t	s_first_error_func[32];	/* function where the error happened */
	uint32_t	s_first_error_line;	/* line number where error happened */
	uint32_t	s_last_error_time;	/* most recent time of an error */
	uint32_t	s_last_error_ino;	/* inode involved in last error */
	uint32_t	s_last_error_line;	/* line number where error happened */
	uint64_t	s_last_error_block;	/* block involved of last error */
	uint8_t	s_last_error_func[32];	/* function where the error happened */
#define EXT4_S_ERR_END offsetof(struct ext4_super_block, s_mount_opts)
	uint8_t	s_mount_opts[64];
	uint32_t	s_usr_quota_inum;	/* inode for tracking user quota */
	uint32_t	s_grp_quota_inum;	/* inode for tracking group quota */
	uint32_t	s_overhead_clusters;	/* overhead blocks/clusters in fs */
	uint32_t	s_backup_bgs[2];	/* groups with sparse_super2 SBs */
	uint8_t	s_encrypt_algos[4];	/* Encryption algorithms in use  */
	uint8_t	s_encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	uint32_t	s_lpf_ino;		/* Location of the lost+found inode */
	uint32_t	s_prj_quota_inum;	/* inode for tracking project quota */
	uint32_t	s_checksum_seed;	/* crc32c(uuid) if csum_seed set */
	uint8_t	s_wtime_hi;
	uint8_t	s_mtime_hi;
	uint8_t	s_mkfs_time_hi;
	uint8_t	s_lastcheck_hi;
	uint8_t	s_first_error_time_hi;
	uint8_t	s_last_error_time_hi;
	uint8_t	s_first_error_errcode;
	uint8_t    s_last_error_errcode;
	uint16_t  s_encoding;		/* Filename charset encoding */
	uint16_t  s_encoding_flags;	/* Filename charset encoding flags */
	uint32_t  s_orphan_file_inum;	/* Inode for tracking orphan inodes */
	uint32_t	s_reserved[94];		/* Padding to the end of the block */
	uint32_t	s_checksum;		/* crc32c(superblock) */
};

/*
 * Structure of an inode on the disk
 */
struct ext4_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size_lo;	/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Inode Change time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks_lo;	/* Blocks count, 注意，这个字段不是指ext4文件的块（block）数，而是指磁盘扇区数，见文档4.1 */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__le32  l_i_version;
		} linux1;
		struct {
			__u32  h_i_translator;
		} hurd1;
		struct {
			__u32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[EXT4_N_BLOCKS];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl_lo;	/* File ACL */
	__le32	i_size_high;
	__le32	i_obso_faddr;	/* Obsoleted fragment address */
	union {
		struct {
			__le16	l_i_blocks_high; /* were l_i_reserved1 */
			__le16	l_i_file_acl_high;
			__le16	l_i_uid_high;	/* these 2 fields */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__le16	l_i_checksum_lo;/* crc32c(uuid+inum+inode) LE */
			__le16	l_i_reserved;
		} linux2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			__u16	h_i_mode_high;
			__u16	h_i_uid_high;
			__u16	h_i_gid_high;
			__u32	h_i_author;
		} hurd2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			__le16	m_i_file_acl_high;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
	__le16	i_extra_isize;
	__le16	i_checksum_hi;	/* crc32c(uuid+inum+inode) BE */
	__le32  i_ctime_extra;  /* extra Change time      (nsec << 2 | epoch) */
	__le32  i_mtime_extra;  /* extra Modification time(nsec << 2 | epoch) */
	__le32  i_atime_extra;  /* extra Access time      (nsec << 2 | epoch) */
	__le32  i_crtime;       /* File Creation time */
	__le32  i_crtime_extra; /* extra FileCreationtime (nsec << 2 | epoch) */
	__le32  i_version_hi;	/* high 32 bits for 64-bit version */
	__le32	i_projid;	/* Project ID */
};

/*
 * Structure of a blocks group descriptor
 */
struct ext4_group_desc
{
	__le32	bg_block_bitmap_lo;	/* Blocks bitmap block */
	__le32	bg_inode_bitmap_lo;	/* Inodes bitmap block */
	__le32	bg_inode_table_lo;	/* Inodes table block */
	__le16	bg_free_blocks_count_lo;/* Free blocks count */
	__le16	bg_free_inodes_count_lo;/* Free inodes count */
	__le16	bg_used_dirs_count_lo;	/* Directories count */
	__le16	bg_flags;		/* EXT4_BG_flags (INODE_UNINIT, etc) */
	__le32  bg_exclude_bitmap_lo;   /* Exclude bitmap for snapshots */
	__le16  bg_block_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+bbitmap) LE */
	__le16  bg_inode_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+ibitmap) LE */
	__le16  bg_itable_unused_lo;	/* Unused inodes count */
	__le16  bg_checksum;		/* crc16(sb_uuid+group+desc) */
	__le32	bg_block_bitmap_hi;	/* Blocks bitmap block MSB */
	__le32	bg_inode_bitmap_hi;	/* Inodes bitmap block MSB */
	__le32	bg_inode_table_hi;	/* Inodes table block MSB */
	__le16	bg_free_blocks_count_hi;/* Free blocks count MSB */
	__le16	bg_free_inodes_count_hi;/* Free inodes count MSB */
	__le16	bg_used_dirs_count_hi;	/* Directories count MSB */
	__le16  bg_itable_unused_hi;    /* Unused inodes count MSB */
	__le32  bg_exclude_bitmap_hi;   /* Exclude bitmap block MSB */
	__le16  bg_block_bitmap_csum_hi;/* crc32c(s_uuid+grp_num+bbitmap) BE */
	__le16  bg_inode_bitmap_csum_hi;/* crc32c(s_uuid+grp_num+ibitmap) BE */
	__u32   bg_reserved;
};

/*
 * This is the extent tail on-disk structure.
 * All other extent structures are 12 bytes long.  It turns out that
 * block_size % 12 >= 4 for at least all powers of 2 greater than 512, which
 * covers all valid ext4 block sizes.  Therefore, this tail structure can be
 * crammed into the end of the block without having to rebalance the tree.
 */
struct ext4_extent_tail {
	__le32	et_checksum;	/* crc32c(uuid+inum+extent_block) */
};

/*
 * This is the extent on-disk structure.
 * It's used at the bottom of the tree.
 */
struct ext4_extent {
	__le32	ee_block;	/* first logical block extent covers 逻辑block是指文件的逻辑块？ */
	__le16	ee_len;		/* number of blocks covered by extent */
	__le16	ee_start_hi;	/* high 16 bits of physical block */
	__le32	ee_start_lo;	/* low 32 bits of physical block */
};

/*
 * This is index on-disk structure.
 * It's used at all the levels except the bottom.
 */
struct ext4_extent_idx {
	__le32	ei_block;	/* index covers logical blocks from 'block' */
	__le32	ei_leaf_lo;	/* pointer to the physical block of the next *
				 * level. leaf or next index could be there */
	__le16	ei_leaf_hi;	/* high 16 bits of physical block */
	__u16	ei_unused;
};

/*
 * Each block (leaves and indexes), even inode-stored has header.
 */
struct ext4_extent_header {
	__le16	eh_magic;	/* probably will support different formats */
	__le16	eh_entries;	/* number of valid entries */
	__le16	eh_max;		/* capacity of store in entries */
	__le16	eh_depth;	/* has tree real underlying blocks? */
	__le32	eh_generation;	/* generation of the tree */
};

/*
 * The new version of the directory entry.  Since EXT4 structures are
 * stored in intel byte order, and the name_len field could never be
 * bigger than 255 chars, it's safe to reclaim the extra byte for the
 * file_type field.
 */
struct ext4_dir_entry_2 {
	__le32 inode; /* Inode number */
	__le16 rec_len; /* Directory entry length */
	__u8 name_len; /* Name length */
	__u8 file_type; /* See file type macros EXT4_FT_* below */
	char name[EXT4_NAME_LEN]; /* File name */
};

typedef struct ext4_super_block ext4_super_block_t;
typedef struct ext4_group_desc ext4_group_desc_t;
typedef struct ext4_inode	ext4_inode_t;
typedef struct ext4_extent_header ext4_extent_header_t;
typedef struct ext4_extent_idx ext4_extent_idx_t;
typedef struct ext4_extent ext4_extent_t;
typedef struct ext4_extent_tail ext4_extent_tail_t;
typedef struct ext4_dir_entry_2	ext4_dir_entry_2_t;

int ext4_fill_super();
void ext4_rw_ondisk_inode(int inode_num, ext4_inode_t *pinode, int rw);
int ext4_readdir(ext4_inode_t *pinode, int offset, void *buf, int len);
ext4_inode_t *ext4_create_inode(ext4_inode_t *parent_inode, int type);
int ext4_rw_ondisk_super_bgd(int rw);
void ext4_rw_ondisk_block(int blockno, void *buff, int rw);

#endif	/* _EXT4_H */
