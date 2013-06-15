/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001   Free Software Foundation, Inc.
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

#define ASM_FILE
#include <shared.h>

#ifndef STAGE1_5
#include <stage2_size.h>
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
* @note 注释详细内容:
* 
* GURB代码从功能上主要分为Stage1（一般安装在启动扇区MBR）和Stage2（主要功能，
* 包括加载/boot/grub/menu.lst和执行GRUB 的shell命令脚步）。但是由于一个扇区实在太小，
* 不能包含识别多种文件系统的代码，而Stage2较大，如果简单按照连续扇区的方式存储不是
* 很合适，因此往往为了存储Stage2还需要文件系统。为了能在各种文件系统中存储Stage2
* （并在启动时可以寻找到并加载之），引入了Stage1.5的概念。在代码中，Stage1.5部分
* 是加载后面真正Stage2的。e2fs_stage1_5, fat_stage1_5, ffs_stage1_5, jfs_stage1_5,
*  minix_stage1_5, reiserfs_stage1_5, vstafs_stage1_5, xfs_stage1_5这些模块都是属于
*  Stage1.5的，可以看见这些都是属于某种文件系统的识别代码。
*
* 源文件【grub-0.97/stage2/start.S】属于Stage2的总入口，该部分代码跟MBR所在的
* Stage1代码都只占据1个扇区磁盘容量，且其位置恰好位于MBR后，位于扇区编号为1的
* 扇区。在这个文件中，实现了如下功能：如果编译时定义了STAGE1_5，那么就先加载
* STAGE1_5对应代码，并跳转到Stage1.5让Stage1.5的代码来通过文件系统识别Stage2
* 并加载；如果没定义STAGE1_5，那么就接着1号扇区连续加载STAGE2_SIZE个扇区的
* Stage2代码。下面我们就对这个文件进行详细分析。
**/	
/*
 *  defines for the code go here
 */

	/* Absolute addresses
	   This makes the assembler generate the address without support
	   from the linker. (ELF can't relocate 16-bit addresses!) */
#ifdef STAGE1_5
# define ABS(x) (x-_start+0x2000)
#else
# define ABS(x) (x-_start+0x8000)
#endif /* STAGE1_5 */
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
* 首先是与【grub-0.97/stage1/stage1.S】中有类似功能的ABS(x)宏定义，这个ABS(x)
* 的功能也是能从符号相对于_start的偏移而得到其绝对地址。
*
* 而从的附录【A.3 GRUB 0.97的stage2编译】中的分析我们可以知道，对于没有STAGE1_5的
* 情形，Stage2是被链接在0x8000处的；而对于有STAGE1_5的情形，Stage2是被链接到0x2000的。
* 这也就算为什么下面的ABS(x)会根据有没有定义STAGE1_5而有两种不同的定义。
*/	
	/* Print message string */
#define MSG(x)	movw $ABS(x), %si; call message

	.file	"start.S"

	.text

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
* 实际的代码是从标号_start开始的，并且到目前为止，仍然处于16位模式。从对Stage1的
* 分析可知，这个start.S的代码是被加载到0x8000处的，且是跳转到0x8000处继续执行的。
* 因此，寄存器和堆栈都还是沿用Stage1留下来的。
*
* 首先是将DX（即启动磁盘号）和SI寄存器保存到堆栈上。然后调用MSG()输出一条通知消息。
* 而这里的notification_string也是根据STAGE1_5来区别定义的。
*
* 接着的代码是尝试根据从firstlist - BOOTSEC_LISTSIZE开始的数据区间，判断要读取的
* 扇区数，并继续读取直到读完，然后尝试跳转并运行读取进来的代码。
*
* 为了分析这一段代码的工作原理，我们必须看看firstlist - BOOTSEC_LISTSIZE这个数据
* 区间的结构。
*
* 首先，BOOTSEC_LISTSIZE是在【grub-0.97/stage2/shared.h】中定义的，其值为8。
* 而这段数据区间开始于_start + 0x200 - BOOTSEC_LISTSIZE，实际上，就是
*【grub-0.97/stage2/start.S】这个512字节的最后8个字节。
*
* 回到前面的代码，“movw	$ABS(firstlist - BOOTSEC_LISTSIZE), %di”实际上就是将这
* 里得到的_start + 0x200 - BOOTSEC_LISTSIZE（也就是blocklist_default_start的地址）
* 的绝对地址放到DI寄存器中。
*
* 而“movl	(%di), %ebp”就是将blocklist_default_start地址处的值，默认是2，
* 放到EBP寄存器中。
*
* 再下一条“cmpw	$0, 4(%di)”就是看blocklist_default_len地址处的值是否为0，而这个
* blocklist_default_len处的值根据有没有定义STAGE1_5而有所不同，在没有定义
* STAGE1_5 的情况下，是(STAGE2_SIZE + 511) >> 9，也就是没有Stage1.5时要读入真正的
* Stage2的大小（加511表示至少要保证所有的Stage2代码都要读入，而右移9位表示转换
* 成512字节的扇区数）。在定义了STAGE1_5的情况下，这里blocklist_default_len处的
* 值虽然在编译的时候还是0，但是如这里的注释所言，会在安装的时候，由安装程序根
* 据实际的Stage1.5的代码（看具体是要支持什么文件系统）的大小来填入。关于这一点，
* 后面的附录B在安装分析处会详细介绍是如何填入的，这里暂时略过。
* 
* 如果判断出不需要读取扇区，“je	bootit”就会跳转到bootit处执行，否则就会接着
* 执行setup_sectors处的代码，进而通过“cmpb	$0, -1(%si)”判读是使用LBA还是使用
* CHS模式读取扇区。这里读取扇区的代码与Stage1中的代码工作原理完全一样，我们不再
* 详述。有一点要说明的是，由于Phoenix EDD BIOS只能最多一次读取0x7F个扇区，
* 因此对每次读取的扇区数有所限制，每次选择可读的最大扇区数，然后从4(%di)
* 处减去读取的扇区数，那么4(%di)这里就是剩下还需要读取的扇区数。每次读完，
* 都会到copy_buffer处将读入的扇区拷贝到目的地址。而这里拷贝的目的地址，是
* 根据blocklist_default_seg处的值，并按照是否有STAGE1_5来决定的。如果有定
* 义STAGE1_5，则拷贝到0x220段地址处，实际对应0x2200线性地址处；如果没有定
* 义STAGE1_5，则拷贝到0x820段地址处，实际对应0x8200线性地址处。
*/
	/* Tell GAS to generate 16-bit instructions so that this code works
	   in real mode. */
	.code16

	.globl	start, _start
start:
_start:	
	/*
	 * _start is loaded at 0x8000 and is jumped to with
	 * CS:IP 0:0x8000 in stage2.
	 */

	/* 
	 * we continue to use the stack for stage1 and assume that
	 * some registers are set to correct values. See stage1.S
	 * for more information.
	 */
	
	/* save drive reference first thing! */
	pushw	%dx

	/* print a notification message on the screen */
	pushw	%si
	MSG(notification_string)
	popw	%si
	
	/* this sets up for the first run through "bootloop" */
	movw	$ABS(firstlist - BOOTSEC_LISTSIZE), %di

	/* save the sector number of the second sector in %ebp */
	movl	(%di), %ebp

        /* this is the loop for reading the secondary boot-loader in */
bootloop:

	/* check the number of sectors to read */
	cmpw	$0, 4(%di)

	/* if zero, go to the start function */
	je	bootit

setup_sectors:	
	/* check if we use LBA or CHS */
	cmpb	$0, -1(%si)

	/* jump to chs_mode if zero */
	je	chs_mode

lba_mode:	
	/* load logical sector start */
	movl	(%di), %ebx

	/* the maximum is limited to 0x7f because of Phoenix EDD */
	xorl	%eax, %eax
	movb	$0x7f, %al

	/* how many do we really want to read? */
	cmpw	%ax, 4(%di)	/* compare against total number of sectors */

	/* which is greater? */
	jg	1f

	/* if less than, set to total */
	movw	4(%di), %ax

1:	
	/* subtract from total */
	subw	%ax, 4(%di)

	/* add into logical sector start */
	addl	%eax, (%di)

	/* set up disk address packet */

	/* the size and the reserved byte */
	movw	$0x0010, (%si)

	/* the number of sectors */
	movw	%ax, 2(%si)

	/* the absolute address (low 32 bits) */
	movl	%ebx, 8(%si)

	/* the segment of buffer address */
	movw	$BUFFERSEG, 6(%si)

	/* save %ax from destruction! */
	pushw	%ax

	/* zero %eax */
	xorl	%eax, %eax

	/* the offset of buffer address */
	movw	%ax, 4(%si)

	/* the absolute address (high 32 bits) */
	movl	%eax, 12(%si)


/*
 * BIOS call "INT 0x13 Function 0x42" to read sectors from disk into memory
 *	Call with	%ah = 0x42
 *			%dl = drive number
 *			%ds:%si = segment:offset of disk address packet
 *	Return:
 *			%al = 0x0 on success; err code on failure
 */

	movb	$0x42, %ah
	int	$0x13

	jc	read_error

	movw	$BUFFERSEG, %bx
	jmp	copy_buffer
			
chs_mode:	
	/* load logical sector start (bottom half) */
	movl	(%di), %eax

	/* zero %edx */
	xorl	%edx, %edx

	/* divide by number of sectors */
	divl	(%si)

	/* save sector start */
	movb	%dl, 10(%si)

	xorl	%edx, %edx	/* zero %edx */
	divl	4(%si)		/* divide by number of heads */

	/* save head start */
	movb	%dl, 11(%si)

	/* save cylinder start */
	movw	%ax, 12(%si)

	/* do we need too many cylinders? */
	cmpw	8(%si), %ax
	jge	geometry_error

	/* determine the maximum sector length of this read */
	movw	(%si), %ax	/* get number of sectors per track/head */

	/* subtract sector start */
	subb	10(%si), %al

	/* how many do we really want to read? */
	cmpw	%ax, 4(%di)	/* compare against total number of sectors */


	/* which is greater? */
	jg	2f

	/* if less than, set to total */
	movw	4(%di), %ax

2:	
	/* subtract from total */
	subw	%ax, 4(%di)

	/* add into logical sector start */
	addl	%eax, (%di)

/*
 *  This is the loop for taking care of BIOS geometry translation (ugh!)
 */

	/* get high bits of cylinder */
	movb	13(%si), %dl

	shlb	$6, %dl		/* shift left by 6 bits */
	movb	10(%si), %cl	/* get sector */

	incb	%cl		/* normalize sector (sectors go
					from 1-N, not 0-(N-1) ) */
	orb	%dl, %cl	/* composite together */
	movb	12(%si), %ch	/* sector+hcyl in cl, cylinder in ch */

	/* restore %dx */
	popw	%dx
	pushw	%dx

	/* head number */
	movb	11(%si), %dh

	pushw	%ax	/* save %ax from destruction! */

/*
 * BIOS call "INT 0x13 Function 0x2" to read sectors from disk into memory
 *	Call with	%ah = 0x2
 *			%al = number of sectors
 *			%ch = cylinder
 *			%cl = sector (bits 6-7 are high bits of "cylinder")
 *			%dh = head
 *			%dl = drive (0x80 for hard disk, 0x0 for floppy disk)
 *			%es:%bx = segment:offset of buffer
 *	Return:
 *			%al = 0x0 on success; err code on failure
 */

	movw	$BUFFERSEG, %bx
	movw	%bx, %es	/* load %es segment with disk buffer */

	xorw	%bx, %bx	/* %bx = 0, put it at 0 in the segment */
	movb	$0x2, %ah	/* function 2 */
	int	$0x13

	jc	read_error

	/* save source segment */
	movw	%es, %bx
	
copy_buffer:	

	/* load addresses for copy from disk buffer to destination */
	movw	6(%di), %es	/* load destination segment */

	/* restore %ax */
	popw	%ax

	/* determine the next possible destination address (presuming
		512 byte sectors!) */
	shlw	$5, %ax		/* shift %ax five bits to the left */
	addw	%ax, 6(%di)	/* add the corrected value to the destination
				   address for next time */

	/* save addressing regs */
	pusha
	pushw	%ds

	/* get the copy length */
	shlw	$4, %ax
	movw	%ax, %cx

	xorw	%di, %di	/* zero offset of destination addresses */
	xorw	%si, %si	/* zero offset of source addresses */
	movw	%bx, %ds	/* restore the source segment */

	cld		/* sets the copy direction to forward */

	/* perform copy */
	rep		/* sets a repeat */
	movsb		/* this runs the actual copy */

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
* 接着，用MSG()输出notification_step，就是一个点“.”。然后判断如果还有要读取的扇区，
* 就跳回setup_sectors继续读取；否则，就从DI寄存器中减去BOOTSEC_LISTSIZE，然后跳转到
* bootloop再次判断是否有没有下一个数据区段要读取。
*
* 这就说明，从这个start.S的512字节末尾的最后8字节倒着数，可能有几个8字节大小的数据区
* 段，每个区段都定义一个需要读取的Stage2(或者Stage1.5)（当然，这些8字节的数据定义区段
* 不能跨越可执行代码）。而实际上,一般只需要1个8字节的数据定义区段就可以满足需要。
*
* 剩下的代码就是读取完所有代码后，跳转到bootit继续执行。
*/
	/* restore addressing regs and print a dot with correct DS 
	   (MSG modifies SI, which is saved, and unused AX and BX) */
	popw	%ds
	MSG(notification_step)
	popa

	/* check if finished with this dataset */
	cmpw	$0, 4(%di)
	jne	setup_sectors

	/* update position to load from */
	subw	$BOOTSEC_LISTSIZE, %di

	/* jump to bootloop */
	jmp	bootloop

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
* 前面知道，读入Stage2或者Stage1.5时拷贝的目的地址，是根据blocklist_default_seg
* 处的值，并按照是否有STAGE1_5来决定的。如果有定义STAGE1_5，则拷贝到0x220段地址处，
* 实际对应0x2200线性地址处；如果没有定义STAGE1_5，则拷贝到0x820段地址处，实际对
* 应0x8200线性地址处。因此，这里在通过MSG()输出notification_done之后，所跳转的
* 目的地址也是按照是否有STAGE1_5来判断的，有STAGE1_5时远跳转到0x0:0x2200处，
* 没有STAGE1_5时远跳转到0x0:0x8200处。
**/
/* END OF MAIN LOOP */

bootit:
	/* print a newline */
	MSG(notification_done)
	popw	%dx	/* this makes sure %dl is our "boot" drive */
#ifdef STAGE1_5
	ljmp	$0, $0x2200
#else /* ! STAGE1_5 */
	ljmp	$0, $0x8200
#endif /* ! STAGE1_5 */


/*
 * BIOS Geometry translation error (past the end of the disk geometry!).
 */
geometry_error:
	MSG(geometry_error_string)
	jmp	general_error

/*
 * Read error on the disk.
 */
read_error:
	MSG(read_error_string)

general_error:
	MSG(general_error_string)

/* go here when you need to stop the machine hard after an error condition */
stop:	jmp	stop

#ifdef STAGE1_5
notification_string:	.string "Loading stage1.5"
#else
notification_string:	.string "Loading stage2"
#endif

notification_step:	.string "."
notification_done:	.string "\r\n"
	
geometry_error_string:	.string "Geom"
read_error_string:	.string "Read"
general_error_string:	.string " Error"

/*
 * message: write the string pointed to by %si
 *
 *   WARNING: trashes %si, %ax, and %bx
 */

	/*
	 * Use BIOS "int 10H Function 0Eh" to write character in teletype mode
	 *	%ah = 0xe	%al = character
	 *	%bh = page	%bl = foreground color (graphics modes)
	 */
1:
	movw	$0x0001, %bx
	movb	$0xe, %ah
	int	$0x10		/* display a byte */

	incw	%si
message:
	movb	(%si), %al
	cmpb	$0, %al
	jne	1b	/* if not end of string, jmp to display */
	ret
lastlist:

/*
 *  This area is an empty space between the main body of code below which
 *  grows up (fixed after compilation, but between releases it may change
 *  in size easily), and the lists of sectors to read, which grows down
 *  from a fixed top location.
 */

	.word 0
	.word 0

	. = _start + 0x200 - BOOTSEC_LISTSIZE
	
        /* fill the first data listing with the default */
blocklist_default_start:
	.long 2		/* this is the sector start parameter, in logical
			   sectors from the start of the disk, sector 0 */
blocklist_default_len:
			/* this is the number of sectors to read */
#ifdef STAGE1_5
	.word 0		/* the command "install" will fill this up */
#else
	.word (STAGE2_SIZE + 511) >> 9
#endif
blocklist_default_seg:
#ifdef STAGE1_5
	.word 0x220
#else
	.word 0x820	/* this is the segment of the starting address
			   to load the data into */
#endif
	
firstlist:	/* this label has to be after the list data!!! */
