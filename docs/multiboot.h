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
* @attention ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��6��6��
*
* @note ע����ϸ����: Multiboot�淶�ľ�ȷ���� 

* ��Դ��http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
* 
* 3. Multiboot�淶�ľ�ȷ����
* 
* ��������/OSӳ��ӿ���Ҫ�����������棺
* 
* 1���������򿴵��� OS ӳ��ĸ�ʽ�� 
* 2��������������������ϵͳʱ������״̬�� 
* 3���������򴫵ݸ�����ϵͳ����Ϣ�ĸ�ʽ��
* 
* 3.1 OSӳ���ʽ
*
* һ��OSӳ�������һ����ͨ��ĳ�ֲ���ϵͳʹ�õı�׼��ʽ��32λ��ִ���ļ�����֮ͬ��������
* �ܱ����ӵ�һ����Ĭ�ϵ������ַ�Աܿ�PC��I/O������������ı������򣬵�Ȼ��Ҳ����ʹ��
* ���������������ɰ��Ķ�����
* 
* ����OSӳ����ʹ�õĸ�ʽ��Ҫ��ͷ֮�⣬OSӳ�񻹱���������һ��Multibootͷ��Multibootͷ
* ���������İ�����OSӳ���ǰ8192�ֽ��ڣ����ұ�����longword��32λ������ġ�ͨ����˵����
* ��λ��Խ��ǰԽ�ã����ҿ���Ƕ����text�ε���ʼ����λ�������Ŀ�ִ���ļ�ͷ֮ǰ��
* 
* 3.1.1 Multiboot��ͷ�ֲ�
*
* Multiboot ͷ�ķֲ��������±���ʾ��
* 
* ƫ���� ���� ���� ��ע
* 0  u32  magic  ����
* 4  u32  flags  ���� 
* 8  u32  checksum  ���� 
* 12  u32  header_addr  ���flags[16]����λ 
* 16  u32  load_addr  ���flags[16]����λ 
* 20  u32  load_end_addr  ���flags[16]����λ 
* 24  u32  bss_end_addr  ���flags[16]����λ 
* 28  u32  entry_addr  ���flags[16]����λ 
* 32  u32  mode_type  ���flags[2]����λ 
* 36  u32  width  ���flags[2]����λ 
* 40  u32  height  ���flags[2]����λ 
* 44  u32  depth  ���flags[2]����λ
* 
* magic��flags��checksum����ͷ��magic���ж��壬header_addr��load_addr��load_end_addr��
* bss_end_addr��entry_addr����ͷ�ĵ�ַ���ж��壬mode_type��width��height��depth����
* ��ͷ��ͼ�����ж��塣
* 
* 3.1.2 Multibootͷ�� magic ��
* 
* - magic 
*
* magic���Ǳ�־ͷ��ħ�������������ʮ������ֵ0x1BADB002��
* 
* - flags 
* 
* flags��ָ��OSӳ����Ҫ���������ṩ��֧�ֵ����ԡ�0-15λָ�������������������ĳЩ
* ֵ�����õ�����ĳ��ԭ�������ܲ���������Ӧ�������������֪�û�����������ʧ�ܡ�
* 16-31λָ����ѡ�����ԣ��������������֧��ĳЩλ�������Լ򵥵ĺ������ǲ�����������
* ��Ȼ������flags������δ�����λ���뱻��Ϊ0��������flags��ȿ������ڰ汾����Ҳ������
* �ڼ򵥵�����ѡ�� 
* 
* ���������flags���е�0λ�����е�����ģ�齫��ҳ��4KB���߽���롣��Щ����ϵͳ�ܹ�����
* ��ʱ����������ģ���ҳֱ��ӳ�䵽һ����ҳ�ĵ�ַ�ռ䣬�����Ҫ����ģ����ҳ����ġ�
* 
* ���������flags���е�1λ�������ͨ��Multiboot��Ϣ�ṹ���μ�������Ϣ��ʽ����mem_*��
* ���������ڴ����Ϣ��������������ܹ������ڴ�ֲ���mmap_*�򣩲�����ȷʵ���ڣ���Ҳ��
* ������
* 
* ���������flags���е�2λ���й���Ƶģʽ���μ�������Ϣ��ʽ������Ϣ������ں���Ч��
* 
* ���������flags���е�16λ����Multibootͷ��ƫ����8-24������Ч����������Ӧ��ʹ������
* ������ʵ�ʿ�ִ��ͷ�е��������㽫OSӳ�����뵽�������ں�ӳ��ΪELF��ʽ�򲻱��ṩ
* ��������Ϣ���������ӳ����a.out��ʽ��������ʲô��ʽ�Ļ��ͱ����ṩ��Щ��Ϣ�����ݵ�
* �������������ܹ�����ELF��ʽ��ӳ��Ҳ�����뽫�����ַ��ϢǶ��Multibootͷ�е�ӳ��
* ����Ҳ����ֱ��֧�������Ŀ�ִ�и�ʽ������һ��a.out��������壬���ⲻ�Ǳ���ġ�
* 
* - checksum 
* 
* ��checksum��һ��32λ���޷���ֵ������������magic��Ҳ����magic��flags�����ʱ����
* ��������32λ���޷���ֵ0����magic + flags + checksum = 0����
* 
* 3.1.3 Multibootͷ�ĵ�ַ��
* 
* ������flags�ĵ�16λ�����ĵ�ַ���������ַ�����ǵ��������£�
* 
* - header_addr 
* 
* ������Ӧ��Multibootͷ�Ŀ�ʼ���ĵ�ַ������Ҳ��magicֵ�������ַ�����������ͬ��OSӳ��
* ƫ�����������ڴ�֮���ӳ�䡣
* 
* - load_addr
* 
* ����text�ο�ʼ���������ַ����OSӳ���ļ��еĶ��ƫ�ƿ�ʼ������ͷλ�õ�ƫ�������壬
* �����header_addr - load_addr����load_addr����С�ڵ���header_addr��
* 
* - load_end_addr
* 
* ����data�ν������������ַ����load_end_addr-load_addr��ָ������������Ҫ����������ݡ�
* �ⰵʾ��text��data�α�����OSӳ�������������е�a.out��ִ�и�ʽ�������������������
* ��Ϊ0����������ٶ�text��data��ռ������ OS ӳ���ļ���
* 
* - bss_end_addr
* 
* ����bss�ν������������ַ������������������ʼ��Ϊ0������������������⽫����ģ��
* ���������ڲ�ϵͳ��ص����ݷŵ������������Ϊ0����������ٶ�û��bss�Ρ�
* 
* - entry_addr
* 
* ����ϵͳ����ڵ㣬�������������ת�����
* 
* 3.1.4 Multibootͷ��ͼ����
* 
* ���е�ͼ����ͨ��flags�ĵ�2λ����������ָ�����Ƽ���ͼ��ģʽ��ע�⣬��ֻ��OSӳ���Ƽ�
* ��ģʽ�������ģʽ���ڣ����������趨��������û�����ȷָ����һ��ģʽ�Ļ���������
* �����ܵĻ�����������ת��һ�����Ƶ�ģʽ��
* 
* ���ǵ��������£�
* 
* - mode_type 
* 
* ���Ϊ0�ʹ�������ͼ��ģʽ�����Ϊ1�����׼EGA�ı�ģʽ����������ֵ�����Ա�������չ��
* ע�⼴ʹ�����Ϊ0����������Ҳ��������һ���ı�ģʽ��
* 
* - width 
* 
* ������������ͼ��ģʽ�����������������ı�ģʽ�������ַ�����0����OSӳ��Դ�û��Ҫ��
* 
* - height 
* 
* ������������ͼ��ģʽ�����������������ı�ģʽ�������ַ�����0����OSӳ��Դ�û��Ҫ��
* 
* - depth 
* 
* ��ͼ��ģʽ�£�����ÿ�����ص�λ�������ı�ģʽ��Ϊ0��0����OSӳ��Դ�û��Ҫ��
* 
* 3.2 ����״̬
* 
* �������������32λ����ϵͳʱ������״̬�������£�
* 
* - EAX 
* 
* �������ħ��0x2BADB002�����ֵָ������ϵͳ�Ǳ�һ������Multiboot�淶��������������
* �ģ�������������һ����������Ҳ���������������ϵͳ����
* 
* - EBX 
* 
* ������������������ṩ��Multiboot��Ϣ�ṹ�������ַ���μ�������Ϣ��ʽ����
* 
* - CS 
* 
* ������һ��ƫ����λ��0��0xFFFFFFFF֮���32λ�ɶ�/��ִ�д���Ρ�����ľ�ȷֵδ���塣
* 
* - DS, ES, FS, GS, SS 
* 
* ������һ��ƫ����λ��0��0xFFFFFFFF֮���32λ�ɶ�/��ִ�д���Ρ�����ľ�ȷֵδ���塣
* 
* - A20 gate
* 
* �����Ѿ�������
* 
* - CR0 
* 
* ��31λ��PG������Ϊ0����0λ��PE������Ϊ1������λδ���塣
* 
* - EFLAGS 
* 
* ��17λ��VM������Ϊ0����9λ��IF������Ϊ1 ������λδ���塣
* 
* ���������Ĵ������Ĵ����ͱ�־λδ���塣�������
* 
* - ESP 
* 
* ����Ҫʹ�ö�ջʱ��OSӳ������Լ�����һ����
* 
* - GDTR 
* 
* ���ܶμĴ������������������ˣ�GDTRҲ��������Ч�ģ�����OSӳ������������κζμĴ���
* ����ʹ��������ͬ��ֵҲ���У���ֱ�����趨���Լ���GDT��
* 
* - IDTR 
* 
* OSӳ�����������������IDT֮����ܿ��жϡ�
* 
* ������ˣ������Ļ���״̬Ӧ�ñ������������������Ĺ���˳��Ҳ����ͬBIOS������DOS��
* ������������Ǵ����������Ļ�����ʼ����״̬һ�������仰˵������ϵͳӦ���ܹ��������
* ����BIOS���ã�ֱ�����Լ���дBIOS���ݽṹ֮ǰ�����У�����������뽫PIC�趨Ϊ������
* BIOS/DOS ״̬�����������п����ڽ���32λģʽʱ�ı����ǡ�
* 
* 3.3 ������Ϣ��ʽ
* 
* �ڽ������ϵͳʱ��EBX�Ĵ�������Multiboot��Ϣ���ݽṹ�������ַ����������ͨ��������
* Ҫ��������Ϣ���ݸ�����ϵͳ������ϵͳ���԰��Լ�����Ҫʹ�û��ߺ����κβ��֣����е���
* �����򴫵ݵ���Ϣֻ�ǽ����Եġ�
* 
* Multiboot��Ϣ�ṹ��������ص��ӽṹ������������������κ�λ�ã���Ȼ�����˱������ں�
* ������ģ������򣩡����������֮ǰ�������ǲ���ϵͳ�����Ρ�
* 
* Multiboot��Ϣ�ṹ����ĿǰΪֹ����ģ��ĸ�ʽ���£�
* 
*              +---------------------------+
*      0       | flags                     |    (����)
*              +---------------------------+
*      4       | mem_lower                 |    (���flags[0]����λ�����)
*      8       | mem_upper                 |    (���flags[0]����λ�����)
*              +---------------------------+
*      12      | boot_device               |    (���flags[1]����λ�����)
*              +---------------------------+
*      16      | cmdline                   |    (���flags[2]����λ�����)
*              +---------------------------+
*      20      | mods_count                |    (���flags[3]����λ�����)
*      24      | mods_addr                 |    (���flags[3]����λ�����)
*              +---------------------------+
* 28 - 40      | syms                      |    (���flags[4]��flags[5]����λ�����)
*              |                           |
*              +---------------------------+
*      44      | mmap_length               |    (���flags[6]����λ�����)
*      48      | mmap_addr                 |    (���flags[6]����λ�����)
*              +---------------------------+
*      52      | drives_length             |    (���flags[7]����λ�����)
*      56      | drives_addr               |    (���flags[7]����λ�����)
*              +---------------------------+
*      60      | config_table              |    (���flags[8]����λ�����)
*              +---------------------------+
*      64      | boot_loader_name          |    (���flags[9]����λ�����)
*              +---------------------------+
*      68      | apm_table                 |    (���flags[10]����λ�����)
*              +---------------------------+
*      72      | vbe_control_info          |    (���flags[11]����λ�����)
*      76      | vbe_mode_info             |
*      80      | vbe_mode                  |
*      82      | vbe_interface_seg         |
*      84      | vbe_interface_off         |
*      86      | vbe_interface_len         |
*              +---------------------------+
* 
* ��һ��longwordָ��Multiboot��Ϣ�ṹ�е��������Ƿ���Ч������Ŀǰδ�����λ����
* ������������Ϊ0������ϵͳӦ�ú����κ���������λ����ˣ�flags��Ҳ��������һ��
* �汾��־���������������ƻ�����չMultiboot��Ϣ�ṹ��
* 
* ���������flags�еĵ�0λ����mem_*����Ч��mem_lower��mem_upper�ֱ�ָ���˵Ͷ˺�
* �߶��ڴ�Ĵ�С����λ��K���Ͷ��ڴ���׵�ַ��0���߶��ڴ���׵�ַ��1M���Ͷ��ڴ��
* ������ֵ��640K�����صĸ߶��ڴ��������ֵ�����ֵ��ȥ1M����������֤�����ֵ��
* 
* ���������flags�еĵ�1λ����boot_device����Ч����ָ������������ĸ�BIOS�����豸
* �����OSӳ�����OSӳ���Ǵ�һ��BIOS��������ģ������;����ܳ��֣���3λ����
* ��0��������ϵͳ����ʹ�������������ȷ������root�豸��������һ��Ҫ��������
* boot_device�����ĸ����ֽڵ�������ɣ�
* 
*      +-------+-------+-------+-------+
*      | drive | part1 | part2 | part3 |
*      +-------+-------+-------+-------+
* 
* ��һ���ֽڰ�����BIOS�������ţ����ĸ�ʽ��BIOS��INT0x13�ͼ����̽ӿ���ͬ�����磬
* 0x00�����һ��������������0x80�����һ��Ӳ����������
* 
* ʣ�µ������ֽ�ָ��������������part1ָ�����������ţ�part2ָ��һ�����������е�һ��
* �ӷ������ȵȡ����������Ǵ�0��ʼ����ʹ�õķ����ֽڱ��뱻��Ϊ0xFF�����磬�������
* ���򵥵ķ�Ϊ��һ��һ��DOS��������part1�������DOS�����ţ�part2��part3����0xFF��
* ��һ�������ǣ����һ�������ȱ���ΪDOS�������������е�һ��DOS�����ֱ���Ϊ����ʹ
* ��BSD���̱�ǩ���Ե�BSD��������part1����DOS�����ţ�part2����DOS�����ڵ�BSD�ӷ�����
* part3��0xFF��
* 
* DOS��չ�����ķ����Ŵ�4��ʼ����������Ƕ���ӷ���һ����������չ�����ĵײ�ֲ�����
* �ֲ�Ƕ�׵ġ����磬�����������Ӵ�ͳ��DOS�����̵ĵڶ���������������part1��5��
* part2��part3����0xFF��
* 
* ���������flags longword�ĵ�2λ����cmdline����Ч��������Ҫ���͸��ں˵������в�
* ���������ַ�������в�����һ������C������0��ֹ���ַ�����
* 
* ���������flags�ĵ�3λ����mods��ָ����ͬ�ں�һͬ���������Щ����ģ�飬�Լ�����
* �����ҵ����ǡ�mods_count�����������ģ��ĸ�����mods_addr�����˵�һ��ģ��ṹ
* �������ַ��mods_count������0�����ʾû�������κ�ģ�飬��ʹ������flags�ĵ�1λ
* ʱҲ�п�����������ÿ��ģ��ṹ�ĸ�ʽ���£�
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
* ǰ���������������ģ��Ŀ�ʼ�ͽ�����ַ��string���ṩ��һ���Զ����������ģ����
* �ص��ַ�����������0��ֹ��ASCII�ַ�����ͬ�ں������в���һ�������û��ʲô��ģ��
* �йص��ַ�����string�������0����������£�����ַ���Ҳ���������в��������磬
* �������ϵͳ������ģ��������ִ�г���Ļ���������һ��·���������磬�������ϵͳ
* ������ģ�������ļ�ϵͳ�е��ļ��Ļ�������������ȡ���ڲ���ϵͳ��reserved�������
* ����������Ϊ0��������ϵͳ���ԡ�
* 
* ע�⣺��4λ�͵�5λ�ǻ���ġ�
* 
* ���������flags�ĵ�4λ���������Multiboot��Ϣ�ṹ�ĵ�28λ��ʼ��������Ч�ģ�
* 
*            +------------------+
*      28    | tabsize          |
*      32    | strsize          |
*      36    | addr             |
*      40    | reserved (0)     |
*            +------------------+
* 
* ��ָ������������ҵ�a.out��ʽ�ں�ӳ��ķ��ű�addr��a.out��ʽ��nlist�ṹ�����
* ��С��4�ֽ��޷��ų��������������ַ�������������鱾��Ȼ����һϵ����0��ֹ��ASCII
* �ַ����Ĵ�С��4�ֽ��޷��ų�����������sizeof(unsigned long)����Ȼ�����ַ�������
* tabsize���ڷ��ű�Ĵ�С������λ�ڷ���section��ͷ������strsize���ڷ��ű�ָ�����
* ������Ĵ�С������λ��string section��ͷ������ע��tabsize������0������ζ��û��
* ���ţ������Ѿ�������flags�ĵ�4λ��
* 
* ���������flags�ĵ�5λ���������Multiboot��Ϣ�ṹ�ĵ�28λ��ʼ��������Ч�ģ�
* 
*            +----------------+
*      28    | num            |
*      32    | size           |
*      36    | addr           |
*      40    | shndx          |
*            +----------------+
* 
* ��ָ������������ҵ� ELF ��ʽ�ں�ӳ���sectionͷ��ÿ��Ĵ�С��һ���м����Լ�
* ��Ϊ�����������ַ��������Ƕ�Ӧ�ڿ�ִ�п����Ӹ�ʽ��ELF����programͷ�е�
* shdr_* �shdr_num�ȣ������е�section���ᱻ���룬ELF sectionͷ�������ַ��ָ
* �����е�section���ڴ��е�λ�ã��μ�i386 ELF�ĵ��Եõ���ζ�ȡsectionͷ�ĸ���
* ��ϸ�ڣ���ע�⣬shdr_num������0����־��û�з��ţ������Ѿ�������flags�ĵ�5λ��
* 
* ���������flags�ĵ� 6 λ����mmap_*������Ч�ģ�ָ��������BIOS�ṩ���ڴ�ֲ���
* �������ĵ�ַ�ͳ��ȡ�mmap_addr�ǻ������ĵ�ַ��mmap_length�ǻ��������ܴ�С��
* ��������һ�����߶������Ĵ�С/�ṹ�ԣ�sizeʵ����������������һ���Եģ���
* �ɵģ�
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
* size����ؽṹ�Ĵ�С����λ���ֽڣ������ܴ�����Сֵ20��base_addr_low��������ַ
* �ĵ�32λ��base_addr_high�Ǹ�32λ��������ַ�ܹ���64λ��length_low���ڴ������
* С�ĵ�32λ��length_high���ڴ������С�ĸ�32λ���ܹ���64λ��type����Ӧ��ַ����
* �����ͣ�1�������RAM������������ֵ����������
* 
* ���Ա�֤���ṩ���ڴ�ֲ��г������пɹ�����ʹ�õı�׼�ڴ档
* 
* ���������flags�ĵ�7λ����drives_*������Ч�ģ�ָ����һ���������ṹ�������ַ
* ������ṹ�Ĵ�С��drives_addr�ǵ�ַ��drives_length���������ṹ���ܴ�С��ע�⣬
* drives_length������0��ÿ���������ṹ�ĸ�ʽ���£�
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
* size��ָ���˽ṹ�Ĵ�С�����ݶ˿ڵ�������ͬ�������С���ܱ仯��ע�⣬�����С
* ���ܲ����ڣ�10 + 2 * �˿��������������ڶ����ԭ��
* 
* drive_number����� BIOS �������š�drive_mode���������������ʹ�õķ���ģʽ��
* Ŀǰ��ģʽ�������£�
* 
* 0 - CHS ģʽ����ͳ�ġ�����/��ͷ/������Ѱַģʽ���� 
* 
* 1 - LBA ģʽ���߼���Ѱַģʽ���� 
* 
* ��������drive_cylinders��drive_heads��drive_sectors��ָ����BIOS��⵽������
* ���Ĳ�����drive_cylinders������������drive_heads������ͷ����drive_sectors����
* ÿ�ŵ�����������
* 
* drive_ports�������BIOS����ʹ�õ�I/O�˿ڵ����顣����������0�����߶���޷���
* ���ֽ�������������0��ֹ��ע�⣬�����п��ܰ����κ�ʵ����������������ص�I/O��
* �ڣ�����DMA�������Ķ˿ڣ���
* 
* ���������flags�ĵ�8λ����config_table����Ч��ָ����GET CONFIGURATION BIOS����
* ���ص�ROM���ñ�������ַ��������BIOS����ʧ���ˣ��������Ĵ�С������0��
* 
* ���������flags�ĵ�9λ����boot_loader_name����Ч���������������������������ڴ�
* �еĵ�ַ����������������������C������0��ֹ���ַ�����
* 
* ���������flags�ĵ�10λ����apm_table����Ч������������APM��������ַ��
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
* ��version��cseg��offset��cseg_16��dseg��flags��cseg_len��cseg_16_len��dseg_len
* �ֱ�ָ���˰汾�š�����ģʽ32λ����Ρ���ڵ��ƫ����������ģʽ16λ����Ρ�����
* ģʽ16λ���ݶΡ���־λ������ģʽ32λ����εĳ��ȡ�����ģʽ16λ����εĳ��Ⱥ�
* ����ģʽ16λ���ݶεĳ��ȡ�ֻ��offset����4�ֽڣ����������2�ֽڡ��μ��߼���
* Դ����APM��BIOS�ӿڹ淶��
* 
* ���������flags�ĵ�11λ����graphics table��Ч��ǰ�����ں��Ѿ���Multibootͷ��
* ָ����һ��ͼ��ģʽ�� ��vbe_control_info��vbe_mode_info�ֱ������VBE����00h
* ���ص�VBE������Ϣ�������ַ����VBE����01h���ص�VBEģʽ��Ϣ��
* 
* ��vbe_modeָ���˵�ǰ����ʾģʽ�����е���Ϣ����VBE 3.0��׼��
* 
* �������vbe_interface_seg��vbe_interface_off��vbe_interface_len������VBE 2.0+
* �ж���ı���ģʽ�ӿڡ����û����Щ��Ϣ����Щ����0 ��ע��VBE 3.0��������һ��
* ����ģʽ�ӿڣ�������ǰ�İ汾�Ǽ��ݵġ��������Ҫʹ����Щ�µı���ģʽ�ӿڣ���
* �����Լ��ҵ������
* 
* graphics table�е����ǰ���VBE��Ƶģ�����Multiboot������������ڷ�VBEģʽ��ģ
* ��VBEģʽ��
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
