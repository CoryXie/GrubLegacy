/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2001,2002  Free Software Foundation, Inc.
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

/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����: GRUB��stage�ļ�
* 
* 1. stage�ļ�
*
* GRUB 0.97 ���м���images�ļ�������������ļ�stage1�� stage2����ѡ��Ϊstage1_5
* �����������������ļ�nxgrub��pxegrub��stage1 ����������GURB��һ�������ӳ���ļ���
* ͨ�����Ǳ�Ƕ�뵽 MBR������һ����������������֮�С���Ϊ PC ������������ 512 ��
* �ڣ�����stage1Ҳ��512�ֽڡ�stage1�����þ��Ǵ�һ�����ش��̼���stage 2 ����
* stage1_5 ����Ϊ��С�����ƣ�stage1 ��stage2 ����stage1_5��λ�ý��б��롣
*
* stage1��stage2�ļ�һ��λ��/boot/grub/Ŀ¼�£������Ŀ¼�»��кܶ�stage1_5����
* �������Ҷ������ļ�ϵͳ��ʽ�����ġ����ǵ�Ŀ������stage1��stage2֮��һ��������
* Ҳ����stage1����stage1_5, Ȼ��stage1_5����stage2��stage1 ��stage1_5�Ĳ�֮ͬ��
* �� stage1�޷�ʶ���ļ�ϵͳ��stage1_5���ԡ���Ϊ Stage2 ̫���ˣ��޷���Ƕ�뵽ĳ��
* �̶������򣬶�stage1_5���԰�װ�� MBR ֮���λ�á�
*
* 2.grub������ʽ
*
* 1)stage1_5 -> stage2 ��ʽ
*
* ���ȼ���Ƿ��Ǻ��ʵ��ļ�ϵͳ��������������ھ�ʹ���ļ�ϵͳ�߼��ķ�ʽstage2 ��
* ����Ҳ�������blocklist�ķ�ʽ����stage2���ҵ��˹��������Ҳ�������
*
* 2)stage1 -> stage2��ʽ
*
* ���ȷ��stage1_5û�б���װ��MBR֮�� stage1�ͻ�ʹ�ü�¼��stage2��blocklist
* Ѱ��stage2,�ҵ�����������Ҳ�������
*
* 3. GRUB��ִ������
*
* ��ϵͳ�ӵ�󣬹̻���BIOS�еĳ������ȶ�ϵͳӲ�������Լ죬�Լ�ͨ���󣬾ͼ�������
* �����ϵ�MBR����������Ȩ����MBR�еĳ���(stage1)��stage1ִ�У��ж��Լ��Ƿ�GRUB��
* �������������stage1_5�������stage1_5�������תȥ�����������������ţ�stage2
* �����ز�ִ�У���stage2����stage1_5�����ļ�ϵͳ��������grub.conf����ʾ�����˵�
* ���û�ѡ��Ȼ������û���ѡ���Ĭ�����ü��ز���ϵͳ�ںˣ���������Ȩ��������
* ϵͳ�ںˣ����ں���ɲ���ϵͳ��������
*
* 4. GRUB�漰��������Ҫ���ļ���
*
* ��һ������stage1��������װ��MBR������0��0�ŵ��ĵ�1����������СΪ512�ֽڣ�446��
* �ڴ���+64�ֽڷ�����+2�ֽڱ�־55AA������������ش����0��0����2������start����
*
* �ڶ�����stage1_5��stage1_5����ʶ���ļ�ϵͳ�ͼ���stage2������stage1_5�����ж����
* ��֧�ֲ�ͬ�ļ�ϵͳ�Ķ�ȡ���ڰ�װGRUB��ʱ��GRUB����ݵ�ǰ/boot/�������ͣ�����
* ��Ӧ��stage1_5��0��0�ŵ��ĵ�3������stage1_5����start���صġ�
*
* ��������stage2����������ʾ�����˵����ṩ�û������ӿڣ��������û�ѡ���Ĭ������
* ���ز���ϵͳ�ںˡ�ͬǰ�����ļ���ͬ��stage2�Ǵ���ڴ�����/boot/grub�¡�
*
* ���ĸ���menu.lst(/boot/grub/grub.conf������)��grub.conf��һ�����ڽű����ı���
* �������а����˵���ʾ�����ú͸�������ϵͳ���ں˼������á�GRUB����grub.conf��ʾ
* �����˵����ṩͬ�û��������档GRUB���Ǹ����û�ѡ���Ĭ�����ú�grub.conf���ں�
* ���ü�����Ӧ���ں˳��򣬲��ѿ���Ȩ�����ں˳���ʹ���ں˳�����������Ĳ���ϵ
* ͳ��������
*
* ������Ҫ�ļ���GRUB����������������Ҫ�ļ�֮�⣬������֧�ֽ������ܵ�һЩ���̳���
* ��Ҫ����/sbin/�µ�grub��grub-install��grub-md5-crypt��grub-terminfo��
* /usr/bin/mbchk���Լ�/boot/grub�µ��豸ӳ���ļ�(device.map)�Ͳ˵�����ͼ���ļ�
* (splash.xpm.gz)��
*
* ͨ������ķ����ܽᣬ���Ժ����׵ؿ�����GRUBʵ���ϰ��������֣�һ���ֱ���װ�ڴ�
* �̵���������������һ���������ļ�����ʽ���ڡ�Ҫ��GRUB��������ϵͳ���ͱ�������
* ��GRUB��stage1��stage1_5(�����ļ�ϵͳ�Զ�ѡ���Ƿ�װ)��װ�����̵�����������
* ���⣬�ڴ��̵�/boot/grub�´�����grub.conf��device.map���ļ���֧�ֽ����ĳ���
* ������Щ���������PATH��������ָ����·���С��߱�����Щ֪ʶ�����Ų����ǰ�װ��
* ���á����ݻ��޸�GRUB�����Ǽ����ѵ����顣
*/

/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����: GRUB 0.97��stage1.5����
* 
* Stage1.5���̵����úܵ�һ�����Ƿǳ��ؼ���������Ҫ���þ��ǹ���һ��
* boot����ϵͳ��Ӧ���ļ�ϵͳ����������ͨ���ļ�ϵͳ��·����/boot/grub/��Ѱ��
* stage2������Ҫ��core.img���������ص��ڴ��п�ʼִ�С�
*
* Stage1.5������0��0��3������ʼ�ĵط�����һֱ����ʮ��k�ֽڵ����򣬾���Ĵ�С
* ����Ӧ���ļ�ϵͳ�Ĵ�С�йأ������漰����0��0��1-3+x�������ⲿ������Ϊ����
* ������BIOS��������κ����ݡ�����Ϊ������ת����GPT������ʽ��ϵͳ�����ܱ�
* ��ȷ��������������ʾ��MBR�����������������������ռ�ݣ���Stage1.5���̱���
* ���ɶ��ֲ�ͬ���ͣ����ǹ������ƣ�����򵥽���һ�»�����stage1.5���̵��ļ�ϵͳ��
* e2fs_stage1_5�����ext2fs��������ext2��ext3�ļ�ϵͳ����fat_stage1_5�����fat
* �ļ�ϵͳ��������fat32��fat16����ffs_stage1_5��jfs_stage1_5��minix_stage1_5��
* reiserfs_stage1_5��vstafs_stage1_5��xfs_stage1_5����Щ�ļ�����Ϊstage1.5���̣�
* ��Щ�ļ�ÿ�����ٶ���11k���ϡ�����֮�⻹�������Ƚ�������ļ����ֱ�Ϊnbgrub��
* pxegrub���������ļ���Ҫ������������ʱʹ�ã�ֻ�Ǹ�ʽ��ͬ���ѣ����Ǻ�������
* stage2��ֻ����Ҫ������������ȡ�����ļ���
*
* ����stage1.5�����л��漰������ļ�ϵͳ��Ӧ���ļ�����˱�������Ҫ��ext2fsΪ��
* ����˵���������ļ�ϵͳ������ƣ�����ͬ�����з�����⡣
*
* ����ext2fs�ļ�ϵͳ���������ɸ��ļ�ϵͳ��stage1.5�����ļ���e2fs_stage1_5����
* ����Ϊstage2/fsys_ext2fs.c�ļ���
*
* ��stage2/filesys.h�ļ��ж�����ÿ���ļ�ϵͳ����Ľӿڣ������ϲ���ã���Ϊ
* stage2����Ѱ�Һ��Ĵ���ʹ�ã��ļ�ϵͳһ�㱻����Ľӿ���Ҫ���������������ֱ�
* ��mount��read��dir��������Ӧext2fs���䶨��ĺ���Ϊ��
*
* #ifdef FSYS_EXT2FS
* #define FSYS_EXT2FS_NUM 1
* int ext2fs_mount (void);
* int ext2fs_read (char *buf, int len);
* int ext2fs_dir (char *dirname);
* #else
* #define FSYS_EXT2FS_NUM 0
* #endif
*
* ���ext2fs�����ϵĺ������ƣ�ÿ��������������stage2/fsys_ext2fs.c�ļ��б����壬
* ������û�а����κε�д�Ĺ��̣�����bootloader���Խ������Ϳ�����������ˣ�û��
* Ҫ����ϵͳ����д����������ext2fs_mount�������ڼ���ļ�ϵͳ���ͣ�����superblock
* ���뵽�ڴ��У�ext2fs_read������ext2fs_dir�������ڶ��ļ�ϵͳ����Ĳ�����
* ��stage2/fsys_ext2fs.c�ļ��г�����Ҫ�������������Ķ���֮�⣬����Ҫ�ļ�ϵͳ��
* ���Ե����ݽṹ��superblock��inode��group�ṹ����Щ�ṹ�����������
* include/linux/ext2_fs.h�ļ��У���ͨ����Щ���ݽṹ����һ���ļ�ϵͳ��
*
* �����������Ȥ�������Ŵ����µ��ļ�ϵͳ��֧�֣����Բ���Ŀǰ���ڵ�һЩ�ļ�ϵͳ
* ��ģ�壨ʵ������д��
*/

static int saved_sector = -1;
/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
* 
* ͨ�� disk_read_hook = disk_read_savesect_func;����Ϊ��ȡ�������ݵĹ��ӣ�Ȼ����
* ��ȡʱ��grub_read()�����һ��ʹ��disk_read_func = disk_read_hook;��Ϊ�ڲ����ӣ�
* ���Ա�����󱻶�ȡ�������ţ���cmain()�������ж��Ƿ���ʵ��ȡ������(saved_sector
* �����ٵ���-1)��
*/

static void
disk_read_savesect_func (int sector, int offset, int length)
{
  saved_sector = sector;
}
/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
*
* GRUB 0.97 Stage1.5 cmain()��������
*
* ע����GRUB 0.97��������cmain()�����Ķ��壬�ֱ����Բ�ͬ��Դ�ļ���
*
* 1��grub-0.97/stage2/stage1_5.c�����cmain()����
* 2��grub-0.97/stage2/stage2.c�����cmain()����
*
* ��������Ŀǰ��עStage1.5�������Ҫ�ȷ���Դ�ļ���grub-0.97/stage2/stage1_5.c��
* �е�cmain()������
* 
* �������������Ҫ���á�grub-0.97/stage2/disk_io.c���еĺ���ʵ���ļ�����GRUB 
* 0.97��Stage2��
*
* 1) �����ǵ���"grub_open (config_file)"�������config_file���������
* config_file��������������Stage1.5�������������Ƕ����ڻ�����Դ�ļ�
*��grub-0.97/stage2/asm.S���еı���config_file���������������ָ��һ��"�ṹ��"��
* ���һ���ֶ���4�ֽڵ�ֵ��Ĭ����0xffffffff���ڶ����ֶ���һ���ַ������飬ֵΪ
* "/boot/grub/stage2"��
*
* �����������ǿ��ǵ��ǵ�һ�ε���grub_open()�����Σ���������open_partition()
* Ӧ�û�ʧ�ܣ��Ӷ������attempt_mount ()���������ǿ��ǵ�EXT2�ļ�ϵͳ�����
* mount_func()Ӧ������fsys_table�����ж����ext2fs_mount()�������ext2fs_mount()
* ��Ҫ���ж��ļ�ϵͳ���ͣ������ļ�ϵͳ���������SUPERBLOCK�ṹ�����򣬽�һ���ж�
* SUPERBLOCK->s_magic��Ӧ�ǲ���EXT2_SUPER_MAGIC����������ʱ����EXT2�ļ�ϵͳ����
* ��ϸ�ڡ��̶�������grub_open()��һ��Ҫ�������飨���Է�Stage1.5�Ĵ��룩����Ӧ
* EXT2�ļ�ϵͳ��������õ�Ӧ����ext2fs_dir()�������Ѿ����ļ�ϵͳ��������Ϣ����
* SUPERBLOCK�ṹ���С�
*
* 2) ��grub_open()���غ�Stage1.5��cmain()�ͻ���������
* "grub_read ((char *) 0x8000, SECTOR_SIZE * 2)"�Լ�
* "grub_read ((char *) 0x8000 + SECTOR_SIZE * 2, -1)"����ʵ��һ������grub_read()
* �ᷢ�֣�Stage1.5��Ӧ��grub_read()��Ҫ���뻹�ǵ���read_func()����ָ�롣�����
* read_func()����ָ�����EXT2�ļ�ϵͳ����ext2fs_read()��������
*��grub-0.97/stage2/fsys_ext2fs.c����
*
* 3) �ڶ���Stage2�����ݺ󣬻����grub_close()�رա������grub_close()������
*��grub-0.97/stage2/disk_io.c������ˣ���������Ҫ��Ҳ�ǵ��ö�Ӧ���ļ�ϵͳ֧��
* ����ʵ�ֵ�close_func()����������EXT2�ļ�ϵͳ�������ָ��Ϊ�ա�
*
* 4) �����ȡStage2�ļ��ɹ���������"chain_stage2 (0, 0x8200, saved_sector)"��
* ����ִ��Stage2���ⲿ�ִ�������һ�ڷ�����
*/

void
cmain (void)
{
  grub_printf ("\n\nGRUB loading, please wait...\n");

  /*
   *  Here load the true second-stage boot-loader.
   */

  if (grub_open (config_file))
    {
      int ret;

      disk_read_hook = disk_read_savesect_func;
      grub_read ((char *) 0x8000, SECTOR_SIZE * 2);
      disk_read_hook = NULL;

      /* Sanity check: catch an internal error.  */
      if (saved_sector == -1)
	{
	  grub_printf ("internal error: the second sector of Stage 2 is unknown.");
	  stop ();
	}
      
      ret = grub_read ((char *) 0x8000 + SECTOR_SIZE * 2, -1);
      
      grub_close ();

      if (ret)
	chain_stage2 (0, 0x8200, saved_sector);
    }

  /*
   *  If not, then print error message and die.
   */

  print_error ();

  stop ();
}
