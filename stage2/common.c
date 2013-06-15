/* common.c - miscellaneous shared variables and routines */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2004  Free Software Foundation, Inc.
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

#include <shared.h>

#ifdef SUPPORT_DISKLESS
# define GRUB	1
# include <etherboot.h>
#endif

/*
 *  Shared BIOS/boot data.
 */

struct multiboot_info mbi;
unsigned long saved_drive;
unsigned long saved_partition;
unsigned long cdrom_drive;
#ifndef STAGE1_5
unsigned long saved_mem_upper;

/* This saves the maximum size of extended memory (in KB).  */
unsigned long extended_memory;
#endif
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容: GRUB 0.97报错信息说明
* 
* GRUB 0.97 Stage 1 处理错误的总体方式是打出一串错误信息并停止。这时按 <CTRL>-<ALT>-<DEL> 
* 可以重启。
*
* 以下是 stage 1 错误信息的完整列表： 
*
* 1. "Hard Disk Error"
*
* 硬盘错误，在从硬盘读取Stage2 或Stage1.5 的时候，未能确定硬盘的容量和结构参数。
*
* 2. "Floppy Error" 
*
* 软盘错误，在从软盘读取Stage2 或Stage1.5 的时候，未能确定软盘的容量和结构参数。
* 这个错误被单独列出是因为软盘的检测过程与硬盘不同。
*
* 3. "Read Error" 
*
* 读取错误，当试图读取Stage 2 或Stage1.5 的时候，发生磁盘读取错误。
*
* 4. "Geom Error" 
*
* 物理错误，Stage2 或Stage1.5的位置不在能被 BIOS 读调用直接支持的磁盘区域。这
* 可以是因为 BIOS 转换过来的结构参数已经被用户改变，或者在安装后磁盘被移动到
* 另一台机器或另一个控制器上，或者 GRUB 不是 GRUB 自己安装的（如果是的话，这
* 个错误的 stage 2 版本应该在安装过程中就已经出现并且不可能完成安装）。 
*
* GRUB 0.97 Stage 2 处理错误的总体方式是终止有问题的操作，打出错误信息，然后
*（如果可能的话）要么在已经出错的事实上继续执行，要么等待用户来处理错误。 
*
* 以下是全部的Stage 2 错误信息号（同样适用于Stage 1.5）:
* 
* 1. "Filename must be either an absolute filename or blocklist" 
* 
*  文件名必须使用绝对路径或者区块列表。文件名不符合GRUB语法规范，详情请参见文件
*  系统说明。
*
* 2. "Bad file or directory type" 
*
*  无效的文件或路径。指定文件不符合规范，比如指定了一个符号链接、目录或者浮点
*  输入输出单元。
*
* 3. "Bad or corrupt data while decompressing file" 
*
*  解压文件时发现已损坏的数据(当一个被认为是压缩文件的文件头损坏时返回此错误)。
*  解压缩时发现内部错误代码，这通常意味着文件已损坏。
*
* 4. "Bad or incompatible header in compressed file"
*
*  损坏或者不兼容的压缩文件头信息。当一个被认为是压缩文件的文件头损坏时返回此错误。
*
* 5. "Partition table invalid or corrupt"
*
*  分区表无效或损坏。当对分区表完整性的总体检查失败时返回此错误。这是个不好的迹象。
*
* 6. "Mismatched or corrupt version of stage1/stage2"
*
*  不匹配或坏损的 stage1/stage2 版本。当安装命令指向不匹配或坏损的 stage1 或 
*  stage2 版本时返回此错误。它大体上并不能检测损坏，但这是对版本号的总体检查，
*  应当要正确才是。
*
* 7. "Loading below 1MB is not supported"
*
*  不支持从低于 1MB （的地址）装载。当一个内核的起始地址低于 1MB 的边界时返回
*  此错误。 Linux zImage 格式的内核是一个例外，能够（在低于1MB地址）处理，因
*  为它有一个固定的装载地址并且最大尺寸有限。
*
* 8. "Kernel must be loaded before booting"
*
*  内核必须在开始引导系统之前被载入。GRUB开始执行引导系统时发现没有内核。
*
* 9. "Unknown boot failure"
*
*  未知引导错误。因未知原因导致引导尝试失败。
*
* 10. "Unsupported Multiboot features requested"
*
*  请求了不支持的多重引导特性。当多重引导头里的多重引导特征字请求了一个无法识别
*  的特性时返回此错误。这个问题的要点在于内核需要某种也许GRUB无法提供的特殊处理。
*
* 11. "Unrecognized device string"
*
*  无法识别的设备。设备标识符不符合预期文件系统的语法规范。详情请参见文件系统说明。
*
* 12. "Invalid device requested"
*
*  指定设备无效。当一个设备标识符可识别但是错误无法归类为其他的设备错误号时返回
*  此错误。
*
* 13. "Invalid or unsupported executable format"
*
*  无效或不支持的可执行文件格式。当装载的内核映像不能被识别为多重启动（映像）或
*  某种被支持的原生格式（如 Linux zImage 或 bzImage, FreeBSD, 或 NetBSD）时返回
*  此错误。
*
* 14. "Filesystem compatibility error, cannot read whole file"
*
*  文件系统兼容性错误，无法完整读取文件。某些文件系统在GRUB中有可读取长度限制，
*  当超出时会出现此提示。
*
* 15. "File not found"
*
*  没有找到文件。其他信息正确（如磁盘、分区），但没有找到指定文件。
*
* 16. "Inconsistent filesystem structure"
*
*  文件系统结构不一致。这个错误由文件系统本身的代码返回，表示对磁盘上文件系统结
*  构的总体检查结果和它所期望的不相符而产生的内部错误。这通常是由损坏的文件系统
*  或 GRUB 里处理它的代码的 bug 所导致。
*
* 17. "Cannot mount selected partition"
*
*  无法挂载分区。指定分区存在，但文件系统无法被GRUB识别。
*
* 18. "Selected cylinder exceeds maximum supported by BIOS"
*
*  所选分区最大柱面数超出。BIOS支持范围所需读取的顺序区块超出BIOS支持区。这个
*  错误通常发生于较早型号的电脑（对于某些(E)IDE硬盘，早期主板BIOS只能读取小于
*  512M的区域并且整个硬盘不能大于8G）。
*
* 19. "Linux kernel must be loaded before initrd"
*
*  Linux内核必须在initrd之前被载入。这个错误通常是因为initrd语句被放到了Linux内
*  核语句之前。
*
* 20. "Multiboot kernel must be loaded before modules"
*
*  多重引导内核必须在模块之前加载。当模块加载命令在多重引导内核加载之前使用时返
*  回此错误。然而（这个错误）只有在这种情况下（内核是 multiboot 内核）才有意义,
*  因为 GRUB 完全不知道如何在这种模块和一个非 multiboot 内核之间通讯。
*
* 21. "Selected disk does not exist"
*
*  所选磁盘不存在。当一个设备或完整文件名的设备字部分指向系统里一个不存在的或
*  无法被 BIOS 识别到的磁盘或 BIOS设备时返回此错误。
* 
* 22. "No such partition"
*
*  没有此分区。 当在一个设备或完整文件名的设备字部分里请求一个指定磁盘上不存在
*  的分区时返回此错误。
*
* 23. "Error while parsing number"
*
*  对数字的语法分析错误。当 GRUB 预期读到一个数字但是碰到非法数据时返回此错误。
*
* 24. "Attempt to access block outside partition"
*
*  所需访问区块超出分区。所需读取的顺序区块超出磁盘分区。通常是因为磁盘文件系统
*  已被破坏，或者GRUB代码行有缺陷（GRUB也同时是一个很棒的调试工具）。
*
* 25. "Disk read error"
*
*  磁盘读取错误。从磁盘分区查询或读取数据时出现磁盘读取错误。
*
* 26. "Too many symbolic links"
*
*  符号链接太多了。链接数超出允许值（目前为5个），有可能是符号链接被循环引用了。
*
* 27. "Unrecognized command"
*
*  无法识别的指令。命令行或者配置文件内选定项目的引导顺序字段中有无法识别的指令。
*
* 28. "Selected item cannot fit into memory"
*
*  所选项目无法载入内存内核、模块或者映像文件之一无法载入内存。有时可能只是因为
*  太大了。
*
* 29. "Disk write error"
*
* 磁盘写入错误。试图写入指定磁盘时出错。这通常是因为激活分区失败所致。
*
* 30. "Invalid argument"
*
*  无效的参数。某一命令行中存在无效参数。
*
* 31. "File is not sector aligned"
*
*  文件所处扇区未对齐。这个错误只会发生在用户直接通过区块表访问ReiserFS分区时
* （例如从命令行安装系统）。如出现这种情况，则应该在分区挂载命令中加入'-o notail'
*  选项。
*
* 32. "Must be authenticated"
*
*  必须通过验证。这个信息出现于用户试图运行一个被保护的项目，此时你应该输入正确
*  的密码才能继续后续操作。
*
* 33. "Serial device not configured"
*
*  无法配置串行设备。如果用户试图在所有串行设备初始化完成之前切换到其中之一时会
*  发生此错误。
*
*  34. "No spare sectors on the disk"
*
*  磁盘中无多余扇区。磁盘空间不足会出现此错误。原因是但用户试图将Stage 1.5写入
*  到MBR后的未使用扇区时，但是第一个分区紧跟在MBR之后，或者这些扇区被EZ-BIOS所
*  占用。
*/
/*
 *  Error code stuff.
 */

grub_error_t errnum = ERR_NONE;

#ifndef STAGE1_5

char *err_list[] =
{
  [ERR_NONE] = 0,
  [ERR_BAD_ARGUMENT] = "Invalid argument",
  [ERR_BAD_FILENAME] =
  "Filename must be either an absolute pathname or blocklist",
  [ERR_BAD_FILETYPE] = "Bad file or directory type",
  [ERR_BAD_GZIP_DATA] = "Bad or corrupt data while decompressing file",
  [ERR_BAD_GZIP_HEADER] = "Bad or incompatible header in compressed file",
  [ERR_BAD_PART_TABLE] = "Partition table invalid or corrupt",
  [ERR_BAD_VERSION] = "Mismatched or corrupt version of stage1/stage2",
  [ERR_BELOW_1MB] = "Loading below 1MB is not supported",
  [ERR_BOOT_COMMAND] = "Kernel must be loaded before booting",
  [ERR_BOOT_FAILURE] = "Unknown boot failure",
  [ERR_BOOT_FEATURES] = "Unsupported Multiboot features requested",
  [ERR_DEV_FORMAT] = "Unrecognized device string",
  [ERR_DEV_NEED_INIT] = "Device not initialized yet",
  [ERR_DEV_VALUES] = "Invalid device requested",
  [ERR_EXEC_FORMAT] = "Invalid or unsupported executable format",
  [ERR_FILELENGTH] =
  "Filesystem compatibility error, cannot read whole file",
  [ERR_FILE_NOT_FOUND] = "File not found",
  [ERR_FSYS_CORRUPT] = "Inconsistent filesystem structure",
  [ERR_FSYS_MOUNT] = "Cannot mount selected partition",
  [ERR_GEOM] = "Selected cylinder exceeds maximum supported by BIOS",
  [ERR_NEED_LX_KERNEL] = "Linux kernel must be loaded before initrd",
  [ERR_NEED_MB_KERNEL] = "Multiboot kernel must be loaded before modules",
  [ERR_NO_DISK] = "Selected disk does not exist",
  [ERR_NO_DISK_SPACE] = "No spare sectors on the disk",
  [ERR_NO_PART] = "No such partition",
  [ERR_NUMBER_OVERFLOW] = "Overflow while parsing number",
  [ERR_NUMBER_PARSING] = "Error while parsing number",
  [ERR_OUTSIDE_PART] = "Attempt to access block outside partition",
  [ERR_PRIVILEGED] = "Must be authenticated",
  [ERR_READ] = "Disk read error",
  [ERR_SYMLINK_LOOP] = "Too many symbolic links",
  [ERR_UNALIGNED] = "File is not sector aligned",
  [ERR_UNRECOGNIZED] = "Unrecognized command",
  [ERR_WONT_FIT] = "Selected item cannot fit into memory",
  [ERR_WRITE] = "Disk write error",
};


/* static for BIOS memory map fakery */
static struct AddrRangeDesc fakemap[3] =
{
  {20, 0, 0, MB_ARD_MEMORY},
  {20, 0x100000, 0, MB_ARD_MEMORY},
  {20, 0x1000000, 0, MB_ARD_MEMORY}
};
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月10日
*
* @note 注释详细内容:
* 
* 本函数实现查找从参数bottom开始到最高可用内存的大小的功能。这是通过扫描一个由
* mbi.mmap_addr和mbi.mmap_length描述的内存映射buffer中每一项，找到这些内存映射
* 描述中描述的最高可用内存地址来实现的。返回从bottom开始到最高可用内存的大小。
* 由于GRUB 0.97只支持32位地址，因此最高地址不超过4GB。注意mbi.mmap_addr和
* mbi.mmap_length描述的内存映射buffer中每一项的结构如下:
*
*         +-------------------+
* -4      | size              |
*         +-------------------+
* 0       | base_addr         |
* 8       | length            |
* 16      | type              |
*         +-------------------+
*
* 其中的size是只后面描述结构的大小，因此这也是代码中要用代码:
*
* "addr += *((unsigned long *) addr) + 4"
* 
* 来实现到下一个描述项的原因。
*/
/* A big problem is that the memory areas aren't guaranteed to be:
   (1) contiguous, (2) sorted in ascending order, or (3) non-overlapping.
   Thus this kludge.  */
static unsigned long
mmap_avail_at (unsigned long bottom)
{
  unsigned long long top;
  unsigned long addr;
  int cont;
  
  top = bottom;
  do
    {
      for (cont = 0, addr = mbi.mmap_addr;
	   addr < mbi.mmap_addr + mbi.mmap_length;
	   addr += *((unsigned long *) addr) + 4)
	{
	  struct AddrRangeDesc *desc = (struct AddrRangeDesc *) addr;
	  
	  if (desc->Type == MB_ARD_MEMORY
	      && desc->BaseAddr <= top
	      && desc->BaseAddr + desc->Length > top)
	    {
	      top = desc->BaseAddr + desc->Length;
	      cont++;
	    }
	}
    }
  while (cont);

  /* For now, GRUB assumes 32bits addresses, so...  */
  if (top > 0xFFFFFFFF)
    top = 0xFFFFFFFF;
  
  return (unsigned long) top - bottom;
}
#endif /* ! STAGE1_5 */
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月10日
*
* @note 注释详细内容:
* 
* 本函数实现初始化BIOS传递的参数的功能。这里主要初始化了Multiboot要求的如下参数:
* mbi.mem_lower,mbi.mem_upper,mbi.mmap_addr,mbi.mmap_length,mbi.drives_addr,以及
* mbi.drives_length等信息。接着调用cmain()进入Stage 2的主循环。
*/
/* This queries for BIOS information.  */
void
init_bios_info (void)
{
#ifndef STAGE1_5
  unsigned long cont, memtmp, addr;
  int drive;
#endif

  /*
   *  Get information from BIOS on installed RAM.
   */

  mbi.mem_lower = get_memsize (0);
  mbi.mem_upper = get_memsize (1);

#ifndef STAGE1_5
  /*
   *  We need to call this somewhere before trying to put data
   *  above 1 MB, since without calling it, address line 20 will be wired
   *  to 0.  Not too desirable.
   */

  gateA20 (1);

  /* Store the size of extended memory in EXTENDED_MEMORY, in order to
     tell it to non-Multiboot OSes.  */
  extended_memory = mbi.mem_upper;
  
  /*
   *  The "mbi.mem_upper" variable only recognizes upper memory in the
   *  first memory region.  If there are multiple memory regions,
   *  the rest are reported to a Multiboot-compliant OS, but otherwise
   *  unused by GRUB.
   */

  addr = get_code_end ();
  mbi.mmap_addr = addr;
  mbi.mmap_length = 0;
  cont = 0;

  do
    {
      cont = get_mmap_entry ((void *) addr, cont);

      /* If the returned buffer's length is zero, quit. */
      if (! *((unsigned long *) addr))
	break;

      mbi.mmap_length += *((unsigned long *) addr) + 4;
      addr += *((unsigned long *) addr) + 4;
    }
  while (cont);

  if (mbi.mmap_length)
    {
      unsigned long long max_addr;
      
      /*
       *  This is to get the lower memory, and upper memory (up to the
       *  first memory hole), into the "mbi.mem_{lower,upper}"
       *  elements.  This is for OS's that don't care about the memory
       *  map, but might care about total RAM available.
       */
      mbi.mem_lower = mmap_avail_at (0) >> 10;
      mbi.mem_upper = mmap_avail_at (0x100000) >> 10;

      /* Find the maximum available address. Ignore any memory holes.  */
      for (max_addr = 0, addr = mbi.mmap_addr;
	   addr < mbi.mmap_addr + mbi.mmap_length;
	   addr += *((unsigned long *) addr) + 4)
	{
	  struct AddrRangeDesc *desc = (struct AddrRangeDesc *) addr;
	  
	  if (desc->Type == MB_ARD_MEMORY && desc->Length > 0
	      && desc->BaseAddr + desc->Length > max_addr)
	    max_addr = desc->BaseAddr + desc->Length;
	}

      extended_memory = (max_addr - 0x100000) >> 10;
    }
  else if ((memtmp = get_eisamemsize ()) != -1)
    {
      cont = memtmp & ~0xFFFF;
      memtmp = memtmp & 0xFFFF;

      if (cont != 0)
	extended_memory = (cont >> 10) + 0x3c00;
      else
	extended_memory = memtmp;
      
      if (!cont || (memtmp == 0x3c00))
	memtmp += (cont >> 10);
      else
	{
	  /* XXX should I do this at all ??? */

	  mbi.mmap_addr = (unsigned long) fakemap;
	  mbi.mmap_length = sizeof (fakemap);
	  fakemap[0].Length = (mbi.mem_lower << 10);
	  fakemap[1].Length = (memtmp << 10);
	  fakemap[2].Length = cont;
	}

      mbi.mem_upper = memtmp;
    }

  saved_mem_upper = mbi.mem_upper;

  /* Get the drive info.  */
  /* FIXME: This should be postponed until a Multiboot kernel actually
     requires it, because this could slow down the start-up
     unreasonably.  */
  mbi.drives_length = 0;
  mbi.drives_addr = addr;

  /* For now, GRUB doesn't probe floppies, since it is trivial to map
     floppy drives to BIOS drives.  */
  for (drive = 0x80; drive < 0x88; drive++)
    {
      struct geometry geom;
      struct drive_info *info = (struct drive_info *) addr;
      unsigned short *port;
      
      /* Get the geometry. This ensures that the drive is present.  */
      if (get_diskinfo (drive, &geom))
	break;
      
      /* Clean out the I/O map.  */
      grub_memset ((char *) io_map, 0,
		   IO_MAP_SIZE * sizeof (unsigned short));

      /* Disable to probe I/O ports temporarily, because this doesn't
	 work with some BIOSes (maybe they are too buggy).  */
#if 0
      /* Track the int13 handler.  */
      track_int13 (drive);
#endif

      /* Set the information.  */
      info->drive_number = drive;
      info->drive_mode = ((geom.flags & BIOSDISK_FLAG_LBA_EXTENSION)
			  ? MB_DI_LBA_MODE : MB_DI_CHS_MODE);
      info->drive_cylinders = geom.cylinders;
      info->drive_heads = geom.heads;
      info->drive_sectors = geom.sectors;

      addr += sizeof (struct drive_info);
      for (port = io_map; *port; port++, addr += sizeof (unsigned short))
	*((unsigned short *) addr) = *port;

      info->size = addr - (unsigned long) info;
      mbi.drives_length += info->size;
    }

  /* Get the ROM configuration table by INT 15, AH=C0h.  */
  mbi.config_table = get_rom_config_table ();

  /* Set the boot loader name.  */
  mbi.boot_loader_name = (unsigned long) "GNU GRUB " VERSION;

  /* Get the APM BIOS table.  */
  get_apm_info ();
  if (apm_bios_info.version)
    mbi.apm_table = (unsigned long) &apm_bios_info;
  
  /*
   *  Initialize other Multiboot Info flags.
   */

  mbi.flags = (MB_INFO_MEMORY | MB_INFO_CMDLINE | MB_INFO_BOOTDEV
	       | MB_INFO_DRIVE_INFO | MB_INFO_CONFIG_TABLE
	       | MB_INFO_BOOT_LOADER_NAME);
  
  if (apm_bios_info.version)
    mbi.flags |= MB_INFO_APM_TABLE;

#endif /* STAGE1_5 */

  /* Set boot drive and partition.  */
  saved_drive = boot_drive;
  saved_partition = install_partition;

  /* Set cdrom drive.  */
  {
    struct geometry geom;
    
    /* Get the geometry.  */
    if (get_diskinfo (boot_drive, &geom)
	|| ! (geom.flags & BIOSDISK_FLAG_CDROM))
      cdrom_drive = GRUB_INVALID_DRIVE;
    else
      cdrom_drive = boot_drive;
  }
  
  /* Start main routine here.  */
  cmain ();
}
