/* cmdline.c - the device-independent GRUB text command line */
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

grub_jmp_buf restart_cmdline_env;
/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现查找下一个有效命令行字符串参数的功能。实际上是跳到第一个空格，制表符，
* 以及在参数after_equal为非零时跳到第一个等号，并继续跳过这些字符，直到第一个不是
* 这些字符中的字符，并返回到此的指针。
*/
/* Find the next word from CMDLINE and return the pointer. If
   AFTER_EQUAL is non-zero, assume that the character `=' is treated as
   a space. Caution: this assumption is for backward compatibility.  */
char *
skip_to (int after_equal, char *cmdline)
{
  /* Skip until we hit whitespace, or maybe an equal sign. */
  while (*cmdline && *cmdline != ' ' && *cmdline != '\t' &&
	 ! (after_equal && *cmdline == '='))
    cmdline ++;

  /* Skip whitespace, and maybe equal signs. */
  while (*cmdline == ' ' || *cmdline == '\t' ||
	 (after_equal && *cmdline == '='))
    cmdline ++;

  return cmdline;
}

/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现打印一条命令行帮助消息的功能。
*/

/* Print a helpful message for the command-line interface.  */
void
print_cmdline_message (int forever)
{
  printf (" [ Minimal BASH-like line editing is supported.  For the first word, TAB\n"
	  "   lists possible command completions.  Anywhere else TAB lists the possible\n"
	  "   completions of a device/filename.%s ]\n",
	  (forever ? "" : "  ESC at any time exits."));
}
/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现查找名字为参数command的命令的功能。通过比较builtin_table中的每一项
* 的name字段是否与参数command相同来识别命令。返回找到的struct builtin的指针。
*
* 函数find_command()执行了如下步骤：
*
* 1）对输入参数command进行预处理，找到第一个空格，然后认为空格之前的可能是一个
*    命令的名字，因此对这个位置设为0，使得第一个空格之前的缓冲区成为一个独立的
*    字符串。
*
* 2）将前一步得到的字符串针对整个builtin_table表进行遍历，比较被遍历的每一项中
*    的name字段。如果在表中发现了这个命令，则返回指向当前builtin 结构的指针。
*    如果没有发现这个命令则返回0 同时返回一个errnum为ERR_UNRECOGNIZED，表示没
*    找到可识别的内建命令。
*
*    如果成功的找到了一条指令，然后通过调用在【grub-0.97/stage2/cmdline.c】中
*    的skip_to()函数，获得当前builtin 指针所指向结构的命令的参数。
*
*    这个skip_to()函数就是对输入的命令行，从头开始，比较每个字符，略过所有的
*    "非字符串结尾，空格，TAB键，或者在after_equal为非0时的非'='"；然后再略过
*    "字符串结尾，空格，TAB键，或者在after_equal为非0时的'='"，从而到达参数部分，
*    并返回得到的参数部分的指针。
*/
/* Find the builtin whose command name is COMMAND and return the
   pointer. If not found, return 0.  */
struct builtin *
find_command (char *command)
{
  char *ptr;
  char c;
  struct builtin **builtin;

  /* Find the first space and terminate the command name.  */
  ptr = command;
  while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '=')
    ptr ++;

  c = *ptr;
  *ptr = 0;

  /* Seek out the builtin whose command name is COMMAND.  */
  for (builtin = builtin_table; *builtin != 0; builtin++)
    {
      int ret = grub_strcmp (command, (*builtin)->name);

      if (ret == 0)
	{
	  /* Find the builtin for COMMAND.  */
	  *ptr = c;
	  return *builtin;
	}
      else if (ret < 0)
	break;
    }

  /* Cannot find COMMAND.  */
  errnum = ERR_UNRECOGNIZED;
  *ptr = c;
  return 0;
}
/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现初始化命令行的功能。初始化一些全局变量。例如，这里保存了启动磁盘到
* saved_drive和分区到saved_partition，并将当前磁盘current_drive设置为
* GRUB_INVALID_DRIVE。此后调用init_builtins()函数，来自
*【grub-0.97/stage2/builtins.c】。
*/
/* Initialize the data for the command-line.  */
static void
init_cmdline (void)
{
  /* Initialization.  */
  saved_drive = boot_drive;
  saved_partition = install_partition;
  current_drive = GRUB_INVALID_DRIVE;
  errnum = 0;
  count_lines = -1;
  
  /* Restore memory probe state.  */
  mbi.mem_upper = saved_mem_upper;
  if (mbi.mmap_length)
    mbi.flags |= MB_INFO_MEM_MAP;

  /* Initialize the data for the builtin commands.  */
  init_builtins ();
}
/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现命令行主循环的功能。主要执行的步骤如下:
*
* 1) 调用init_cmdline()初始化一些全局变量。
* 2) grub_setjmp (restart_cmdline_env)来保存当前的环境上下文，从而在后面的处理
*    中可以重新回到这里。例如，在【grub-0.97/stage2/builtins.c】中对terminal命令的
*    处理函数terminal_func()中，最后就会通过grub_longjmp()跳转回到这里。
* 3) 进入命令主循环。
* 
* 实际上，这个命令行主循环非常简单，完成了如下工作：
*
* a）调用"get_cmdline (PACKAGE "> ", heap, 2048, 0, 1)"等待并读取用户在命令行输
*    入命令，保存在heap缓冲区中。
* b）调用"builtin = find_command (heap)"解析保存在heap缓冲区中的输入命令，返回
*    到由struct builtin *builtin指向的命令结构中。
* c）如果这个struct builtin *builtin指向的命令结构不是BUILTIN_CMDLINE类型的命令，
*    那么不予执行，而是接着继续回到前面读取命令。
* d）使用"arg = skip_to (1, heap)"跳过命令本身，从命令行中获取命令的参数字符串。
* e）使用"(builtin->func) (arg, BUILTIN_CMDLINE)"实际调用命令的处理函数。
*/
/* Enter the command-line interface. HEAP is used for the command-line
   buffer. Return only if FOREVER is nonzero and get_cmdline returns
   nonzero (ESC is pushed).  */
void
enter_cmdline (char *heap, int forever)
{
  /* Initialize the data and print a message.  */
  init_cmdline ();
  grub_setjmp (restart_cmdline_env);
  init_page ();
#ifdef SUPPORT_DISKLESS
  print_network_configuration ();
  grub_putchar ('\n');
#endif
  print_cmdline_message (forever);
  
  while (1)
    {
      struct builtin *builtin;
      char *arg;

      *heap = 0;
      print_error ();
      errnum = ERR_NONE;

      /* Get the command-line with the minimal BASH-like interface.  */
      if (get_cmdline (PACKAGE "> ", heap, 2048, 0, 1))
	return;

      /* If there was no command, grab a new one. */
      if (! heap[0])
	continue;

      /* Find a builtin.  */
      builtin = find_command (heap);
      if (! builtin)
	continue;

      /* If BUILTIN cannot be run in the command-line, skip it.  */
      if (! (builtin->flags & BUILTIN_CMDLINE))
	{
	  errnum = ERR_UNRECOGNIZED;
	  continue;
	}

      /* Invalidate the cache, because the user may exchange removable
	 disks.  */
      buf_drive = -1;

      /* Start to count lines, only if the internal pager is in use.  */
      if (use_pager)
	count_lines = 0;
      
      /* Run BUILTIN->FUNC.  */
      arg = skip_to (1, heap);
      (builtin->func) (arg, BUILTIN_CMDLINE);

      /* Finish the line count.  */
      count_lines = -1;
    }
}
/**
* @brief 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
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
* 本函数实现运行命令行脚本的功能。主要执行的步骤如下:
*
* 1) 调用init_cmdline()初始化一些全局变量。
* 2) 进入命令脚本主循环。
* 
* a）调用grub_memmove()拷贝一个命令行，保存在heap缓冲区中。
* b）调用"builtin = find_command (heap)"解析保存在heap缓冲区中的输入命令，返回
*    到由struct builtin *builtin指向的命令结构中。
* c）如果这个struct builtin *builtin指向的命令结构不是BUILTIN_CMDLINE类型的命令，
*    那么不予执行，而是接着继续回到前面读取命令。
* d）使用"arg = skip_to (1, heap)"跳过命令本身，从命令行中获取命令的参数字符串。
* e）使用"(builtin->func) (arg, BUILTIN_CMDLINE)"实际调用命令的处理函数。
*/
/* Run an entry from the script SCRIPT. HEAP is used for the
   command-line buffer. If an error occurs, return non-zero, otherwise
   return zero.  */
int
run_script (char *script, char *heap)
{
  char *old_entry;
  char *cur_entry = script;

  /* Initialize the data.  */
  init_cmdline ();

  while (1)
    {
      struct builtin *builtin;
      char *arg;

      print_error ();

      if (errnum)
	{
	  errnum = ERR_NONE;

	  /* If a fallback entry is defined, don't prompt a user's
	     intervention.  */
	  if (fallback_entryno < 0)
	    {
	      grub_printf ("\nPress any key to continue...");
	      (void) getkey ();
	    }
	  
	  return 1;
	}

      /* Copy the first string in CUR_ENTRY to HEAP.  */
      old_entry = cur_entry;
      while (*cur_entry++)
	;

      grub_memmove (heap, old_entry, (int) cur_entry - (int) old_entry);
      if (! *heap)
	{
	  /* If there is no more command in SCRIPT...  */

	  /* If any kernel is not loaded, just exit successfully.  */
	  if (kernel_type == KERNEL_TYPE_NONE)
	    return 0;

	  /* Otherwise, the command boot is run implicitly.  */
	  grub_memmove (heap, "boot", 5);
	}

      /* Find a builtin.  */
      builtin = find_command (heap);
      if (! builtin)
	{
	  grub_printf ("%s\n", old_entry);
	  continue;
	}

      if (! (builtin->flags & BUILTIN_NO_ECHO))
	grub_printf ("%s\n", old_entry);

      /* If BUILTIN cannot be run in the command-line, skip it.  */
      if (! (builtin->flags & BUILTIN_CMDLINE))
	{
	  errnum = ERR_UNRECOGNIZED;
	  continue;
	}

      /* Invalidate the cache, because the user may exchange removable
	 disks.  */
      buf_drive = -1;

      /* Run BUILTIN->FUNC.  */
      arg = skip_to (1, heap);
      (builtin->func) (arg, BUILTIN_SCRIPT);
    }
}
