/* filesys.h - abstract filesystem interface */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2004  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "pc_slice.h"

#ifdef FSYS_FFS
#define FSYS_FFS_NUM 1
int ffs_mount (void);
int ffs_read (char *buf, int len);
int ffs_dir (char *dirname);
int ffs_embed (int *start_sector, int needed_sectors);
#else
#define FSYS_FFS_NUM 0
#endif

#ifdef FSYS_UFS2
#define FSYS_UFS2_NUM 1
int ufs2_mount (void);
int ufs2_read (char *buf, int len);
int ufs2_dir (char *dirname);
int ufs2_embed (int *start_sector, int needed_sectors);
#else
#define FSYS_UFS2_NUM 0
#endif

#ifdef FSYS_FAT
#define FSYS_FAT_NUM 1
int fat_mount (void);
int fat_read (char *buf, int len);
int fat_dir (char *dirname);
#else
#define FSYS_FAT_NUM 0
#endif

#ifdef FSYS_EXT2FS
#define FSYS_EXT2FS_NUM 1
int ext2fs_mount (void);
int ext2fs_read (char *buf, int len);
int ext2fs_dir (char *dirname);
#else
#define FSYS_EXT2FS_NUM 0
#endif

#ifdef FSYS_MINIX
#define FSYS_MINIX_NUM 1
int minix_mount (void);
int minix_read (char *buf, int len);
int minix_dir (char *dirname);
#else
#define FSYS_MINIX_NUM 0
#endif

#ifdef FSYS_REISERFS
#define FSYS_REISERFS_NUM 1
int reiserfs_mount (void);
int reiserfs_read (char *buf, int len);
int reiserfs_dir (char *dirname);
int reiserfs_embed (int *start_sector, int needed_sectors);
#else
#define FSYS_REISERFS_NUM 0
#endif

#ifdef FSYS_VSTAFS
#define FSYS_VSTAFS_NUM 1
int vstafs_mount (void);
int vstafs_read (char *buf, int len);
int vstafs_dir (char *dirname);
#else
#define FSYS_VSTAFS_NUM 0
#endif

#ifdef FSYS_JFS
#define FSYS_JFS_NUM 1
int jfs_mount (void);
int jfs_read (char *buf, int len);
int jfs_dir (char *dirname);
int jfs_embed (int *start_sector, int needed_sectors);
#else
#define FSYS_JFS_NUM 0
#endif

#ifdef FSYS_XFS
#define FSYS_XFS_NUM 1
int xfs_mount (void);
int xfs_read (char *buf, int len);
int xfs_dir (char *dirname);
#else
#define FSYS_XFS_NUM 0
#endif

#ifdef FSYS_TFTP
#define FSYS_TFTP_NUM 1
int tftp_mount (void);
int tftp_read (char *buf, int len);
int tftp_dir (char *dirname);
void tftp_close (void);
#else
#define FSYS_TFTP_NUM 0
#endif

#ifdef FSYS_ISO9660
#define FSYS_ISO9660_NUM 1
int iso9660_mount (void);
int iso9660_read (char *buf, int len);
int iso9660_dir (char *dirname);
#else
#define FSYS_ISO9660_NUM 0
#endif

#ifndef NUM_FSYS
#define NUM_FSYS	\
  (FSYS_FFS_NUM + FSYS_FAT_NUM + FSYS_EXT2FS_NUM + FSYS_MINIX_NUM	\
   + FSYS_REISERFS_NUM + FSYS_VSTAFS_NUM + FSYS_JFS_NUM + FSYS_XFS_NUM	\
   + FSYS_TFTP_NUM + FSYS_ISO9660_NUM + FSYS_UFS2_NUM)
#endif

/* defines for the block filesystem info area */
#ifndef NO_BLOCK_FILES
#define BLK_CUR_FILEPOS      (*((int*)FSYS_BUF))
#define BLK_CUR_BLKLIST      (*((int*)(FSYS_BUF+4)))
#define BLK_CUR_BLKNUM       (*((int*)(FSYS_BUF+8)))
#define BLK_MAX_ADDR         (FSYS_BUF+0x7FF9)
#define BLK_BLKSTART(l)      (*((int*)l))
#define BLK_BLKLENGTH(l)     (*((int*)(l+4)))
#define BLK_BLKLIST_START    (FSYS_BUF+12)
#define BLK_BLKLIST_INC_VAL  8
#endif /* NO_BLOCK_FILES */

/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月3日
*
* @note 注释详细内容:
* 
* 该结构代表文件系统结构。
*
* @name: 文件系统名字。
* @mount_func: 文件系统加载(初始化)函数指针。
* @read_func: 文件系统读取函数指针。
* @dir_func: 文件系统目录/文件打开函数指针。
* @close_func: 文件系统关闭函数指针。
* @embed_func: 文件系统嵌入Stage1.5的函数指针。
*
* 这些函数指针类似系统调用，在stage2/disk_io.c 中定义了grub_open,grub_close,
* grub_read,grub_dir全局函数用于stage2 的文件操作:打开，关闭，读，切换目录。
*
* 为了简化文件系统驱动的编写，grub 不支持磁盘写（对于一个loader 来说也没有必要
* 去写磁盘）。
*
* 任何一个文件系统驱动必须在fsys_table（stage2/disk_io.c）数组中去放置一个
* struct fsys_entry 的结构体。
*
* 读入文件流程分析说明。假设在grub 的menulist 中有：kernel (hd0,0)/boot/vmlinuz
*（或者我们在grub shell 中执行此命令）。
* 
* stage2 会先调用grub_open("(hd0,0)/boot/vmlinuz") 来打开文件。
*
* 在执行这个函数中，grub 会先在fsys_table 中循环调用fsys_entry::mount_func 去
* 发现一个返回值为真的文件系统，即为当前的文件系统。然后利用当前文件系统驱动的
* fsys_entry::dir_func 去打开/boot/vmlinuz。
*
* 然后stage2 会调用grub_read(buf,0)读入全部的/boot/vmlinuz 文件至内存中的buf
* 地址。grub_read 是fsys_entry::read_func 的封装。
*
* 最后stage2 回调用grub_close 关闭文件，跟前面一样，这个调用仅仅是当前文件系统
* fsys_entry::close_func 的简单封装。
*/
/* this next part is pretty ugly, but it keeps it in one place! */

struct fsys_entry
{
  char *name;
  int (*mount_func) (void);
  int (*read_func) (char *buf, int len);
  int (*dir_func) (char *dirname);
  void (*close_func) (void);
  int (*embed_func) (int *start_sector, int needed_sectors);
};

#ifdef STAGE1_5
# define print_possibilities 0
#else
extern int print_possibilities;
#endif

extern int fsmax;
extern struct fsys_entry fsys_table[NUM_FSYS + 1];
