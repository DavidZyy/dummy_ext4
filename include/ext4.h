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

#define EXT4_LABEL_MAX			16

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

#endif	/* _EXT4_H */
