/* -*-Asm-*- */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2004   Free Software Foundation, Inc.
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

#include <stage1.h>
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
* 我们先给出GRUB 0.97启动流程的总体介绍，下文会深入源代码对其详细介绍。
*
* 1）GRUB 0.97具有两个主要启动运行阶段，分别命名为Stage1和Stage2；
*
* 2）GRUB 0.97的Stage1是安装在启动扇区的，大小只有一个扇区，即512字节；一般而言
* Stage1安装在扇区号0（LBA模式）；
*
* 3）GRUB 0.97的Stage2按照安装方式可以分为有Stage1.5和没有Stage1.5 两种。不论有
* 没有Stage1.5，Stage2都是从【grub-0.97/stage2/start.S】开始的，并且这段代码大
* 小也是512字节，且一般是安装在扇区号1（LBA模式）；
*
* 4）GRUB 0.97的Stage1代码为【grub-0.97/stage1/stage1.S】，负责加载Stage2的第一
* 段代码，即【grub-0.97/stage2/start.S】对应代码；
*
* 5）GRUB 0.97的Stage2的【grub-0.97/stage2/start.S】是有Stage1.5和没有Stage1.5
* 的分水岭；如果没有Stage1.5，则会直接从扇区号2（LBA模式）开始加载连续的扇区（
* 大小为STAGE2_SIZE）。如果有，则会先加载Stage1.5，进而由Stage1.5来从文件系统
* 加载Stage2的真实代码。
*/	
/*
 *  defines for the code go here
 */
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
* ABS是在【grub-0.97/stage1/stage1.S】开始处定义的一个宏，该宏将一个标号的地址
* 与_start标号相减，得到该标号在代码中的相对偏移地址，再加上0x7c00即得到CS=0的
* 情况下的该标号的绝对地址（LJMP之后被置于IP中）。
*/
	/* Absolute addresses
	   This makes the assembler generate the address without support
	   from the linker. (ELF can't relocate 16-bit addresses!) */
#define ABS(x) (x-_start+0x7c00)
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
* 打印消息是通过调用MSG()宏来实现的。实际是先将一条消息的绝对地址放到SI寄存器中，
* 然后使用call调用message函数来实现输出的。
*/
	/* Print message string */
#define MSG(x)	movw $ABS(x), %si; call message

	/* XXX:	binutils-2.9.1.0.x doesn't produce a short opcode for this. */
#define	MOV_MEM_TO_AL(x)	.byte 0xa0;  .word x
	
	.file	"stage1.S"
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
* GRUB 0.97 Stage1运行流程
*
* 第1步，X86 CPU启动时，CS=0xFFFF，IP=0，按照实模式工作，从地址0xFFFF0处取到第
* 一条指令；BIOS ROM的第一条指令就是放在这里的，因此开始了BIOS的运行。
*
* 第2步，BIOS运行过程中，除了做基本的硬件检测和硬件初始化之外，还有一个关键的
* 地方是要设置实模式下的中端向量表【Interrupt Vector Table (IVT)】，位于0地址
* 处，并安装对应中断号的中断处理程序(ISR)。BIOS接着会检测系统的现有启动设备（
* 用户可设置各个启动设备的相对顺序），依次调用INT 0x19来读取其第一个扇区，并
* 检测其启动标记【最末尾2个字节】是否正确。 如果启动标记正确，就会把该启动扇区
* 加载到内存地址0x07C00处，并且跳转到0x07C00处接着执行。因此，在从实模式切换到
* 保护模式之前，实际上都还是在BIOS的上下文中运行，因此就可以做BIOS提供的中断调
* 用，所使用的中断服务程序也就是前面在中断向量表中安装的ISR。
*
* 第3步，BIOS加载启动扇区并跳转到0x07C00处后，就开始了真正的Bootloader的执行，
* 这就是我们这里要分析的GRUB 0.97的第一段真正被CPU执行的代码。这段代码位于
* 【grub-0.97/stage1/stage1.S】。BIOS跳转到0x07C00是通过JMP实现的 (CS:IP 
* 0:0x7C00)，因此，当在stage1.S中开始运行时的代码段寄存器和指令指针寄存分别是
* CS=0，IP=0x7C00。特别需要注意的是，目前CPU还处于16位实模式，因此需要明确告
* 知gas汇编器生成16位X86指令，否则无法正确运行。
*/
	.text

	/* Tell GAS to generate 16-bit instructions so that this code works
	   in real mode. */
	.code16

.globl _start; _start:
	/*
	 * _start is loaded at 0x7c00 and is jumped to with CS:IP 0:0x7c00
	 */
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
* GRUB 0.97 Stage1运行流程
*
* 第4步，由于为了兼容FAT/HPFS磁盘文件系统格式，要求在启动扇区的第4字节开始处有
* 一个所谓的"BIOS参数块"，因此我们的代码必须越过这一个参数块，从而第一条指令就
* 是做一个跳转，跳转到紧接着参数块的地方开始运行。
*/

	/*
	 * Beginning of the sector is compatible with the FAT/HPFS BIOS
	 * parameter block.
	 */

	jmp	after_BPB
	nop	/* do I care about this ??? */

	/*
	 * This space is for the BIOS parameter block!!!!  Don't change
	 * the first jump, nor start the code anywhere but right after
	 * this area.
	 */

	. = _start + 4

	/* scratch space */
mode:
	.byte	0
disk_address_packet:	
sectors:
	.long	0
heads:
	.long	0
cylinders:
	.word	0
sector_start:
	.byte	0
head_start:
	.byte	0
cylinder_start:
	.word	0
	/* more space... */

	. = _start + STAGE1_BPBEND

	/*
	 * End of BIOS parameter block.
	 */

stage1_version:	
	.byte	COMPAT_VERSION_MAJOR, COMPAT_VERSION_MINOR
boot_drive:	
	.byte	GRUB_INVALID_DRIVE	/* the disk to load stage2 from */
force_lba:
	.byte	0
stage2_address:
	.word	0x8000
stage2_sector:
	.long	1
stage2_segment:
	.word	0x800

after_BPB:
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
* GRUB 0.97 Stage1运行流程
*
* 第5步，禁止中断产生，因为紧接着的代码会尝试更正一些BIOS的bug以及设置堆栈指针，
* 这些操作最好是在原子状态进行（即不被中断干扰的情况下）。
*/
/* general setup */
	cli		/* we're not safe here! */
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
* GRUB 0.97 Stage1运行流程
*
* 第6步，检测启动设备（设备号由BIOS在跳转前保存在DL寄存器中），但是有些BIOS错
* 误地传递了启动设备号，因此这里要尝试修正。注意这里似乎实际代码和注释有所脱节，
* 因为这里是一个直接的跳转指令，注释中所说的测试DL是否正确设置掩码的操作是不能
* 被执行的。可能的原因是，大部分的BIOS还是可以正确传递启动扇区的，因此大部分情
* 况这段代码不需要执行。如果真的遇到这样的BIOS，应该还是要注释掉第一个"jmp 1f"
* 指令。
*/
	/*
	 * This is a workaround for buggy BIOSes which don't pass boot
	 * drive correctly. If GRUB is installed into a HDD, check if
	 * DL is masked correctly. If not, assume that the BIOS passed
	 * a bogus value and set DL to 0x80, since this is the only
	 * possible boot drive. If GRUB is installed into a floppy,
	 * this does nothing (only jump).
	 */
boot_drive_check:	
	jmp	1f
	testb	$0x80, %dl
	jnz	1f
	movb	$0x80, %dl
1:	
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
* GRUB 0.97 Stage1运行流程
*
* 第7步，由于有些BIOS跳转到0x07C00的动作是使用JMP 07C0:0000的方式实现的，而不是
* JMP 0000:7C00的方式，因此到这里第一条指令的时候，CS=0x07C0，而IP=0x0000，这与
* 我们下面需要使用的CS和IP相对值不一致，因此这里需要一个强制的办法来更新CS和IP
* 的值。这里采用的就是LJMP指令的方式，该指令执行之后，CS会被更改为0，而IP会更改
* 为$ABS(real_start)，即下一条要执行的指令的绝对地址。
*/
	/*
	 * ljmp to the next instruction because some bogus BIOSes
	 * jump to 07C0:0000 instead of 0000:7C00.
	 */
	ljmp	$0, $ABS(real_start)
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
* GRUB 0.97 Stage1运行流程
*
* 第8步，在前一步已经将CS强制置为0，我们还需要将其他的需要使用的段寄存器，包括
* 数据段和堆栈段寄存器，都置为0。之后，再设置堆栈指针寄存器为STAGE1_STACKSEG，
* 该值在【grub-0.97/stage1/stage1.h】中定义为0x2000。这样一来，CS=DS=SS=0，从
* 而CPU的运行地址不再依赖于这几个段寄存器，相当于禁止了分段机制，与普通的RISC
* 阵营的CPU寻址模式相似，只需要关注地址的偏移量部分。当设置了这些基本寄存器之
* 后，就可以允许中断了，因此打开中断使能。
*/
real_start:	

	/* set up %ds and %ss as offset from 0 */
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %ss

	/* set up the REAL stack */
	movw	$STAGE1_STACKSEG, %sp

	sti		/* we're safe again */
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
* GRUB 0.97 Stage1运行流程
*
* 第9步，检查在BIOS参数块中的boot_drive地址处是否有一个合法的磁盘号，如果在启动
* 扇区的这个地址被写入了一个合法的启动磁盘号，这将这个磁盘号更新到DL寄存器中，
* 否则继续使用从BIOS传递来的磁盘号。这个磁盘号会在后面被使用来读取GRUB 0.97的
* Stage2的映像的。注意，这里虽然我们的源代码在boot_drive这个位置填入的值是一个
* 无效的值（GRUB_INVALID_DRIVE，即0xFF），但是外部程序，例如一个二进制编辑程序，
* 或者在运行中的操作系统内的磁盘管理程序，都可以修改启动扇区的这个位置，填入一
* 个适当的值来改变这个用以加载Stage2的磁盘号，从而改变加载的启动映像。从安全角
* 度讲，这是一个值得注意的地方。
*/
	/*
	 *  Check if we have a forced disk reference here
	 */
	MOV_MEM_TO_AL(ABS(boot_drive))	/* movb	ABS(boot_drive), %al */
	cmpb	$GRUB_INVALID_DRIVE, %al
	je	1f
	movb	%al, %dl
1:
	/* save drive reference first thing! */
	pushw	%dx
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
* GRUB 0.97 Stage1运行流程
*
* 第10步，将DX寄存器压入堆栈（从而在下面的代码中可以使用DX寄存器，需要启动磁盘
* 号时再从堆栈处弹出）。接着打印一条消息"GRUB"，告诉用户GRUB正在启动。
*/
	/* print a notification message on the screen */
	MSG(notification_string)
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
* GRUB 0.97 Stage1运行流程
*
* 第11步，检查驱动磁盘是否支持LBA模式。并根据检测结果决定按照LBA模式还是CHS模式
* 访问启动设备。首先，如果启动磁盘是软驱，那么肯定不支持LBA模式，直接跳转到
* chs_mode。
*
* 注意，这里使用的是BIOS INT 0x13的扩展来实现LBA支持的检测的。根据来自《BIOS 
* Enhanced Disk Drive Specification》对这个BIOS中断调用扩展的描述。如果调用失
* 败（Carry Set），或者BX没有按照规范要求被设置为0xAA55，CX中的bit0（Fixed 
* disk access subset）没有设置，那么只能使用CHS模式。反之，如果CX中的bit0
*（Fixed disk access subset）被设置，则可以使用该功能支持的disk address packet 
* 结构来访问磁盘，而disk address packet中使用的就是磁盘扇区的LBA地址。
*/	/* do not probe LBA if the drive is a floppy */
	testb	$STAGE1_BIOS_HD_FLAG, %dl
	jz	chs_mode
			
	/* check if LBA is supported */
	movb	$0x41, %ah
	movw	$0x55aa, %bx
	int	$0x13

	/* 
	 *  %dl may have been clobbered by INT 13, AH=41H.
	 *  This happens, for example, with AST BIOS 1.04.
	 */
	popw	%dx
	pushw	%dx

	/* use CHS if fails */
	jc	chs_mode
	cmpw	$0xaa55, %bx
	jne	chs_mode

	/* check if AH=0x42 is supported if FORCE_LBA is zero */
	MOV_MEM_TO_AL(ABS(force_lba))	/* movb	ABS(force_lba), %al */
	testb	%al, %al
	jnz	lba_mode
	andw	$1, %cx
	jz	chs_mode
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
* GRUB 0.97 Stage1运行流程
*
* 第12步，使用LBA模式读取。这里首先填写一个叫做"disk address packet"的数据结构，
* 具体结构我们马上在后面给出。然后调用BIOS INT 0x13的扩展0x42来实际读取。
*
* 这里我们发现第一句是"movl	0x10(%si), %ecx"，且注释说"save the total number of 
* sectors"，但是我们通过分析，发现根本之前没有明确的对0x10(%si)赋值，前面唯一
* 设置了SI寄存器的是在MSG宏处，但是显然那里对SI的值与这里的sector无关；此外，
* 我们可以发现这里的ECX寄存器之后没有被使用到，因此可以判定这里的第一件没有实际
* 用处。
*
* 下面一句，将disk_address_packet标号的绝对地址设置给SI寄存器，从而后面可以用
* 个SI寄存器加上一定的偏移量来寻址disk_address_packet的字段，从而对其设置具体
* 值。这里，标号disk_address_packet实际和其他几个标号共用一段地址空间，这段共
* 享的空间总共14字节，但是后面可以在附加一些字节（从而可以满足
* disk_address_packet至少16字节的要求）。这段共享的空间是因为LBA和CHS模式只能
* 选择其中一个，因此他们工作的时候的参数变量也只需其中一种。
*
* 下一条指令"movb	$1, -1(%si)"，将mode变量设置为1（从disk_address_packet地址
* 往前面一个字节就是mode变量的地址）；再下一条，将Stage2的扇区号保存到EBX寄存
* 器中；再接着就是填写disk_address_packet的前面两项，指定该packet为16字节；再
* 下一条，设定Stage2的扇区个数，固定只有一个扇区。接着再将EBX寄存器中的值，也
* 即前面保存的Stage2的扇区号填入disk_address_packet的"Starting logical block 
* address"字段；再接着就是将disk_address_packet的"Address of transfer buffer"
* 设置为0x7000:0x0000，也就是说，Stage2会被读入到0x70000处(STAGE1_BUFFERSEG在
*【grub-0.97/stage1/stage1.h】中定义为0x7000)。 后面就是真的调用"int $0x13"来
* 读取Stage2的过程，如果失败，则继续尝试使用CHS模式读取。如果成功，则跳转到
* copy_buffer处执行。在跳转之前，将STAGE1_BUFFERSEG设置到BX寄存器中，在
* copy_buffer执行过程中会被用到。
*/	
lba_mode:
	/* save the total number of sectors */
	movl	0x10(%si), %ecx

	/* set %si to the disk address packet */
	movw	$ABS(disk_address_packet), %si

	/* set the mode to non-zero */
	movb	$1, -1(%si)
	
	movl	ABS(stage2_sector), %ebx

	/* the size and the reserved byte */
	movw	$0x0010, (%si)

	/* the blocks */
	movw	$1, 2(%si)
	
	/* the absolute address (low 32 bits) */
	movl	%ebx, 8(%si)

	/* the segment of buffer address */
	movw	$STAGE1_BUFFERSEG, 6(%si)

	xorl	%eax, %eax
	movw	%ax, 4(%si)
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

	/* LBA read is not supported, so fallback to CHS.  */
	jc	chs_mode

	movw	$STAGE1_BUFFERSEG, %bx
	jmp	copy_buffer
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
* GRUB 0.97 Stage1运行流程
*
* 第14步，如果没有执行LBA模式，那么就会落到chs_mode。下面分析chs_mode部分代码的
* 实现。
*
* 首先是"int	$0x13"调用0x08，该调用的输入输出参数如下图所示，如果成功，则跳
* 转到final_init执行，否则看是否可能需要探测Floppy，如果需要就跳转到floppy_probe
*（具体分析见下文），最糟糕的情形是出现硬盘出错，那么只能跳转hd_probe_error。
*
* 根据读取的磁盘参数，final_init执行一些进一步的操作，主要是填写在前面看到过的
* 与disk_address_packet共享的一段变量地址空间里面的变量，包括sectors，heads，
* cylinders，sector_start，head_start，以及cylinder_start，相关代码相对简单，
* 其过程不再详述。
*
* 下面的setup_sectors会进一步根据这些参数来构造BIOS的"INT 0x13 Function 0x2"
* 需要的参数值，并调用该BIOS调用来讲Stage2读入到STAGE1_BUFFERSEG指定的段地址处。
* 最后还是接着运行copy_buffer,将Stage2的代码从0x70000拷贝到0x8000处，并接着跳
* 转到Stage2处运行。
*/		
chs_mode:	
	/*
	 *  Determine the hard disk geometry from the BIOS!
	 *  We do this first, so that LS-120 IDE floppies work correctly.
	 */
	movb	$8, %ah
	int	$0x13
	jnc	final_init

	/*
	 *  The call failed, so maybe use the floppy probe instead.
	 */
	testb	$STAGE1_BIOS_HD_FLAG, %dl
	jz	floppy_probe

	/* Nope, we definitely have a hard disk, and we're screwed. */
	jmp	hd_probe_error

final_init:
	
	movw	$ABS(sectors), %si

	/* set the mode to zero */
	movb	$0, -1(%si)
	
	/* save number of heads */
	xorl	%eax, %eax
	movb	%dh, %al
	incw	%ax
	movl	%eax, 4(%si)

	xorw	%dx, %dx
	movb	%cl, %dl
	shlw	$2, %dx
	movb	%ch, %al
	movb	%dh, %ah

	/* save number of cylinders */
	incw	%ax
	movw	%ax, 8(%si)

	xorw	%ax, %ax
	movb	%dl, %al
	shrb	$2, %al

	/* save number of sectors */
	movl	%eax, (%si)

setup_sectors:
	/* load logical sector start (bottom half) */
	movl	ABS(stage2_sector), %eax

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
	
	/* head number */
	movb	11(%si), %dh

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

	movw	$STAGE1_BUFFERSEG, %bx
	movw	%bx, %es	/* load %es segment with disk buffer */

	xorw	%bx, %bx	/* %bx = 0, put it at 0 in the segment */
	movw	$0x0201, %ax	/* function 2 */
	int	$0x13

	jc	read_error

	movw	%es, %bx
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
* GRUB 0.97 Stage1运行流程
*
* 第13步，执行copy_buffer。首先就是将stage2_segment的值设置给ES寄存器，即ES设置
* 为0x800，即设置了copy操作的目的地址。后面将BX寄存器设置给DS寄存器，即设置了
* copy操作的源地址；接着调用rep movsw搬移0x100（即256）次，每次2字节，总共512字
* 节。注意，这里的movsw 操作方向是：(ES:DI)�(DS:SI)；且这里使用了cld，因此每次
* copy之后SI和DI都要递增2字节。再需要注意的是，在执行拷贝之前和之后，有pusha和
* popa操作，保存了ECX和SI寄存器。最后，一句jmp	*(stage2_address)跳转到了Stage2
* 执行（stage2_address的值是0x8000）。
*/
copy_buffer:
	movw	ABS(stage2_segment), %es

	/*
	 * We need to save %cx and %si because the startup code in
	 * stage2 uses them without initializing them.
	 */
	pusha
	pushw	%ds
	
	movw	$0x100, %cx
	movw	%bx, %ds
	xorw	%si, %si
	xorw	%di, %di
	
	cld
	
	rep
	movsw

	popw	%ds
	popa

	/* boot stage2 */
	jmp	*(stage2_address)

/* END OF MAIN LOOP */

/*
 * BIOS Geometry translation error (past the end of the disk geometry!).
 */
geometry_error:
	MSG(geometry_error_string)
	jmp	general_error

/*
 * Disk probe failure.
 */
hd_probe_error:
	MSG(hd_probe_error_string)
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

notification_string:	.string "GRUB "
geometry_error_string:	.string "Geom"
hd_probe_error_string:	.string "Hard Disk"
read_error_string:	.string "Read"
general_error_string:	.string " Error"
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
* GRUB 0.97 Stage1运行流程
*
* 实际message()函数的实现很简单：从SI寄存器所指的地址处加载一个字节到AL寄存器，
* 比较验证该字节不是字符串末尾的'\0'字节，然后调用BIOS终端int 0x10实现输出该字
* 节，该操作直到遇到字符串末尾的'\0'字节才结束，返回被调用地址处。
*/
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
message:
	lodsb
	cmpb	$0, %al
	jne	1b	/* if not end of string, jmp to display */
	ret

	/*
	 *  Windows NT breaks compatibility by embedding a magic
	 *  number here.
	 */

	. = _start + STAGE1_WINDOWS_NT_MAGIC
nt_magic:	
	.long 0
	.word 0

	/*
	 *  This is where an MBR would go if on a hard disk.  The code
	 *  here isn't even referenced unless we're on a floppy.  Kinda
	 *  sneaky, huh?
	 */

part_start:	
	. = _start + STAGE1_PARTSTART

probe_values:
	.byte	36, 18, 15, 9, 0
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
* GRUB 0.97 Stage1运行流程
*
* 第15步，floppy_probe相关代码。实际上在【grub-0.97/stage1/stage1.S】的最后部分
* 代码对应于启动扇区的最后部分。这部分代码主要是为了和Windows启动扇区兼容。特别
* 的，在位于偏移量0x1b8处（即STAGE1_WINDOWS_NT_MAGIC处），Windows NT嵌入了两个
* 奇怪数字。此外，从0x1be（即STAGE1_PARTSTART）到0x1fe（即STAGE1_PARTEND）区间
* 的64字节是传统的硬盘分区表区段。在这一段，对于硬盘就会保存实际的分区表信息。
* 如果存储这个启动扇区的磁盘是软盘的话，则这一区段可以不用（软盘无需分区），因
* 此这一段可以保存代码。GRUB 0.97假设这里是用软盘做启动，因此这里就加入了一段
* 探测软驱的floppy_probe代码。代码先将probe_values地址（该地址实际就是分区表的
* 起始地址）减去1字节，并将其加载到SI寄存器中。接着进入probe_loop，调用"INT 13h 
* AH=0"来重置软驱；接着通过递增SI寄存器并从其所指的地址加载一个字节到CL寄存器
*（也即依次读取probe_values的值），并比较这个值是否为0（即到了probe_values的末
* 尾），如果到了probe_values的末尾就打印fd_probe_error_string消息并退出
* probe_loop到general_error处。否则就会尝试调用"INT 13h AH=2"来读取一个扇区到
* STAGE1_BUFFERSEG指定的段地址处。如果失败就循环probe_loop直到遇到probe_values
* 的末尾。否则就算读取成功，修正参数后跳转到final_init处继续，从而回到前面描述
* 的第14步，进而执行copy_buffer，最终还是跳转到Stage2的最终地址处执行。
*/
floppy_probe:
/*
 *  Perform floppy probe.
 */

	movw	$ABS(probe_values-1), %si

probe_loop:
	/* reset floppy controller INT 13h AH=0 */
	xorw	%ax, %ax
	int	$0x13

	incw	%si
	movb	(%si), %cl

	/* if number of sectors is 0, display error and die */
	cmpb	$0, %cl
	jne	1f

/*
 * Floppy disk probe failure.
 */
	MSG(fd_probe_error_string)
	jmp	general_error

fd_probe_error_string:	.string "Floppy"

1:
	/* perform read */
	movw	$STAGE1_BUFFERSEG, %bx
	movw	$0x201, %ax
	movb	$0, %ch
	movb	$0, %dh
	int	$0x13

	/* if error, jump to "probe_loop" */
	jc	probe_loop

	/* %cl is already the correct value! */
	movb	$1, %dh
	movb	$79, %ch

	jmp	final_init

	. = _start + STAGE1_PARTEND

/* the last 2 bytes in the sector 0 contain the signature */
	.word	STAGE1_SIGNATURE
