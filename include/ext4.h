/**
 * @file ext4.h
 * @author Yangyang Zhu (1929772352@qq.com)
 * @version 0.1
 * @date 2023-03-05
 * 
 * @copyright Copyright (c) 2023
 * Codes stealed from linux fs/ext4/ext4.h
 */

#ifndef _EXT4_H
#define _EXT4_H

#include <stdint.h>
#include "bio.h"

#define __le8 uint8_t
#define __le16 uint16_t
#define __le32 uint32_t
#define __le64 uint64_t

#define __u8 uint8_t
#define __u16 uint16_t
#define __u32 uint32_t
#define __u64 uint64_t



/* we can use mkfs.ext4 -b option to specify the block size, here I set it to 1024 bytes, see manual */
#define EXT4_BLOCK_SIZE			1024
/* the max counts of block group*/
#define EXT4_MAX_BG_CNT			10

/* convert block counts to sector counts, one ext4 block size equals to ? physical sector size */
#define EXT4_BLOCK2SECTOR_CNT		EXT4_BLOCK_SIZE / SECTOR_SIZE

#define EXT4_BLOCKNUM2SECTORNUM(num)	num*EXT4_BLOCK2SECTOR_CNT

#define EXT4_LABEL_MAX			16

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

/* the inode number of root directory */
#define EXT4_ROOT_DIR_INODE_NUM 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Finding an inode */


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
	uint16_t	s_block_group_nr;	/* block group # of this superblock */
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
	__le32	i_blocks_lo;	/* Blocks count */
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


typedef struct ext4_super_block ext4_super_block_t;
typedef struct ext4_group_desc ext4_group_desc_t;

int ext4_fill_super();
#endif	/* _EXT4_H */
