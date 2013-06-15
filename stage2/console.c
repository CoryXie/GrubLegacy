/* term_console.c - console input and output */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002  Free Software Foundation, Inc.
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

/* These functions are defined in asm.S instead of this file:
   console_putchar, console_checkkey, console_getkey, console_getxy,
   console_gotoxy, console_cls, and console_nocursor.  */

int console_current_color = A_NORMAL;
static int console_standard_color = A_NORMAL;
static int console_normal_color = A_NORMAL;
static int console_highlight_color = A_REVERSE;
static color_state console_color_state = COLOR_STATE_STANDARD;
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
* 本函数实现设置console的颜色状态的功能。主要有如下选项: 
*
* - COLOR_STATE_STANDARD
* - COLOR_STATE_NORMAL
* - COLOR_STATE_HIGHLIGHT
*/
void
console_setcolorstate (color_state state)
{
  switch (state) {
    case COLOR_STATE_STANDARD:
      console_current_color = console_standard_color;
      break;
    case COLOR_STATE_NORMAL:
      console_current_color = console_normal_color;
      break;
    case COLOR_STATE_HIGHLIGHT:
      console_current_color = console_highlight_color;
      break;
    default:
      console_current_color = console_standard_color;
      break;
  }

  console_color_state = state;
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
* 本函数实现设置console的颜色的功能。将全局变量console_normal_color和console_highlight_color
* 设置为参数normal_color和highlight_color对应的颜色值，然后调用console_setcolorstate()
* 来更改颜色状态。
*/
void
console_setcolor (int normal_color, int highlight_color)
{
  console_normal_color = normal_color;
  console_highlight_color = highlight_color;

  console_setcolorstate (console_color_state);
}
