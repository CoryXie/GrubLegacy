/* hercules.c - hercules console interface */
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

#ifdef SUPPORT_HERCULES

#include <shared.h>
#include <hercules.h>
#include <term.h>

/* The position of the cursor.  */
static int herc_x;
static int herc_y;

static int herc_standard_color = A_NORMAL;
static int herc_normal_color = A_NORMAL;
static int herc_highlight_color = A_REVERSE;
static int herc_current_color = A_NORMAL;
static color_state herc_color_state = COLOR_STATE_STANDARD;
static int herc_cursor_state = 1;
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
* 本函数实现写端口的功能。参数port指定要写的端口地址；参数value指定要写入的值。
*/
/* Write a byte to a port.  */
static inline void
outb (unsigned short port, unsigned char value)
{
  asm volatile ("outb	%b0, %w1" : : "a" (value), "Nd" (port));
}
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
* 本函数实现设置HERCULES控制台光标位置的功能。
*/
static void
herc_set_cursor (void)
{
  unsigned offset = herc_y * HERCULES_WIDTH + herc_x;
  
  outb (HERCULES_INDEX_REG, 0x0f);
  outb (0x80, 0);
  outb (HERCULES_DATA_REG, offset & 0xFF);
  outb (0x80, 0);
  
  outb (HERCULES_INDEX_REG, 0x0e);
  outb (0x80, 0);
  outb (HERCULES_DATA_REG, offset >> 8);
  outb (0x80, 0);
}
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
* 本函数实现输出字符到HERCULES控制台的功能。
*/
void
hercules_putchar (int c)
{
  switch (c)
    {
    case '\b':
      if (herc_x > 0)
	herc_x--;
      break;
      
    case '\n':
      herc_y++;
      break;
      
    case '\r':
      herc_x = 0;
      break;

    case '\a':
      break;

    default:
      {
	volatile unsigned short *video
	  = (unsigned short *) HERCULES_VIDEO_ADDR;
	
	video[herc_y * HERCULES_WIDTH + herc_x]
	  = (herc_current_color << 8) | c;
	herc_x++;
	if (herc_x >= HERCULES_WIDTH)
	  {
	    herc_x = 0;
	    herc_y++;
	  }
      }
      break;
    }

  if (herc_y >= HERCULES_HEIGHT)
    {
      volatile unsigned long *video = (unsigned long *) HERCULES_VIDEO_ADDR;
      int i;
      
      herc_y = HERCULES_HEIGHT - 1;
      grub_memmove ((char *) HERCULES_VIDEO_ADDR,
		    (char *) HERCULES_VIDEO_ADDR + HERCULES_WIDTH * 2,
		    HERCULES_WIDTH * (HERCULES_HEIGHT - 1) * 2);
      for (i = HERCULES_WIDTH * (HERCULES_HEIGHT - 1) / 2;
	   i < HERCULES_WIDTH * HERCULES_HEIGHT / 2;
	   i++)
	video[i] = 0x07200720;
    }
}
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
* 本函数实现清屏HERCULES控制台的功能。
*/
void
hercules_cls (void)
{
  int i;
  volatile unsigned long *video = (unsigned long *) HERCULES_VIDEO_ADDR;
  
  for (i = 0; i < HERCULES_WIDTH * HERCULES_HEIGHT / 2; i++)
    video[i] = 0x07200720;

  herc_x = herc_y = 0;
  herc_set_cursor ();
}
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
* 本函数实现获取HERCULES控制台当前光标位置的功能。
*/
int
hercules_getxy (void)
{
  return (herc_x << 8) | herc_y;
}
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
* 本函数实现设置HERCULES控制台光标到特定x,y位置的功能。
*/
void
hercules_gotoxy (int x, int y)
{
  herc_x = x;
  herc_y = y;
  herc_set_cursor ();
}
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
* 本函数实现设置HERCULES控制台颜色状态的功能。
*/
void
hercules_setcolorstate (color_state state)
{
  switch (state) {
    case COLOR_STATE_STANDARD:
      herc_current_color = herc_standard_color;
      break;
    case COLOR_STATE_NORMAL:
      herc_current_color = herc_normal_color;
      break;
    case COLOR_STATE_HIGHLIGHT:
      herc_current_color = herc_highlight_color;
      break;
    default:
      herc_current_color = herc_standard_color;
      break;
  }

  herc_color_state = state;
}
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
* 本函数实现设置HERCULES控制台颜色的功能。
*/
void
hercules_setcolor (int normal_color, int highlight_color)
{
  herc_normal_color = normal_color;
  herc_highlight_color = highlight_color;
  
  hercules_setcolorstate (herc_color_state);
}
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
* 本函数实现设置HERCULES控制台开启/关闭的功能。
*/
int
hercules_setcursor (int on)
{
  int old_state = herc_cursor_state;
  
  outb (HERCULES_INDEX_REG, 0x0a);
  outb (0x80, 0);
  outb (HERCULES_DATA_REG, on ? 0 : (1 << 5));
  outb (0x80, 0);
  herc_cursor_state = on;

  return old_state;
}

#endif /* SUPPORT_HERCULES */
