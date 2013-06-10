/* boot.c - load and bootstrap a kernel */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2003,2004  Free Software Foundation, Inc.
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

#include "freebsd.h"
#include "imgact_aout.h"
#include "i386-elf.h"

static int cur_addr;
entry_func entry_addr;
static struct mod_list mll[99];
static int linux_mem_size;
/**
* @topic 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @group 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月6日
*
* @details 注释详细内容: Multiboot规范的精确定义 

* 来源：http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
* 
* 3. Multiboot规范的精确定义
* 
* 引导程序/OS映像接口主要包括三个方面：
* 
* 1）引导程序看到的 OS 映像的格式。 
* 2）当引导程序启动操作系统时机器的状态。 
* 3）引导程序传递给操作系统的信息的格式。
* 
* 3.1 OS映像格式
*
* 一个OS映像可以是一个普通的某种操作系统使用的标准格式的32位可执行文件，不同之处是它可
* 能被连接到一个非默认的载入地址以避开PC的I/O区域或者其它的保留区域，当然它也不能使用
* 共享库或其它这样可爱的东西。
* 
* 除了OS映像所使用的格式需要的头之外，OS映像还必须额外包括一个Multiboot头。Multiboot头
* 必须完整的包含在OS映像的前8192字节内，并且必须是longword（32位）对齐的。通常来说，它
* 的位置越靠前越好，并且可以嵌入在text段的起始处，位于真正的可执行文件头之前。
* 
* 3.1.1 Multiboot的头分布
*
* Multiboot 头的分布必须如下表所示：
* 
* 偏移量 类型 域名 备注
* 0  u32  magic  必需
* 4  u32  flags  必需 
* 8  u32  checksum  必需 
* 12  u32  header_addr  如果flags[16]被置位 
* 16  u32  load_addr  如果flags[16]被置位 
* 20  u32  load_end_addr  如果flags[16]被置位 
* 24  u32  bss_end_addr  如果flags[16]被置位 
* 28  u32  entry_addr  如果flags[16]被置位 
* 32  u32  mode_type  如果flags[2]被置位 
* 36  u32  width  如果flags[2]被置位 
* 40  u32  height  如果flags[2]被置位 
* 44  u32  depth  如果flags[2]被置位
* 
* magic、flags和checksum域在头的magic域中定义，header_addr、load_addr、load_end_addr、
* bss_end_addr和entry_addr域在头的地址域中定义，mode_type、width、height和depth域则
* 在头的图形域中定义。
* 
* 3.1.2 Multiboot头的 magic 域
* 
* - magic 
*
* magic域是标志头的魔数，它必须等于十六进制值0x1BADB002。
* 
* - flags 
* 
* flags域指出OS映像需要引导程序提供或支持的特性。0-15位指出需求：如果引导程序发现某些
* 值被设置但出于某种原因不理解或不能不能满足相应的需求，它必须告知用户并宣告引导失败。
* 16-31位指出可选的特性：如果引导程序不能支持某些位，它可以简单的忽略它们并正常引导。
* 自然，所有flags字中尚未定义的位必须被置为0。这样，flags域既可以用于版本控制也可以用
* 于简单的特性选择。 
* 
* 如果设置了flags字中的0位，所有的引导模块将按页（4KB）边界对齐。有些操作系统能够在启
* 动时将包含引导模块的页直接映射到一个分页的地址空间，因此需要引导模块是页对齐的。
* 
* 如果设置了flags字中的1位，则必须通过Multiboot信息结构（参见引导信息格式）的mem_*域
* 包括可用内存的信息。如果引导程序能够传递内存分布（mmap_*域）并且它确实存在，则也包
* 括它。
* 
* 如果设置了flags字中的2位，有关视频模式表（参见引导信息格式）的信息必须对内核有效。
* 
* 如果设置了flags字中的16位，则Multiboot头中偏移量8-24的域有效，引导程序应该使用它们
* 而不是实际可执行头中的域来计算将OS映象载入到那里。如果内核映象为ELF格式则不必提供
* 这样的信息，但是如果映象是a.out格式或者其他什么格式的话就必须提供这些信息。兼容的
* 引导程序必须既能够载入ELF格式的映象也能载入将载入地址信息嵌入Multiboot头中的映象；
* 它们也可以直接支持其他的可执行格式，例如一个a.out的特殊变体，但这不是必须的。
* 
* - checksum 
* 
* 域checksum是一个32位的无符号值，当与其他的magic域（也就是magic和flags）相加时，结
* 果必须是32位的无符号值0（即magic + flags + checksum = 0）。
* 
* 3.1.3 Multiboot头的地址域
* 
* 所有由flags的第16位开启的地址域都是物理地址。它们的意义如下：
* 
* - header_addr 
* 
* 包含对应于Multiboot头的开始处的地址——这也是magic值的物理地址。这个域用来同步OS映象
* 偏移量和物理内存之间的映射。
* 
* - load_addr
* 
* 包含text段开始处的物理地址。从OS映象文件中的多大偏移开始载入由头位置的偏移量定义，
* 相减（header_addr - load_addr）。load_addr必须小于等于header_addr。
* 
* - load_end_addr
* 
* 包含data段结束处的物理地址。（load_end_addr-load_addr）指出了引导程序要载入多少数据。
* 这暗示了text和data段必须在OS映象中连续；现有的a.out可执行格式满足这个条件。如果这个
* 域为0，引导程序假定text和data段占据整个 OS 映象文件。
* 
* - bss_end_addr
* 
* 包含bss段结束处的物理地址。引导程序将这个区域初始化为0，并保留这个区域以免将引导模块
* 和其他的于查系统相关的数据放到这里。如果这个域为0，引导程序假定没有bss段。
* 
* - entry_addr
* 
* 操作系统的入口点，引导程序最后将跳转到那里。
* 
* 3.1.4 Multiboot头的图形域
* 
* 所有的图形域都通过flags的第2位开启。它们指出了推荐的图形模式。注意，这只是OS映象推荐
* 的模式。如果该模式存在，引导程序将设定它，如果用户不明确指出另一个模式的话。否则，如
* 果可能的话，引导程序将转入一个相似的模式。
* 
* 他们的意义如下：
* 
* - mode_type 
* 
* 如果为0就代表线性图形模式，如果为1代表标准EGA文本模式。所有其他值保留以备将来扩展。
* 注意即使这个域为0，引导程序也可能设置一个文本模式。
* 
* - width 
* 
* 包含列数。在图形模式下它是象素数，在文本模式下它是字符数。0代表OS映象对此没有要求。
* 
* - height 
* 
* 包含行数。在图形模式下它是象素数，在文本模式下它是字符数。0代表OS映象对此没有要求。
* 
* - depth 
* 
* 在图形模式下，包含每个象素的位数，在文本模式下为0。0代表OS映象对此没有要求。
* 
* 3.2 机器状态
* 
* 当引导程序调用32位操作系统时，机器状态必须如下：
* 
* - EAX 
* 
* 必须包含魔数0x2BADB002；这个值指出操作系统是被一个符合Multiboot规范的引导程序载入
* 的（这样就算是另一种引导程序也可以引导这个操作系统）。
* 
* - EBX 
* 
* 必须包含由引导程序提供的Multiboot信息结构的物理地址（参见引导信息格式）。
* 
* - CS 
* 
* 必须是一个偏移量位于0到0xFFFFFFFF之间的32位可读/可执行代码段。这里的精确值未定义。
* 
* - DS, ES, FS, GS, SS 
* 
* 必须是一个偏移量位于0到0xFFFFFFFF之间的32位可读/可执行代码段。这里的精确值未定义。
* 
* - A20 gate
* 
* 必须已经开启。
* 
* - CR0 
* 
* 第31位（PG）必须为0。第0位（PE）必须为1。其他位未定义。
* 
* - EFLAGS 
* 
* 第17位（VM）必须为0。第9位（IF）必须为1 。其他位未定义。
* 
* 所有其他的处理器寄存器和标志位未定义。这包括：
* 
* - ESP 
* 
* 当需要使用堆栈时，OS映象必须自己创建一个。
* 
* - GDTR 
* 
* 尽管段寄存器像上面那样定义了，GDTR也可能是无效的，所以OS映象决不能载入任何段寄存器
* （即使是载入相同的值也不行！）直到它设定了自己的GDT。
* 
* - IDTR 
* 
* OS映象必须在设置完它的IDT之后才能开中断。
* 
* 尽管如此，其他的机器状态应该被引导程序留做正常的工作顺序，也就是同BIOS（或者DOS，
* 如果引导程序是从那里启动的话）初始化的状态一样。换句话说，操作系统应该能够在载入后
* 进行BIOS调用，直到它自己重写BIOS数据结构之前。还有，引导程序必须将PIC设定为正常的
* BIOS/DOS 状态，尽管它们有可能在进入32位模式时改变它们。
* 
* 3.3 引导信息格式
* 
* 在进入操作系统时，EBX寄存器包含Multiboot信息数据结构的物理地址，引导程序通过它将重
* 要的引导信息传递给操作系统。操作系统可以按自己的需要使用或者忽略任何部分；所有的引
* 导程序传递的信息只是建议性的。
* 
* Multiboot信息结构和它的相关的子结构可以由引导程序放在任何位置（当然，除了保留给内核
* 和引导模块的区域）。如何在利用之前保护它是操作系统的责任。
* 
* Multiboot信息结构（就目前为止定义的）的格式如下：
* 
*              +---------------------------+
*      0       | flags                     |    (必需)
*              +---------------------------+
*      4       | mem_lower                 |    (如果flags[0]被置位则出现)
*      8       | mem_upper                 |    (如果flags[0]被置位则出现)
*              +---------------------------+
*      12      | boot_device               |    (如果flags[1]被置位则出现)
*              +---------------------------+
*      16      | cmdline                   |    (如果flags[2]被置位则出现)
*              +---------------------------+
*      20      | mods_count                |    (如果flags[3]被置位则出现)
*      24      | mods_addr                 |    (如果flags[3]被置位则出现)
*              +---------------------------+
* 28 - 40      | syms                      |    (如果flags[4]或flags[5]被置位则出现)
*              |                           |
*              +---------------------------+
*      44      | mmap_length               |    (如果flags[6]被置位则出现)
*      48      | mmap_addr                 |    (如果flags[6]被置位则出现)
*              +---------------------------+
*      52      | drives_length             |    (如果flags[7]被置位则出现)
*      56      | drives_addr               |    (如果flags[7]被置位则出现)
*              +---------------------------+
*      60      | config_table              |    (如果flags[8]被置位则出现)
*              +---------------------------+
*      64      | boot_loader_name          |    (如果flags[9]被置位则出现)
*              +---------------------------+
*      68      | apm_table                 |    (如果flags[10]被置位则出现)
*              +---------------------------+
*      72      | vbe_control_info          |    (如果flags[11]被置位则出现)
*      76      | vbe_mode_info             |
*      80      | vbe_mode                  |
*      82      | vbe_interface_seg         |
*      84      | vbe_interface_off         |
*      86      | vbe_interface_len         |
*              +---------------------------+
* 
* 第一个longword指出Multiboot信息结构中的其它域是否有效。所有目前未定义的位必须
* 被引导程序设为0。操作系统应该忽略任何它不理解的位。因此，flags域也可以视作一个
* 版本标志符，这样可以无破坏的扩展Multiboot信息结构。
* 
* 如果设置了flags中的第0位，则mem_*域有效。mem_lower和mem_upper分别指出了低端和
* 高端内存的大小，单位是K。低端内存的首地址是0，高端内存的首地址是1M。低端内存的
* 最大可能值是640K。返回的高端内存的最大可能值是最大值减去1M。但并不保证是这个值。
* 
* 如果设置了flags中的第1位，则boot_device域有效，并指出引导程序从哪个BIOS磁盘设备
* 载入的OS映像。如果OS映像不是从一个BIOS磁盘载入的，这个域就决不能出现（第3位必须
* 是0）。操作系统可以使用这个域来帮助确定它的root设备，但并不一定要这样做。
* boot_device域由四个单字节的子域组成：
* 
*      +-------+-------+-------+-------+
*      | drive | part1 | part2 | part3 |
*      +-------+-------+-------+-------+
* 
* 第一个字节包含了BIOS驱动器号，它的格式与BIOS的INT0x13低级磁盘接口相同：例如，
* 0x00代表第一个软盘驱动器，0x80代表第一个硬盘驱动器。
* 
* 剩下的三个字节指出了引导分区。part1指出顶级分区号，part2指出一个顶级分区中的一个
* 子分区，等等。分区号总是从0开始。不使用的分区字节必须被设为0xFF。例如，如果磁盘
* 被简单的分为单一的一层DOS分区，则part1包含这个DOS分区号，part2和part3都是0xFF。
* 另一个例子是，如果一个磁盘先被分为DOS分区，并且其中的一个DOS分区又被分为几个使
* 用BSD磁盘标签策略的BSD分区，则part1包含DOS分区号，part2包含DOS分区内的BSD子分区，
* part3是0xFF。
* 
* DOS扩展分区的分区号从4开始，而不是像嵌套子分区一样，尽管扩展分区的底层分布就是
* 分层嵌套的。例如，如果引导程序从传统的DOS风格磁盘的第二个分区启动，则part1是5，
* part2和part3都是0xFF。
* 
* 如果设置了flags longword的第2位，则cmdline域有效，并包含要传送给内核的命令行参
* 数的物理地址。命令行参数是一个正常C风格的以0终止的字符串。
* 
* 如果设置了flags的第3位，则mods域指出了同内核一同载入的有哪些引导模块，以及在哪
* 里能找到它们。mods_count包含了载入的模块的个数；mods_addr包含了第一个模块结构
* 的物理地址。mods_count可以是0，这表示没有载入任何模块，即使设置了flags的第1位
* 时也有可能是这样。每个模块结构的格式如下：
* 
*             +------------------+
*      0      | mod_start        |
*      4      | mod_end          |
*             +------------------+
*      8      | string           |
*             +------------------+
*      12     | reserved (0)     |
*             +------------------+
* 
* 前两个域包含了引导模块的开始和结束地址。string域提供了一个自定义的与引导模块相
* 关的字符串；它是以0中止的ASCII字符串，同内核命令行参数一样。如果没有什么与模块
* 有关的字符串，string域可以是0。典型情况下，这个字符串也许是命令行参数（例如，
* 如果操作系统将引导模块视作可执行程序的话），或者一个路径名（例如，如果操作系统
* 将引导模块视作文件系统中的文件的话），它的意义取决于操作系统。reserved域必须由
* 引导程序设为0并被操作系统忽略。
* 
* 注意：第4位和第5位是互斥的。
* 
* 如果设置了flags的第4位，则下面从Multiboot信息结构的第28位开始的域是有效的：
* 
*            +------------------+
*      28    | tabsize          |
*      32    | strsize          |
*      36    | addr             |
*      40    | reserved (0)     |
*            +------------------+
* 
* 这指出在哪里可以找到a.out格式内核映像的符号表。addr是a.out格式的nlist结构数组的
* 大小（4字节无符号长整数）的物理地址，紧接着是数组本身，然后是一系列以0中止的ASCII
* 字符串的大小（4字节无符号长整数，加上sizeof(unsigned long)），然后是字符串本身。
* tabsize等于符号表的大小参数（位于符号section的头部），strsize等于符号表指向的字
* 符串表的大小参数（位于string section的头部）。注意tabsize可以是0，这意味着没有
* 符号，尽管已经设置了flags的第4位。
* 
* 如果设置了flags的第5位，则下面从Multiboot信息结构的第28位开始的域是有效的：
* 
*            +----------------+
*      28    | num            |
*      32    | size           |
*      36    | addr           |
*      40    | shndx          |
*            +----------------+
* 
* 这指出在哪里可以找到 ELF 格式内核映像的section头表、每项的大小、一共有几项以及
* 作为名字索引的字符串表。它们对应于可执行可连接格式（ELF）的program头中的
* shdr_* 项（shdr_num等）。所有的section都会被载入，ELF section头的物理地址域指
* 向所有的section在内存中的位置（参见i386 ELF文档以得到如何读取section头的更多
* 的细节）。注意，shdr_num可以是0，标志着没有符号，尽管已经设置了flags的第5位。
* 
* 如果设置了flags的第 6 位，则mmap_*域是有效的，指出保存由BIOS提供的内存分布的
* 缓冲区的地址和长度。mmap_addr是缓冲区的地址，mmap_length是缓冲区的总大小。
* 缓冲区由一个或者多个下面的大小/结构对（size实际上是用来跳过下一个对的）组
* 成的：
* 
*             +-----------------------+
*      -4     | size                  |
*             +-----------------------+
*      0      | base_addr_low         |
*      4      | base_addr_high        |
*      8      | length_low            |
*      12     | length_high           |
*      16     | type                  |
*             +---------------------- +
* 
* size是相关结构的大小，单位是字节，它可能大于最小值20。base_addr_low是启动地址
* 的低32位，base_addr_high是高32位，启动地址总共有64位。length_low是内存区域大
* 小的低32位，length_high是内存区域大小的高32位，总共是64位。type是相应地址区间
* 的类型，1代表可用RAM，所有其它的值代表保留区域。
* 
* 可以保证所提供的内存分布列出了所有可供正常使用的标准内存。
* 
* 如果设置了flags的第7位，则drives_*域是有效的，指出第一个驱动器结构的物理地址
* 和这个结构的大小。drives_addr是地址，drives_length是驱动器结构的总大小。注意，
* drives_length可以是0。每个驱动器结构的格式如下：
* 
*              +---------------------+
*      0       | size                |
*              +---------------------+
*      4       | drive_number        |
*              +---------------------+
*      5       | drive_mode          |
*              +---------------------+
*      6       | drive_cylinders     |
*      8       | drive_heads         |
*      9       | drive_sectors       |
*              +---------------------+
* 10 - xx      | drive_ports         |
*              +---------------------+
* 
* size域指出了结构的大小。依据端口的数量不同，这个大小可能变化。注意，这个大小
* 可能不等于（10 + 2 * 端口数），这是由于对齐的原因。
* 
* drive_number域包含 BIOS 驱动器号。drive_mode域代表了引导程序使用的访问模式。
* 目前，模式定义如下：
* 
* 0 - CHS 模式（传统的“柱面/磁头/扇区”寻址模式）。 
* 
* 1 - LBA 模式（逻辑块寻址模式）。 
* 
* 这三个域，drive_cylinders、drive_heads和drive_sectors，指出了BIOS检测到的驱动
* 器的参数。drive_cylinders包含柱面数，drive_heads包含磁头数，drive_sectors包含
* 每磁道的扇区数。
* 
* drive_ports域包含了BIOS代码使用的I/O端口的数组。这个数组包含0个或者多个无符号
* 两字节整数，并且以0中止。注意，数组中可能包含任何实际上与驱动器不相关的I/O端
* 口（例如DMA控制器的端口）。
* 
* 如果设置了flags的第8位，则config_table域有效，指出由GET CONFIGURATION BIOS调用
* 返回的ROM配置表的物理地址。如果这个BIOS调用失败了，则这个表的大小必须是0。
* 
* 如果设置了flags的第9位，则boot_loader_name域有效，包含了引导程序名字在物理内存
* 中的地址。引导程序名字是正常的C风格的以0中止的字符串。
* 
* 如果设置了flags的第10位，则apm_table域有效，包含了如下APM表的物理地址：
* 
*              +-------------------+
*      0       | version           |
*      2       | cseg              |
*      4       | offset            |
*      8       | cseg_16           |
*      10      | dseg              |
*      12      | flags             |
*      14      | cseg_len          |
*      16      | cseg_16_len       |
*      18      | dseg_len          |
*              +-------------------+
* 
* 域version、cseg、offset、cseg_16、dseg、flags、cseg_len、cseg_16_len、dseg_len
* 分别指出了版本号、保护模式32位代码段、入口点的偏移量、保护模式16位代码段、保护
* 模式16位数据段、标志位、保护模式32位代码段的长度、保护模式16位代码段的长度和
* 保护模式16位数据段的长度。只有offset域是4字节，其余的域都是2字节。参见高级电
* 源管理（APM）BIOS接口规范。
* 
* 如果设置了flags的第11位，则graphics table有效。前提是内核已经在Multiboot头中
* 指定了一种图形模式。 域vbe_control_info和vbe_mode_info分别包含由VBE函数00h
* 返回的VBE控制信息的物理地址和由VBE函数01h返回的VBE模式信息。
* 
* 域vbe_mode指出了当前的显示模式，其中的信息符合VBE 3.0标准。
* 
* 其余的域vbe_interface_seg、vbe_interface_off和vbe_interface_len包含了VBE 2.0+
* 中定义的保护模式接口。如果没有这些信息，这些域都是0 。注意VBE 3.0定义了另一个
* 保护模式接口，它与以前的版本是兼容的。如果你想要使用这些新的保护模式接口，你
* 必须自己找到这个表。
* 
* graphics table中的域是按照VBE设计的，但是Multiboot引导程序可以在非VBE模式下模
* 拟VBE模式。
**/
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
* 本函数实现读入内核映像功能。参数kernel为要读取的内核文件名；参数arg为内核参数；
* 参数suggested_type为调用者猜测的内核类型；参数load_flags为加载标志。
*
* 首先调用grub_open (kernel)打开内核文件；并通过grub_read()读入文件的前一段内容
* (至少32字节，至多8KB)，并从这段头信息中检查是否发现multiboot_header结构；如果
* 发现是multiboot的映像，则设type为KERNEL_TYPE_MULTIBOOT。
*
* 如果是multiboot, FreeBSD 或者 NetBSD内核，则可以支持ELF模式加载。根据这些，
* 可以设置全局变量entry_addr;并且可以根据suggested_type来设置type变量。
*
* 如果是multiboot_header的flags标志指明是MULTIBOOT_AOUT_KLUDGE,则可以确定起始地址
* entry_addr = (entry_func) pu.mb->entry_addr;其中pu.mb指向找到的multiboot_header。
*
* 如果映像为aout格式，则可以entry_addr = (entry_func) pu.aout->a_entry;其中pu.aout
* 指向文件头的buffer。
*
* 如果映像不是以上几种，则检查映像头部是否符合Linux内核映像格式(boot_flag == 
* BOOTSEC_SIGNATURE)。如果的确是Linux内核映像，则按照Linux版本不同，初始化各种版
* 本需要的信息。其中包括linux_data_real_addr，linux_data_tmp_addr，linux_mem_size
* 等全局变量。此外，还根据版本不同，对Linux Header进行了一些fixup操作。
*
* 最后，根据分析出来的映像类型，按照aout格式或者ELF格式，读取所有的剩余映像内容
* 到cur_addr执行的地址处(cur_addr在之前分析文件头时初始化)。根据这些信息，初始化
* multiboot信息到mbi全局结构中。
*/

/*
 *  The next two functions, 'load_image' and 'load_module', are the building
 *  blocks of the multiboot loader component.  They handle essentially all
 *  of the gory details of loading in a bootable image and the modules.
 */

kernel_t
load_image (char *kernel, char *arg, kernel_t suggested_type,
	    unsigned long load_flags)
{
  int len, i, exec_type = 0, align_4k = 1;
  entry_func real_entry_addr = 0;
  kernel_t type = KERNEL_TYPE_NONE;
  unsigned long flags = 0, text_len = 0, data_len = 0, bss_len = 0;
  char *str = 0, *str2 = 0;
  struct linux_kernel_header *lh;
  union
    {
      struct multiboot_header *mb;
      struct exec *aout;
      Elf32_Ehdr *elf;
    }
  pu;
  /* presuming that MULTIBOOT_SEARCH is large enough to encompass an
     executable header */
  unsigned char buffer[MULTIBOOT_SEARCH];

  /* sets the header pointer to point to the beginning of the
     buffer by default */
  pu.aout = (struct exec *) buffer;

  if (!grub_open (kernel))
    return KERNEL_TYPE_NONE;

  if (!(len = grub_read (buffer, MULTIBOOT_SEARCH)) || len < 32)
    {
      grub_close ();
      
      if (!errnum)
	errnum = ERR_EXEC_FORMAT;

      return KERNEL_TYPE_NONE;
    }

  for (i = 0; i < len; i++)
    {
      if (MULTIBOOT_FOUND ((int) (buffer + i), len - i))
	{
	  flags = ((struct multiboot_header *) (buffer + i))->flags;
	  if (flags & MULTIBOOT_UNSUPPORTED)
	    {
	      grub_close ();
	      errnum = ERR_BOOT_FEATURES;
	      return KERNEL_TYPE_NONE;
	    }
	  type = KERNEL_TYPE_MULTIBOOT;
	  str2 = "Multiboot";
	  break;
	}
    }

  /* Use BUFFER as a linux kernel header, if the image is Linux zImage
     or bzImage.  */
  lh = (struct linux_kernel_header *) buffer;
  
  /* ELF loading supported if multiboot, FreeBSD and NetBSD.  */
  if ((type == KERNEL_TYPE_MULTIBOOT
       || pu.elf->e_ident[EI_OSABI] == ELFOSABI_FREEBSD
       || grub_strcmp (pu.elf->e_ident + EI_BRAND, "FreeBSD") == 0
       || suggested_type == KERNEL_TYPE_NETBSD)
      && len > sizeof (Elf32_Ehdr)
      && BOOTABLE_I386_ELF ((*((Elf32_Ehdr *) buffer))))
    {
      if (type == KERNEL_TYPE_MULTIBOOT)
	entry_addr = (entry_func) pu.elf->e_entry;
      else
	entry_addr = (entry_func) (pu.elf->e_entry & 0xFFFFFF);

      if (entry_addr < (entry_func) 0x100000)
	errnum = ERR_BELOW_1MB;

      /* don't want to deal with ELF program header at some random
         place in the file -- this generally won't happen */
      if (pu.elf->e_phoff == 0 || pu.elf->e_phnum == 0
	  || ((pu.elf->e_phoff + (pu.elf->e_phentsize * pu.elf->e_phnum))
	      >= len))
	errnum = ERR_EXEC_FORMAT;
      str = "elf";

      if (type == KERNEL_TYPE_NONE)
	{
	  /* At the moment, there is no way to identify a NetBSD ELF
	     kernel, so rely on the suggested type by the user.  */
	  if (suggested_type == KERNEL_TYPE_NETBSD)
	    {
	      str2 = "NetBSD";
	      type = suggested_type;
	    }
	  else
	    {
	      str2 = "FreeBSD";
	      type = KERNEL_TYPE_FREEBSD;
	    }
	}
    }
  else if (flags & MULTIBOOT_AOUT_KLUDGE)
    {
      pu.mb = (struct multiboot_header *) (buffer + i);
      entry_addr = (entry_func) pu.mb->entry_addr;
      cur_addr = pu.mb->load_addr;
      /* first offset into file */
      grub_seek (i - (pu.mb->header_addr - cur_addr));

      /* If the load end address is zero, load the whole contents.  */
      if (! pu.mb->load_end_addr)
	pu.mb->load_end_addr = cur_addr + filemax;
      
      text_len = pu.mb->load_end_addr - cur_addr;
      data_len = 0;

      /* If the bss end address is zero, assume that there is no bss area.  */
      if (! pu.mb->bss_end_addr)
	pu.mb->bss_end_addr = pu.mb->load_end_addr;
      
      bss_len = pu.mb->bss_end_addr - pu.mb->load_end_addr;

      if (pu.mb->header_addr < pu.mb->load_addr
	  || pu.mb->load_end_addr <= pu.mb->load_addr
	  || pu.mb->bss_end_addr < pu.mb->load_end_addr
	  || (pu.mb->header_addr - pu.mb->load_addr) > i)
	errnum = ERR_EXEC_FORMAT;

      if (cur_addr < 0x100000)
	errnum = ERR_BELOW_1MB;

      pu.aout = (struct exec *) buffer;
      exec_type = 2;
      str = "kludge";
    }
  else if (len > sizeof (struct exec) && !N_BADMAG ((*(pu.aout))))
    {
      entry_addr = (entry_func) pu.aout->a_entry;

      if (type == KERNEL_TYPE_NONE)
	{
	  /*
	   *  If it doesn't have a Multiboot header, then presume
	   *  it is either a FreeBSD or NetBSD executable.  If so,
	   *  then use a magic number of normal ordering, ZMAGIC to
	   *  determine if it is FreeBSD.
	   *
	   *  This is all because freebsd and netbsd seem to require
	   *  masking out some address bits...  differently for each
	   *  one...  plus of course we need to know which booting
	   *  method to use.
	   */
	  entry_addr = (entry_func) ((int) entry_addr & 0xFFFFFF);
	  
	  if (buffer[0] == 0xb && buffer[1] == 1)
	    {
	      type = KERNEL_TYPE_FREEBSD;
	      cur_addr = (int) entry_addr;
	      str2 = "FreeBSD";
	    }
	  else
	    {
	      type = KERNEL_TYPE_NETBSD;
	      cur_addr = (int) entry_addr & 0xF00000;
	      if (N_GETMAGIC ((*(pu.aout))) != NMAGIC)
		align_4k = 0;
	      str2 = "NetBSD";
	    }
	}

      /* first offset into file */
      grub_seek (N_TXTOFF (*(pu.aout)));
      text_len = pu.aout->a_text;
      data_len = pu.aout->a_data;
      bss_len = pu.aout->a_bss;

      if (cur_addr < 0x100000)
	errnum = ERR_BELOW_1MB;

      exec_type = 1;
      str = "a.out";
    }
  else if (lh->boot_flag == BOOTSEC_SIGNATURE
	   && lh->setup_sects <= LINUX_MAX_SETUP_SECTS)
    {
      int big_linux = 0;
      int setup_sects = lh->setup_sects;

      if (lh->header == LINUX_MAGIC_SIGNATURE && lh->version >= 0x0200)
	{
	  big_linux = (lh->loadflags & LINUX_FLAG_BIG_KERNEL);
	  lh->type_of_loader = LINUX_BOOT_LOADER_TYPE;

	  /* Put the real mode part at as a high location as possible.  */
	  linux_data_real_addr
	    = (char *) ((mbi.mem_lower << 10) - LINUX_SETUP_MOVE_SIZE);
	  /* But it must not exceed the traditional area.  */
	  if (linux_data_real_addr > (char *) LINUX_OLD_REAL_MODE_ADDR)
	    linux_data_real_addr = (char *) LINUX_OLD_REAL_MODE_ADDR;

	  if (lh->version >= 0x0201)
	    {
	      lh->heap_end_ptr = LINUX_HEAP_END_OFFSET;
	      lh->loadflags |= LINUX_FLAG_CAN_USE_HEAP;
	    }

	  if (lh->version >= 0x0202)
	    lh->cmd_line_ptr = linux_data_real_addr + LINUX_CL_OFFSET;
	  else
	    {
	      lh->cl_magic = LINUX_CL_MAGIC;
	      lh->cl_offset = LINUX_CL_OFFSET;
	      lh->setup_move_size = LINUX_SETUP_MOVE_SIZE;
	    }
	}
      else
	{
	  /* Your kernel is quite old...  */
	  lh->cl_magic = LINUX_CL_MAGIC;
	  lh->cl_offset = LINUX_CL_OFFSET;
	  
	  setup_sects = LINUX_DEFAULT_SETUP_SECTS;

	  linux_data_real_addr = (char *) LINUX_OLD_REAL_MODE_ADDR;
	}
      
      /* If SETUP_SECTS is not set, set it to the default (4).  */
      if (! setup_sects)
	setup_sects = LINUX_DEFAULT_SETUP_SECTS;

      data_len = setup_sects << 9;
      text_len = filemax - data_len - SECTOR_SIZE;

      linux_data_tmp_addr = (char *) LINUX_BZIMAGE_ADDR + text_len;
      
      if (! big_linux
	  && text_len > linux_data_real_addr - (char *) LINUX_ZIMAGE_ADDR)
	{
	  grub_printf (" linux 'zImage' kernel too big, try 'make bzImage'\n");
	  errnum = ERR_WONT_FIT;
	}
      else if (linux_data_real_addr + LINUX_SETUP_MOVE_SIZE
	       > RAW_ADDR ((char *) (mbi.mem_lower << 10)))
	errnum = ERR_WONT_FIT;
      else
	{
	  grub_printf ("   [Linux-%s, setup=0x%x, size=0x%x]\n",
		       (big_linux ? "bzImage" : "zImage"), data_len, text_len);

	  /* Video mode selection support. What a mess!  */
	  /* NOTE: Even the word "mess" is not still enough to
	     represent how wrong and bad the Linux video support is,
	     but I don't want to hear complaints from Linux fanatics
	     any more. -okuji  */
	  {
	    char *vga;
	
	    /* Find the substring "vga=".  */
	    vga = grub_strstr (arg, "vga=");
	    if (vga)
	      {
		char *value = vga + 4;
		int vid_mode;
	    
		/* Handle special strings.  */
		if (substring ("normal", value) < 1)
		  vid_mode = LINUX_VID_MODE_NORMAL;
		else if (substring ("ext", value) < 1)
		  vid_mode = LINUX_VID_MODE_EXTENDED;
		else if (substring ("ask", value) < 1)
		  vid_mode = LINUX_VID_MODE_ASK;
		else if (safe_parse_maxint (&value, &vid_mode))
		  ;
		else
		  {
		    /* ERRNUM is already set inside the function
		       safe_parse_maxint.  */
		    grub_close ();
		    return KERNEL_TYPE_NONE;
		  }
	    
		lh->vid_mode = vid_mode;
	      }
	  }

	  /* Check the mem= option to limit memory used for initrd.  */
	  {
	    char *mem;
	
	    mem = grub_strstr (arg, "mem=");
	    if (mem)
	      {
		char *value = mem + 4;
	    
		safe_parse_maxint (&value, &linux_mem_size);
		switch (errnum)
		  {
		  case ERR_NUMBER_OVERFLOW:
		    /* If an overflow occurs, use the maximum address for
		       initrd instead. This is good, because MAXINT is
		       greater than LINUX_INITRD_MAX_ADDRESS.  */
		    linux_mem_size = LINUX_INITRD_MAX_ADDRESS;
		    errnum = ERR_NONE;
		    break;
		
		  case ERR_NONE:
		    {
		      int shift = 0;
		  
		      switch (grub_tolower (*value))
			{
			case 'g':
			  shift += 10;
			case 'm':
			  shift += 10;
			case 'k':
			  shift += 10;
			default:
			  break;
			}
		  
		      /* Check an overflow.  */
		      if (linux_mem_size > (MAXINT >> shift))
			linux_mem_size = LINUX_INITRD_MAX_ADDRESS;
		      else
			linux_mem_size <<= shift;
		    }
		    break;
		
		  default:
		    linux_mem_size = 0;
		    errnum = ERR_NONE;
		    break;
		  }
	      }
	    else
	      linux_mem_size = 0;
	  }
      
	  /* It is possible that DATA_LEN + SECTOR_SIZE is greater than
	     MULTIBOOT_SEARCH, so the data may have been read partially.  */
	  if (data_len + SECTOR_SIZE <= MULTIBOOT_SEARCH)
	    grub_memmove (linux_data_tmp_addr, buffer,
			  data_len + SECTOR_SIZE);
	  else
	    {
	      grub_memmove (linux_data_tmp_addr, buffer, MULTIBOOT_SEARCH);
	      grub_read (linux_data_tmp_addr + MULTIBOOT_SEARCH,
			 data_len + SECTOR_SIZE - MULTIBOOT_SEARCH);
	    }
	  
	  if (lh->header != LINUX_MAGIC_SIGNATURE ||
	      lh->version < 0x0200)
	    /* Clear the heap space.  */
	    grub_memset (linux_data_tmp_addr + ((setup_sects + 1) << 9),
			 0,
			 (64 - setup_sects - 1) << 9);
      
	  /* Copy command-line plus memory hack to staging area.
	     NOTE: Linux has a bug that it doesn't handle multiple spaces
	     between two options and a space after a "mem=" option isn't
	     removed correctly so the arguments to init could be like
	     {"init", "", "", NULL}. This affects some not-very-clever
	     shells. Thus, the code below does a trick to avoid the bug.
	     That is, copy "mem=XXX" to the end of the command-line, and
	     avoid to copy spaces unnecessarily. Hell.  */
	  {
	    char *src = skip_to (0, arg);
	    char *dest = linux_data_tmp_addr + LINUX_CL_OFFSET;
	
	    while (dest < linux_data_tmp_addr + LINUX_CL_END_OFFSET && *src)
	      *(dest++) = *(src++);
	
	    /* Old Linux kernels have problems determining the amount of
	       the available memory.  To work around this problem, we add
	       the "mem" option to the kernel command line.  This has its
	       own drawbacks because newer kernels can determine the
	       memory map more accurately.  Boot protocol 2.03, which
	       appeared in Linux 2.4.18, provides a pointer to the kernel
	       version string, so we could check it.  But since kernel
	       2.4.18 and newer are known to detect memory reliably, boot
	       protocol 2.03 already implies that the kernel is new
	       enough.  The "mem" option is added if neither of the
	       following conditions is met:
	       1) The "mem" option is already present.
	       2) The "kernel" command is used with "--no-mem-option".
	       3) GNU GRUB is configured not to pass the "mem" option.
	       4) The kernel supports boot protocol 2.03 or newer.  */
	    if (! grub_strstr (arg, "mem=")
		&& ! (load_flags & KERNEL_LOAD_NO_MEM_OPTION)
		&& lh->version < 0x0203		/* kernel version < 2.4.18 */
		&& dest + 15 < linux_data_tmp_addr + LINUX_CL_END_OFFSET)
	      {
		*dest++ = ' ';
		*dest++ = 'm';
		*dest++ = 'e';
		*dest++ = 'm';
		*dest++ = '=';
	    
		dest = convert_to_ascii (dest, 'u', (extended_memory + 0x400));
		*dest++ = 'K';
	      }
	
	    *dest = 0;
	  }
      
	  /* offset into file */
	  grub_seek (data_len + SECTOR_SIZE);
      
	  cur_addr = (int) linux_data_tmp_addr + LINUX_SETUP_MOVE_SIZE;
	  grub_read ((char *) LINUX_BZIMAGE_ADDR, text_len);
      
	  if (errnum == ERR_NONE)
	    {
	      grub_close ();
	  
	      /* Sanity check.  */
	      if (suggested_type != KERNEL_TYPE_NONE
		  && ((big_linux && suggested_type != KERNEL_TYPE_BIG_LINUX)
		      || (! big_linux && suggested_type != KERNEL_TYPE_LINUX)))
		{
		  errnum = ERR_EXEC_FORMAT;
		  return KERNEL_TYPE_NONE;
		}
	  
	      /* Ugly hack.  */
	      linux_text_len = text_len;
	  
	      return big_linux ? KERNEL_TYPE_BIG_LINUX : KERNEL_TYPE_LINUX;
	    }
	}
    }
  else				/* no recognizable format */
    errnum = ERR_EXEC_FORMAT;

  /* return if error */
  if (errnum)
    {
      grub_close ();
      return KERNEL_TYPE_NONE;
    }

  /* fill the multiboot info structure */
  mbi.cmdline = (int) arg;
  mbi.mods_count = 0;
  mbi.mods_addr = 0;
  mbi.boot_device = (current_drive << 24) | current_partition;
  mbi.flags &= ~(MB_INFO_MODS | MB_INFO_AOUT_SYMS | MB_INFO_ELF_SHDR);
  mbi.syms.a.tabsize = 0;
  mbi.syms.a.strsize = 0;
  mbi.syms.a.addr = 0;
  mbi.syms.a.pad = 0;

  printf ("   [%s-%s", str2, str);

  str = "";

  if (exec_type)		/* can be loaded like a.out */
    {
      if (flags & MULTIBOOT_AOUT_KLUDGE)
	str = "-and-data";

      printf (", loadaddr=0x%x, text%s=0x%x", cur_addr, str, text_len);

      /* read text, then read data */
      if (grub_read ((char *) RAW_ADDR (cur_addr), text_len) == text_len)
	{
	  cur_addr += text_len;

	  if (!(flags & MULTIBOOT_AOUT_KLUDGE))
	    {
	      /* we have to align to a 4K boundary */
	      if (align_4k)
		cur_addr = (cur_addr + 0xFFF) & 0xFFFFF000;
	      else
		printf (", C");

	      printf (", data=0x%x", data_len);

	      if ((grub_read ((char *) RAW_ADDR (cur_addr), data_len)
		   != data_len)
		  && !errnum)
		errnum = ERR_EXEC_FORMAT;
	      cur_addr += data_len;
	    }

	  if (!errnum)
	    {
	      memset ((char *) RAW_ADDR (cur_addr), 0, bss_len);
	      cur_addr += bss_len;

	      printf (", bss=0x%x", bss_len);
	    }
	}
      else if (!errnum)
	errnum = ERR_EXEC_FORMAT;

      if (!errnum && pu.aout->a_syms
	  && pu.aout->a_syms < (filemax - filepos))
	{
	  int symtab_err, orig_addr = cur_addr;

	  /* we should align to a 4K boundary here for good measure */
	  if (align_4k)
	    cur_addr = (cur_addr + 0xFFF) & 0xFFFFF000;

	  mbi.syms.a.addr = cur_addr;

	  *((int *) RAW_ADDR (cur_addr)) = pu.aout->a_syms;
	  cur_addr += sizeof (int);
	  
	  printf (", symtab=0x%x", pu.aout->a_syms);

	  if (grub_read ((char *) RAW_ADDR (cur_addr), pu.aout->a_syms)
	      == pu.aout->a_syms)
	    {
	      cur_addr += pu.aout->a_syms;
	      mbi.syms.a.tabsize = pu.aout->a_syms;

	      if (grub_read ((char *) &i, sizeof (int)) == sizeof (int))
		{
		  *((int *) RAW_ADDR (cur_addr)) = i;
		  cur_addr += sizeof (int);

		  mbi.syms.a.strsize = i;

		  i -= sizeof (int);

		  printf (", strtab=0x%x", i);

		  symtab_err = (grub_read ((char *) RAW_ADDR (cur_addr), i)
				!= i);
		  cur_addr += i;
		}
	      else
		symtab_err = 1;
	    }
	  else
	    symtab_err = 1;

	  if (symtab_err)
	    {
	      printf ("(bad)");
	      cur_addr = orig_addr;
	      mbi.syms.a.tabsize = 0;
	      mbi.syms.a.strsize = 0;
	      mbi.syms.a.addr = 0;
	    }
	  else
	    mbi.flags |= MB_INFO_AOUT_SYMS;
	}
    }
  else
    /* ELF executable */
    {
      unsigned loaded = 0, memaddr, memsiz, filesiz;
      Elf32_Phdr *phdr;

      /* reset this to zero for now */
      cur_addr = 0;

      /* scan for program segments */
      for (i = 0; i < pu.elf->e_phnum; i++)
	{
	  phdr = (Elf32_Phdr *)
	    (pu.elf->e_phoff + ((int) buffer)
	     + (pu.elf->e_phentsize * i));
	  if (phdr->p_type == PT_LOAD)
	    {
	      /* offset into file */
	      grub_seek (phdr->p_offset);
	      filesiz = phdr->p_filesz;
	      
	      if (type == KERNEL_TYPE_FREEBSD || type == KERNEL_TYPE_NETBSD)
		memaddr = RAW_ADDR (phdr->p_paddr & 0xFFFFFF);
	      else
		memaddr = RAW_ADDR (phdr->p_paddr);
	      
	      memsiz = phdr->p_memsz;
	      if (memaddr < RAW_ADDR (0x100000))
		errnum = ERR_BELOW_1MB;

	      /* If the memory range contains the entry address, get the
		 physical address here.  */
	      if (type == KERNEL_TYPE_MULTIBOOT
		  && (unsigned) entry_addr >= phdr->p_vaddr
		  && (unsigned) entry_addr < phdr->p_vaddr + memsiz)
		real_entry_addr = (entry_func) ((unsigned) entry_addr
						+ memaddr - phdr->p_vaddr);
		
	      /* make sure we only load what we're supposed to! */
	      if (filesiz > memsiz)
		filesiz = memsiz;
	      /* mark memory as used */
	      if (cur_addr < memaddr + memsiz)
		cur_addr = memaddr + memsiz;
	      printf (", <0x%x:0x%x:0x%x>", memaddr, filesiz,
		      memsiz - filesiz);
	      /* increment number of segments */
	      loaded++;

	      /* load the segment */
	      if (memcheck (memaddr, memsiz)
		  && grub_read ((char *) memaddr, filesiz) == filesiz)
		{
		  if (memsiz > filesiz)
		    memset ((char *) (memaddr + filesiz), 0, memsiz - filesiz);
		}
	      else
		break;
	    }
	}

      if (! errnum)
	{
	  if (! loaded)
	    errnum = ERR_EXEC_FORMAT;
	  else
	    {
	      /* Load ELF symbols.  */
	      Elf32_Shdr *shdr = NULL;
	      int tab_size, sec_size;
	      int symtab_err = 0;

	      mbi.syms.e.num = pu.elf->e_shnum;
	      mbi.syms.e.size = pu.elf->e_shentsize;
	      mbi.syms.e.shndx = pu.elf->e_shstrndx;
	      
	      /* We should align to a 4K boundary here for good measure.  */
	      if (align_4k)
		cur_addr = (cur_addr + 0xFFF) & 0xFFFFF000;
	      
	      tab_size = pu.elf->e_shentsize * pu.elf->e_shnum;
	      
	      grub_seek (pu.elf->e_shoff);
	      if (grub_read ((char *) RAW_ADDR (cur_addr), tab_size)
		  == tab_size)
		{
		  mbi.syms.e.addr = cur_addr;
		  shdr = (Elf32_Shdr *) mbi.syms.e.addr;
		  cur_addr += tab_size;
		  
		  printf (", shtab=0x%x", cur_addr);
  		  
		  for (i = 0; i < mbi.syms.e.num; i++)
		    {
		      /* This section is a loaded section,
			 so we don't care.  */
		      if (shdr[i].sh_addr != 0)
			continue;
		      
		      /* This section is empty, so we don't care.  */
		      if (shdr[i].sh_size == 0)
			continue;
		      
		      /* Align the section to a sh_addralign bits boundary.  */
		      cur_addr = ((cur_addr + shdr[i].sh_addralign) & 
				  - (int) shdr[i].sh_addralign);
		      
		      grub_seek (shdr[i].sh_offset);
		      
		      sec_size = shdr[i].sh_size;

		      if (! (memcheck (cur_addr, sec_size)
			     && (grub_read ((char *) RAW_ADDR (cur_addr),
					    sec_size)
				 == sec_size)))
			{
			  symtab_err = 1;
			  break;
			}
		      
		      shdr[i].sh_addr = cur_addr;
		      cur_addr += sec_size;
		    }
		}
	      else 
		symtab_err = 1;
	      
	      if (mbi.syms.e.addr < RAW_ADDR(0x10000))
		symtab_err = 1;
	      
	      if (symtab_err) 
		{
		  printf ("(bad)");
		  mbi.syms.e.num = 0;
		  mbi.syms.e.size = 0;
		  mbi.syms.e.addr = 0;
		  mbi.syms.e.shndx = 0;
		  cur_addr = 0;
		}
	      else
		mbi.flags |= MB_INFO_ELF_SHDR;
	    }
	}
    }

  if (! errnum)
    {
      grub_printf (", entry=0x%x]\n", (unsigned) entry_addr);
      
      /* If the entry address is physically different from that of the ELF
	 header, correct it here.  */
      if (real_entry_addr)
	entry_addr = real_entry_addr;
    }
  else
    {
      putchar ('\n');
      type = KERNEL_TYPE_NONE;
    }

  grub_close ();

  /* Sanity check.  */
  if (suggested_type != KERNEL_TYPE_NONE && suggested_type != type)
    {
      errnum = ERR_EXEC_FORMAT;
      return KERNEL_TYPE_NONE;
    }
  
  return type;
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
* 本函数实现读入模块功能。参数module指定要读入的模块文件名；参数arg指定要读入的
* 模块需要的参数。读入模块放置于cur_addr地址处(并且cur_addr会被更新)。之后根据模
* 块信息填写mbi和mll全局变量。
*/

int
load_module (char *module, char *arg)
{
  int len;

  /* if we are supposed to load on 4K boundaries */
  cur_addr = (cur_addr + 0xFFF) & 0xFFFFF000;

  if (!grub_open (module))
    return 0;

  len = grub_read ((char *) cur_addr, -1);
  if (! len)
    {
      grub_close ();
      return 0;
    }

  printf ("   [Multiboot-module @ 0x%x, 0x%x bytes]\n", cur_addr, len);

  /* these two simply need to be set if any modules are loaded at all */
  mbi.flags |= MB_INFO_MODS;
  mbi.mods_addr = (int) mll;

  mll[mbi.mods_count].cmdline = (int) arg;
  mll[mbi.mods_count].mod_start = cur_addr;
  cur_addr += len;
  mll[mbi.mods_count].mod_end = cur_addr;
  mll[mbi.mods_count].pad = 0;

  /* increment number of modules included */
  mbi.mods_count++;

  grub_close ();
  return 1;
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
* 本函数实现读入initrd功能。参数initrd为initrd的文件名。读入的位置为cur_addr。
* 并且更新Linux Header的ramdisk_image和ramdisk_size字段。返回非0表示成功。
*/

int
load_initrd (char *initrd)
{
  int len;
  unsigned long moveto;
  unsigned long max_addr;
  struct linux_kernel_header *lh
    = (struct linux_kernel_header *) (cur_addr - LINUX_SETUP_MOVE_SIZE);
  
#ifndef NO_DECOMPRESSION
  no_decompression = 1;
#endif
  
  if (! grub_open (initrd))
    goto fail;

  len = grub_read ((char *) cur_addr, -1);
  if (! len)
    {
      grub_close ();
      goto fail;
    }

  if (linux_mem_size)
    moveto = linux_mem_size;
  else
    moveto = (mbi.mem_upper + 0x400) << 10;
  
  moveto = (moveto - len) & 0xfffff000;
  max_addr = (lh->header == LINUX_MAGIC_SIGNATURE && lh->version >= 0x0203
	      ? lh->initrd_addr_max : LINUX_INITRD_MAX_ADDRESS);
  if (moveto + len >= max_addr)
    moveto = (max_addr - len) & 0xfffff000;
  
  /* XXX: Linux 2.3.xx has a bug in the memory range check, so avoid
     the last page.
     XXX: Linux 2.2.xx has a bug in the memory range check, which is
     worse than that of Linux 2.3.xx, so avoid the last 64kb. *sigh*  */
  moveto -= 0x10000;
  memmove ((void *) RAW_ADDR (moveto), (void *) cur_addr, len);

  printf ("   [Linux-initrd @ 0x%x, 0x%x bytes]\n", moveto, len);

  /* FIXME: Should check if the kernel supports INITRD.  */
  lh->ramdisk_image = RAW_ADDR (moveto);
  lh->ramdisk_size = len;

  grub_close ();

 fail:
  
#ifndef NO_DECOMPRESSION
  no_decompression = 0;
#endif

  return ! errnum;
}


#ifdef GRUB_UTIL
/* Dummy function to fake the *BSD boot.  */
static void
bsd_boot_entry (int flags, int bootdev, int sym_start, int sym_end,
		int mem_upper, int mem_lower)
{
  stop ();
}
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
* 本函数实现BSD启动功能。参数type为内核类型；参数bootdev为启动磁盘号；参数arg为
* 启动参数。解析arg参数得到clval作为启动标志。如果为KERNEL_TYPE_FREEBSD,则初始化
* bootinfo，并通过(*entry_addr) (clval, bootdev, 0, 0, 0, ((int) (&bi)));启动。
* 否则通过 (*entry_addr) (clval, bootdev, 0, end_mark, extended_memory, mbi.mem_lower);
* 来启动。
*/

/*
 *  All "*_boot" commands depend on the images being loaded into memory
 *  correctly, the variables in this file being set up correctly, and
 *  the root partition being set in the 'saved_drive' and 'saved_partition'
 *  variables.
 */


void
bsd_boot (kernel_t type, int bootdev, char *arg)
{
  char *str;
  int clval = 0, i;
  struct bootinfo bi;

#ifdef GRUB_UTIL
  entry_addr = (entry_func) bsd_boot_entry;
#else
  stop_floppy ();
#endif

  while (*(++arg) && *arg != ' ');
  str = arg;
  while (*str)
    {
      if (*str == '-')
	{
	  while (*str && *str != ' ')
	    {
	      if (*str == 'C')
		clval |= RB_CDROM;
	      if (*str == 'a')
		clval |= RB_ASKNAME;
	      if (*str == 'b')
		clval |= RB_HALT;
	      if (*str == 'c')
		clval |= RB_CONFIG;
	      if (*str == 'd')
		clval |= RB_KDB;
	      if (*str == 'D')
		clval |= RB_MULTIPLE;
	      if (*str == 'g')
		clval |= RB_GDB;
	      if (*str == 'h')
		clval |= RB_SERIAL;
	      if (*str == 'm')
		clval |= RB_MUTE;
	      if (*str == 'r')
		clval |= RB_DFLTROOT;
	      if (*str == 's')
		clval |= RB_SINGLE;
	      if (*str == 'v')
		clval |= RB_VERBOSE;
	      str++;
	    }
	  continue;
	}
      str++;
    }

  if (type == KERNEL_TYPE_FREEBSD)
    {
      clval |= RB_BOOTINFO;

      bi.bi_version = BOOTINFO_VERSION;

      *arg = 0;
      while ((--arg) > (char *) MB_CMDLINE_BUF && *arg != '/');
      if (*arg == '/')
	bi.bi_kernelname = arg + 1;
      else
	bi.bi_kernelname = 0;

      bi.bi_nfs_diskless = 0;
      bi.bi_n_bios_used = 0;	/* this field is apparently unused */

      for (i = 0; i < N_BIOS_GEOM; i++)
	{
	  struct geometry geom;

	  /* XXX Should check the return value.  */
	  get_diskinfo (i + 0x80, &geom);
	  /* FIXME: If HEADS or SECTORS is greater than 255, then this will
	     break the geometry information. That is a drawback of BSD
	     but not of GRUB.  */
	  bi.bi_bios_geom[i] = (((geom.cylinders - 1) << 16)
				+ (((geom.heads - 1) & 0xff) << 8)
				+ (geom.sectors & 0xff));
	}

      bi.bi_size = sizeof (struct bootinfo);
      bi.bi_memsizes_valid = 1;
      bi.bi_bios_dev = saved_drive;
      bi.bi_basemem = mbi.mem_lower;
      bi.bi_extmem = extended_memory;

      if (mbi.flags & MB_INFO_AOUT_SYMS)
	{
	  bi.bi_symtab = mbi.syms.a.addr;
	  bi.bi_esymtab = mbi.syms.a.addr + 4
	    + mbi.syms.a.tabsize + mbi.syms.a.strsize;
	}
#if 0
      else if (mbi.flags & MB_INFO_ELF_SHDR)
	{
	  /* FIXME: Should check if a symbol table exists and, if exists,
	     pass the table to BI.  */
	}
#endif
      else
	{
	  bi.bi_symtab = 0;
	  bi.bi_esymtab = 0;
	}

      /* call entry point */
      (*entry_addr) (clval, bootdev, 0, 0, 0, ((int) (&bi)));
    }
  else
    {
      /*
       *  We now pass the various bootstrap parameters to the loaded
       *  image via the argument list.
       *
       *  This is the official list:
       *
       *  arg0 = 8 (magic)
       *  arg1 = boot flags
       *  arg2 = boot device
       *  arg3 = start of symbol table (0 if not loaded)
       *  arg4 = end of symbol table (0 if not loaded)
       *  arg5 = transfer address from image
       *  arg6 = transfer address for next image pointer
       *  arg7 = conventional memory size (640)
       *  arg8 = extended memory size (8196)
       *
       *  ...in actuality, we just pass the parameters used by the kernel.
       */

      /* call entry point */
      unsigned long end_mark;

      if (mbi.flags & MB_INFO_AOUT_SYMS)
	end_mark = (mbi.syms.a.addr + 4
		    + mbi.syms.a.tabsize + mbi.syms.a.strsize);
      else
	/* FIXME: it should be mbi.syms.e.size.  */
	end_mark = 0;
      
      (*entry_addr) (clval, bootdev, 0, end_mark,
		     extended_memory, mbi.mem_lower);
    }
}
