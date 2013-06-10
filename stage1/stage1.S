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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* ÎÒÃÇÏÈ¸ø³öGRUB 0.97Æô¶¯Á÷³ÌµÄ×ÜÌå½éÉÜ£¬ÏÂÎÄ»áÉîÈëÔ´´úÂë¶ÔÆäÏêÏ¸½éÉÜ¡£
*
* 1£©GRUB 0.97¾ßÓĞÁ½¸öÖ÷ÒªÆô¶¯ÔËĞĞ½×¶Î£¬·Ö±ğÃüÃûÎªStage1ºÍStage2£»
*
* 2£©GRUB 0.97µÄStage1ÊÇ°²×°ÔÚÆô¶¯ÉÈÇøµÄ£¬´óĞ¡Ö»ÓĞÒ»¸öÉÈÇø£¬¼´512×Ö½Ú£»Ò»°ã¶øÑÔ
* Stage1°²×°ÔÚÉÈÇøºÅ0£¨LBAÄ£Ê½£©£»
*
* 3£©GRUB 0.97µÄStage2°´ÕÕ°²×°·½Ê½¿ÉÒÔ·ÖÎªÓĞStage1.5ºÍÃ»ÓĞStage1.5 Á½ÖÖ¡£²»ÂÛÓĞ
* Ã»ÓĞStage1.5£¬Stage2¶¼ÊÇ´Ó¡¾grub-0.97/stage2/start.S¡¿¿ªÊ¼µÄ£¬²¢ÇÒÕâ¶Î´úÂë´ó
* Ğ¡Ò²ÊÇ512×Ö½Ú£¬ÇÒÒ»°ãÊÇ°²×°ÔÚÉÈÇøºÅ1£¨LBAÄ£Ê½£©£»
*
* 4£©GRUB 0.97µÄStage1´úÂëÎª¡¾grub-0.97/stage1/stage1.S¡¿£¬¸ºÔğ¼ÓÔØStage2µÄµÚÒ»
* ¶Î´úÂë£¬¼´¡¾grub-0.97/stage2/start.S¡¿¶ÔÓ¦´úÂë£»
*
* 5£©GRUB 0.97µÄStage2µÄ¡¾grub-0.97/stage2/start.S¡¿ÊÇÓĞStage1.5ºÍÃ»ÓĞStage1.5
* µÄ·ÖË®Áë£»Èç¹ûÃ»ÓĞStage1.5£¬Ôò»áÖ±½Ó´ÓÉÈÇøºÅ2£¨LBAÄ£Ê½£©¿ªÊ¼¼ÓÔØÁ¬ĞøµÄÉÈÇø£¨
* ´óĞ¡ÎªSTAGE2_SIZE£©¡£Èç¹ûÓĞ£¬Ôò»áÏÈ¼ÓÔØStage1.5£¬½ø¶øÓÉStage1.5À´´ÓÎÄ¼şÏµÍ³
* ¼ÓÔØStage2µÄÕæÊµ´úÂë¡£
*/	
/*
 *  defines for the code go here
 */
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* ABSÊÇÔÚ¡¾grub-0.97/stage1/stage1.S¡¿¿ªÊ¼´¦¶¨ÒåµÄÒ»¸öºê£¬¸Ãºê½«Ò»¸ö±êºÅµÄµØÖ·
* Óë_start±êºÅÏà¼õ£¬µÃµ½¸Ã±êºÅÔÚ´úÂëÖĞµÄÏà¶ÔÆ«ÒÆµØÖ·£¬ÔÙ¼ÓÉÏ0x7c00¼´µÃµ½CS=0µÄ
* Çé¿öÏÂµÄ¸Ã±êºÅµÄ¾ø¶ÔµØÖ·£¨LJMPÖ®ºó±»ÖÃÓÚIPÖĞ£©¡£
*/
	/* Absolute addresses
	   This makes the assembler generate the address without support
	   from the linker. (ELF can't relocate 16-bit addresses!) */
#define ABS(x) (x-_start+0x7c00)
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* ´òÓ¡ÏûÏ¢ÊÇÍ¨¹ıµ÷ÓÃMSG()ºêÀ´ÊµÏÖµÄ¡£Êµ¼ÊÊÇÏÈ½«Ò»ÌõÏûÏ¢µÄ¾ø¶ÔµØÖ··Åµ½SI¼Ä´æÆ÷ÖĞ£¬
* È»ºóÊ¹ÓÃcallµ÷ÓÃmessageº¯ÊıÀ´ÊµÏÖÊä³öµÄ¡£
*/
	/* Print message string */
#define MSG(x)	movw $ABS(x), %si; call message

	/* XXX:	binutils-2.9.1.0.x doesn't produce a short opcode for this. */
#define	MOV_MEM_TO_AL(x)	.byte 0xa0;  .word x
	
	.file	"stage1.S"
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ1²½£¬X86 CPUÆô¶¯Ê±£¬CS=0xFFFF£¬IP=0£¬°´ÕÕÊµÄ£Ê½¹¤×÷£¬´ÓµØÖ·0xFFFF0´¦È¡µ½µÚ
* Ò»ÌõÖ¸Áî£»BIOS ROMµÄµÚÒ»ÌõÖ¸Áî¾ÍÊÇ·ÅÔÚÕâÀïµÄ£¬Òò´Ë¿ªÊ¼ÁËBIOSµÄÔËĞĞ¡£
*
* µÚ2²½£¬BIOSÔËĞĞ¹ı³ÌÖĞ£¬³ıÁË×ö»ù±¾µÄÓ²¼ş¼ì²âºÍÓ²¼ş³õÊ¼»¯Ö®Íâ£¬»¹ÓĞÒ»¸ö¹Ø¼üµÄ
* µØ·½ÊÇÒªÉèÖÃÊµÄ£Ê½ÏÂµÄÖĞ¶ËÏòÁ¿±í¡¾Interrupt Vector Table (IVT)¡¿£¬Î»ÓÚ0µØÖ·
* ´¦£¬²¢°²×°¶ÔÓ¦ÖĞ¶ÏºÅµÄÖĞ¶Ï´¦Àí³ÌĞò(ISR)¡£BIOS½Ó×Å»á¼ì²âÏµÍ³µÄÏÖÓĞÆô¶¯Éè±¸£¨
* ÓÃ»§¿ÉÉèÖÃ¸÷¸öÆô¶¯Éè±¸µÄÏà¶ÔË³Ğò£©£¬ÒÀ´Îµ÷ÓÃINT 0x19À´¶ÁÈ¡ÆäµÚÒ»¸öÉÈÇø£¬²¢
* ¼ì²âÆäÆô¶¯±ê¼Ç¡¾×îÄ©Î²2¸ö×Ö½Ú¡¿ÊÇ·ñÕıÈ·¡£ Èç¹ûÆô¶¯±ê¼ÇÕıÈ·£¬¾Í»á°Ñ¸ÃÆô¶¯ÉÈÇø
* ¼ÓÔØµ½ÄÚ´æµØÖ·0x07C00´¦£¬²¢ÇÒÌø×ªµ½0x07C00´¦½Ó×ÅÖ´ĞĞ¡£Òò´Ë£¬ÔÚ´ÓÊµÄ£Ê½ÇĞ»»µ½
* ±£»¤Ä£Ê½Ö®Ç°£¬Êµ¼ÊÉÏ¶¼»¹ÊÇÔÚBIOSµÄÉÏÏÂÎÄÖĞÔËĞĞ£¬Òò´Ë¾Í¿ÉÒÔ×öBIOSÌá¹©µÄÖĞ¶Ïµ÷
* ÓÃ£¬ËùÊ¹ÓÃµÄÖĞ¶Ï·şÎñ³ÌĞòÒ²¾ÍÊÇÇ°ÃæÔÚÖĞ¶ÏÏòÁ¿±íÖĞ°²×°µÄISR¡£
*
* µÚ3²½£¬BIOS¼ÓÔØÆô¶¯ÉÈÇø²¢Ìø×ªµ½0x07C00´¦ºó£¬¾Í¿ªÊ¼ÁËÕæÕıµÄBootloaderµÄÖ´ĞĞ£¬
* Õâ¾ÍÊÇÎÒÃÇÕâÀïÒª·ÖÎöµÄGRUB 0.97µÄµÚÒ»¶ÎÕæÕı±»CPUÖ´ĞĞµÄ´úÂë¡£Õâ¶Î´úÂëÎ»ÓÚ
* ¡¾grub-0.97/stage1/stage1.S¡¿¡£BIOSÌø×ªµ½0x07C00ÊÇÍ¨¹ıJMPÊµÏÖµÄ (CS:IP 
* 0:0x7C00)£¬Òò´Ë£¬µ±ÔÚstage1.SÖĞ¿ªÊ¼ÔËĞĞÊ±µÄ´úÂë¶Î¼Ä´æÆ÷ºÍÖ¸ÁîÖ¸Õë¼Ä´æ·Ö±ğÊÇ
* CS=0£¬IP=0x7C00¡£ÌØ±ğĞèÒª×¢ÒâµÄÊÇ£¬Ä¿Ç°CPU»¹´¦ÓÚ16Î»ÊµÄ£Ê½£¬Òò´ËĞèÒªÃ÷È·¸æ
* Öªgas»ã±àÆ÷Éú³É16Î»X86Ö¸Áî£¬·ñÔòÎŞ·¨ÕıÈ·ÔËĞĞ¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ4²½£¬ÓÉÓÚÎªÁË¼æÈİFAT/HPFS´ÅÅÌÎÄ¼şÏµÍ³¸ñÊ½£¬ÒªÇóÔÚÆô¶¯ÉÈÇøµÄµÚ4×Ö½Ú¿ªÊ¼´¦ÓĞ
* Ò»¸öËùÎ½µÄ"BIOS²ÎÊı¿é"£¬Òò´ËÎÒÃÇµÄ´úÂë±ØĞëÔ½¹ıÕâÒ»¸ö²ÎÊı¿é£¬´Ó¶øµÚÒ»ÌõÖ¸Áî¾Í
* ÊÇ×öÒ»¸öÌø×ª£¬Ìø×ªµ½½ô½Ó×Å²ÎÊı¿éµÄµØ·½¿ªÊ¼ÔËĞĞ¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ5²½£¬½ûÖ¹ÖĞ¶Ï²úÉú£¬ÒòÎª½ô½Ó×ÅµÄ´úÂë»á³¢ÊÔ¸üÕıÒ»Ğ©BIOSµÄbugÒÔ¼°ÉèÖÃ¶ÑÕ»Ö¸Õë£¬
* ÕâĞ©²Ù×÷×îºÃÊÇÔÚÔ­×Ó×´Ì¬½øĞĞ£¨¼´²»±»ÖĞ¶Ï¸ÉÈÅµÄÇé¿öÏÂ£©¡£
*/
/* general setup */
	cli		/* we're not safe here! */
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ6²½£¬¼ì²âÆô¶¯Éè±¸£¨Éè±¸ºÅÓÉBIOSÔÚÌø×ªÇ°±£´æÔÚDL¼Ä´æÆ÷ÖĞ£©£¬µ«ÊÇÓĞĞ©BIOS´í
* ÎóµØ´«µİÁËÆô¶¯Éè±¸ºÅ£¬Òò´ËÕâÀïÒª³¢ÊÔĞŞÕı¡£×¢ÒâÕâÀïËÆºõÊµ¼Ê´úÂëºÍ×¢ÊÍÓĞËùÍÑ½Ú£¬
* ÒòÎªÕâÀïÊÇÒ»¸öÖ±½ÓµÄÌø×ªÖ¸Áî£¬×¢ÊÍÖĞËùËµµÄ²âÊÔDLÊÇ·ñÕıÈ·ÉèÖÃÑÚÂëµÄ²Ù×÷ÊÇ²»ÄÜ
* ±»Ö´ĞĞµÄ¡£¿ÉÄÜµÄÔ­ÒòÊÇ£¬´ó²¿·ÖµÄBIOS»¹ÊÇ¿ÉÒÔÕıÈ·´«µİÆô¶¯ÉÈÇøµÄ£¬Òò´Ë´ó²¿·ÖÇé
* ¿öÕâ¶Î´úÂë²»ĞèÒªÖ´ĞĞ¡£Èç¹ûÕæµÄÓöµ½ÕâÑùµÄBIOS£¬Ó¦¸Ã»¹ÊÇÒª×¢ÊÍµôµÚÒ»¸ö"jmp 1f"
* Ö¸Áî¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ7²½£¬ÓÉÓÚÓĞĞ©BIOSÌø×ªµ½0x07C00µÄ¶¯×÷ÊÇÊ¹ÓÃJMP 07C0:0000µÄ·½Ê½ÊµÏÖµÄ£¬¶ø²»ÊÇ
* JMP 0000:7C00µÄ·½Ê½£¬Òò´Ëµ½ÕâÀïµÚÒ»ÌõÖ¸ÁîµÄÊ±ºò£¬CS=0x07C0£¬¶øIP=0x0000£¬ÕâÓë
* ÎÒÃÇÏÂÃæĞèÒªÊ¹ÓÃµÄCSºÍIPÏà¶ÔÖµ²»Ò»ÖÂ£¬Òò´ËÕâÀïĞèÒªÒ»¸öÇ¿ÖÆµÄ°ì·¨À´¸üĞÂCSºÍIP
* µÄÖµ¡£ÕâÀï²ÉÓÃµÄ¾ÍÊÇLJMPÖ¸ÁîµÄ·½Ê½£¬¸ÃÖ¸ÁîÖ´ĞĞÖ®ºó£¬CS»á±»¸ü¸ÄÎª0£¬¶øIP»á¸ü¸Ä
* Îª$ABS(real_start)£¬¼´ÏÂÒ»ÌõÒªÖ´ĞĞµÄÖ¸ÁîµÄ¾ø¶ÔµØÖ·¡£
*/
	/*
	 * ljmp to the next instruction because some bogus BIOSes
	 * jump to 07C0:0000 instead of 0000:7C00.
	 */
	ljmp	$0, $ABS(real_start)
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ8²½£¬ÔÚÇ°Ò»²½ÒÑ¾­½«CSÇ¿ÖÆÖÃÎª0£¬ÎÒÃÇ»¹ĞèÒª½«ÆäËûµÄĞèÒªÊ¹ÓÃµÄ¶Î¼Ä´æÆ÷£¬°üÀ¨
* Êı¾İ¶ÎºÍ¶ÑÕ»¶Î¼Ä´æÆ÷£¬¶¼ÖÃÎª0¡£Ö®ºó£¬ÔÙÉèÖÃ¶ÑÕ»Ö¸Õë¼Ä´æÆ÷ÎªSTAGE1_STACKSEG£¬
* ¸ÃÖµÔÚ¡¾grub-0.97/stage1/stage1.h¡¿ÖĞ¶¨ÒåÎª0x2000¡£ÕâÑùÒ»À´£¬CS=DS=SS=0£¬´Ó
* ¶øCPUµÄÔËĞĞµØÖ·²»ÔÙÒÀÀµÓÚÕâ¼¸¸ö¶Î¼Ä´æÆ÷£¬Ïàµ±ÓÚ½ûÖ¹ÁË·Ö¶Î»úÖÆ£¬ÓëÆÕÍ¨µÄRISC
* ÕóÓªµÄCPUÑ°Ö·Ä£Ê½ÏàËÆ£¬Ö»ĞèÒª¹Ø×¢µØÖ·µÄÆ«ÒÆÁ¿²¿·Ö¡£µ±ÉèÖÃÁËÕâĞ©»ù±¾¼Ä´æÆ÷Ö®
* ºó£¬¾Í¿ÉÒÔÔÊĞíÖĞ¶ÏÁË£¬Òò´Ë´ò¿ªÖĞ¶ÏÊ¹ÄÜ¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ9²½£¬¼ì²éÔÚBIOS²ÎÊı¿éÖĞµÄboot_driveµØÖ·´¦ÊÇ·ñÓĞÒ»¸öºÏ·¨µÄ´ÅÅÌºÅ£¬Èç¹ûÔÚÆô¶¯
* ÉÈÇøµÄÕâ¸öµØÖ·±»Ğ´ÈëÁËÒ»¸öºÏ·¨µÄÆô¶¯´ÅÅÌºÅ£¬Õâ½«Õâ¸ö´ÅÅÌºÅ¸üĞÂµ½DL¼Ä´æÆ÷ÖĞ£¬
* ·ñÔò¼ÌĞøÊ¹ÓÃ´ÓBIOS´«µİÀ´µÄ´ÅÅÌºÅ¡£Õâ¸ö´ÅÅÌºÅ»áÔÚºóÃæ±»Ê¹ÓÃÀ´¶ÁÈ¡GRUB 0.97µÄ
* Stage2µÄÓ³ÏñµÄ¡£×¢Òâ£¬ÕâÀïËäÈ»ÎÒÃÇµÄÔ´´úÂëÔÚboot_driveÕâ¸öÎ»ÖÃÌîÈëµÄÖµÊÇÒ»¸ö
* ÎŞĞ§µÄÖµ£¨GRUB_INVALID_DRIVE£¬¼´0xFF£©£¬µ«ÊÇÍâ²¿³ÌĞò£¬ÀıÈçÒ»¸ö¶ş½øÖÆ±à¼­³ÌĞò£¬
* »òÕßÔÚÔËĞĞÖĞµÄ²Ù×÷ÏµÍ³ÄÚµÄ´ÅÅÌ¹ÜÀí³ÌĞò£¬¶¼¿ÉÒÔĞŞ¸ÄÆô¶¯ÉÈÇøµÄÕâ¸öÎ»ÖÃ£¬ÌîÈëÒ»
* ¸öÊÊµ±µÄÖµÀ´¸Ä±äÕâ¸öÓÃÒÔ¼ÓÔØStage2µÄ´ÅÅÌºÅ£¬´Ó¶ø¸Ä±ä¼ÓÔØµÄÆô¶¯Ó³Ïñ¡£´Ó°²È«½Ç
* ¶È½²£¬ÕâÊÇÒ»¸öÖµµÃ×¢ÒâµÄµØ·½¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ10²½£¬½«DX¼Ä´æÆ÷Ñ¹Èë¶ÑÕ»£¨´Ó¶øÔÚÏÂÃæµÄ´úÂëÖĞ¿ÉÒÔÊ¹ÓÃDX¼Ä´æÆ÷£¬ĞèÒªÆô¶¯´ÅÅÌ
* ºÅÊ±ÔÙ´Ó¶ÑÕ»´¦µ¯³ö£©¡£½Ó×Å´òÓ¡Ò»ÌõÏûÏ¢"GRUB"£¬¸æËßÓÃ»§GRUBÕıÔÚÆô¶¯¡£
*/
	/* print a notification message on the screen */
	MSG(notification_string)
/**
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ11²½£¬¼ì²éÇı¶¯´ÅÅÌÊÇ·ñÖ§³ÖLBAÄ£Ê½¡£²¢¸ù¾İ¼ì²â½á¹û¾ö¶¨°´ÕÕLBAÄ£Ê½»¹ÊÇCHSÄ£Ê½
* ·ÃÎÊÆô¶¯Éè±¸¡£Ê×ÏÈ£¬Èç¹ûÆô¶¯´ÅÅÌÊÇÈíÇı£¬ÄÇÃ´¿Ï¶¨²»Ö§³ÖLBAÄ£Ê½£¬Ö±½ÓÌø×ªµ½
* chs_mode¡£
*
* ×¢Òâ£¬ÕâÀïÊ¹ÓÃµÄÊÇBIOS INT 0x13µÄÀ©Õ¹À´ÊµÏÖLBAÖ§³ÖµÄ¼ì²âµÄ¡£¸ù¾İÀ´×Ô¡¶BIOS 
* Enhanced Disk Drive Specification¡·¶ÔÕâ¸öBIOSÖĞ¶Ïµ÷ÓÃÀ©Õ¹µÄÃèÊö¡£Èç¹ûµ÷ÓÃÊ§
* °Ü£¨Carry Set£©£¬»òÕßBXÃ»ÓĞ°´ÕÕ¹æ·¶ÒªÇó±»ÉèÖÃÎª0xAA55£¬CXÖĞµÄbit0£¨Fixed 
* disk access subset£©Ã»ÓĞÉèÖÃ£¬ÄÇÃ´Ö»ÄÜÊ¹ÓÃCHSÄ£Ê½¡£·´Ö®£¬Èç¹ûCXÖĞµÄbit0
*£¨Fixed disk access subset£©±»ÉèÖÃ£¬Ôò¿ÉÒÔÊ¹ÓÃ¸Ã¹¦ÄÜÖ§³ÖµÄdisk address packet 
* ½á¹¹À´·ÃÎÊ´ÅÅÌ£¬¶ødisk address packetÖĞÊ¹ÓÃµÄ¾ÍÊÇ´ÅÅÌÉÈÇøµÄLBAµØÖ·¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ12²½£¬Ê¹ÓÃLBAÄ£Ê½¶ÁÈ¡¡£ÕâÀïÊ×ÏÈÌîĞ´Ò»¸ö½Ğ×ö"disk address packet"µÄÊı¾İ½á¹¹£¬
* ¾ßÌå½á¹¹ÎÒÃÇÂíÉÏÔÚºóÃæ¸ø³ö¡£È»ºóµ÷ÓÃBIOS INT 0x13µÄÀ©Õ¹0x42À´Êµ¼Ê¶ÁÈ¡¡£
*
* ÕâÀïÎÒÃÇ·¢ÏÖµÚÒ»¾äÊÇ"movl	0x10(%si), %ecx"£¬ÇÒ×¢ÊÍËµ"save the total number of 
* sectors"£¬µ«ÊÇÎÒÃÇÍ¨¹ı·ÖÎö£¬·¢ÏÖ¸ù±¾Ö®Ç°Ã»ÓĞÃ÷È·µÄ¶Ô0x10(%si)¸³Öµ£¬Ç°ÃæÎ¨Ò»
* ÉèÖÃÁËSI¼Ä´æÆ÷µÄÊÇÔÚMSGºê´¦£¬µ«ÊÇÏÔÈ»ÄÇÀï¶ÔSIµÄÖµÓëÕâÀïµÄsectorÎŞ¹Ø£»´ËÍâ£¬
* ÎÒÃÇ¿ÉÒÔ·¢ÏÖÕâÀïµÄECX¼Ä´æÆ÷Ö®ºóÃ»ÓĞ±»Ê¹ÓÃµ½£¬Òò´Ë¿ÉÒÔÅĞ¶¨ÕâÀïµÄµÚÒ»¼şÃ»ÓĞÊµ¼Ê
* ÓÃ´¦¡£
*
* ÏÂÃæÒ»¾ä£¬½«disk_address_packet±êºÅµÄ¾ø¶ÔµØÖ·ÉèÖÃ¸øSI¼Ä´æÆ÷£¬´Ó¶øºóÃæ¿ÉÒÔÓÃ
* ¸öSI¼Ä´æÆ÷¼ÓÉÏÒ»¶¨µÄÆ«ÒÆÁ¿À´Ñ°Ö·disk_address_packetµÄ×Ö¶Î£¬´Ó¶ø¶ÔÆäÉèÖÃ¾ßÌå
* Öµ¡£ÕâÀï£¬±êºÅdisk_address_packetÊµ¼ÊºÍÆäËû¼¸¸ö±êºÅ¹²ÓÃÒ»¶ÎµØÖ·¿Õ¼ä£¬Õâ¶Î¹²
* ÏíµÄ¿Õ¼ä×Ü¹²14×Ö½Ú£¬µ«ÊÇºóÃæ¿ÉÒÔÔÚ¸½¼ÓÒ»Ğ©×Ö½Ú£¨´Ó¶ø¿ÉÒÔÂú×ã
* disk_address_packetÖÁÉÙ16×Ö½ÚµÄÒªÇó£©¡£Õâ¶Î¹²ÏíµÄ¿Õ¼äÊÇÒòÎªLBAºÍCHSÄ£Ê½Ö»ÄÜ
* Ñ¡ÔñÆäÖĞÒ»¸ö£¬Òò´ËËûÃÇ¹¤×÷µÄÊ±ºòµÄ²ÎÊı±äÁ¿Ò²Ö»ĞèÆäÖĞÒ»ÖÖ¡£
*
* ÏÂÒ»ÌõÖ¸Áî"movb	$1, -1(%si)"£¬½«mode±äÁ¿ÉèÖÃÎª1£¨´Ódisk_address_packetµØÖ·
* ÍùÇ°ÃæÒ»¸ö×Ö½Ú¾ÍÊÇmode±äÁ¿µÄµØÖ·£©£»ÔÙÏÂÒ»Ìõ£¬½«Stage2µÄÉÈÇøºÅ±£´æµ½EBX¼Ä´æ
* Æ÷ÖĞ£»ÔÙ½Ó×Å¾ÍÊÇÌîĞ´disk_address_packetµÄÇ°ÃæÁ½Ïî£¬Ö¸¶¨¸ÃpacketÎª16×Ö½Ú£»ÔÙ
* ÏÂÒ»Ìõ£¬Éè¶¨Stage2µÄÉÈÇø¸öÊı£¬¹Ì¶¨Ö»ÓĞÒ»¸öÉÈÇø¡£½Ó×ÅÔÙ½«EBX¼Ä´æÆ÷ÖĞµÄÖµ£¬Ò²
* ¼´Ç°Ãæ±£´æµÄStage2µÄÉÈÇøºÅÌîÈëdisk_address_packetµÄ"Starting logical block 
* address"×Ö¶Î£»ÔÙ½Ó×Å¾ÍÊÇ½«disk_address_packetµÄ"Address of transfer buffer"
* ÉèÖÃÎª0x7000:0x0000£¬Ò²¾ÍÊÇËµ£¬Stage2»á±»¶ÁÈëµ½0x70000´¦(STAGE1_BUFFERSEGÔÚ
*¡¾grub-0.97/stage1/stage1.h¡¿ÖĞ¶¨ÒåÎª0x7000)¡£ ºóÃæ¾ÍÊÇÕæµÄµ÷ÓÃ"int $0x13"À´
* ¶ÁÈ¡Stage2µÄ¹ı³Ì£¬Èç¹ûÊ§°Ü£¬Ôò¼ÌĞø³¢ÊÔÊ¹ÓÃCHSÄ£Ê½¶ÁÈ¡¡£Èç¹û³É¹¦£¬ÔòÌø×ªµ½
* copy_buffer´¦Ö´ĞĞ¡£ÔÚÌø×ªÖ®Ç°£¬½«STAGE1_BUFFERSEGÉèÖÃµ½BX¼Ä´æÆ÷ÖĞ£¬ÔÚ
* copy_bufferÖ´ĞĞ¹ı³ÌÖĞ»á±»ÓÃµ½¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ14²½£¬Èç¹ûÃ»ÓĞÖ´ĞĞLBAÄ£Ê½£¬ÄÇÃ´¾Í»áÂäµ½chs_mode¡£ÏÂÃæ·ÖÎöchs_mode²¿·Ö´úÂëµÄ
* ÊµÏÖ¡£
*
* Ê×ÏÈÊÇ"int	$0x13"µ÷ÓÃ0x08£¬¸Ãµ÷ÓÃµÄÊäÈëÊä³ö²ÎÊıÈçÏÂÍ¼ËùÊ¾£¬Èç¹û³É¹¦£¬ÔòÌø
* ×ªµ½final_initÖ´ĞĞ£¬·ñÔò¿´ÊÇ·ñ¿ÉÄÜĞèÒªÌ½²âFloppy£¬Èç¹ûĞèÒª¾ÍÌø×ªµ½floppy_probe
*£¨¾ßÌå·ÖÎö¼ûÏÂÎÄ£©£¬×îÔã¸âµÄÇéĞÎÊÇ³öÏÖÓ²ÅÌ³ö´í£¬ÄÇÃ´Ö»ÄÜÌø×ªhd_probe_error¡£
*
* ¸ù¾İ¶ÁÈ¡µÄ´ÅÅÌ²ÎÊı£¬final_initÖ´ĞĞÒ»Ğ©½øÒ»²½µÄ²Ù×÷£¬Ö÷ÒªÊÇÌîĞ´ÔÚÇ°Ãæ¿´µ½¹ıµÄ
* Óëdisk_address_packet¹²ÏíµÄÒ»¶Î±äÁ¿µØÖ·¿Õ¼äÀïÃæµÄ±äÁ¿£¬°üÀ¨sectors£¬heads£¬
* cylinders£¬sector_start£¬head_start£¬ÒÔ¼°cylinder_start£¬Ïà¹Ø´úÂëÏà¶Ô¼òµ¥£¬
* Æä¹ı³Ì²»ÔÙÏêÊö¡£
*
* ÏÂÃæµÄsetup_sectors»á½øÒ»²½¸ù¾İÕâĞ©²ÎÊıÀ´¹¹ÔìBIOSµÄ"INT 0x13 Function 0x2"
* ĞèÒªµÄ²ÎÊıÖµ£¬²¢µ÷ÓÃ¸ÃBIOSµ÷ÓÃÀ´½²Stage2¶ÁÈëµ½STAGE1_BUFFERSEGÖ¸¶¨µÄ¶ÎµØÖ·´¦¡£
* ×îºó»¹ÊÇ½Ó×ÅÔËĞĞcopy_buffer,½«Stage2µÄ´úÂë´Ó0x70000¿½±´µ½0x8000´¦£¬²¢½Ó×ÅÌø
* ×ªµ½Stage2´¦ÔËĞĞ¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ13²½£¬Ö´ĞĞcopy_buffer¡£Ê×ÏÈ¾ÍÊÇ½«stage2_segmentµÄÖµÉèÖÃ¸øES¼Ä´æÆ÷£¬¼´ESÉèÖÃ
* Îª0x800£¬¼´ÉèÖÃÁËcopy²Ù×÷µÄÄ¿µÄµØÖ·¡£ºóÃæ½«BX¼Ä´æÆ÷ÉèÖÃ¸øDS¼Ä´æÆ÷£¬¼´ÉèÖÃÁË
* copy²Ù×÷µÄÔ´µØÖ·£»½Ó×Åµ÷ÓÃrep movsw°áÒÆ0x100£¨¼´256£©´Î£¬Ã¿´Î2×Ö½Ú£¬×Ü¹²512×Ö
* ½Ú¡£×¢Òâ£¬ÕâÀïµÄmovsw ²Ù×÷·½ÏòÊÇ£º(ES:DI)(DS:SI)£»ÇÒÕâÀïÊ¹ÓÃÁËcld£¬Òò´ËÃ¿´Î
* copyÖ®ºóSIºÍDI¶¼ÒªµİÔö2×Ö½Ú¡£ÔÙĞèÒª×¢ÒâµÄÊÇ£¬ÔÚÖ´ĞĞ¿½±´Ö®Ç°ºÍÖ®ºó£¬ÓĞpushaºÍ
* popa²Ù×÷£¬±£´æÁËECXºÍSI¼Ä´æÆ÷¡£×îºó£¬Ò»¾äjmp	*(stage2_address)Ìø×ªµ½ÁËStage2
* Ö´ĞĞ£¨stage2_addressµÄÖµÊÇ0x8000£©¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* Êµ¼Êmessage()º¯ÊıµÄÊµÏÖºÜ¼òµ¥£º´ÓSI¼Ä´æÆ÷ËùÖ¸µÄµØÖ·´¦¼ÓÔØÒ»¸ö×Ö½Úµ½AL¼Ä´æÆ÷£¬
* ±È½ÏÑéÖ¤¸Ã×Ö½Ú²»ÊÇ×Ö·û´®Ä©Î²µÄ'\0'×Ö½Ú£¬È»ºóµ÷ÓÃBIOSÖÕ¶Ëint 0x10ÊµÏÖÊä³ö¸Ã×Ö
* ½Ú£¬¸Ã²Ù×÷Ö±µ½Óöµ½×Ö·û´®Ä©Î²µÄ'\0'×Ö½Ú²Å½áÊø£¬·µ»Ø±»µ÷ÓÃµØÖ·´¦¡£
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
* @topic ±¾×¢ÊÍµÃµ½ÁË"ºË¸ß»ù"¿Æ¼¼ÖØ´ó×¨Ïî2012Äê¿ÎÌâ¡°¿ªÔ´²Ù×÷ÏµÍ³ÄÚºË·ÖÎöºÍ°²È«ĞÔÆÀ¹À
*£¨¿ÎÌâ±àºÅ£º2012ZX01039-004£©¡±µÄ×ÊÖú¡£
*
* @group ×¢ÊÍÌí¼Óµ¥Î»£ºÇå»ª´óÑ§¡ª¡ª03ÈÎÎñ£¨LinuxÄÚºËÏà¹ØÍ¨ÓÃ»ù´¡Èí¼ş°ü·ÖÎö£©³Ğµ£µ¥Î»
*
* @author ×¢ÊÍÌí¼ÓÈËÔ±£ºĞ»ÎÄÑ§
*
* @date ×¢ÊÍÌí¼ÓÈÕÆÚ£º2013Äê5ÔÂ3ÈÕ
*
* @details ×¢ÊÍÏêÏ¸ÄÚÈİ:
*
* GRUB 0.97 Stage1ÔËĞĞÁ÷³Ì
*
* µÚ15²½£¬floppy_probeÏà¹Ø´úÂë¡£Êµ¼ÊÉÏÔÚ¡¾grub-0.97/stage1/stage1.S¡¿µÄ×îºó²¿·Ö
* ´úÂë¶ÔÓ¦ÓÚÆô¶¯ÉÈÇøµÄ×îºó²¿·Ö¡£Õâ²¿·Ö´úÂëÖ÷ÒªÊÇÎªÁËºÍWindowsÆô¶¯ÉÈÇø¼æÈİ¡£ÌØ±ğ
* µÄ£¬ÔÚÎ»ÓÚÆ«ÒÆÁ¿0x1b8´¦£¨¼´STAGE1_WINDOWS_NT_MAGIC´¦£©£¬Windows NTÇ¶ÈëÁËÁ½¸ö
* Ææ¹ÖÊı×Ö¡£´ËÍâ£¬´Ó0x1be£¨¼´STAGE1_PARTSTART£©µ½0x1fe£¨¼´STAGE1_PARTEND£©Çø¼ä
* µÄ64×Ö½ÚÊÇ´«Í³µÄÓ²ÅÌ·ÖÇø±íÇø¶Î¡£ÔÚÕâÒ»¶Î£¬¶ÔÓÚÓ²ÅÌ¾Í»á±£´æÊµ¼ÊµÄ·ÖÇø±íĞÅÏ¢¡£
* Èç¹û´æ´¢Õâ¸öÆô¶¯ÉÈÇøµÄ´ÅÅÌÊÇÈíÅÌµÄ»°£¬ÔòÕâÒ»Çø¶Î¿ÉÒÔ²»ÓÃ£¨ÈíÅÌÎŞĞè·ÖÇø£©£¬Òò
* ´ËÕâÒ»¶Î¿ÉÒÔ±£´æ´úÂë¡£GRUB 0.97¼ÙÉèÕâÀïÊÇÓÃÈíÅÌ×öÆô¶¯£¬Òò´ËÕâÀï¾Í¼ÓÈëÁËÒ»¶Î
* Ì½²âÈíÇıµÄfloppy_probe´úÂë¡£´úÂëÏÈ½«probe_valuesµØÖ·£¨¸ÃµØÖ·Êµ¼Ê¾ÍÊÇ·ÖÇø±íµÄ
* ÆğÊ¼µØÖ·£©¼õÈ¥1×Ö½Ú£¬²¢½«Æä¼ÓÔØµ½SI¼Ä´æÆ÷ÖĞ¡£½Ó×Å½øÈëprobe_loop£¬µ÷ÓÃ"INT 13h 
* AH=0"À´ÖØÖÃÈíÇı£»½Ó×ÅÍ¨¹ıµİÔöSI¼Ä´æÆ÷²¢´ÓÆäËùÖ¸µÄµØÖ·¼ÓÔØÒ»¸ö×Ö½Úµ½CL¼Ä´æÆ÷
*£¨Ò²¼´ÒÀ´Î¶ÁÈ¡probe_valuesµÄÖµ£©£¬²¢±È½ÏÕâ¸öÖµÊÇ·ñÎª0£¨¼´µ½ÁËprobe_valuesµÄÄ©
* Î²£©£¬Èç¹ûµ½ÁËprobe_valuesµÄÄ©Î²¾Í´òÓ¡fd_probe_error_stringÏûÏ¢²¢ÍË³ö
* probe_loopµ½general_error´¦¡£·ñÔò¾Í»á³¢ÊÔµ÷ÓÃ"INT 13h AH=2"À´¶ÁÈ¡Ò»¸öÉÈÇøµ½
* STAGE1_BUFFERSEGÖ¸¶¨µÄ¶ÎµØÖ·´¦¡£Èç¹ûÊ§°Ü¾ÍÑ­»·probe_loopÖ±µ½Óöµ½probe_values
* µÄÄ©Î²¡£·ñÔò¾ÍËã¶ÁÈ¡³É¹¦£¬ĞŞÕı²ÎÊıºóÌø×ªµ½final_init´¦¼ÌĞø£¬´Ó¶ø»Øµ½Ç°ÃæÃèÊö
* µÄµÚ14²½£¬½ø¶øÖ´ĞĞcopy_buffer£¬×îÖÕ»¹ÊÇÌø×ªµ½Stage2µÄ×îÖÕµØÖ·´¦Ö´ĞĞ¡£
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
