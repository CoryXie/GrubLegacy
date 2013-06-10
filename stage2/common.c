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
* @topic ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @group ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @details ע����ϸ����: GRUB 0.97������Ϣ˵��
* 
* GRUB 0.97 Stage 1 �����������巽ʽ�Ǵ��һ��������Ϣ��ֹͣ����ʱ�� <CTRL>-<ALT>-<DEL> 
* ����������
*
* ������ stage 1 ������Ϣ�������б� 
*
* 1. "Hard Disk Error"
*
* Ӳ�̴����ڴ�Ӳ�̶�ȡStage2 ��Stage1.5 ��ʱ��δ��ȷ��Ӳ�̵������ͽṹ������
*
* 2. "Floppy Error" 
*
* ���̴����ڴ����̶�ȡStage2 ��Stage1.5 ��ʱ��δ��ȷ�����̵������ͽṹ������
* ������󱻵����г�����Ϊ���̵ļ�������Ӳ�̲�ͬ��
*
* 3. "Read Error" 
*
* ��ȡ���󣬵���ͼ��ȡStage 2 ��Stage1.5 ��ʱ�򣬷������̶�ȡ����
*
* 4. "Geom Error" 
*
* �������Stage2 ��Stage1.5��λ�ò����ܱ� BIOS ������ֱ��֧�ֵĴ���������
* ��������Ϊ BIOS ת�������Ľṹ�����Ѿ����û��ı䣬�����ڰ�װ����̱��ƶ���
* ��һ̨��������һ���������ϣ����� GRUB ���� GRUB �Լ���װ�ģ�����ǵĻ�����
* ������� stage 2 �汾Ӧ���ڰ�װ�����о��Ѿ����ֲ��Ҳ�������ɰ�װ���� 
*
* GRUB 0.97 Stage 2 �����������巽ʽ����ֹ������Ĳ��������������Ϣ��Ȼ��
*��������ܵĻ���Ҫô���Ѿ��������ʵ�ϼ���ִ�У�Ҫô�ȴ��û���������� 
*
* ������ȫ����Stage 2 ������Ϣ�ţ�ͬ��������Stage 1.5��:
* 
* 1. "Filename must be either an absolute filename or blocklist" 
* 
*  �ļ�������ʹ�þ���·�����������б��ļ���������GRUB�﷨�淶��������μ��ļ�
*  ϵͳ˵����
*
* 2. "Bad file or directory type" 
*
*  ��Ч���ļ���·����ָ���ļ������Ϲ淶������ָ����һ���������ӡ�Ŀ¼���߸���
*  ���������Ԫ��
*
* 3. "Bad or corrupt data while decompressing file" 
*
*  ��ѹ�ļ�ʱ�������𻵵�����(��һ������Ϊ��ѹ���ļ����ļ�ͷ��ʱ���ش˴���)��
*  ��ѹ��ʱ�����ڲ�������룬��ͨ����ζ���ļ����𻵡�
*
* 4. "Bad or incompatible header in compressed file"
*
*  �𻵻��߲����ݵ�ѹ���ļ�ͷ��Ϣ����һ������Ϊ��ѹ���ļ����ļ�ͷ��ʱ���ش˴���
*
* 5. "Partition table invalid or corrupt"
*
*  ��������Ч���𻵡����Է����������Ե�������ʧ��ʱ���ش˴������Ǹ����õļ���
*
* 6. "Mismatched or corrupt version of stage1/stage2"
*
*  ��ƥ������ stage1/stage2 �汾������װ����ָ��ƥ������ stage1 �� 
*  stage2 �汾ʱ���ش˴����������ϲ����ܼ���𻵣������Ƕ԰汾�ŵ������飬
*  Ӧ��Ҫ��ȷ���ǡ�
*
* 7. "Loading below 1MB is not supported"
*
*  ��֧�ִӵ��� 1MB ���ĵ�ַ��װ�ء���һ���ں˵���ʼ��ַ���� 1MB �ı߽�ʱ����
*  �˴��� Linux zImage ��ʽ���ں���һ�����⣬�ܹ����ڵ���1MB��ַ��������
*  Ϊ����һ���̶���װ�ص�ַ�������ߴ����ޡ�
*
* 8. "Kernel must be loaded before booting"
*
*  �ں˱����ڿ�ʼ����ϵͳ֮ǰ�����롣GRUB��ʼִ������ϵͳʱ����û���ںˡ�
*
* 9. "Unknown boot failure"
*
*  δ֪����������δ֪ԭ������������ʧ�ܡ�
*
* 10. "Unsupported Multiboot features requested"
*
*  �����˲�֧�ֵĶ����������ԡ�����������ͷ��Ķ�������������������һ���޷�ʶ��
*  ������ʱ���ش˴�����������Ҫ�������ں���Ҫĳ��Ҳ��GRUB�޷��ṩ�����⴦��
*
* 11. "Unrecognized device string"
*
*  �޷�ʶ����豸���豸��ʶ��������Ԥ���ļ�ϵͳ���﷨�淶��������μ��ļ�ϵͳ˵����
*
* 12. "Invalid device requested"
*
*  ָ���豸��Ч����һ���豸��ʶ����ʶ���Ǵ����޷�����Ϊ�������豸�����ʱ����
*  �˴���
*
* 13. "Invalid or unsupported executable format"
*
*  ��Ч��֧�ֵĿ�ִ���ļ���ʽ����װ�ص��ں�ӳ���ܱ�ʶ��Ϊ����������ӳ�񣩻�
*  ĳ�ֱ�֧�ֵ�ԭ����ʽ���� Linux zImage �� bzImage, FreeBSD, �� NetBSD��ʱ����
*  �˴���
*
* 14. "Filesystem compatibility error, cannot read whole file"
*
*  �ļ�ϵͳ�����Դ����޷�������ȡ�ļ���ĳЩ�ļ�ϵͳ��GRUB���пɶ�ȡ�������ƣ�
*  ������ʱ����ִ���ʾ��
*
* 15. "File not found"
*
*  û���ҵ��ļ���������Ϣ��ȷ������̡�����������û���ҵ�ָ���ļ���
*
* 16. "Inconsistent filesystem structure"
*
*  �ļ�ϵͳ�ṹ��һ�¡�����������ļ�ϵͳ����Ĵ��뷵�أ���ʾ�Դ������ļ�ϵͳ��
*  �����������������������Ĳ�������������ڲ�������ͨ�������𻵵��ļ�ϵͳ
*  �� GRUB �ﴦ�����Ĵ���� bug �����¡�
*
* 17. "Cannot mount selected partition"
*
*  �޷����ط�����ָ���������ڣ����ļ�ϵͳ�޷���GRUBʶ��
*
* 18. "Selected cylinder exceeds maximum supported by BIOS"
*
*  ��ѡ�������������������BIOS֧�ַ�Χ�����ȡ��˳�����鳬��BIOS֧���������
*  ����ͨ�������ڽ����ͺŵĵ��ԣ�����ĳЩ(E)IDEӲ�̣���������BIOSֻ�ܶ�ȡС��
*  512M������������Ӳ�̲��ܴ���8G����
*
* 19. "Linux kernel must be loaded before initrd"
*
*  Linux�ں˱�����initrd֮ǰ�����롣�������ͨ������Ϊinitrd��䱻�ŵ���Linux��
*  �����֮ǰ��
*
* 20. "Multiboot kernel must be loaded before modules"
*
*  ���������ں˱�����ģ��֮ǰ���ء���ģ����������ڶ��������ں˼���֮ǰʹ��ʱ��
*  �ش˴���Ȼ�����������ֻ������������£��ں��� multiboot �ںˣ���������,
*  ��Ϊ GRUB ��ȫ��֪�����������ģ���һ���� multiboot �ں�֮��ͨѶ��
*
* 21. "Selected disk does not exist"
*
*  ��ѡ���̲����ڡ���һ���豸�������ļ������豸�ֲ���ָ��ϵͳ��һ�������ڵĻ�
*  �޷��� BIOS ʶ�𵽵Ĵ��̻� BIOS�豸ʱ���ش˴���
* 
* 22. "No such partition"
*
*  û�д˷����� ����һ���豸�������ļ������豸�ֲ���������һ��ָ�������ϲ�����
*  �ķ���ʱ���ش˴���
*
* 23. "Error while parsing number"
*
*  �����ֵ��﷨�������󡣵� GRUB Ԥ�ڶ���һ�����ֵ��������Ƿ�����ʱ���ش˴���
*
* 24. "Attempt to access block outside partition"
*
*  ����������鳬�������������ȡ��˳�����鳬�����̷�����ͨ������Ϊ�����ļ�ϵͳ
*  �ѱ��ƻ�������GRUB��������ȱ�ݣ�GRUBҲͬʱ��һ���ܰ��ĵ��Թ��ߣ���
*
* 25. "Disk read error"
*
*  ���̶�ȡ���󡣴Ӵ��̷�����ѯ���ȡ����ʱ���ִ��̶�ȡ����
*
* 26. "Too many symbolic links"
*
*  ��������̫���ˡ���������������ֵ��ĿǰΪ5�������п����Ƿ������ӱ�ѭ�������ˡ�
*
* 27. "Unrecognized command"
*
*  �޷�ʶ���ָ������л��������ļ���ѡ����Ŀ������˳���ֶ������޷�ʶ���ָ�
*
* 28. "Selected item cannot fit into memory"
*
*  ��ѡ��Ŀ�޷������ڴ��ںˡ�ģ�����ӳ���ļ�֮һ�޷������ڴ档��ʱ����ֻ����Ϊ
*  ̫���ˡ�
*
* 29. "Disk write error"
*
* ����д�������ͼд��ָ������ʱ������ͨ������Ϊ�������ʧ�����¡�
*
* 30. "Invalid argument"
*
*  ��Ч�Ĳ�����ĳһ�������д�����Ч������
*
* 31. "File is not sector aligned"
*
*  �ļ���������δ���롣�������ֻ�ᷢ�����û�ֱ��ͨ����������ReiserFS����ʱ
* ������������а�װϵͳ��������������������Ӧ���ڷ������������м���'-o notail'
*  ѡ�
*
* 32. "Must be authenticated"
*
*  ����ͨ����֤�������Ϣ�������û���ͼ����һ������������Ŀ����ʱ��Ӧ��������ȷ
*  ��������ܼ�������������
*
* 33. "Serial device not configured"
*
*  �޷����ô����豸������û���ͼ�����д����豸��ʼ�����֮ǰ�л�������֮һʱ��
*  �����˴���
*
*  34. "No spare sectors on the disk"
*
*  �������޶������������̿ռ䲻�����ִ˴���ԭ���ǵ��û���ͼ��Stage 1.5д��
*  ��MBR���δʹ������ʱ�����ǵ�һ������������MBR֮�󣬻�����Щ������EZ-BIOS��
*  ռ�á�
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
* @topic ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @group ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��10��
*
* @details ע����ϸ����:
* 
* ������ʵ�ֲ��ҴӲ���bottom��ʼ����߿����ڴ�Ĵ�С�Ĺ��ܡ�����ͨ��ɨ��һ����
* mbi.mmap_addr��mbi.mmap_length�������ڴ�ӳ��buffer��ÿһ��ҵ���Щ�ڴ�ӳ��
* ��������������߿����ڴ��ַ��ʵ�ֵġ����ش�bottom��ʼ����߿����ڴ�Ĵ�С��
* ����GRUB 0.97ֻ֧��32λ��ַ�������ߵ�ַ������4GB��ע��mbi.mmap_addr��
* mbi.mmap_length�������ڴ�ӳ��buffer��ÿһ��Ľṹ����:
*
*         +-------------------+
* -4      | size              |
*         +-------------------+
* 0       | base_addr         |
* 8       | length            |
* 16      | type              |
*         +-------------------+
*
* ���е�size��ֻ���������ṹ�Ĵ�С�������Ҳ�Ǵ�����Ҫ�ô���:
*
* "addr += *((unsigned long *) addr) + 4"
* 
* ��ʵ�ֵ���һ���������ԭ��
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
* @topic ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @group ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��10��
*
* @details ע����ϸ����:
* 
* ������ʵ�ֳ�ʼ��BIOS���ݵĲ����Ĺ��ܡ�������Ҫ��ʼ����MultibootҪ������²���:
* mbi.mem_lower,mbi.mem_upper,mbi.mmap_addr,mbi.mmap_length,mbi.drives_addr,�Լ�
* mbi.drives_length����Ϣ�����ŵ���cmain()����Stage 2����ѭ����
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
