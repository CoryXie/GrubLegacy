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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月3日
*
* @note 注释详细内容: GRUB Stage2 分析总结
* 
* 通过分析，这个核心模块主要的工作是完成了GRUB 这个微型操作系统的从磁盘到内存的
* 装载和运行。在asm.S 这个文件中提供了从汇编代码到C 代码转换的接口，也是从这里开
* 始正式载入了GRUB 这个微型操作系统，可以说是GRUB 运行的一个入口。同时，在asm.S
* 文件中，对底层的方法用汇编语言进行了封装，方便在以后的C 代码中调用。然后经过对
* BIOS 进行一些初始化以后，正式进入了GRUB 的主程序，即在stage2 中的cmain 入口。从
* 此这个微型的操作系统开始正式运行。然后值得注意的是buildin 这个数据结构，这个结构
* 就是GRUB 所有支持命令的数据结构。结构包括了一个用来识别的名字和一个用来调用的
* 方法。GRUB 通过接收外部输入的指令的方式，来间接的启动和装载其他的操作系统。
*/

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
* @note 注释详细内容: grub.conf的写法
* 
* default：定义默认引导的操作系统。0 表示第一个操作系统，1表示第2个，依此类推。
*
* hiddenmenu：用于启动时隐藏菜单，除非在timeout之前按下 ESC 才能看到菜单。
*
* timeout：定义多少秒内如果用户没有按下键盘上的某个按键，就自动引导 default 
*          所指定的操作系统。
*
* splashimage：指定启动的背景图片，一般为压缩文件。路径为绝对路径。
*
* title：定义引导项目的名称。
*
* root：指定boot分区所在磁盘及分区，如：root (hd0,0)。
*
* kernel：指定kernel文件所在绝对目录地址，如：
*
* kernel /boot/vmlinuz-2.6.18-92.el5 ro root=LABEL=/ rhgb quiet
*
* initrd：指定ramdisk盘所在绝对目录地址，如：
*
* initrd /boot/initrd-2.6.18-92.el5.img
* 
* 注意：
*
* kernel与initrd这两个设置项中，指定的路径都是绝对路径。因为这两个文件都
* 存放在/boot目录。而且/boot所在的分区已经在root (hd[0-n],[0-n])中指定，所以
* 就无需再指明kernel与initrd在哪个分区了。如果boot分区为独立分区，那么前面的
* /boot省略掉。如果boot分区为非独立分区，那么必须加上/boot。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现打开预设菜单的功能，将全局变量preset_menu_offset设置为0，且返回布尔
* 值preset_menu != 0,表面预设菜单是否存在。
* 
* GRUB_UTIL是在生成GRUB的模拟工具时才用的，因此正常工作的GRUB可以不做考虑。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现从preset_menu字符串中读取一个预设菜单行的功能。调用grub_memmove()函
* 数将preset_menu_offset所指的当前预设菜单行读取到参数buf空间中。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现关闭预设菜单的功能。实际是将preset_menu设置为0完成的。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现获得第num项菜单项的功能。参数list为菜单的起始位置；参数num为要略过的
* 菜单项个数，也就是后面要返回的第几项菜单项；参数nested只有在list的实际参数为
* config_entries才为1，而在list参数的实际参数为menu_entries时都为0。该函数返回对
* 应的菜单项。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现显示一个菜单项的功能。参数y为要显示的行号；参数highlight为是否高亮；
* 参数entry为要显示的菜单项。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现显示一组菜单项的功能。参数y为开始要显示的行；参数size为要显示的菜单
* 项的个数；参数first为第一个菜单项；参数entryno为菜单项编号；参数menu_entries为
* 要显示的菜单数组。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现在TERM_DUMB类型的终端(也就是不能做fancy things的终端)中显示一组菜单
* 项的功能。对TERM_DUMB类型的终端，直接调用grub_putchar()和grub_printf()实现菜
* 单的显示。参数size为要显示的菜单项个数；参数first为要显示的第一个菜单项；参数
* 为要显示的菜单项数组。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现显示菜单边界的功能。参数y为要显示的边界的第一行；参数size为菜单行数。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 函数run_menu()是grub 中整个菜单界面的主循环。这个函数的基本流程如下:
*
* 1) 首先有一个计时器grub_timout 进行计时，如果grub_timeout < 0，那么就强行显示
*    菜单。
* 2) 如果没有显示菜单，则在屏幕上显示"Press `ESC' to enter the menu..."，并进入
*    一个死循环中，当用户按下ESC 键，则马上显示菜单。
* 3) 如果grub_timout超时，那么就直接进入第一个，即默认的那个启动菜单项。
* 4) 如果显示菜单，则显示所有可以选择的入口。
* 5) 不论是否显示菜单，最后程序都将跳转到boot_entry。首先清空屏幕，把光标定于第
*    一行。然后再次进入循环，如果还没有设置启动入口，则通过调用get_entry()函数来
*    获取一个默认的入口。
* 6) 然后调用在【grub-0.97/stage2/cmdline.c】中的run_script()函数解释这个入口。
*    run_script()函数对这个入口以后的命令脚本使用find_command()函数进行解析。
*
* 下面描述该函数的实现细节:
* 
* 1. 主循环开始
*
*    run_menu()本身是一个循环程序，需要监控用户在菜单界面的输入并启动对应的菜单
*    项的执行（实际就是转化成命令的执行）。因此，一开始，就是一个restart标签，
*    后面会在需要重新开始循环时使用类似"goto restart;"的方式跳转到这个标签。
*
*    接着，检查当前的终端是否是所谓的TERM_DUMB，也就是同时显示所有的菜单项的终
*    端；如果不是这样的终端，那么一次最多显示11个菜单项，因此，如果参数entryno
*   （也就是默认要显示的菜单项）大于11，那么就递减entryno，并同时递增first_entry
*   （第一个要显示的菜单项）。
*
* 2. 处理等待超时
*
*   1）如果全局变量grub_timeout小于0，那么认为要么没被初始化，要么就已经被递减
*      到超时了，因此就强制显示菜单项（设置show_menu变量为1）。因此，就不会执行
*      下面的步骤，而是直接跳到下一节分析的代码。
*   2）如果show_menu变量为0，那么就不显示菜单项。并执行下面的步骤：
*      a) 获取当前时间到变量time1中。
*      b) 检查是否按下了ESC按键，如果按下了ESC按键，那么设置grub_timeout为-1，
*         并将show_menu设置为1，并退出，跳转到下一节要分析的代码。
*      c) 如果grub_timeout大于0，再次获取获取当前时间到变量time1中，并将这个值
*         保存到time2中（供下次比较）。
*      d) 如果grub_timeout递减到0，则设置grub_timeout为-1，并跳转到标号
*         boot_entry处，启动菜单项。
*
* 3. 显示菜单
*
*   1）	调用init_page ()，初始化分页功能，实际上是清屏幕之后打印一个GRUB版本的
*       字符串消息。
*   2）	调用setcursor (0)，将光标暂时禁止掉。实际上是调用当前终端的回调函数
*       current_term->setcursor (0)。
*   3）	如果当前终端是所谓的TERM_DUMB终端，那么直接调用print_entries_raw()，显
*       示所有的menu_entries。否则，就调用print_border (3, 12)先打印一条在y=3
*       位置的菜单边界。
*   4）	打印一条消息，告诉用户用DISP_UP和DISP_DOWN来选择高亮的菜单项。
*   5）	如果还没被认证，并要求密码，那么打印一条消息，告诉用户可以按Enter键直
*       接启动选中的菜单项，或者按'p'键来输入密码，从而打开下一步的特性功能。
*   6）	否则，如果有config_entries，那么打印一条消息告诉用户可以输入'e'来编辑
*       菜单项，或者输入'c'来进入命令行界面。如果没有config_entries，那么打印
*       一条消息，告诉用户可以输入'b'来启动，输入'e'来编辑菜单项，或者输入'c'
*       来进入命令行界面，或者'o'来在当前选中行的后面添加一行，或者输入'O'来在
*       当前选中的行前面添加一行，或者'd'来删除一行，或者输入ESC来回到主菜单。
*   7）	如果当前终端是TERM_DUMB类型终端，那么打印一条消息，输出当前显示的菜单
*       项。否则，调用print_entries()来从first_entry开始的menu_entries。
*
* 4. 循环等待用户输入
*
*   接下来的代码是用一个循环来等待用户的输入。在这个循环中，第一步还是检查是否
*   等待超时。
*
*   1）	如果超时，则设置grub_timeout为-1并跳出。
*   2）	否则，打印一条消息告诉用户选择的entryno会在grub_timeout秒之后被启动。
*   3）	接着，接收用户输入，并将之转换为ASCII字符。
*
* 5. 解析并执行用户输入
*
*   1)	输入的按键是'^' 或者 'v'，则递减或者递增entryno，并调用print_entry()来
*       更新菜单显示。
*   2)	输入的按键是"Page Up"或者"Page Down"，则递减（或者递增）first_entry值12，
*       并更新entryno，然后调用print_entries()来整体重新显示菜单页。
*   3)	如果有config_entries，则如果输入命令是'\n'或者'\r'，或者值6，那么直接
*       退出命令等待循环，进入后面的boot_entry代码处，启动选中的菜单项。
*   4)	如果输入命令为'd'，'o'，或者'O'，则处理删除一行，在选中行后添加一行，
*       或者在选中行前添加一行等功能。
*   5)	如果输入值为27，直接return。
*   6)	如果输入值为'b'，则跳出循环，到boot_entry代码处。
*   7）	如果尚未认证并且要求输入密码，则当用户输入'p'时，进入输入密码验证。
*       最后"goto restart"跳回住菜单循环开始处。
*   8）	如果输入命令是'e'，则进入编辑当前选中菜单项模式。
*   9）如果输入的是'c'，则进入命令行模式。
*
* 6. 执行选中菜单项
*
*   如果在菜单界面中选中了要启动的项，并按下了Enter键，那么就会进入菜单项的执行。
*   实际上是执行了下列步骤：
*   1）	清屏。
*   2）	设置显示光标。
*   3）	显示要启动的菜单项。
*   4）	获取选中的菜单项到cur_entry。
*   5）	调用run_script()执行该菜单项，如果失败，则退回entryno到fallback_entryno。
*   6）	任何失败或者完成（除非真的启动了OS或者chainloader），都返回到restart标签。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 该函数实现从配置文件(或者预设菜单)中读取一个命令行的功能。参数cmdline为输出参
* 数，被读取的命令行会被保存到这个空间中；参数maxlen为最大读取的字节数；参数
* read_from_file为标志是从配置文件还是从预设菜单中读取命令行。该函数返回读取的
* 命令行的字节数。
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
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @note 注释详细内容:
* 
* 文件【grub-0.97/stage2/stage2.c】中的cmain() 函数就是GRUB 0.97的主体函数，这
* 相当于一个小型的操作系统（虽然没有进程调度，而是通过单一的死循环实现，整个
* Stage2 就在这个死循环中运行）。下面对这个死循环执行的过程进行分析。
*
* 1) 对内部变量进行初始化
*
* 进入死循环的第一个操作时调用reset()函数对Stage2 的内部变量进行初始化。
*
* 这个reset函数基本上完成了如下功能：
*
* a) 将【grub-0.97/stage2/char_io.c】中的全局变量count_lines复位为-1。
* b) 将cmain()函数的config_len，menu_len, num_entries等变量初始化为0。
* c) 将config_entries指向一块位于【mbi.drives_addr + mbi.drives_length】的缓冲区。
* d) 将menu_entries指向MENU_BUF所在地址的一块缓冲区。在【grub-0.97/stage2/shared.h】
*    中定义了一些缓冲区地址。
* e) 调用【grub-0.97/stage2/builtins.c】中的init_config()初始化一些配置数据。
*
* 2) 打开一个"默认文件"
*
* 接着的代码尝试打开一个"默认文件"，即"/boot/grub/ default"。这段代码完成了以下
* 功能：
*
* a) 从config_file的名字获得default_file的名字。首先，将default_file的第一个字节
*    设为0，因此接着的grub_strncat()就会直接将config_file的内容拼接到default_file
*    开始的内容里面。而config_file变量在【grub-0.97/stage2/asm.S】中定义为字符串
*    "/boot/grub/menu.lst"。接着，从生成的default_file的最后一个字节向前搜索，直
*    到找到"/"字符，然后再将"/"字符后面的字符设为0，并调用grub_strncat()再次来拼
*    接，从而将default_file变量变成了"/boot/grub/ default "。
* b) 接着，使用grub_open (default_file)尝试打开这个default_file，也就是
     "/boot/grub/ default "文件。注意，到这里因为是Stage2，因此已经可以支持所有
     可以支持的文件系统，从而grub_open()会尝试使用所有可用的文件系统支持代码来打
     开，直到最后打开这个文件。
* c) 如果打开成功，就会调用grub_read()来读取这个文件的内容到buf中（实际只读入对
     多10个字节）。
* d) 如果读取成功，就会调用safe_parse_maxint()来将读入的buf转换成一个整数。这个
     整数对应于默认要启动操作系统的索引号，被保存到变量saved_entryno中。这就是
     这段代码的主要目的--识别一个默认启动目标操作系统。
* e) 关闭这个default_file（即"/boot/grub/ default "）。
*
* 3) 判断从预设菜单还是配置文件读取命令
*
* 再接下来的代码是一个do…while()循环，是一个在cmain()的死循环内部的一个嵌套循环。
* 在这个内部嵌套循环中要完成一些对预设菜单的解析工作，见下面的分析。
* 
* I）通过open_preset_menu()函数尝试打开已经设置好的菜单。如果用户没有设置好的菜
* 单，那么将返回0，如果已经设置好了菜单则不返回0。
*
* 这里的open_preset_menu()，read_from_preset_menu()以及close_preset_menu()这几
* 个函数都是在定义了(PRESET_MENU_STRING) || defined(SUPPORT_DISKLESS)的情况下才
* 有意义，否则将默认返回为0或者执行空操作。
*
* a) 其中PRESET_MENU_STRING是在configure的时候使用了"--enable-preset-menu=FILE"
*    参数才会有定义。如果使用了该参数，那么configure会在配置的时候就会从这个文件
*    读入一个字符串，并将这个字符串使用"#define PRESET_MENU_STRING"的方式定义到
*    config.h中，否则，这个宏是不会被定义的。
* b) 此外，SUPPORT_DISKLESS也是要再configure的时候使用"--enable-diskless"才能打
*    开的宏，否则不定义。
* c) 因此，默认情况下我们直接使用configure来配置的话，这两样都是不被打开的，因此
*    这几个函数要么将默认返回为0要么执行空操作。
* d) 如果的确打开了(PRESET_MENU_STRING) || defined(SUPPORT_DISKLESS)，那么
*    open_preset_menu()就会返回preset_menu是否为空（在第一次调用时，因为在这个
*    前提下这二者之一已经定义，因此不会为空），并且将preset_menu_offset设为0，
*    也就是使得后面的read_from_preset_menu()函数会从这个preset_menu指向的缓冲区
*    的第一个字节开始读；而read_from_preset_menu()函数实际上实现了将preset_menu
*    指向的缓冲区读入到参数buf的功能（直到该缓存指向的字符串末尾）。
*    close_preset_menu()就是将preset_menu设为空，从而open_preset_menu()再次调用
*    时就不会再打开。

* II) 如果没有成功打开预设菜单，那么将通过grub_open()函数尝试打开config_file
*   （实际上前面已经分析过，这个文件就是"/boot/grub/menu.lst"）。如果仍然打开
*   失败，则跳出整个循环。
*
* 到此为止，变量is_preset代表是否打开了预设菜单（preset_menu），如果没有打开预
* 设菜单，那么is_opened就代表是否打开了配置文件（config_file，即
* "/boot/grub/menu.lst"）。
*
* 4) 从预设菜单或配置文件读入命令并解析处理
*
* 根据前一步打开的情况来判断是从预设菜单读入还是从配置文件读入命令（这里
* "!is_preset"实际代表的是是否打开了配置文件"/boot/grub/menu.lst"）。实际上是通
* 过把!is_preset 传入get_line_from_config()函数，将对应的命令读入到cmline 中。
* 然后通过find_command()函数查找有没有这条命令。使用while()循环实现对这个
* get_line_from_config()的多次调用(直到返回值为0表示读到文件末尾)，因此就是要将
* 对应的预设菜单或者配置文件里的所有命令都读完。
*
* 代码对读入的每一行,都尝试解释,寻找对应的命令,返回到"struct builtin *builtin"中。
* 对每个命令进行解析和处理：
*
* I）如果命令是"title"开始的BUILTIN_TITLE命令，则做特殊处理。在配置文件或者预设
*    菜单中，对于每个可启动的操作系统，都会以"title"命令开始一个新的项目。
* II) 如果遇到的命令是类似"dhcp"这样的BUILTIN_MENU内建菜单命令，则直接运行该命令
*    对应的处理函数。在文件【grub-0.97/stage2/builtins.c】中定义了许多这样的
*    BUILTIN_MENU内建菜单命令。
* III) 在处理完一个命令行后，将menu_entries和config_entries的最后一个字节设为0
*   （字符串末尾）；然后调用grub_memmove()将menu_entries拼接到config_entries的
*    后面。并将menu_entries重新指向在config_entries的后面的搬移过去的位置
*   （config_entries + config_len）。
*
* 5) 确保fallback和default项正确
*
* 6) 选择运行命令行还是运行菜单
*
* 如果没有可以显示的菜单项（num_entries等于0），就会调用enter_cmdline()进入命令
* 行接口。否则，将调用run_menu()显示一个菜单，其中每个菜单项来自在前面解析配置
* 文件（或者预设菜单）得出的menu_entries。参考【附录D：GRUB 0.97的启动项编辑操
* 作】，如果选择编辑每个菜单项，就会进入下一级菜单，里面的每个菜单项是从
* config_entries得出的对应于选定一级菜单项的配置数据。
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
