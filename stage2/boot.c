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
/**
* @attention ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
* 
* ������ʵ�ֶ����ں�ӳ���ܡ�����kernelΪҪ��ȡ���ں��ļ���������argΪ�ں˲�����
* ����suggested_typeΪ�����߲²���ں����ͣ�����load_flagsΪ���ر�־��
*
* ���ȵ���grub_open (kernel)���ں��ļ�����ͨ��grub_read()�����ļ���ǰһ������
* (����32�ֽڣ�����8KB)���������ͷ��Ϣ�м���Ƿ���multiboot_header�ṹ�����
* ������multiboot��ӳ������typeΪKERNEL_TYPE_MULTIBOOT��
*
* �����multiboot, FreeBSD ���� NetBSD�ںˣ������֧��ELFģʽ���ء�������Щ��
* ��������ȫ�ֱ���entry_addr;���ҿ��Ը���suggested_type������type������
*
* �����multiboot_header��flags��־ָ����MULTIBOOT_AOUT_KLUDGE,�����ȷ����ʼ��ַ
* entry_addr = (entry_func) pu.mb->entry_addr;����pu.mbָ���ҵ���multiboot_header��
*
* ���ӳ��Ϊaout��ʽ�������entry_addr = (entry_func) pu.aout->a_entry;����pu.aout
* ָ���ļ�ͷ��buffer��
*
* ���ӳ�������ϼ��֣�����ӳ��ͷ���Ƿ����Linux�ں�ӳ���ʽ(boot_flag == 
* BOOTSEC_SIGNATURE)�������ȷ��Linux�ں�ӳ������Linux�汾��ͬ����ʼ�����ְ�
* ����Ҫ����Ϣ�����а���linux_data_real_addr��linux_data_tmp_addr��linux_mem_size
* ��ȫ�ֱ��������⣬�����ݰ汾��ͬ����Linux Header������һЩfixup������
*
* ��󣬸��ݷ���������ӳ�����ͣ�����aout��ʽ����ELF��ʽ����ȡ���е�ʣ��ӳ������
* ��cur_addrִ�еĵ�ַ��(cur_addr��֮ǰ�����ļ�ͷʱ��ʼ��)��������Щ��Ϣ����ʼ��
* multiboot��Ϣ��mbiȫ�ֽṹ�С�
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
* @attention ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
* 
* ������ʵ�ֶ���ģ�鹦�ܡ�����moduleָ��Ҫ�����ģ���ļ���������argָ��Ҫ�����
* ģ����Ҫ�Ĳ���������ģ�������cur_addr��ַ��(����cur_addr�ᱻ����)��֮�����ģ
* ����Ϣ��дmbi��mllȫ�ֱ�����
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
* @attention ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
* 
* ������ʵ�ֶ���initrd���ܡ�����initrdΪinitrd���ļ����������λ��Ϊcur_addr��
* ���Ҹ���Linux Header��ramdisk_image��ramdisk_size�ֶΡ����ط�0��ʾ�ɹ���
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
* @attention ��ע�͵õ���"�˸߻�"�Ƽ��ش�ר��2012����⡰��Դ����ϵͳ�ں˷����Ͱ�ȫ������
*�������ţ�2012ZX01039-004������������
*
* @copyright ע����ӵ�λ���廪��ѧ����03����Linux�ں����ͨ�û���������������е���λ
*
* @author ע�������Ա��л��ѧ
*
* @date ע��������ڣ�2013��5��3��
*
* @note ע����ϸ����:
* 
* ������ʵ��BSD�������ܡ�����typeΪ�ں����ͣ�����bootdevΪ�������̺ţ�����argΪ
* ��������������arg�����õ�clval��Ϊ������־�����ΪKERNEL_TYPE_FREEBSD,���ʼ��
* bootinfo����ͨ��(*entry_addr) (clval, bootdev, 0, 0, 0, ((int) (&bi)));������
* ����ͨ�� (*entry_addr) (clval, bootdev, 0, end_mark, extended_memory, mbi.mem_lower);
* ��������
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
