/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000,2001,2002,2004,2005  Free Software Foundation, Inc.
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
#include <term.h>
/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��6��3��
*
* @note ע����ϸ����: GRUB Stage2 �����ܽ�
* 
* ͨ���������������ģ����Ҫ�Ĺ����������GRUB ���΢�Ͳ���ϵͳ�ĴӴ��̵��ڴ��
* װ�غ����С���asm.S ����ļ����ṩ�˴ӻ����뵽C ����ת���Ľӿڣ�Ҳ�Ǵ����￪
* ʼ��ʽ������GRUB ���΢�Ͳ���ϵͳ������˵��GRUB ���е�һ����ڡ�ͬʱ����asm.S
* �ļ��У��Եײ�ķ����û�����Խ����˷�װ���������Ժ��C �����е��á�Ȼ�󾭹���
* BIOS ����һЩ��ʼ���Ժ���ʽ������GRUB �������򣬼���stage2 �е�cmain ��ڡ���
* �����΢�͵Ĳ���ϵͳ��ʼ��ʽ���С�Ȼ��ֵ��ע�����buildin ������ݽṹ������ṹ
* ����GRUB ����֧����������ݽṹ���ṹ������һ������ʶ������ֺ�һ���������õ�
* ������GRUB ͨ�������ⲿ�����ָ��ķ�ʽ������ӵ�������װ�������Ĳ���ϵͳ��
*/

/**
* @brief ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��6��3��
*
* @note ע����ϸ����: grub.conf��д��
* 
* default������Ĭ�������Ĳ���ϵͳ��0 ��ʾ��һ������ϵͳ��1��ʾ��2�����������ơ�
*
* hiddenmenu����������ʱ���ز˵���������timeout֮ǰ���� ESC ���ܿ����˵���
*
* timeout�����������������û�û�а��¼����ϵ�ĳ�����������Զ����� default 
*          ��ָ���Ĳ���ϵͳ��
*
* splashimage��ָ�������ı���ͼƬ��һ��Ϊѹ���ļ���·��Ϊ����·����
*
* title������������Ŀ�����ơ�
*
* root��ָ��boot�������ڴ��̼��������磺root (hd0,0)��
*
* kernel��ָ��kernel�ļ����ھ���Ŀ¼��ַ���磺
*
* kernel /boot/vmlinuz-2.6.18-92.el5 ro root=LABEL=/ rhgb quiet
*
* initrd��ָ��ramdisk�����ھ���Ŀ¼��ַ���磺
*
* initrd /boot/initrd-2.6.18-92.el5.img
* 
* ע�⣺
*
* kernel��initrd�������������У�ָ����·�����Ǿ���·������Ϊ�������ļ���
* �����/bootĿ¼������/boot���ڵķ����Ѿ���root (hd[0-n],[0-n])��ָ��������
* ��������ָ��kernel��initrd���ĸ������ˡ����boot����Ϊ������������ôǰ���
* /bootʡ�Ե������boot����Ϊ�Ƕ�����������ô�������/boot��
*/

grub_jmp_buf restart_env;

#if defined(PRESET_MENU_STRING) || defined(SUPPORT_DISKLESS)

# if defined(PRESET_MENU_STRING)
static const char *preset_menu = PRESET_MENU_STRING;
# elif defined(SUPPORT_DISKLESS)
/* Execute the command "bootp" automatically.  */
static const char *preset_menu = "bootp\n";
# endif /* SUPPORT_DISKLESS */

static int preset_menu_offset;
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
* �ú���ʵ�ִ�Ԥ��˵��Ĺ��ܣ���ȫ�ֱ���preset_menu_offset����Ϊ0���ҷ��ز���
* ֵpreset_menu != 0,����Ԥ��˵��Ƿ���ڡ�
* 
* GRUB_UTIL��������GRUB��ģ�⹤��ʱ���õģ��������������GRUB���Բ������ǡ�
*/
static int
open_preset_menu (void)
{
#ifdef GRUB_UTIL
  /* Unless the user explicitly requests to use the preset menu,
     always opening the preset menu fails in the grub shell.  */
  if (! use_preset_menu)
    return 0;
#endif /* GRUB_UTIL */
  
  preset_menu_offset = 0;
  return preset_menu != 0;
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
* �ú���ʵ�ִ�preset_menu�ַ����ж�ȡһ��Ԥ��˵��еĹ��ܡ�����grub_memmove()��
* ����preset_menu_offset��ָ�ĵ�ǰԤ��˵��ж�ȡ������buf�ռ��С�
*/
static int
read_from_preset_menu (char *buf, int maxlen)
{
  int len = grub_strlen (preset_menu + preset_menu_offset);

  if (len > maxlen)
    len = maxlen;

  grub_memmove (buf, preset_menu + preset_menu_offset, len);
  preset_menu_offset += len;

  return len;
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
* �ú���ʵ�ֹر�Ԥ��˵��Ĺ��ܡ�ʵ���ǽ�preset_menu����Ϊ0��ɵġ�
*/
static void
close_preset_menu (void)
{
  /* Disable the preset menu.  */
  preset_menu = 0;
}

#else /* ! PRESET_MENU_STRING && ! SUPPORT_DISKLESS */

#define open_preset_menu()	0
#define read_from_preset_menu(buf, maxlen)	0
#define close_preset_menu()

#endif /* ! PRESET_MENU_STRING && ! SUPPORT_DISKLESS */
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
* �ú���ʵ�ֻ�õ�num��˵���Ĺ��ܡ�����listΪ�˵�����ʼλ�ã�����numΪҪ�Թ���
* �˵��������Ҳ���Ǻ���Ҫ���صĵڼ���˵������nestedֻ����list��ʵ�ʲ���Ϊ
* config_entries��Ϊ1������list������ʵ�ʲ���Ϊmenu_entriesʱ��Ϊ0���ú������ض�
* Ӧ�Ĳ˵��
*/
static char *
get_entry (char *list, int num, int nested)
{
  int i;

  for (i = 0; i < num; i++)
    {
      do
	{
	  while (*(list++));
	}
      while (nested && *(list++));
    }

  return list;
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
* �ú���ʵ����ʾһ���˵���Ĺ��ܡ�����yΪҪ��ʾ���кţ�����highlightΪ�Ƿ������
* ����entryΪҪ��ʾ�Ĳ˵��
*/
/* Print an entry in a line of the menu box.  */
static void
print_entry (int y, int highlight, char *entry)
{
  int x;

  if (current_term->setcolorstate)
    current_term->setcolorstate (COLOR_STATE_NORMAL);
  
  if (highlight && current_term->setcolorstate)
    current_term->setcolorstate (COLOR_STATE_HIGHLIGHT);

  gotoxy (2, y);
  grub_putchar (' ');
  for (x = 3; x < 75; x++)
    {
      if (*entry && x <= 72)
	{
	  if (x == 72)
	    grub_putchar (DISP_RIGHT);
	  else
	    grub_putchar (*entry++);
	}
      else
	grub_putchar (' ');
    }
  gotoxy (74, y);

  if (current_term->setcolorstate)
    current_term->setcolorstate (COLOR_STATE_STANDARD);
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
* �ú���ʵ����ʾһ��˵���Ĺ��ܡ�����yΪ��ʼҪ��ʾ���У�����sizeΪҪ��ʾ�Ĳ˵�
* ��ĸ���������firstΪ��һ���˵������entrynoΪ�˵����ţ�����menu_entriesΪ
* Ҫ��ʾ�Ĳ˵����顣
*/
/* Print entries in the menu box.  */
static void
print_entries (int y, int size, int first, int entryno, char *menu_entries)
{
  int i;
  
  gotoxy (77, y + 1);

  if (first)
    grub_putchar (DISP_UP);
  else
    grub_putchar (' ');

  menu_entries = get_entry (menu_entries, first, 0);

  for (i = 0; i < size; i++)
    {
      print_entry (y + i + 1, entryno == i, menu_entries);

      while (*menu_entries)
	menu_entries++;

      if (*(menu_entries - 1))
	menu_entries++;
    }

  gotoxy (77, y + size);

  if (*menu_entries)
    grub_putchar (DISP_DOWN);
  else
    grub_putchar (' ');

  gotoxy (74, y + entryno + 1);
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
* �ú���ʵ����TERM_DUMB���͵��ն�(Ҳ���ǲ�����fancy things���ն�)����ʾһ��˵�
* ��Ĺ��ܡ���TERM_DUMB���͵��նˣ�ֱ�ӵ���grub_putchar()��grub_printf()ʵ�ֲ�
* ������ʾ������sizeΪҪ��ʾ�Ĳ˵������������firstΪҪ��ʾ�ĵ�һ���˵������
* ΪҪ��ʾ�Ĳ˵������顣
*/
static void
print_entries_raw (int size, int first, char *menu_entries)
{
  int i;

#define LINE_LENGTH 67

  for (i = 0; i < LINE_LENGTH; i++)
    grub_putchar ('-');
  grub_putchar ('\n');

  for (i = first; i < size; i++)
    {
      /* grub's printf can't %02d so ... */
      if (i < 10)
	grub_putchar (' ');
      grub_printf ("%d: %s\n", i, get_entry (menu_entries, i, 0));
    }

  for (i = 0; i < LINE_LENGTH; i++)
    grub_putchar ('-');
  grub_putchar ('\n');

#undef LINE_LENGTH
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
* �ú���ʵ����ʾ�˵��߽�Ĺ��ܡ�����yΪҪ��ʾ�ı߽�ĵ�һ�У�����sizeΪ�˵�������
*/
static void
print_border (int y, int size)
{
  int i;

  if (current_term->setcolorstate)
    current_term->setcolorstate (COLOR_STATE_NORMAL);
  
  gotoxy (1, y);

  grub_putchar (DISP_UL);
  for (i = 0; i < 73; i++)
    grub_putchar (DISP_HORIZ);
  grub_putchar (DISP_UR);

  i = 1;
  while (1)
    {
      gotoxy (1, y + i);

      if (i > size)
	break;
      
      grub_putchar (DISP_VERT);
      gotoxy (75, y + i);
      grub_putchar (DISP_VERT);

      i++;
    }

  grub_putchar (DISP_LL);
  for (i = 0; i < 73; i++)
    grub_putchar (DISP_HORIZ);
  grub_putchar (DISP_LR);

  if (current_term->setcolorstate)
    current_term->setcolorstate (COLOR_STATE_STANDARD);
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
* ����run_menu()��grub �������˵��������ѭ������������Ļ�����������:
*
* 1) ������һ����ʱ��grub_timout ���м�ʱ�����grub_timeout < 0����ô��ǿ����ʾ
*    �˵���
* 2) ���û����ʾ�˵���������Ļ����ʾ"Press `ESC' to enter the menu..."��������
*    һ����ѭ���У����û�����ESC ������������ʾ�˵���
* 3) ���grub_timout��ʱ����ô��ֱ�ӽ����һ������Ĭ�ϵ��Ǹ������˵��
* 4) �����ʾ�˵�������ʾ���п���ѡ�����ڡ�
* 5) �����Ƿ���ʾ�˵��������򶼽���ת��boot_entry�����������Ļ���ѹ�궨�ڵ�
*    һ�С�Ȼ���ٴν���ѭ���������û������������ڣ���ͨ������get_entry()������
*    ��ȡһ��Ĭ�ϵ���ڡ�
* 6) Ȼ������ڡ�grub-0.97/stage2/cmdline.c���е�run_script()�������������ڡ�
*    run_script()�������������Ժ������ű�ʹ��find_command()�������н�����
*
* ���������ú�����ʵ��ϸ��:
* 
* 1. ��ѭ����ʼ
*
*    run_menu()������һ��ѭ��������Ҫ����û��ڲ˵���������벢������Ӧ�Ĳ˵�
*    ���ִ�У�ʵ�ʾ���ת���������ִ�У�����ˣ�һ��ʼ������һ��restart��ǩ��
*    ���������Ҫ���¿�ʼѭ��ʱʹ������"goto restart;"�ķ�ʽ��ת�������ǩ��
*
*    ���ţ���鵱ǰ���ն��Ƿ�����ν��TERM_DUMB��Ҳ����ͬʱ��ʾ���еĲ˵������
*    �ˣ���������������նˣ���ôһ�������ʾ11���˵����ˣ��������entryno
*   ��Ҳ����Ĭ��Ҫ��ʾ�Ĳ˵������11����ô�͵ݼ�entryno����ͬʱ����first_entry
*   ����һ��Ҫ��ʾ�Ĳ˵����
*
* 2. ����ȴ���ʱ
*
*   1�����ȫ�ֱ���grub_timeoutС��0����ô��ΪҪôû����ʼ����Ҫô���Ѿ����ݼ�
*      ����ʱ�ˣ���˾�ǿ����ʾ�˵������show_menu����Ϊ1������ˣ��Ͳ���ִ��
*      ����Ĳ��裬����ֱ��������һ�ڷ����Ĵ��롣
*   2�����show_menu����Ϊ0����ô�Ͳ���ʾ�˵����ִ������Ĳ��裺
*      a) ��ȡ��ǰʱ�䵽����time1�С�
*      b) ����Ƿ�����ESC���������������ESC��������ô����grub_timeoutΪ-1��
*         ����show_menu����Ϊ1�����˳�����ת����һ��Ҫ�����Ĵ��롣
*      c) ���grub_timeout����0���ٴλ�ȡ��ȡ��ǰʱ�䵽����time1�У��������ֵ
*         ���浽time2�У����´αȽϣ���
*      d) ���grub_timeout�ݼ���0��������grub_timeoutΪ-1������ת�����
*         boot_entry���������˵��
*
* 3. ��ʾ�˵�
*
*   1��	����init_page ()����ʼ����ҳ���ܣ�ʵ����������Ļ֮���ӡһ��GRUB�汾��
*       �ַ�����Ϣ��
*   2��	����setcursor (0)���������ʱ��ֹ����ʵ�����ǵ��õ�ǰ�ն˵Ļص�����
*       current_term->setcursor (0)��
*   3��	�����ǰ�ն�����ν��TERM_DUMB�նˣ���ôֱ�ӵ���print_entries_raw()����
*       ʾ���е�menu_entries�����򣬾͵���print_border (3, 12)�ȴ�ӡһ����y=3
*       λ�õĲ˵��߽硣
*   4��	��ӡһ����Ϣ�������û���DISP_UP��DISP_DOWN��ѡ������Ĳ˵��
*   5��	�����û����֤����Ҫ�����룬��ô��ӡһ����Ϣ�������û����԰�Enter��ֱ
*       ������ѡ�еĲ˵�����߰�'p'�����������룬�Ӷ�����һ�������Թ��ܡ�
*   6��	���������config_entries����ô��ӡһ����Ϣ�����û���������'e'���༭
*       �˵����������'c'�����������н��档���û��config_entries����ô��ӡ
*       һ����Ϣ�������û���������'b'������������'e'���༭�˵����������'c'
*       �����������н��棬����'o'���ڵ�ǰѡ���еĺ������һ�У���������'O'����
*       ��ǰѡ�е���ǰ�����һ�У�����'d'��ɾ��һ�У���������ESC���ص����˵���
*   7��	�����ǰ�ն���TERM_DUMB�����նˣ���ô��ӡһ����Ϣ�������ǰ��ʾ�Ĳ˵�
*       ����򣬵���print_entries()����first_entry��ʼ��menu_entries��
*
* 4. ѭ���ȴ��û�����
*
*   �������Ĵ�������һ��ѭ�����ȴ��û������롣�����ѭ���У���һ�����Ǽ���Ƿ�
*   �ȴ���ʱ��
*
*   1��	�����ʱ��������grub_timeoutΪ-1��������
*   2��	���򣬴�ӡһ����Ϣ�����û�ѡ���entryno����grub_timeout��֮��������
*   3��	���ţ������û����룬����֮ת��ΪASCII�ַ���
*
* 5. ������ִ���û�����
*
*   1)	����İ�����'^' ���� 'v'����ݼ����ߵ���entryno��������print_entry()��
*       ���²˵���ʾ��
*   2)	����İ�����"Page Up"����"Page Down"����ݼ������ߵ�����first_entryֵ12��
*       ������entryno��Ȼ�����print_entries()������������ʾ�˵�ҳ��
*   3)	�����config_entries�����������������'\n'����'\r'������ֵ6����ôֱ��
*       �˳�����ȴ�ѭ������������boot_entry���봦������ѡ�еĲ˵��
*   4)	�����������Ϊ'd'��'o'������'O'������ɾ��һ�У���ѡ���к����һ�У�
*       ������ѡ����ǰ���һ�еȹ��ܡ�
*   5)	�������ֵΪ27��ֱ��return��
*   6)	�������ֵΪ'b'��������ѭ������boot_entry���봦��
*   7��	�����δ��֤����Ҫ���������룬���û�����'p'ʱ����������������֤��
*       ���"goto restart"����ס�˵�ѭ����ʼ����
*   8��	�������������'e'�������༭��ǰѡ�в˵���ģʽ��
*   9������������'c'�������������ģʽ��
*
* 6. ִ��ѡ�в˵���
*
*   ����ڲ˵�������ѡ����Ҫ���������������Enter������ô�ͻ����˵����ִ�С�
*   ʵ������ִ�������в��裺
*   1��	������
*   2��	������ʾ��ꡣ
*   3��	��ʾҪ�����Ĳ˵��
*   4��	��ȡѡ�еĲ˵��cur_entry��
*   5��	����run_script()ִ�иò˵�����ʧ�ܣ����˻�entryno��fallback_entryno��
*   6��	�κ�ʧ�ܻ�����ɣ��������������OS����chainloader���������ص�restart��ǩ��
*/
static void
run_menu (char *menu_entries, char *config_entries, int num_entries,
	  char *heap, int entryno)
{
  int c, time1, time2 = -1, first_entry = 0;
  char *cur_entry = 0;

  /*
   *  Main loop for menu UI.
   */

restart:
  /* Dumb terminal always use all entries for display 
     invariant for TERM_DUMB: first_entry == 0  */
  if (! (current_term->flags & TERM_DUMB))
    {
      while (entryno > 11)
	{
	  first_entry++;
	  entryno--;
	}
    }

  /* If the timeout was expired or wasn't set, force to show the menu
     interface. */
  if (grub_timeout < 0)
    show_menu = 1;
  
  /* If SHOW_MENU is false, don't display the menu until ESC is pressed.  */
  if (! show_menu)
    {
      /* Get current time.  */
      while ((time1 = getrtsecs ()) == 0xFF)
	;

      while (1)
	{
	  /* Check if ESC is pressed.  */
	  if (checkkey () != -1 && ASCII_CHAR (getkey ()) == '\e')
	    {
	      grub_timeout = -1;
	      show_menu = 1;
	      break;
	    }

	  /* If GRUB_TIMEOUT is expired, boot the default entry.  */
	  if (grub_timeout >=0
	      && (time1 = getrtsecs ()) != time2
	      && time1 != 0xFF)
	    {
	      if (grub_timeout <= 0)
		{
		  grub_timeout = -1;
		  goto boot_entry;
		}
	      
	      time2 = time1;
	      grub_timeout--;
	      
	      /* Print a message.  */
	      grub_printf ("\rPress `ESC' to enter the menu... %d   ",
			   grub_timeout);
	    }
	}
    }

  /* Only display the menu if the user wants to see it. */
  if (show_menu)
    {
      init_page ();
      setcursor (0);

      if (current_term->flags & TERM_DUMB)
	print_entries_raw (num_entries, first_entry, menu_entries);
      else
	print_border (3, 12);

      grub_printf ("\n\
      Use the %c and %c keys to select which entry is highlighted.\n",
		   DISP_UP, DISP_DOWN);
      
      if (! auth && password)
	{
	  printf ("\
      Press enter to boot the selected OS or \'p\' to enter a\n\
      password to unlock the next set of features.");
	}
      else
	{
	  if (config_entries)
	    printf ("\
      Press enter to boot the selected OS, \'e\' to edit the\n\
      commands before booting, or \'c\' for a command-line.");
	  else
	    printf ("\
      Press \'b\' to boot, \'e\' to edit the selected command in the\n\
      boot sequence, \'c\' for a command-line, \'o\' to open a new line\n\
      after (\'O\' for before) the selected line, \'d\' to remove the\n\
      selected line, or escape to go back to the main menu.");
	}

      if (current_term->flags & TERM_DUMB)
	grub_printf ("\n\nThe selected entry is %d ", entryno);
      else
	print_entries (3, 12, first_entry, entryno, menu_entries);
    }

  /* XX using RT clock now, need to initialize value */
  while ((time1 = getrtsecs()) == 0xFF);

  while (1)
    {
      /* Initialize to NULL just in case...  */
      cur_entry = NULL;

      if (grub_timeout >= 0 && (time1 = getrtsecs()) != time2 && time1 != 0xFF)
	{
	  if (grub_timeout <= 0)
	    {
	      grub_timeout = -1;
	      break;
	    }

	  /* else not booting yet! */
	  time2 = time1;

	  if (current_term->flags & TERM_DUMB)
	      grub_printf ("\r    Entry %d will be booted automatically in %d seconds.   ", 
			   entryno, grub_timeout);
	  else
	    {
	      gotoxy (3, 22);
	      grub_printf ("The highlighted entry will be booted automatically in %d seconds.    ",
			   grub_timeout);
	      gotoxy (74, 4 + entryno);
	  }
	  
	  grub_timeout--;
	}

      /* Check for a keypress, however if TIMEOUT has been expired
	 (GRUB_TIMEOUT == -1) relax in GETKEY even if no key has been
	 pressed.  
	 This avoids polling (relevant in the grub-shell and later on
	 in grub if interrupt driven I/O is done).  */
      if (checkkey () >= 0 || grub_timeout < 0)
	{
	  /* Key was pressed, show which entry is selected before GETKEY,
	     since we're comming in here also on GRUB_TIMEOUT == -1 and
	     hang in GETKEY */
	  if (current_term->flags & TERM_DUMB)
	    grub_printf ("\r    Highlighted entry is %d: ", entryno);

	  c = ASCII_CHAR (getkey ());

	  if (grub_timeout >= 0)
	    {
	      if (current_term->flags & TERM_DUMB)
		grub_putchar ('\r');
	      else
		gotoxy (3, 22);
	      printf ("                                                                    ");
	      grub_timeout = -1;
	      fallback_entryno = -1;
	      if (! (current_term->flags & TERM_DUMB))
		gotoxy (74, 4 + entryno);
	    }

	  /* We told them above (at least in SUPPORT_SERIAL) to use
	     '^' or 'v' so accept these keys.  */
	  if (c == 16 || c == '^')
	    {
	      if (current_term->flags & TERM_DUMB)
		{
		  if (entryno > 0)
		    entryno--;
		}
	      else
		{
		  if (entryno > 0)
		    {
		      print_entry (4 + entryno, 0,
				   get_entry (menu_entries,
					      first_entry + entryno,
					      0));
		      entryno--;
		      print_entry (4 + entryno, 1,
				   get_entry (menu_entries,
					      first_entry + entryno,
					      0));
		    }
		  else if (first_entry > 0)
		    {
		      first_entry--;
		      print_entries (3, 12, first_entry, entryno,
				     menu_entries);
		    }
		}
	    }
	  else if ((c == 14 || c == 'v')
		   && first_entry + entryno + 1 < num_entries)
	    {
	      if (current_term->flags & TERM_DUMB)
		entryno++;
	      else
		{
		  if (entryno < 11)
		    {
		      print_entry (4 + entryno, 0,
				   get_entry (menu_entries,
					      first_entry + entryno,
					      0));
		      entryno++;
		      print_entry (4 + entryno, 1,
				   get_entry (menu_entries,
					      first_entry + entryno,
					      0));
		  }
		else if (num_entries > 12 + first_entry)
		  {
		    first_entry++;
		    print_entries (3, 12, first_entry, entryno, menu_entries);
		  }
		}
	    }
	  else if (c == 7)
	    {
	      /* Page Up */
	      first_entry -= 12;
	      if (first_entry < 0)
		{
		  entryno += first_entry;
		  first_entry = 0;
		  if (entryno < 0)
		    entryno = 0;
		}
	      print_entries (3, 12, first_entry, entryno, menu_entries);
	    }
	  else if (c == 3)
	    {
	      /* Page Down */
	      first_entry += 12;
	      if (first_entry + entryno + 1 >= num_entries)
		{
		  first_entry = num_entries - 12;
		  if (first_entry < 0)
		    first_entry = 0;
		  entryno = num_entries - first_entry - 1;
		}
	      print_entries (3, 12, first_entry, entryno, menu_entries);
	    }

	  if (config_entries)
	    {
	      if ((c == '\n') || (c == '\r') || (c == 6))
		break;
	    }
	  else
	    {
	      if ((c == 'd') || (c == 'o') || (c == 'O'))
		{
		  if (! (current_term->flags & TERM_DUMB))
		    print_entry (4 + entryno, 0,
				 get_entry (menu_entries,
					    first_entry + entryno,
					    0));

		  /* insert after is almost exactly like insert before */
		  if (c == 'o')
		    {
		      /* But `o' differs from `O', since it may causes
			 the menu screen to scroll up.  */
		      if (entryno < 11 || (current_term->flags & TERM_DUMB))
			entryno++;
		      else
			first_entry++;
		      
		      c = 'O';
		    }

		  cur_entry = get_entry (menu_entries,
					 first_entry + entryno,
					 0);

		  if (c == 'O')
		    {
		      grub_memmove (cur_entry + 2, cur_entry,
				    ((int) heap) - ((int) cur_entry));

		      cur_entry[0] = ' ';
		      cur_entry[1] = 0;

		      heap += 2;

		      num_entries++;
		    }
		  else if (num_entries > 0)
		    {
		      char *ptr = get_entry(menu_entries,
					    first_entry + entryno + 1,
					    0);

		      grub_memmove (cur_entry, ptr,
				    ((int) heap) - ((int) ptr));
		      heap -= (((int) ptr) - ((int) cur_entry));

		      num_entries--;

		      if (entryno >= num_entries)
			entryno--;
		      if (first_entry && num_entries < 12 + first_entry)
			first_entry--;
		    }

		  if (current_term->flags & TERM_DUMB)
		    {
		      grub_printf ("\n\n");
		      print_entries_raw (num_entries, first_entry,
					 menu_entries);
		      grub_printf ("\n");
		    }
		  else
		    print_entries (3, 12, first_entry, entryno, menu_entries);
		}

	      cur_entry = menu_entries;
	      if (c == 27)
		return;
	      if (c == 'b')
		break;
	    }

	  if (! auth && password)
	    {
	      if (c == 'p')
		{
		  /* Do password check here! */
		  char entered[32];
		  char *pptr = password;

		  if (current_term->flags & TERM_DUMB)
		    grub_printf ("\r                                    ");
		  else
		    gotoxy (1, 21);

		  /* Wipe out the previously entered password */
		  grub_memset (entered, 0, sizeof (entered));
		  get_cmdline (" Password: ", entered, 31, '*', 0);

		  while (! isspace (*pptr) && *pptr)
		    pptr++;

		  /* Make sure that PASSWORD is NUL-terminated.  */
		  *pptr++ = 0;

		  if (! check_password (entered, password, password_type))
		    {
		      char *new_file = config_file;
		      while (isspace (*pptr))
			pptr++;

		      /* If *PPTR is NUL, then allow the user to use
			 privileged instructions, otherwise, load
			 another configuration file.  */
		      if (*pptr != 0)
			{
			  while ((*(new_file++) = *(pptr++)) != 0)
			    ;

			  /* Make sure that the user will not have
			     authority in the next configuration.  */
			  auth = 0;
			  return;
			}
		      else
			{
			  /* Now the user is superhuman.  */
			  auth = 1;
			  goto restart;
			}
		    }
		  else
		    {
		      grub_printf ("Failed!\n      Press any key to continue...");
		      getkey ();
		      goto restart;
		    }
		}
	    }
	  else
	    {
	      if (c == 'e')
		{
		  int new_num_entries = 0, i = 0;
		  char *new_heap;

		  if (config_entries)
		    {
		      new_heap = heap;
		      cur_entry = get_entry (config_entries,
					     first_entry + entryno,
					     1);
		    }
		  else
		    {
		      /* safe area! */
		      new_heap = heap + NEW_HEAPSIZE + 1;
		      cur_entry = get_entry (menu_entries,
					     first_entry + entryno,
					     0);
		    }

		  do
		    {
		      while ((*(new_heap++) = cur_entry[i++]) != 0);
		      new_num_entries++;
		    }
		  while (config_entries && cur_entry[i]);

		  /* this only needs to be done if config_entries is non-NULL,
		     but it doesn't hurt to do it always */
		  *(new_heap++) = 0;

		  if (config_entries)
		    run_menu (heap, NULL, new_num_entries, new_heap, 0);
		  else
		    {
		      cls ();
		      print_cmdline_message (0);

		      new_heap = heap + NEW_HEAPSIZE + 1;

		      saved_drive = boot_drive;
		      saved_partition = install_partition;
		      current_drive = GRUB_INVALID_DRIVE;

		      if (! get_cmdline (PACKAGE " edit> ", new_heap,
					 NEW_HEAPSIZE + 1, 0, 1))
			{
			  int j = 0;

			  /* get length of new command */
			  while (new_heap[j++])
			    ;

			  if (j < 2)
			    {
			      j = 2;
			      new_heap[0] = ' ';
			      new_heap[1] = 0;
			    }

			  /* align rest of commands properly */
			  grub_memmove (cur_entry + j, cur_entry + i,
					(int) heap - ((int) cur_entry + i));

			  /* copy command to correct area */
			  grub_memmove (cur_entry, new_heap, j);

			  heap += (j - i);
			}
		    }

		  goto restart;
		}
	      if (c == 'c')
		{
		  enter_cmdline (heap, 0);
		  goto restart;
		}
#ifdef GRUB_UTIL
	      if (c == 'q')
		{
		  /* The same as ``quit''.  */
		  stop ();
		}
#endif
	    }
	}
    }
  
  /* Attempt to boot an entry.  */
  
 boot_entry:
  
  cls ();
  setcursor (1);
  
  while (1)
    {
      if (config_entries)
	printf ("  Booting \'%s\'\n\n",
		get_entry (menu_entries, first_entry + entryno, 0));
      else
	printf ("  Booting command-list\n\n");

      if (! cur_entry)
	cur_entry = get_entry (config_entries, first_entry + entryno, 1);

      /* Set CURRENT_ENTRYNO for the command "savedefault".  */
      current_entryno = first_entry + entryno;
      
      if (run_script (cur_entry, heap))
	{
	  if (fallback_entryno >= 0)
	    {
	      cur_entry = NULL;
	      first_entry = 0;
	      entryno = fallback_entries[fallback_entryno];
	      fallback_entryno++;
	      if (fallback_entryno >= MAX_FALLBACK_ENTRIES
		  || fallback_entries[fallback_entryno] < 0)
		fallback_entryno = -1;
	    }
	  else
	    break;
	}
      else
	break;
    }

  show_menu = 1;
  goto restart;
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
* �ú���ʵ�ִ������ļ�(����Ԥ��˵�)�ж�ȡһ�������еĹ��ܡ�����cmdlineΪ�����
* ��������ȡ�������лᱻ���浽����ռ��У�����maxlenΪ����ȡ���ֽ���������
* read_from_fileΪ��־�Ǵ������ļ����Ǵ�Ԥ��˵��ж�ȡ�����С��ú������ض�ȡ��
* �����е��ֽ�����
*/
static int
get_line_from_config (char *cmdline, int maxlen, int read_from_file)
{
  int pos = 0, literal = 0, comment = 0;
  char c;  /* since we're loading it a byte at a time! */
  
  while (1)
    {
      if (read_from_file)
	{
	  if (! grub_read (&c, 1))
	    break;
	}
      else
	{
	  if (! read_from_preset_menu (&c, 1))
	    break;
	}

      /* Skip all carriage returns.  */
      if (c == '\r')
	continue;

      /* Replace tabs with spaces.  */
      if (c == '\t')
	c = ' ';

      /* The previous is a backslash, then...  */
      if (literal)
	{
	  /* If it is a newline, replace it with a space and continue.  */
	  if (c == '\n')
	    {
	      c = ' ';
	      
	      /* Go back to overwrite a backslash.  */
	      if (pos > 0)
		pos--;
	    }
	    
	  literal = 0;
	}
	  
      /* translate characters first! */
      if (c == '\\' && ! literal)
	literal = 1;

      if (comment)
	{
	  if (c == '\n')
	    comment = 0;
	}
      else if (! pos)
	{
	  if (c == '#')
	    comment = 1;
	  else if ((c != ' ') && (c != '\n'))
	    cmdline[pos++] = c;
	}
      else
	{
	  if (c == '\n')
	    break;

	  if (pos < maxlen)
	    cmdline[pos++] = c;
	}
    }

  cmdline[pos] = 0;

  return pos;
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
* �ļ���grub-0.97/stage2/stage2.c���е�cmain() ��������GRUB 0.97�����庯������
* �൱��һ��С�͵Ĳ���ϵͳ����Ȼû�н��̵��ȣ�����ͨ����һ����ѭ��ʵ�֣�����
* Stage2 ���������ѭ�������У�������������ѭ��ִ�еĹ��̽��з�����
*
* 1) ���ڲ��������г�ʼ��
*
* ������ѭ���ĵ�һ������ʱ����reset()������Stage2 ���ڲ��������г�ʼ����
*
* ���reset������������������¹��ܣ�
*
* a) ����grub-0.97/stage2/char_io.c���е�ȫ�ֱ���count_lines��λΪ-1��
* b) ��cmain()������config_len��menu_len, num_entries�ȱ�����ʼ��Ϊ0��
* c) ��config_entriesָ��һ��λ�ڡ�mbi.drives_addr + mbi.drives_length���Ļ�������
* d) ��menu_entriesָ��MENU_BUF���ڵ�ַ��һ�黺�������ڡ�grub-0.97/stage2/shared.h��
*    �ж�����һЩ��������ַ��
* e) ���á�grub-0.97/stage2/builtins.c���е�init_config()��ʼ��һЩ�������ݡ�
*
* 2) ��һ��"Ĭ���ļ�"
*
* ���ŵĴ��볢�Դ�һ��"Ĭ���ļ�"����"/boot/grub/ default"����δ������������
* ���ܣ�
*
* a) ��config_file�����ֻ��default_file�����֡����ȣ���default_file�ĵ�һ���ֽ�
*    ��Ϊ0����˽��ŵ�grub_strncat()�ͻ�ֱ�ӽ�config_file������ƴ�ӵ�default_file
*    ��ʼ���������档��config_file�����ڡ�grub-0.97/stage2/asm.S���ж���Ϊ�ַ���
*    "/boot/grub/menu.lst"�����ţ������ɵ�default_file�����һ���ֽ���ǰ������ֱ
*    ���ҵ�"/"�ַ���Ȼ���ٽ�"/"�ַ�������ַ���Ϊ0��������grub_strncat()�ٴ���ƴ
*    �ӣ��Ӷ���default_file���������"/boot/grub/ default "��
* b) ���ţ�ʹ��grub_open (default_file)���Դ����default_file��Ҳ����
     "/boot/grub/ default "�ļ���ע�⣬��������Ϊ��Stage2������Ѿ�����֧������
     ����֧�ֵ��ļ�ϵͳ���Ӷ�grub_open()�᳢��ʹ�����п��õ��ļ�ϵͳ֧�ִ�������
     ����ֱ����������ļ���
* c) ����򿪳ɹ����ͻ����grub_read()����ȡ����ļ������ݵ�buf�У�ʵ��ֻ�����
     ��10���ֽڣ���
* d) �����ȡ�ɹ����ͻ����safe_parse_maxint()���������bufת����һ�����������
     ������Ӧ��Ĭ��Ҫ��������ϵͳ�������ţ������浽����saved_entryno�С������
     ��δ������ҪĿ��--ʶ��һ��Ĭ������Ŀ�����ϵͳ��
* e) �ر����default_file����"/boot/grub/ default "����
*
* 3) �жϴ�Ԥ��˵����������ļ���ȡ����
*
* �ٽ������Ĵ�����һ��do��while()ѭ������һ����cmain()����ѭ���ڲ���һ��Ƕ��ѭ����
* ������ڲ�Ƕ��ѭ����Ҫ���һЩ��Ԥ��˵��Ľ���������������ķ�����
* 
* I��ͨ��open_preset_menu()�������Դ��Ѿ����úõĲ˵�������û�û�����úõĲ�
* ������ô������0������Ѿ����ú��˲˵��򲻷���0��
*
* �����open_preset_menu()��read_from_preset_menu()�Լ�close_preset_menu()�⼸
* �����������ڶ�����(PRESET_MENU_STRING) || defined(SUPPORT_DISKLESS)������²�
* �����壬����Ĭ�Ϸ���Ϊ0����ִ�пղ�����
*
* a) ����PRESET_MENU_STRING����configure��ʱ��ʹ����"--enable-preset-menu=FILE"
*    �����Ż��ж��塣���ʹ���˸ò�������ôconfigure�������õ�ʱ��ͻ������ļ�
*    ����һ���ַ�������������ַ���ʹ��"#define PRESET_MENU_STRING"�ķ�ʽ���嵽
*    config.h�У�����������ǲ��ᱻ����ġ�
* b) ���⣬SUPPORT_DISKLESSҲ��Ҫ��configure��ʱ��ʹ��"--enable-diskless"���ܴ�
*    ���ĺ꣬���򲻶��塣
* c) ��ˣ�Ĭ�����������ֱ��ʹ��configure�����õĻ������������ǲ����򿪵ģ����
*    �⼸������Ҫô��Ĭ�Ϸ���Ϊ0Ҫôִ�пղ�����
* d) �����ȷ����(PRESET_MENU_STRING) || defined(SUPPORT_DISKLESS)����ô
*    open_preset_menu()�ͻ᷵��preset_menu�Ƿ�Ϊ�գ��ڵ�һ�ε���ʱ����Ϊ�����
*    ǰ���������֮һ�Ѿ����壬��˲���Ϊ�գ������ҽ�preset_menu_offset��Ϊ0��
*    Ҳ����ʹ�ú����read_from_preset_menu()����������preset_menuָ��Ļ�����
*    �ĵ�һ���ֽڿ�ʼ������read_from_preset_menu()����ʵ����ʵ���˽�preset_menu
*    ָ��Ļ��������뵽����buf�Ĺ��ܣ�ֱ���û���ָ����ַ���ĩβ����
*    close_preset_menu()���ǽ�preset_menu��Ϊ�գ��Ӷ�open_preset_menu()�ٴε���
*    ʱ�Ͳ����ٴ򿪡�

* II) ���û�гɹ���Ԥ��˵�����ô��ͨ��grub_open()�������Դ�config_file
*   ��ʵ����ǰ���Ѿ�������������ļ�����"/boot/grub/menu.lst"���������Ȼ��
*   ʧ�ܣ�����������ѭ����
*
* ����Ϊֹ������is_preset�����Ƿ����Ԥ��˵���preset_menu�������û�д�Ԥ
* ��˵�����ôis_opened�ʹ����Ƿ���������ļ���config_file����
* "/boot/grub/menu.lst"����
*
* 4) ��Ԥ��˵��������ļ����������������
*
* ����ǰһ���򿪵�������ж��Ǵ�Ԥ��˵����뻹�Ǵ������ļ������������
* "!is_preset"ʵ�ʴ�������Ƿ���������ļ�"/boot/grub/menu.lst"����ʵ������ͨ
* ����!is_preset ����get_line_from_config()����������Ӧ��������뵽cmline �С�
* Ȼ��ͨ��find_command()����������û���������ʹ��while()ѭ��ʵ�ֶ����
* get_line_from_config()�Ķ�ε���(ֱ������ֵΪ0��ʾ�����ļ�ĩβ)����˾���Ҫ��
* ��Ӧ��Ԥ��˵����������ļ��������������ꡣ
*
* ����Զ����ÿһ��,�����Խ���,Ѱ�Ҷ�Ӧ������,���ص�"struct builtin *builtin"�С�
* ��ÿ��������н����ʹ���
*
* I�����������"title"��ʼ��BUILTIN_TITLE����������⴦���������ļ�����Ԥ��
*    �˵��У�����ÿ���������Ĳ���ϵͳ��������"title"���ʼһ���µ���Ŀ��
* II) �������������������"dhcp"������BUILTIN_MENU�ڽ��˵������ֱ�����и�����
*    ��Ӧ�Ĵ����������ļ���grub-0.97/stage2/builtins.c���ж��������������
*    BUILTIN_MENU�ڽ��˵����
* III) �ڴ�����һ�������к󣬽�menu_entries��config_entries�����һ���ֽ���Ϊ0
*   ���ַ���ĩβ����Ȼ�����grub_memmove()��menu_entriesƴ�ӵ�config_entries��
*    ���档����menu_entries����ָ����config_entries�ĺ���İ��ƹ�ȥ��λ��
*   ��config_entries + config_len����
*
* 5) ȷ��fallback��default����ȷ
*
* 6) ѡ�����������л������в˵�
*
* ���û�п�����ʾ�Ĳ˵��num_entries����0�����ͻ����enter_cmdline()��������
* �нӿڡ����򣬽�����run_menu()��ʾһ���˵�������ÿ���˵���������ǰ���������
* �ļ�������Ԥ��˵����ó���menu_entries���ο�����¼D��GRUB 0.97��������༭��
* ���������ѡ��༭ÿ���˵���ͻ������һ���˵��������ÿ���˵����Ǵ�
* config_entries�ó��Ķ�Ӧ��ѡ��һ���˵�����������ݡ�
*/

/* This is the starting function in C.  */
void
cmain (void)
{
  int config_len, menu_len, num_entries;
  char *config_entries, *menu_entries;
  char *kill_buf = (char *) KILL_BUF;

  auto void reset (void);
  void reset (void)
    {
      count_lines = -1;
      config_len = 0;
      menu_len = 0;
      num_entries = 0;
      config_entries = (char *) mbi.drives_addr + mbi.drives_length;
      menu_entries = (char *) MENU_BUF;
      init_config ();
    }
  
  /* Initialize the environment for restarting Stage 2.  */
  grub_setjmp (restart_env);
  
  /* Initialize the kill buffer.  */
  *kill_buf = 0;

  /* Never return.  */
  for (;;)
    {
      int is_opened, is_preset;

      reset ();
      
      /* Here load the configuration file.  */
      
#ifdef GRUB_UTIL
      if (use_config_file)
#endif /* GRUB_UTIL */
	{
	  char *default_file = (char *) DEFAULT_FILE_BUF;
	  int i;
	  
	  /* Get a saved default entry if possible.  */
	  saved_entryno = 0;
	  *default_file = 0;
	  grub_strncat (default_file, config_file, DEFAULT_FILE_BUFLEN);
	  for (i = grub_strlen(default_file); i >= 0; i--)
	    if (default_file[i] == '/')
	      {
		i++;
		break;
	      }
	  default_file[i] = 0;
	  grub_strncat (default_file + i, "default", DEFAULT_FILE_BUFLEN - i);
	  if (grub_open (default_file))
	    {
	      char buf[10]; /* This is good enough.  */
	      char *p = buf;
	      int len;
	      
	      len = grub_read (buf, sizeof (buf));
	      if (len > 0)
		{
		  buf[sizeof (buf) - 1] = 0;
		  safe_parse_maxint (&p, &saved_entryno);
		}

	      grub_close ();
	    }
	  errnum = ERR_NONE;
	  
	  do
	    {
	      /* STATE 0:  Before any title command.
		 STATE 1:  In a title command.
		 STATE >1: In a entry after a title command.  */
	      int state = 0, prev_config_len = 0, prev_menu_len = 0;
	      char *cmdline;

	      /* Try the preset menu first. This will succeed at most once,
		 because close_preset_menu disables the preset menu.  */
	      is_opened = is_preset = open_preset_menu ();
	      if (! is_opened)
		{
		  is_opened = grub_open (config_file);
		  errnum = ERR_NONE;
		}

	      if (! is_opened)
		break;

	      /* This is necessary, because the menu must be overrided.  */
	      reset ();
	      
	      cmdline = (char *) CMDLINE_BUF;
	      while (get_line_from_config (cmdline, NEW_HEAPSIZE,
					   ! is_preset))
		{
		  struct builtin *builtin;
		  
		  /* Get the pointer to the builtin structure.  */
		  builtin = find_command (cmdline);
		  errnum = 0;
		  if (! builtin)
		    /* Unknown command. Just skip now.  */
		    continue;
		  
		  if (builtin->flags & BUILTIN_TITLE)
		    {
		      char *ptr;
		      
		      /* the command "title" is specially treated.  */
		      if (state > 1)
			{
			  /* The next title is found.  */
			  num_entries++;
			  config_entries[config_len++] = 0;
			  prev_menu_len = menu_len;
			  prev_config_len = config_len;
			}
		      else
			{
			  /* The first title is found.  */
			  menu_len = prev_menu_len;
			  config_len = prev_config_len;
			}
		      
		      /* Reset the state.  */
		      state = 1;
		      
		      /* Copy title into menu area.  */
		      ptr = skip_to (1, cmdline);
		      while ((menu_entries[menu_len++] = *(ptr++)) != 0)
			;
		    }
		  else if (! state)
		    {
		      /* Run a command found is possible.  */
		      if (builtin->flags & BUILTIN_MENU)
			{
			  char *arg = skip_to (1, cmdline);
			  (builtin->func) (arg, BUILTIN_MENU);
			  errnum = 0;
			}
		      else
			/* Ignored.  */
			continue;
		    }
		  else
		    {
		      char *ptr = cmdline;
		      
		      state++;
		      /* Copy config file data to config area.  */
		      while ((config_entries[config_len++] = *ptr++) != 0)
			;
		    }
		}
	      
	      if (state > 1)
		{
		  /* Finish the last entry.  */
		  num_entries++;
		  config_entries[config_len++] = 0;
		}
	      else
		{
		  menu_len = prev_menu_len;
		  config_len = prev_config_len;
		}
	      
	      menu_entries[menu_len++] = 0;
	      config_entries[config_len++] = 0;
	      grub_memmove (config_entries + config_len, menu_entries,
			    menu_len);
	      menu_entries = config_entries + config_len;

	      /* Make sure that all fallback entries are valid.  */
	      if (fallback_entryno >= 0)
		{
		  for (i = 0; i < MAX_FALLBACK_ENTRIES; i++)
		    {
		      if (fallback_entries[i] < 0)
			break;
		      if (fallback_entries[i] >= num_entries)
			{
			  grub_memmove (fallback_entries + i,
					fallback_entries + i + 1,
					((MAX_FALLBACK_ENTRIES - i - 1)
					 * sizeof (int)));
			  i--;
			}
		    }

		  if (fallback_entries[0] < 0)
		    fallback_entryno = -1;
		}
	      /* Check if the default entry is present. Otherwise reset
		 it to fallback if fallback is valid, or to DEFAULT_ENTRY 
		 if not.  */
	      if (default_entry >= num_entries)
		{
		  if (fallback_entryno >= 0)
		    {
		      default_entry = fallback_entries[0];
		      fallback_entryno++;
		      if (fallback_entryno >= MAX_FALLBACK_ENTRIES
			  || fallback_entries[fallback_entryno] < 0)
			fallback_entryno = -1;
		    }
		  else
		    default_entry = 0;
		}
	      
	      if (is_preset)
		close_preset_menu ();
	      else
		grub_close ();
	    }
	  while (is_preset);
	}

      if (! num_entries)
	{
	  /* If no acceptable config file, goto command-line, starting
	     heap from where the config entries would have been stored
	     if there were any.  */
	  enter_cmdline (config_entries, 1);
	}
      else
	{
	  /* Run menu interface.  */
	  run_menu (menu_entries, config_entries, num_entries,
		    menu_entries + menu_len, default_entry);
	}
    }
}
