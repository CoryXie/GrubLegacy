/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Macros.  */

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the Multiboot header.  */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS		0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS		0x00010003
#endif

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* The size of our stack (16KB).  */
#define STACK_SIZE			0x4000

/* C symbol format. HAVE_ASM_USCORE is defined by configure.  */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)			_ ## sym
#else
# define EXT_C(sym)			sym
#endif

#ifndef ASM
/* Do not include here in boot.S.  */

/* Types.  */
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
/* The Multiboot header.  */
typedef struct multiboot_header
{
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;
  unsigned long header_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end_addr;
  unsigned long entry_addr;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  unsigned long tabsize;
  unsigned long strsize;
  unsigned long addr;
  unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct multiboot_info
{
  unsigned long flags;
  unsigned long mem_lower;
  unsigned long mem_upper;
  unsigned long boot_device;
  unsigned long cmdline;
  unsigned long mods_count;
  unsigned long mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  unsigned long mmap_length;
  unsigned long mmap_addr;
} multiboot_info_t;

/* The module structure.  */
typedef struct module
{
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
} memory_map_t;

#endif /* ! ASM */
