/* bios.c - implement C part of low-level BIOS disk input and output */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2003,2004  Free Software Foundation, Inc.
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

#include "shared.h"


/* These are defined in asm.S, and never be used elsewhere, so declare the
   prototypes here.  */
extern int biosdisk_int13_extensions (int ax, int drive, void *dap);
extern int biosdisk_standard (int ah, int drive,
			      int coff, int hoff, int soff,
			      int nsec, int segment);
extern int check_int13_extensions (int drive);
extern int get_diskinfo_standard (int drive,
				  unsigned long *cylinders,
				  unsigned long *heads,
				  unsigned long *sectors);
#if 0
extern int get_diskinfo_floppy (int drive,
				unsigned long *cylinders,
				unsigned long *heads,
				unsigned long *sectors);
#endif

/**
* @topic 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @group 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @details 注释详细内容:
* 
* 本函数实现磁盘读取/写入功能。如果参数read为BIOSDISK_READ则为读取，如果参数read
* 为BIOSDISK_WRITE则为写入；参数drive指定所要操作的磁盘；参数geometry封装了磁盘
* 的类型信息，根据此信息，如果磁盘支持LBA模式扩展(BIOSDISK_FLAG_LBA_EXTENSION)，
* 则通过BIOS int13调用，使用disk_address_packet结构传递操作信息，调用函数
* biosdisk_int13_extensions()实现实际的操作，否则就按照CHS模式操作，使用函数
* biosdisk_standard()实现实际的操作；如果LBA模式读写失败，则会退回到CHS模式重试；
* 参数sector指定读写的起始扇区；参数nsec指定要读写的扇区数；参数segment指定读写
* 操作的目的和源地址。函数失败时返回错误号，成功时返回0。
*/
/* Read/write NSEC sectors starting from SECTOR in DRIVE disk with GEOMETRY
   from/into SEGMENT segment. If READ is BIOSDISK_READ, then read it,
   else if READ is BIOSDISK_WRITE, then write it. If an geometry error
   occurs, return BIOSDISK_ERROR_GEOMETRY, and if other error occurs, then
   return the error number. Otherwise, return 0.  */
int
biosdisk (int read, int drive, struct geometry *geometry,
	  int sector, int nsec, int segment)
{
  int err;
  
  if (geometry->flags & BIOSDISK_FLAG_LBA_EXTENSION)
    {
      struct disk_address_packet
      {
	unsigned char length;
	unsigned char reserved;
	unsigned short blocks;
	unsigned long buffer;
	unsigned long long block;
      } __attribute__ ((packed)) dap;

      /* XXX: Don't check the geometry by default, because some buggy
	 BIOSes don't return the number of total sectors correctly,
	 even if they have working LBA support. Hell.  */
#ifdef NO_BUGGY_BIOS_IN_THE_WORLD
      if (sector >= geometry->total_sectors)
	return BIOSDISK_ERROR_GEOMETRY;
#endif /* NO_BUGGY_BIOS_IN_THE_WORLD */

      /* FIXME: sizeof (DAP) must be 0x10. Should assert that the compiler
	 can't add any padding.  */
      dap.length = sizeof (dap);
      dap.block = sector;
      dap.blocks = nsec;
      dap.reserved = 0;
      /* This is undocumented part. The address is formated in
	 SEGMENT:ADDRESS.  */
      dap.buffer = segment << 16;
      
      err = biosdisk_int13_extensions ((read + 0x42) << 8, drive, &dap);

/* #undef NO_INT13_FALLBACK */
#ifndef NO_INT13_FALLBACK
      if (err)
	{
	  if (geometry->flags & BIOSDISK_FLAG_CDROM)
	    return err;
	  
	  geometry->flags &= ~BIOSDISK_FLAG_LBA_EXTENSION;
	  geometry->total_sectors = (geometry->cylinders
				     * geometry->heads
				     * geometry->sectors);
	  return biosdisk (read, drive, geometry, sector, nsec, segment);
	}
#endif /* ! NO_INT13_FALLBACK */
      
    }
  else
    {
      int cylinder_offset, head_offset, sector_offset;
      int head;

      /* SECTOR_OFFSET is counted from one, while HEAD_OFFSET and
	 CYLINDER_OFFSET are counted from zero.  */
      sector_offset = sector % geometry->sectors + 1;
      head = sector / geometry->sectors;
      head_offset = head % geometry->heads;
      cylinder_offset = head / geometry->heads;
      
      if (cylinder_offset >= geometry->cylinders)
	return BIOSDISK_ERROR_GEOMETRY;

      err = biosdisk_standard (read + 0x02, drive,
			       cylinder_offset, head_offset, sector_offset,
			       nsec, segment);
    }

  return err;
}
/**
* @topic 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @group 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @details 注释详细内容:
* 
* 该函数实现检查CD-ROM emulation状态的功能。参数drive为要检查的磁盘号；参数
* geometry为检查后填写的磁盘参数。通过biosdisk_int13_extensions()调用0x4B01获取
* iso_spec_packet的media_type来查知其模拟的磁盘类型(非模拟的可启动CD-ROM或者模拟
* 的软盘或者硬盘)；根据类型，填写geometry参数中的cylinders，heads，sectors，
* sector_size以及total_sectors等字段。返回1表示磁盘为非模拟的可启动CD-ROM，返回
* -1表示磁盘为模拟的软盘或者硬盘；返回0表示biosdisk_int13_extensions()执行失败。
*/
/* Check bootable CD-ROM emulation status.  */
static int
get_cdinfo (int drive, struct geometry *geometry)
{
  int err;
  struct iso_spec_packet
  {
    unsigned char size;
    unsigned char media_type;
    unsigned char drive_no;
    unsigned char controller_no;
    unsigned long image_lba;
    unsigned short device_spec;
    unsigned short cache_seg;
    unsigned short load_seg;
    unsigned short length_sec512;
    unsigned char cylinders;
    unsigned char sectors;
    unsigned char heads;
    
    unsigned char dummy[16];
  } __attribute__ ((packed)) cdrp;
  
  grub_memset (&cdrp, 0, sizeof (cdrp));
  cdrp.size = sizeof (cdrp) - sizeof (cdrp.dummy);
  err = biosdisk_int13_extensions (0x4B01, drive, &cdrp);
  if (! err && cdrp.drive_no == drive)
    {
      if ((cdrp.media_type & 0x0F) == 0)
        {
          /* No emulation bootable CD-ROM */
          geometry->flags = BIOSDISK_FLAG_LBA_EXTENSION | BIOSDISK_FLAG_CDROM;
          geometry->cylinders = 0;
          geometry->heads = 1;
          geometry->sectors = 15;
          geometry->sector_size = 2048;
          geometry->total_sectors = MAXINT;
          return 1;
        }
      else
        {
	  /* Floppy or hard-disk emulation */
          geometry->cylinders
	    = ((unsigned int) cdrp.cylinders
	       + (((unsigned int) (cdrp.sectors & 0xC0)) << 2));
          geometry->heads = cdrp.heads;
          geometry->sectors = cdrp.sectors & 0x3F;
          geometry->sector_size = SECTOR_SIZE;
          geometry->total_sectors = (geometry->cylinders
				     * geometry->heads
				     * geometry->sectors);
          return -1;
        }
    }
  return 0;
}
/**
* @topic 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @group 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @details 注释详细内容:
* 
* 该函数实现检查磁盘信息的功能。参数drive为要检查的磁盘号；参数geometry为检查后
* 填写的磁盘参数。
*
* 如果drive的bit7置位(0x80)，则磁盘为硬盘或者CD-ROM；对这类型的磁盘，调用函数
* check_int13_extensions(),如果返回的version非0，或者drive >= 0x88，则为CD-ROM，
* 因此调用get_cdinfo()来获取磁盘信息；否则，通过biosdisk_int13_extensions()的
* 0x4800功能来获取drive_parameters；如果失败，则继续尝试get_diskinfo_standard()
* 来获得磁盘信息。
*
* 如果drive的bit7没有置位，则磁盘为软盘；对这类磁盘，调用get_diskinfo_standard()
* 来获得磁盘信息。
*
* 根据类型，填写geometry参数中的cylinders，heads，sectors，sector_size以及
* total_sectors等字段。
*
* 失败时返回错误号，否则返回0。
*/
/* Return the geometry of DRIVE in GEOMETRY. If an error occurs, return
   non-zero, otherwise zero.  */
int
get_diskinfo (int drive, struct geometry *geometry)
{
  int err;

  /* Clear the flags.  */
  geometry->flags = 0;
  
  if (drive & 0x80)
    {
      /* hard disk or CD-ROM */
      int version;
      unsigned long total_sectors = 0;
      
      version = check_int13_extensions (drive);

      if (drive >= 0x88 || version)
	{
	  /* Possible CD-ROM - check the status.  */
	  if (get_cdinfo (drive, geometry))
	    return 0;
	}
      
      if (version)
	{
	  struct drive_parameters
	  {
	    unsigned short size;
	    unsigned short flags;
	    unsigned long cylinders;
	    unsigned long heads;
	    unsigned long sectors;
	    unsigned long long total_sectors;
	    unsigned short bytes_per_sector;
	    /* ver 2.0 or higher */
	    unsigned long EDD_configuration_parameters;
	    /* ver 3.0 or higher */
	    unsigned short signature_dpi;
	    unsigned char length_dpi;
	    unsigned char reserved[3];
	    unsigned char name_of_host_bus[4];
	    unsigned char name_of_interface_type[8];
	    unsigned char interface_path[8];
	    unsigned char device_path[8];
	    unsigned char reserved2;
	    unsigned char checksum;

	    /* XXX: This is necessary, because the BIOS of Thinkpad X20
	       writes a garbage to the tail of drive parameters,
	       regardless of a size specified in a caller.  */
	    unsigned char dummy[16];
	  } __attribute__ ((packed)) drp;

	  /* It is safe to clear out DRP.  */
	  grub_memset (&drp, 0, sizeof (drp));
	  
	  /* PhoenixBIOS 4.0 Revision 6.0 for ZF Micro might understand 
	     the greater buffer size for the "get drive parameters" int 
	     0x13 call in its own way.  Supposedly the BIOS assumes even 
	     bigger space is available and thus corrupts the stack.  
	     This is why we specify the exactly necessary size of 0x42 
	     bytes. */
	  drp.size = sizeof (drp) - sizeof (drp.dummy);
	  
	  err = biosdisk_int13_extensions (0x4800, drive, &drp);
	  if (! err)
	    {
	      /* Set the LBA flag.  */
	      geometry->flags = BIOSDISK_FLAG_LBA_EXTENSION;

	      /* I'm not sure if GRUB should check the bit 1 of DRP.FLAGS,
		 so I omit the check for now. - okuji  */
	      /* if (drp.flags & (1 << 1)) */
	       
	      /* FIXME: when the 2TB limit becomes critical, we must
		 change the type of TOTAL_SECTORS to unsigned long
		 long.  */
	      if (drp.total_sectors)
		total_sectors = drp.total_sectors & ~0L;
	      else
		/* Some buggy BIOSes doesn't return the total sectors
		   correctly but returns zero. So if it is zero, compute
		   it by C/H/S returned by the LBA BIOS call.  */
		total_sectors = drp.cylinders * drp.heads * drp.sectors;
	    }
	}

      /* Don't pass GEOMETRY directly, but pass each element instead,
	 so that we can change the structure easily.  */
      err = get_diskinfo_standard (drive,
				   &geometry->cylinders,
				   &geometry->heads,
				   &geometry->sectors);
      if (err)
	return err;

      if (! total_sectors)
	{
	  total_sectors = (geometry->cylinders
			   * geometry->heads
			   * geometry->sectors);
	}
      geometry->total_sectors = total_sectors;
      geometry->sector_size = SECTOR_SIZE;
    }
  else
    {
      /* floppy disk */

      /* First, try INT 13 AH=8h call.  */
      err = get_diskinfo_standard (drive,
				   &geometry->cylinders,
				   &geometry->heads,
				   &geometry->sectors);

#if 0
      /* If fails, then try floppy-specific probe routine.  */
      if (err)
	err = get_diskinfo_floppy (drive,
				   &geometry->cylinders,
				   &geometry->heads,
				   &geometry->sectors);
#endif
      
      if (err)
	return err;

      geometry->total_sectors = (geometry->cylinders
				 * geometry->heads
				 * geometry->sectors);
      geometry->sector_size = SECTOR_SIZE;
    }

  return 0;
}
