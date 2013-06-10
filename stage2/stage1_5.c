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
* @topic 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @group 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年5月3日
*
* @details 注释详细内容: GRUB和stage文件
* 
* 1. stage文件
*
* GRUB 0.97 含有几个images文件，两个必需的文件stage1和 stage2，可选的为stage1_5
* 和两个网络引导的文件nxgrub和pxegrub。stage1 是用于引导GURB的一个必须的映象文件。
* 通常它是被嵌入到 MBR。或者一个分区的引导扇区之中。因为 PC 的引导扇区是 512 字
* 节，所以stage1也是512字节。stage1的作用就是从一个本地磁盘加载stage 2 或者
* stage1_5 。因为大小的限制，stage1 对stage2 或者stage1_5的位置进行编码。
*
* stage1与stage2文件一般位于/boot/grub/目录下，在这个目录下还有很多stage1_5的文
* 件，而且都是以文件系统格式命名的。它们的目的是在stage1和stage2之间搭建一个桥梁，
* 也就是stage1加载stage1_5, 然后stage1_5加载stage2。stage1 和stage1_5的不同之处
* 是 stage1无法识别文件系统，stage1_5可以。因为 Stage2 太大了，无法被嵌入到某个
* 固定的区域，而stage1_5可以安装在 MBR 之后的位置。
*
* 2.grub启动方式
*
* 1)stage1_5 -> stage2 方式
*
* 首先监测是否是合适的文件系统的驱动，如果存在就使用文件系统逻辑的方式stage2 ；
* 如果找不到就用blocklist的方式再找stage2，找到了挂起来，找不到报错。
*
* 2)stage1 -> stage2方式
*
* 如果确认stage1_5没有被安装在MBR之后， stage1就会使用记录的stage2的blocklist
* 寻找stage2,找到后挂起来，找不到报错。
*
* 3. GRUB的执行流程
*
* 当系统加电后，固化在BIOS中的程序首先对系统硬件进行自检，自检通过后，就加载启动
* 磁盘上的MBR，并将控制权交给MBR中的程序(stage1)，stage1执行，判断自己是否GRUB，
* 如果是且配置了stage1_5，则加载stage1_5，否则就转去加载启动扇区，接着，stage2
* 被加载并执行，由stage2借助stage1_5驱动文件系统，并查找grub.conf，显示启动菜单
* 供用户选择，然后根据用户的选择或默认配置加载操作系统内核，并将控制权交给操作
* 系统内核，由内核完成操作系统的启动。
*
* 4. GRUB涉及到几个重要的文件：
*
* 第一个就是stage1。它被安装在MBR扇区（0面0磁道的第1扇区），大小为512字节（446字
* 节代码+64字节分区表+2字节标志55AA），它负责加载存放于0面0道第2扇区的start程序。
*
* 第二个是stage1_5。stage1_5负责识别文件系统和加载stage2，所以stage1_5往往有多个，
* 以支持不同文件系统的读取。在安装GRUB的时候，GRUB会根据当前/boot/分区类型，加载
* 相应的stage1_5到0面0磁道的第3扇区。stage1_5是由start加载的。
*
* 第三个是stage2。它负责显示启动菜单和提供用户交互接口，并根据用户选择或默认配置
* 加载操作系统内核。同前两个文件不同，stage2是存放在磁盘上/boot/grub下。
*
* 第四个是menu.lst(/boot/grub/grub.conf的链接)。grub.conf是一个基于脚本的文本文
* 件，其中包含菜单显示的配置和各个操作系统的内核加载配置。GRUB根据grub.conf显示
* 启动菜单，提供同用户交互界面。GRUB正是根据用户选择或默认配置和grub.conf的内核
* 配置加载相应的内核程序，并把控制权交给内核程序，使得内核程序完成真正的操作系
* 统的启动。
*
* 其它重要文件，GRUB除了上面叙述的主要文件之外，还包括支持交互功能的一些磁盘程序。
* 主要包括/sbin/下的grub、grub-install、grub-md5-crypt和grub-terminfo和
* /usr/bin/mbchk，以及/boot/grub下的设备映像文件(device.map)和菜单背景图像文件
* (splash.xpm.gz)。
*
* 通过上面的分析总结，可以很容易地看出，GRUB实际上包含两部分，一部分被安装在磁
* 盘的特殊扇区，另外一部分则以文件的形式存在。要让GRUB启动操作系统，就必须首先
* 把GRUB的stage1和stage1_5(根据文件系统自动选择是否安装)安装到磁盘的特殊扇区，
* 另外，在磁盘的/boot/grub下存在有grub.conf、device.map等文件和支持交互的程序，
* 而且这些程序必须在PATH环境变量指定的路径中。具备了这些知识，相信不管是安装、
* 配置、备份或修复GRUB都不是件很难的是情。
*/

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
* @details 注释详细内容: GRUB 0.97中stage1.5过程
* 
* Stage1.5过程的作用很单一，但是非常关键。它的主要功用就是构造一个
* boot分区系统对应的文件系统，这样可以通过文件系统的路径（/boot/grub/）寻找
* stage2过程需要的core.img，进而加载到内存中开始执行。
*
* Stage1.5存在于0面0道3扇区开始的地方，并一直延续十几k字节的区域，具体的大小
* 与相应的文件系统的大小有关（文中涉及到了0面0道1-3+x扇区，这部分扇区为保留
* 扇区，BIOS不会放置任何数据。正因为如此如果转换到GPT分区形式，系统将不能被
* 正确引导，如上文所示，MBR后面的扇区都被其他内容所占据）。Stage1.5过程被构
* 建成多种不同类型，但是功能类似，下面简单介绍一下基本的stage1.5过程的文件系统。
* e2fs_stage1_5（针对ext2fs，可引导ext2和ext3文件系统）、fat_stage1_5（针对fat
* 文件系统，可引导fat32和fat16）、ffs_stage1_5、jfs_stage1_5、minix_stage1_5、
* reiserfs_stage1_5、vstafs_stage1_5和xfs_stage1_5，这些文件被称为stage1.5过程，
* 这些文件每个至少都在11k以上。除此之外还有两个比较特殊的文件，分别为nbgrub和
* pxegrub，这两个文件主要是在网络引导时使用，只是格式不同而已，他们很类似与
* stage2，只是需要建立网络来获取配置文件。
*
* 由于stage1.5过程中会涉及到多个文件系统对应的文件，因此本文中主要以ext2fs为例
* 进行说明，其他文件系统与此类似，可以同样进行分析理解。
*
* 对于ext2fs文件系统，用于生成该文件系统的stage1.5过程文件（e2fs_stage1_5）的
* 代码为stage2/fsys_ext2fs.c文件。
*
* 在stage2/filesys.h文件中定义了每个文件系统对外的接口，用于上层调用，作为
* stage2过程寻找核心代码使用，文件系统一般被定义的接口主要就是三个函数，分别
* 是mount、read和dir函数。对应ext2fs，其定义的函数为：
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
* 针对ext2fs有如上的函数名称，每个函数将具体在stage2/fsys_ext2fs.c文件中被定义，
* 这里面没有包含任何的写的过程，对于bootloader而言仅仅读就可以完成任务了，没必
* 要对其系统进行写操作。其中ext2fs_mount函数用于检查文件系统类型，并将superblock
* 读入到内存中；ext2fs_read函数和ext2fs_dir函数用于对文件系统具体的操作。
* 在stage2/fsys_ext2fs.c文件中除了需要对这三个函数的定义之外，还需要文件系统的
* 属性的数据结构（superblock、inode和group结构，这些结构最初被定义在
* include/linux/ext2_fs.h文件中），通过这些数据结构描述一个文件系统。
*
* 如果读者有兴趣可以试着创建新的文件系统的支持，可以参照目前存在的一些文件系统
* 的模板（实例）编写。
*/

static int saved_sector = -1;
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
* 通过 disk_read_hook = disk_read_savesect_func;设置为读取扇区数据的钩子，然后在
* 读取时在grub_read()里面进一步使用disk_read_func = disk_read_hook;设为内部钩子，
* 用以保存最后被读取的扇区号，在cmain()中用以判断是否真实读取了数据(saved_sector
* 不能再等于-1)。
*/

static void
disk_read_savesect_func (int sector, int offset, int length)
{
  saved_sector = sector;
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
* GRUB 0.97 Stage1.5 cmain()函数分析
*
* 注意在GRUB 0.97中有两个cmain()函数的定义，分别来自不同的源文件：
*
* 1）grub-0.97/stage2/stage1_5.c里面的cmain()函数
* 2）grub-0.97/stage2/stage2.c里面的cmain()函数
*
* 由于我们目前关注Stage1.5，因此主要先分析源文件【grub-0.97/stage2/stage1_5.c】
* 中的cmain()函数。
* 
* 这个函数代码主要调用【grub-0.97/stage2/disk_io.c】中的函数实现文件读入GRUB 
* 0.97的Stage2。
*
* 1) 首先是调用"grub_open (config_file)"来打开这个config_file，而这里的
* config_file，由于我们是在Stage1.5，因此这个变量是定义在汇编代码源文件
*【grub-0.97/stage2/asm.S】中的变量config_file，并且这个变量是指向一个"结构体"，
* 其第一个字段是4字节的值，默认是0xffffffff，第二个字段是一个字符串数组，值为
* "/boot/grub/stage2"。
*
* 由于这里我们考虑的是第一次调用grub_open()的情形，因此这里的open_partition()
* 应该会失败，从而会调用attempt_mount ()。对于我们考虑的EXT2文件系统，这个
* mount_func()应该是在fsys_table数组中定义的ext2fs_mount()。这里的ext2fs_mount()
* 主要是判断文件系统类型（读入文件系统超级块进入SUPERBLOCK结构体区域，进一步判断
* SUPERBLOCK->s_magic对应是不是EXT2_SUPER_MAGIC），我们暂时忽略EXT2文件系统加载
* 的细节。继而分析在grub_open()下一步要做的事情（忽略非Stage1.5的代码），对应
* EXT2文件系统，这里调用的应该是ext2fs_dir()，并且已经将文件系统超级块信息存入
* SUPERBLOCK结构体中。
*
* 2) 在grub_open()返回后，Stage1.5的cmain()就会连续调用
* "grub_read ((char *) 0x8000, SECTOR_SIZE * 2)"以及
* "grub_read ((char *) 0x8000 + SECTOR_SIZE * 2, -1)"。其实进一步分析grub_read()
* 会发现，Stage1.5对应的grub_read()主要代码还是调用read_func()函数指针。而这个
* read_func()函数指针对于EXT2文件系统就是ext2fs_read()，来自于
*【grub-0.97/stage2/fsys_ext2fs.c】。
*
* 3) 在读完Stage2的内容后，会调用grub_close()关闭。这里的grub_close()定义于
*【grub-0.97/stage2/disk_io.c】。因此，这里最主要的也是调用对应的文件系统支持
* 代码实现的close_func()，不过对于EXT2文件系统这个函数指针为空。
*
* 4) 如果读取Stage2文件成功，则会调用"chain_stage2 (0, 0x8200, saved_sector)"来
* 真正执行Stage2。这部分代码在下一节分析。
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
