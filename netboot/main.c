/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000,2001,2002  Free Software Foundation, Inc.
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

/* Based on "src/main.c" in etherboot-5.0.5.  */

/**************************************************************************
ETHERBOOT -  BOOTP/TFTP Bootstrap Program

Author: Martin Renters
  Date: Dec/93
  
Literature dealing with the network protocols:
       ARP - RFC826
       RARP - RFC903
       UDP - RFC768
       BOOTP - RFC951, RFC2132 (vendor extensions)
       DHCP - RFC2131, RFC2132 (options)
       TFTP - RFC1350, RFC2347 (options), RFC2348 (blocksize), RFC2349 (tsize)
       RPC - RFC1831, RFC1832 (XDR), RFC1833 (rpcbind/portmapper)
       NFS - RFC1094, RFC1813 (v3, useful for clarifications, not implemented)

**************************************************************************/

#define GRUB	1
#include <etherboot.h>
#include <nic.h>

/* #define DEBUG	1 */

struct arptable_t arptable[MAX_ARP];

/* Set if the user pushes Control-C.  */
int ip_abort = 0;
/* Set if an ethernet card is probed and IP addresses are set.  */
int network_ready = 0;

struct rom_info rom;

static int vendorext_isvalid;
static unsigned long netmask;
static struct bootpd_t bootp_data;
static unsigned long xid;
static unsigned char *end_of_rfc1533 = NULL;

#ifndef	NO_DHCP_SUPPORT
#endif /* NO_DHCP_SUPPORT */

/* 銭th */
static unsigned char vendorext_magic[] = {0xE4, 0x45, 0x74, 0x68};
static const unsigned char broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#ifdef	NO_DHCP_SUPPORT

static unsigned char rfc1533_cookie[5] = {RFC1533_COOKIE, RFC1533_END};

#else /* ! NO_DHCP_SUPPORT */

static int dhcp_reply;
static in_addr dhcp_server = {0L};
static in_addr dhcp_addr = {0L};
static unsigned char rfc1533_cookie[] = {RFC1533_COOKIE};
static unsigned char rfc1533_end[] = {RFC1533_END};

static const unsigned char dhcpdiscover[] =
{
  RFC2132_MSG_TYPE, 1, DHCPDISCOVER,	
  RFC2132_MAX_SIZE,2,	/* request as much as we can */
  ETH_MAX_MTU / 256, ETH_MAX_MTU % 256,
  RFC2132_PARAM_LIST, 4, RFC1533_NETMASK, RFC1533_GATEWAY,
  RFC1533_HOSTNAME, RFC1533_EXTENSIONPATH
};

static const unsigned char dhcprequest[] =
{
  RFC2132_MSG_TYPE, 1, DHCPREQUEST,
  RFC2132_SRV_ID, 4, 0, 0, 0, 0,
  RFC2132_REQ_ADDR, 4, 0, 0, 0, 0,
  RFC2132_MAX_SIZE, 2,	/* request as much as we can */
  ETH_MAX_MTU / 256, ETH_MAX_MTU % 256,
  /* request parameters */
  RFC2132_PARAM_LIST,
  /* 4 standard + 2 vendortags */
  4 + 2,
  /* Standard parameters */
  RFC1533_NETMASK, RFC1533_GATEWAY,
  RFC1533_HOSTNAME, RFC1533_EXTENSIONPATH,
  /* Etherboot vendortags */
  RFC1533_VENDOR_MAGIC,
  RFC1533_VENDOR_CONFIGFILE,
};

#endif /* ! NO_DHCP_SUPPORT */

static unsigned short ipchksum (unsigned short *ip, int len);
static unsigned short udpchksum (struct iphdr *packet);
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现打印网络配置的功能。首先是调用eth_probe()探测网卡，接着主要是输出网络
* IP地址，网络地址掩码，服务器地址，网关地址等信息。
*/
void
print_network_configuration (void)
{
  if (! eth_probe ())
    grub_printf ("No ethernet card found.\n");
  else if (! network_ready)
    grub_printf ("Not initialized yet.\n");
  else
    {
      etherboot_printf ("Address: %@\n", arptable[ARP_CLIENT].ipaddr.s_addr);
      etherboot_printf ("Netmask: %@\n", netmask);
      etherboot_printf ("Server: %@\n", arptable[ARP_SERVER].ipaddr.s_addr);
      etherboot_printf ("Gateway: %@\n", arptable[ARP_GATEWAY].ipaddr.s_addr);
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
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现返回网络地址掩码的功能。
*/
/**************************************************************************
DEFAULT_NETMASK - Return default netmask for IP address
**************************************************************************/
static inline unsigned long 
default_netmask (void)
{
  int net = ntohl (arptable[ARP_CLIENT].ipaddr.s_addr) >> 24;
  if (net <= 127)
    return (htonl (0xff000000));
  else if (net < 192)
    return (htonl (0xffff0000));
  else
    return (htonl (0xffffff00));
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现返回配置网络接口的功能。参数ip为IP地址；参数sm为网络地址掩码；
* 参数gw为网关地址；参数svr为服务器地址。执行如下步骤:
* 
* 1)如果sm非空，则调用inet_aton将sm转换为in_addr类型，并设置netmask全局变量。
* 2)如果ip非空，则调用inet_aton将ip转换为in_addr类型，存在于全局变量的
*   arptable[ARP_CLIENT].ipaddr中，且如果此时netmask全局变量还是没有设置，则调用
*   default_netmask设置netmask全局变量为默认掩码。
* 3)如果gw非空，则调用inet_aton将gw转换为in_addr类型，存在于全局变量的
*   arptable[ARP_GATEWAY].ipaddr中，并清空arptable[ARP_GATEWAY].node。
* 4)如果svr非空，则调用inet_aton将svr转换为in_addr类型，存在于全局变量的
*   arptable[ARP_SERVER].ipaddr中，并清空arptable[ARP_SERVER].node。
* 5)根据netmask | arptable[ARP_CLIENT].ipaddr.s_addr来设置network_ready全局变量。
*/
/* ifconfig - configure network interface.  */
int
ifconfig (char *ip, char *sm, char *gw, char *svr)
{
  in_addr tmp;
  
  if (sm) 
    {
      if (! inet_aton (sm, &tmp))
	return 0;
      
      netmask = tmp.s_addr;
    }
  
  if (ip) 
    {
      if (! inet_aton (ip, &arptable[ARP_CLIENT].ipaddr)) 
	return 0;
      
      if (! netmask && ! sm) 
	netmask = default_netmask ();
    }
  
  if (gw && ! inet_aton (gw, &arptable[ARP_GATEWAY].ipaddr)) 
    return 0;

  /* Clear out the ARP entry.  */
  grub_memset (arptable[ARP_GATEWAY].node, 0, ETH_ALEN);
  
  if (svr && ! inet_aton (svr, &arptable[ARP_SERVER].ipaddr)) 
    return 0;

  /* Likewise.  */
  grub_memset (arptable[ARP_SERVER].node, 0, ETH_ALEN);
  
  if (ip || sm)
    {
      if (IP_BROADCAST == (netmask | arptable[ARP_CLIENT].ipaddr.s_addr)
	  || netmask == (netmask | arptable[ARP_CLIENT].ipaddr.s_addr)
	  || ! netmask)
	network_ready = 0;
      else
	network_ready = 1;
    }
  
  return 1;
}

/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现发送一个UDP数据报的功能。参数destip为目标IP地址；参数srcsock为源端
* socket号；参数destsock为目的端socket号；参数len为数据报长度；参数buf为数据报
* 数据缓冲(前面部分为IP头和UDP头，后面才是实际数据)。具体完成了下列步骤:
*
* 1) 将buf强制转换为iphdr结构，然后填充iphdr结构，构造IP头信息。
* 2) 将buf+sizeof (struct iphdr)强制转换为udphdr,然后填充，构造UDP头信息。
* 3) 如果目的地址为广播地址，那么直接调用eth_transmit发送广播报文。
* 4) 否则，如果目的地址不在当前子网，那么就将目的地址设置为网关地址；
* 5) 如果还没有做过ARP请求，那么先进行ARP请求(构造arprequest结构并使用eth_transmit
*    做广播)。
* 6) 最后再调用eth_transmit进行实际的数据传输。
*/
/**************************************************************************
UDP_TRANSMIT - Send a UDP datagram
**************************************************************************/
int 
udp_transmit (unsigned long destip, unsigned int srcsock,
	      unsigned int destsock, int len, const void *buf)
{
  struct iphdr *ip;
  struct udphdr *udp;
  struct arprequest arpreq;
  int arpentry, i;
  int retry;

  ip = (struct iphdr *) buf;
  udp = (struct udphdr *) ((unsigned long) buf + sizeof (struct iphdr));
  ip->verhdrlen = 0x45;
  ip->service = 0;
  ip->len = htons (len);
  ip->ident = 0;
  ip->frags = 0;
  ip->ttl = 60;
  ip->protocol = IP_UDP;
  ip->chksum = 0;
  ip->src.s_addr = arptable[ARP_CLIENT].ipaddr.s_addr;
  ip->dest.s_addr = destip;
  ip->chksum = ipchksum ((unsigned short *) buf, sizeof (struct iphdr));
  udp->src = htons (srcsock);
  udp->dest = htons (destsock);
  udp->len = htons (len - sizeof (struct iphdr));
  udp->chksum = 0;
  udp->chksum = htons (udpchksum (ip));

  if (udp->chksum == 0)
    udp->chksum = 0xffff;
  
  if (destip == IP_BROADCAST)
    {
      eth_transmit (broadcast, IP, len, buf);
    }
  else
    {
      if (((destip & netmask)
	   != (arptable[ARP_CLIENT].ipaddr.s_addr & netmask))
	  && arptable[ARP_GATEWAY].ipaddr.s_addr)
	destip = arptable[ARP_GATEWAY].ipaddr.s_addr;
      
      for (arpentry = 0; arpentry < MAX_ARP; arpentry++)
	if (arptable[arpentry].ipaddr.s_addr == destip)
	  break;
      
      if (arpentry == MAX_ARP)
	{
	  etherboot_printf ("%@ is not in my arp table!\n", destip);
	  return 0;
	}
      
      for (i = 0; i < ETH_ALEN; i++)
	if (arptable[arpentry].node[i])
	  break;
      
      if (i == ETH_ALEN)
	{
	  /* Need to do arp request.  */
#ifdef DEBUG
	  grub_printf ("arp request.\n");
#endif
	  arpreq.hwtype = htons (1);
	  arpreq.protocol = htons (IP);
	  arpreq.hwlen = ETH_ALEN;
	  arpreq.protolen = 4;
	  arpreq.opcode = htons (ARP_REQUEST);
	  grub_memmove (arpreq.shwaddr, arptable[ARP_CLIENT].node,
			ETH_ALEN);
	  grub_memmove (arpreq.sipaddr, (char *) &arptable[ARP_CLIENT].ipaddr,
			sizeof (in_addr));
	  grub_memset (arpreq.thwaddr, 0, ETH_ALEN);
	  grub_memmove (arpreq.tipaddr, (char *) &destip, sizeof (in_addr));
	  
	  for (retry = 1; retry <= MAX_ARP_RETRIES; retry++)
	    {
	      long timeout;
	      
	      eth_transmit (broadcast, ARP, sizeof (arpreq), &arpreq);
	      timeout = rfc2131_sleep_interval (TIMEOUT, retry);
	      
	      if (await_reply (AWAIT_ARP, arpentry, arpreq.tipaddr, timeout))
		goto xmit;

	      if (ip_abort)
		return 0;
	    }
	  
	  return 0;
	}
      
    xmit:
      eth_transmit (arptable[arpentry].node, IP, len, buf);
    }
  
  return 1;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现TFTP下载BOOTP数据或者内核映像的功能。
*
* TFTP的基本介绍如下(http://en.wikipedia.org/wiki/Trivial_File_Transfer_Protocol):
*
* 小型文件传输协议部分基于更早期的文件传输协议，文件传输协议是通用分组协议
*（PUP Protocol）中的一部分。
*
* 小型文件传输协议的一些详细资料：
*
* - 使用UDP（端口69）作为其传输协议（不像FTP使用TCP端口21）。
* - 不能列出目录内容。
* - 无验证或加密机制。
* - 被用于在远程服务器上读取或写入文件。
* - 支持三种不同的传输模式："netascii","octet"和"mail"，前两种符合FTP协议中的
*   "ASCII"和"image（binary）"模式；第三种从来很少使用，目前已经废弃。
*
* 因为小型文件传输协议使用UDP协定，就必须自己支援传输和会话的控制。每个通过TFTP
* 传输的文件构成了一个独立的交换。此传输表现为步锁，任何时间网络上仅仅传递一个包
*（一个数据块或一个首部确认）。由于缺少窗口切换技术，TFTP在有很多潜在连接的情况
* 下仅提供较低的吞吐量。
*
* 由于小型文件传输协议缺少安全性，在开放式因特网上传输非常危险，所以普遍仅仅用
* 于私人本地网络。
* 
* 小型文件传输协议会话的详细资料:
*
* 1) 初始化主机A送一个读请求（RRQ）或写请求（WRQ）包给主机B，包含了文件名和传输模式。
* 2) B向A发一个ACK包应答，同时也通知了A其余送往B包应该发送的端口号。
* 3) 源主机向目的主机送编过号的数据包，除了最后一个都应该包含一个全尺寸的数据块。
*    目的主机用编号的ACK包应答所有的数据包。
* 4) 最终的数据包必须包含少于最大尺寸的数据块以表明这是最后一个包。如果被传输文
*    件正好是尺寸块的整数倍，源主机最后送的数据包就是0字节的。
*
* 按照上面的说明，我们可以分析该函数实现的细节如下:
*
* 1) 首先构造一个tftpreq_t结构，里面包含请求类型TFTP_RRQ，即为读请求（RRQ），以及
*    使用grub_sprintf生成的"%s%coctet%cblksize%c%d"格式的rrq内容，包括指定的name
*    和传输模式octet，以及传输的blksize为TFTP_MAX_PACKET。
* 2) 调用udp_transmit给TFTP_PORT，即69端口发送请求给ARP_SERVER。
* 3) 进入循环等待从服务器的ACK包应答(tftp_t类型的UDP报文)。
* 4) 如果出错，即opcode为TFTP_ERROR，则跳出循环。
* 5) 如果正常opcode为TFTP_OACK，则解析出服务器端决定的后面要发送的packetsize。
* 6) 如果正常opcode为TFTP_DATA，则接收数据，并按照前一步得出的packetsize处理?*    这就要调用参数fnc指定的callback来处理。
* 7) 构造一个TFTP_ACK,并调用udp_transmit传回给服务器端。
* 8) 循环至3)。
*/
/**************************************************************************
TFTP - Download extended BOOTP data, or kernel image
**************************************************************************/
static int
tftp (const char *name, int (*fnc) (unsigned char *, int, int, int))
{
  int retry = 0;
  static unsigned short iport = 2000;
  unsigned short oport = 0;
  unsigned short len, block = 0, prevblock = 0;
  int bcounter = 0;
  struct tftp_t *tr;
  struct tftpreq_t tp;
  int rc;
  int packetsize = TFTP_DEFAULTSIZE_PACKET;
  
  /* Clear out the Rx queue first.  It contains nothing of interest,
   * except possibly ARP requests from the DHCP/TFTP server.  We use
   * polling throughout Etherboot, so some time may have passed since we
   * last polled the receive queue, which may now be filled with
   * broadcast packets.  This will cause the reply to the packets we are
   * about to send to be lost immediately.  Not very clever.  */
  await_reply (AWAIT_QDRAIN, 0, NULL, 0);
  
  tp.opcode = htons (TFTP_RRQ);
  len = (grub_sprintf ((char *) tp.u.rrq, "%s%coctet%cblksize%c%d",
		       name, 0, 0, 0, TFTP_MAX_PACKET)
	 + sizeof (tp.ip) + sizeof (tp.udp) + sizeof (tp.opcode) + 1);
  if (! udp_transmit (arptable[ARP_SERVER].ipaddr.s_addr, ++iport,
		      TFTP_PORT, len, &tp))
    return 0;
  
  for (;;)
    {
      long timeout;
      
#ifdef CONGESTED
      timeout = rfc2131_sleep_interval (block ? TFTP_REXMT : TIMEOUT, retry);
#else
      timeout = rfc2131_sleep_interval (TIMEOUT, retry);
#endif

      if (! await_reply (AWAIT_TFTP, iport, NULL, timeout))
	{
	  if (! block && retry++ < MAX_TFTP_RETRIES)
	    {
	      /* Maybe initial request was lost.  */
	      if (! udp_transmit (arptable[ARP_SERVER].ipaddr.s_addr,
				  ++iport, TFTP_PORT, len, &tp))
		return 0;
	      
	      continue;
	    }
	  
#ifdef CONGESTED
	  if (block && ((retry += TFTP_REXMT) < TFTP_TIMEOUT))
	    {
	      /* We resend our last ack.  */
#ifdef MDEBUG
	      grub_printf ("<REXMT>\n");
#endif
	      udp_transmit (arptable[ARP_SERVER].ipaddr.s_addr,
			    iport, oport,
			    TFTP_MIN_PACKET, &tp);
	      continue;
	    }
#endif
	  /* Timeout.  */
	  break;
	}
      
      tr = (struct tftp_t *) &nic.packet[ETH_HLEN];
      if (tr->opcode == ntohs (TFTP_ERROR))
	{
	  grub_printf ("TFTP error %d (%s)\n",
		       ntohs (tr->u.err.errcode),
		       tr->u.err.errmsg);
	  break;
	}
      
      if (tr->opcode == ntohs (TFTP_OACK))
	{
	  char *p = tr->u.oack.data, *e;
	  
	  /* Shouldn't happen.  */
	  if (prevblock)
	    /* Ignore it.  */
	    continue;
	  
	  len = ntohs (tr->udp.len) - sizeof (struct udphdr) - 2;
	  if (len > TFTP_MAX_PACKET)
	    goto noak;
	  
	  e = p + len;
	  while (*p != '\000' && p < e)
	    {
	      if (! grub_strcmp ("blksize", p))
		{
		  p += 8;
		  if ((packetsize = getdec (&p)) < TFTP_DEFAULTSIZE_PACKET)
		    goto noak;
		  
		  while (p < e && *p)
		    p++;
		  
		  if (p < e)
		    p++;
		}
	      else
		{
		noak:
		  tp.opcode = htons (TFTP_ERROR);
		  tp.u.err.errcode = 8;
		  len = (grub_sprintf ((char *) tp.u.err.errmsg,
				       "RFC1782 error")
			 + sizeof (tp.ip) + sizeof (tp.udp)
			 + sizeof (tp.opcode) + sizeof (tp.u.err.errcode)
			 + 1);
		  udp_transmit (arptable[ARP_SERVER].ipaddr.s_addr,
				iport, ntohs (tr->udp.src),
				len, &tp);
		  return 0;
		}
	    }
	  
	  if (p > e)
	    goto noak;
	  
	  /* This ensures that the packet does not get processed as data!  */
	  block = tp.u.ack.block = 0; 
	}
      else if (tr->opcode == ntohs (TFTP_DATA))
	{
	  len = ntohs (tr->udp.len) - sizeof (struct udphdr) - 4;
	  /* Shouldn't happen.  */
	  if (len > packetsize)
	    /* Ignore it.  */
	    continue;
	  
	  block = ntohs (tp.u.ack.block = tr->u.data.block);
	}
      else
	/* Neither TFTP_OACK nor TFTP_DATA.  */
	break;
      
      if ((block || bcounter) && (block != prevblock + 1))
	/* Block order should be continuous */
	tp.u.ack.block = htons (block = prevblock);
      
      /* Should be continuous.  */
      tp.opcode = htons (TFTP_ACK);
      oport = ntohs (tr->udp.src);
      /* Ack.  */
      udp_transmit (arptable[ARP_SERVER].ipaddr.s_addr, iport,
		    oport, TFTP_MIN_PACKET, &tp);
      
      if ((unsigned short) (block - prevblock) != 1)
	/* Retransmission or OACK, don't process via callback
	 * and don't change the value of prevblock.  */
	continue;
      
      prevblock = block;
      /* Is it the right place to zero the timer?  */
      retry = 0;
      
      if ((rc = fnc (tr->u.data.download,
		     ++bcounter, len, len < packetsize)) >= 0)
	return rc;

      /* End of data.  */
      if (len < packetsize)           
	return 1;
    }
  
  return 0;
}

/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现使用RARP获取本机IP地址和负载信息的功能。
*
* 关于RARP协议(反向地址转换协议)的介绍如下(http://baike.baidu.cn/view/1478464.htm):
*
* 反向地址转换协议就是将局域网中某个主机的物理地址转换为IP地址，比如局域网中有
* 一台主机只知道物理地址而不知道IP地址，那么可以通过RARP协议发出征求自身IP地址
* 的广播请求，然后由RARP服务器负责回答。RARP协议广泛用于获取无盘工作站的IP地址。
*
* 反向地址转换协议（RARP）允许局域网的物理机器从网关服务器的 ARP 表或者缓存上
* 请求其 IP 地址。网络管理员在局域网网关路由器里创建一个表以映射物理地址（MAC）
* 和与其对应的 IP 地址。当设置一台新的机器时，其 RARP 客户机程序需要向路由器上
* 的 RARP 服务器请求相应的 IP 地址。假设在路由表中已经设置了一个记录， RARP 
* 服务器将会返回 IP 地址给机器，此机器就会存储起来以便日后使用。
*
* RARP使用与ARP相同的报头结构，作用与ARP相反。其因为较限于IP地址的运用以及其他
* 的一些缺点，因此渐为更新的BOOTP或DHCP所取代。
*
* RARP的工作原理如下:
*
* 1) 给主机发送一个本地的RARP广播，在此广播包中，声明自己的MAC地址并且请求任何
*    收到此请求的RARP服务器分配一个IP地址；
* 2) 本地网段上的RARP服务器收到此请求后，检查其RARP列表，查找该MAC地址对应的IP
*    地址；
* 3) 如果存在，RARP服务器就给源主机发送一个响应数据包并将此IP地址提供给对方主机
*    使用；
* 4) 如果不存在，RARP服务器对此不做任何的响应；
* 5) 源主机收到从RARP服务器的响应信息，就利用得到的IP地址进行通讯；如果一直没有
*    收到RARP服务器的响应信息，表示初始化失败。
*
* 根据前面的介绍，我们可以分析该函数的实现如下:
*
* 1) 构造arprequest结构，指定RARP_REQUEST。
* 2) 循环调用eth_transmit来传输这个RARP请求，并调用await_reply来等待回复。
*/
/**************************************************************************
RARP - Get my IP address and load information
**************************************************************************/
int 
rarp (void)
{
  int retry;

  /* arp and rarp requests share the same packet structure.  */
  struct arprequest rarpreq;

  /* Make sure that an ethernet is probed.  */
  if (! eth_probe ())
    return 0;

  /* Clear the ready flag.  */
  network_ready = 0;
  
  grub_memset (&rarpreq, 0, sizeof (rarpreq));

  rarpreq.hwtype = htons (1);
  rarpreq.protocol = htons (IP);
  rarpreq.hwlen = ETH_ALEN;
  rarpreq.protolen = 4;
  rarpreq.opcode = htons (RARP_REQUEST);
  grub_memmove ((char *) &rarpreq.shwaddr, arptable[ARP_CLIENT].node,
		ETH_ALEN);
  /* sipaddr is already zeroed out */
  grub_memmove ((char *) &rarpreq.thwaddr, arptable[ARP_CLIENT].node,
		ETH_ALEN);
  /* tipaddr is already zeroed out */

  for (retry = 0; retry < MAX_ARP_RETRIES; ++retry)
    {
      long timeout;
      
      eth_transmit (broadcast, RARP, sizeof (rarpreq), &rarpreq);

      timeout = rfc2131_sleep_interval (TIMEOUT, retry);
      if (await_reply (AWAIT_RARP, 0, rarpreq.shwaddr, timeout))
	break;

      if (ip_abort)
	return 0;
    }

  if (retry < MAX_ARP_RETRIES)
    {
      network_ready = 1;
      return 1;
    }

  return 0;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现使用BOOTP获取本机IP地址和负载信息的功能。
*
* 关于BOOTP协议的介绍如下:
*
* 该协议主要用于有无盘工作站的局域网中，客户端获取IP地址的过程如下：首先，由
* BOOTP启动代码启动客户端，这个时候客户端还没有IP地址，使用广播形式以IP地址
* 255.255.255.255向网络中发出IP地址查询要求。接着，运行BOOTP协议的服务器接收
* 到这个请求，会根据请求中提供的MAC地址找到客户端，并发送一个含有IP地址、服务
* 器IP地址、网关等信息的FOUND帧。最后，客户端会根据该FOUND帧来通过专用TFTP服
* 务器下载启动镜像文件，模拟成磁盘启动。
*
* 根据上面的介绍，我们可以分析本函数实现如下:
*
* 1) 构造bootpip_t结构，指定BOOTP_REQUEST请求。
* 2) 在循环内部执行下面的步骤: 
*    a) 调用udp_transmit发送BOOTP_REQUEST广播请求。
*    b) 调用await_reply等待服务器响应。
*/
/**************************************************************************
BOOTP - Get my IP address and load information
**************************************************************************/
int 
bootp (void)
{
  int retry;
#ifndef	NO_DHCP_SUPPORT
  int reqretry;
#endif /* ! NO_DHCP_SUPPORT */
  struct bootpip_t ip;
  unsigned long starttime;

  /* Make sure that an ethernet is probed.  */
  if (! eth_probe ())
    return 0;

  /* Clear the ready flag.  */
  network_ready = 0;

#ifdef DEBUG
  grub_printf ("network is ready.\n");
#endif
  
  grub_memset (&ip, 0, sizeof (struct bootpip_t));
  ip.bp.bp_op = BOOTP_REQUEST;
  ip.bp.bp_htype = 1;
  ip.bp.bp_hlen = ETH_ALEN;
  starttime = currticks ();
  /* Use lower 32 bits of node address, more likely to be
     distinct than the time since booting */
  grub_memmove (&xid, &arptable[ARP_CLIENT].node[2], sizeof(xid));
  ip.bp.bp_xid = xid += htonl (starttime);
  grub_memmove (ip.bp.bp_hwaddr, arptable[ARP_CLIENT].node, ETH_ALEN);
#ifdef DEBUG
  etherboot_printf ("bp_op = %d\n", ip.bp.bp_op);
  etherboot_printf ("bp_htype = %d\n", ip.bp.bp_htype);
  etherboot_printf ("bp_hlen = %d\n", ip.bp.bp_hlen);
  etherboot_printf ("bp_xid = %d\n", ip.bp.bp_xid);
  etherboot_printf ("bp_hwaddr = %!\n", ip.bp.bp_hwaddr);
  etherboot_printf ("bp_hops = %d\n", (int) ip.bp.bp_hops);
  etherboot_printf ("bp_secs = %d\n", (int) ip.bp.bp_hwaddr);
#endif
  
#ifdef	NO_DHCP_SUPPORT
  /* Request RFC-style options.  */
  grub_memmove (ip.bp.bp_vend, rfc1533_cookie, 5);
#else
  /* Request RFC-style options.  */
  grub_memmove (ip.bp.bp_vend, rfc1533_cookie, sizeof rfc1533_cookie);
  grub_memmove (ip.bp.bp_vend + sizeof rfc1533_cookie, dhcpdiscover,
		sizeof dhcpdiscover);
  grub_memmove (ip.bp.bp_vend + sizeof rfc1533_cookie + sizeof dhcpdiscover,
		rfc1533_end, sizeof rfc1533_end);
#endif /* ! NO_DHCP_SUPPORT */

  for (retry = 0; retry < MAX_BOOTP_RETRIES;)
    {
      long timeout;

#ifdef DEBUG
      grub_printf ("retry = %d\n", retry);
#endif
      
      /* Clear out the Rx queue first.  It contains nothing of
       * interest, except possibly ARP requests from the DHCP/TFTP
       * server.  We use polling throughout Etherboot, so some time
       * may have passed since we last polled the receive queue,
       * which may now be filled with broadcast packets.  This will
       * cause the reply to the packets we are about to send to be
       * lost immediately.  Not very clever.  */
      await_reply (AWAIT_QDRAIN, 0, NULL, 0);

      udp_transmit (IP_BROADCAST, BOOTP_CLIENT, BOOTP_SERVER,
		    sizeof (struct bootpip_t), &ip);
      timeout = rfc2131_sleep_interval (TIMEOUT, retry++);
#ifdef NO_DHCP_SUPPORT
      if (await_reply (AWAIT_BOOTP, 0, NULL, timeout))
	{
	  network_ready = 1;
	  return 1;
	}
#else /* ! NO_DHCP_SUPPORT */
      if (await_reply (AWAIT_BOOTP, 0, NULL, timeout))
	{
	  if (dhcp_reply != DHCPOFFER)
	    {
	      network_ready = 1;
	      return 1;
	    }

	  dhcp_reply = 0;
#ifdef DEBUG
  etherboot_printf ("bp_op = %d\n", (int) ip.bp.bp_op);
  etherboot_printf ("bp_htype = %d\n", (int) ip.bp.bp_htype);
  etherboot_printf ("bp_hlen = %d\n", (int) ip.bp.bp_hlen);
  etherboot_printf ("bp_xid = %d\n", (int) ip.bp.bp_xid);
  etherboot_printf ("bp_hwaddr = %!\n", ip.bp.bp_hwaddr);
  etherboot_printf ("bp_hops = %d\n", (int) ip.bp.bp_hops);
  etherboot_printf ("bp_secs = %d\n", (int) ip.bp.bp_hwaddr);
#endif
	  grub_memmove (ip.bp.bp_vend, rfc1533_cookie, sizeof rfc1533_cookie);
	  grub_memmove (ip.bp.bp_vend + sizeof rfc1533_cookie,
			dhcprequest, sizeof dhcprequest);
	  grub_memmove (ip.bp.bp_vend + sizeof rfc1533_cookie
			+ sizeof dhcprequest,
			rfc1533_end, sizeof rfc1533_end);
	  grub_memmove (ip.bp.bp_vend + 9, (char *) &dhcp_server,
			sizeof (in_addr));
	  grub_memmove (ip.bp.bp_vend + 15, (char *) &dhcp_addr,
			sizeof (in_addr));
#ifdef DEBUG
	  grub_printf ("errnum = %d\n", errnum);
#endif
	  for (reqretry = 0; reqretry < MAX_BOOTP_RETRIES;)
	    {
	      int ret;
#ifdef DEBUG
	      grub_printf ("reqretry = %d\n", reqretry);
#endif
	      
	      ret = udp_transmit (IP_BROADCAST, BOOTP_CLIENT, BOOTP_SERVER,
				  sizeof (struct bootpip_t), &ip);
	      if (! ret)
		grub_printf ("udp_transmit failed.\n");
	      
	      dhcp_reply = 0;
	      timeout = rfc2131_sleep_interval (TIMEOUT, reqretry++);
	      if (await_reply (AWAIT_BOOTP, 0, NULL, timeout))
		if (dhcp_reply == DHCPACK)
		  {
		    network_ready = 1;
		    return 1;
		  }

#ifdef DEBUG
	      grub_printf ("dhcp_reply = %d\n", dhcp_reply);
#endif
	      
	      if (ip_abort)
		return 0;
	    }
	}
#endif /* ! NO_DHCP_SUPPORT */
      
      if (ip_abort)
	return 0;
      
      ip.bp.bp_secs = htons ((currticks () - starttime) / TICKS_PER_SEC);
    }

  /* Timeout.  */
  return 0;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现计算UDP报的Checksum的功能(使用汇编实现)。
*/
/**************************************************************************
UDPCHKSUM - Checksum UDP Packet (one of the rare cases when assembly is
            actually simpler...)
 RETURNS: checksum, 0 on checksum error. This
          allows for using the same routine for RX and TX summing:
          RX  if (packet->udp.chksum && udpchksum(packet))
                  error("checksum error");
          TX  packet->udp.chksum=0;
              if (0==(packet->udp.chksum=udpchksum(packet)))
                  packet->upd.chksum=0xffff;
**************************************************************************/
static inline void
dosum (unsigned short *start, unsigned int len, unsigned short *sum)
{
  __asm__ __volatile__
    ("clc\n"
     "1:\tlodsw\n\t"
     "xchg %%al,%%ah\n\t"	/* convert to host byte order */
     "adcw %%ax,%0\n\t"		/* add carry of previous iteration */
     "loop 1b\n\t"
     "adcw $0,%0"		/* add carry of last iteration */
     : "=b" (*sum), "=S"(start), "=c"(len)
     : "0"(*sum), "1"(start), "2"(len)
     : "ax", "cc"
     );
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现计算UDP报的Checksum的功能(使用dosum实现)。
*/
/* UDP sum:
 * proto, src_ip, dst_ip, udp_dport, udp_sport, 2*udp_len, payload
 */
static unsigned short
udpchksum (struct iphdr *packet)
{
  int len = ntohs (packet->len);
  unsigned short rval;
  
  /* add udplength + protocol number */
  rval = (len - sizeof (struct iphdr)) + IP_UDP;
  
  /* pad to an even number of bytes */
  if (len % 2) {
    ((char *) packet)[len++] = 0;
  }
  
  /* sum over src/dst ipaddr + udp packet */
  len -= (char *) &packet->src - (char *) packet;
  dosum ((unsigned short *) &packet->src, len >> 1, &rval);
  
  /* take one's complement */
  return ~rval;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数等待网路回应的功能。根据响应的包类型ptype，分别处理对应的请求响应。
*/
/**************************************************************************
AWAIT_REPLY - Wait until we get a response for our request
**************************************************************************/
int 
await_reply (int type, int ival, void *ptr, int timeout)
{
  unsigned long time;
  struct iphdr *ip;
  struct udphdr *udp;
  struct arprequest *arpreply;
  struct bootp_t *bootpreply;
  unsigned short ptype;
  unsigned int protohdrlen = (ETH_HLEN + sizeof (struct iphdr)
			      + sizeof (struct udphdr));

  /* Clear the abort flag.  */
  ip_abort = 0;
  
  time = timeout + currticks ();
  /* The timeout check is done below.  The timeout is only checked if
   * there is no packet in the Rx queue.  This assumes that eth_poll()
   * needs a negligible amount of time.  */
  for (;;)
    {
      if (eth_poll ())
	{
	  /* We have something!  */
	  
	  /* Check for ARP - No IP hdr.  */
	  if (nic.packetlen >= ETH_HLEN)
	    {
	      ptype = (((unsigned short) nic.packet[12]) << 8
		       | ((unsigned short) nic.packet[13]));
	    }
	  else
	    /* What else could we do with it?  */
	    continue;
	  
	  if (nic.packetlen >= ETH_HLEN + sizeof (struct arprequest)
	      && ptype == ARP)
	    {
	      unsigned long tmp;

	      arpreply = (struct arprequest *) &nic.packet[ETH_HLEN];
	      
	      if (arpreply->opcode == htons (ARP_REPLY)
		  && ! grub_memcmp (arpreply->sipaddr, ptr, sizeof (in_addr))
		  && type == AWAIT_ARP)
		{
		  grub_memmove ((char *) arptable[ival].node,
				arpreply->shwaddr,
				ETH_ALEN);
		  return 1;
		}
	      
	      grub_memmove ((char *) &tmp, arpreply->tipaddr,
			    sizeof (in_addr));
	      
	      if (arpreply->opcode == htons (ARP_REQUEST)
		  && tmp == arptable[ARP_CLIENT].ipaddr.s_addr)
		{
		  arpreply->opcode = htons (ARP_REPLY);
		  grub_memmove (arpreply->tipaddr, arpreply->sipaddr,
				sizeof (in_addr));
		  grub_memmove (arpreply->thwaddr, (char *) arpreply->shwaddr,
				ETH_ALEN);
		  grub_memmove (arpreply->sipaddr,
				(char *) &arptable[ARP_CLIENT].ipaddr,
				sizeof (in_addr));
		  grub_memmove (arpreply->shwaddr,
				arptable[ARP_CLIENT].node,
				ETH_ALEN);
		  eth_transmit (arpreply->thwaddr, ARP,
				sizeof (struct arprequest),
				arpreply);
#ifdef MDEBUG
		  grub_memmove (&tmp, arpreply->tipaddr, sizeof (in_addr));
		  etherboot_printf ("Sent ARP reply to: %@\n", tmp);
#endif	/* MDEBUG */
		}
	      
	      continue;
	    }

	  if (type == AWAIT_QDRAIN)
	    continue;
	  
	  /* Check for RARP - No IP hdr.  */
	  if (type == AWAIT_RARP
	      && nic.packetlen >= ETH_HLEN + sizeof (struct arprequest)
	      && ptype == RARP)
	    {
	      arpreply = (struct arprequest *) &nic.packet[ETH_HLEN];
	      
	      if (arpreply->opcode == htons (RARP_REPLY)
		  && ! grub_memcmp (arpreply->thwaddr, ptr, ETH_ALEN))
		{
		  grub_memmove ((char *) arptable[ARP_SERVER].node,
				arpreply->shwaddr, ETH_ALEN);
		  grub_memmove ((char *) &arptable[ARP_SERVER].ipaddr,
				arpreply->sipaddr, sizeof (in_addr));
		  grub_memmove ((char *) &arptable[ARP_CLIENT].ipaddr,
				arpreply->tipaddr, sizeof (in_addr));
		  return 1;
		}
	      
	      continue;
	    }

	  /* Anything else has IP header.  */
	  if (nic.packetlen < protohdrlen || ptype != IP)
	    continue;
	  
	  ip = (struct iphdr *) &nic.packet[ETH_HLEN];
	  if (ip->verhdrlen != 0x45
	      || ipchksum ((unsigned short *) ip, sizeof (struct iphdr))
	      || ip->protocol != IP_UDP)
	    continue;
	  
	  /*
	    - Till Straumann <Till.Straumann@TU-Berlin.de>
	    added udp checksum (safer on a wireless link)
	    added fragmentation check: I had a corrupted image
	    in memory due to fragmented TFTP packets - took me
	    3 days to find the cause for this :-(
	  */
	  
	  /* If More Fragments bit and Fragment Offset field
	     are non-zero then packet is fragmented */
	  if (ip->frags & htons(0x3FFF))
	    {
	      grub_printf ("ALERT: got a fragmented packet - reconfigure your server\n");
	      continue;
	    }
	  
	  udp = (struct udphdr *) &nic.packet[(ETH_HLEN
					       + sizeof (struct iphdr))];
	  if (udp->chksum && udpchksum (ip))
	    {
	      grub_printf ("UDP checksum error\n");
	      continue;
	    }
	  
	  /* BOOTP ?  */
	  bootpreply = (struct bootp_t *)
	    &nic.packet[(ETH_HLEN + sizeof (struct iphdr)
			 + sizeof (struct udphdr))];
	  if (type == AWAIT_BOOTP
#ifdef NO_DHCP_SUPPORT
	      && (nic.packetlen
		  >= (ETH_HLEN + sizeof (struct bootp_t) - BOOTP_VENDOR_LEN))
#else
	      && (nic.packetlen
		  >= (ETH_HLEN + sizeof (struct bootp_t) - DHCP_OPT_LEN))
#endif /* ! NO_DHCP_SUPPORT */
	      && udp->dest == htons (BOOTP_CLIENT)
	      && bootpreply->bp_op == BOOTP_REPLY
	      && bootpreply->bp_xid == xid
	      && (! grub_memcmp (broadcast, bootpreply->bp_hwaddr, ETH_ALEN)
		  || ! grub_memcmp (arptable[ARP_CLIENT].node,
				    bootpreply->bp_hwaddr, ETH_ALEN)))
	    {
#ifdef DEBUG
	      grub_printf ("BOOTP packet was received.\n");
#endif
	      arptable[ARP_CLIENT].ipaddr.s_addr
		= bootpreply->bp_yiaddr.s_addr;
#ifndef	NO_DHCP_SUPPORT
	      dhcp_addr.s_addr = bootpreply->bp_yiaddr.s_addr;
#ifdef DEBUG
	      etherboot_printf ("dhcp_addr = %@\n", dhcp_addr.s_addr);
#endif
#endif /* ! NO_DHCP_SUPPORT */
	      netmask = default_netmask ();
	      arptable[ARP_SERVER].ipaddr.s_addr
		= bootpreply->bp_siaddr.s_addr;
	      /* Kill arp.  */
	      grub_memset (arptable[ARP_SERVER].node, 0, ETH_ALEN);
	      arptable[ARP_GATEWAY].ipaddr.s_addr
		= bootpreply->bp_giaddr.s_addr;
	      /* Kill arp.  */
	      grub_memset (arptable[ARP_GATEWAY].node, 0, ETH_ALEN);

	      grub_memmove ((char *) BOOTP_DATA_ADDR, (char *) bootpreply,
			    sizeof (struct bootpd_t));
#ifdef NO_DHCP_SUPPORT
	      decode_rfc1533 (BOOTP_DATA_ADDR->bootp_reply.bp_vend,
			      0, BOOTP_VENDOR_LEN + MAX_BOOTP_EXTLEN, 1);
#else
	      decode_rfc1533 (BOOTP_DATA_ADDR->bootp_reply.bp_vend,
			      0, DHCP_OPT_LEN + MAX_BOOTP_EXTLEN, 1);
#endif /* ! NO_DHCP_SUPPORT */
	      
	      return 1;
	    }
	  
	  /* TFTP ? */
	  if (type == AWAIT_TFTP && ntohs (udp->dest) == ival)
	    return 1;
	}
      else
	{
	  /* Check for abort key only if the Rx queue is empty -
	   * as long as we have something to process, don't
	   * assume that something failed.  It is unlikely that
	   * we have no processing time left between packets.  */
	  if (checkkey () != -1 && ASCII_CHAR (getkey ()) == CTRL_C)
	    {
	      ip_abort = 1;
	      return 0;
	    }
	  
	  /* Do the timeout after at least a full queue walk.  */
	  if ((timeout == 0) || (currticks() > time))
	    {
	      break;
	    }
	}
    }
  
  return 0;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现解码RFC1533头部的功能。
*/
/**************************************************************************
DECODE_RFC1533 - Decodes RFC1533 header
**************************************************************************/
int
decode_rfc1533 (unsigned char *p, int block, int len, int eof)
{
  static unsigned char *extdata = NULL, *extend = NULL;
  unsigned char *extpath = NULL;
  unsigned char *endp;
  
  if (block == 0)
    {
      end_of_rfc1533 = NULL;
      vendorext_isvalid = 0;
      
      if (grub_memcmp (p, rfc1533_cookie, 4))
	/* no RFC 1533 header found */
	return 0;
      
      p += 4;
      endp = p + len;
    }
  else
    {
      if (block == 1)
	{
	  if (grub_memcmp (p, rfc1533_cookie, 4))
	    /* no RFC 1533 header found */
	    return 0;
	  
	  p += 4;
	  len -= 4;
	}
      
      if (extend + len
	  <= ((unsigned char *)
	      &(BOOTP_DATA_ADDR->bootp_extension[MAX_BOOTP_EXTLEN])))
	{
	  grub_memmove (extend, p, len);
	  extend += len;
	}
      else
	{
	  grub_printf ("Overflow in vendor data buffer! Aborting...\n");
	  *extdata = RFC1533_END;
	  return 0;
	}
      
      p = extdata;
      endp = extend;
    }

  if (! eof)
    return -1;
  
  while (p < endp)
    {
      unsigned char c = *p;
      
      if (c == RFC1533_PAD)
	{
	  p++;
	  continue;
	}
      else if (c == RFC1533_END)
	{
	  end_of_rfc1533 = endp = p;
	  continue;
	}
      else if (c == RFC1533_NETMASK)
	{
	  grub_memmove ((char *) &netmask, p + 2, sizeof (in_addr));
	}
      else if (c == RFC1533_GATEWAY)
	{
	  /* This is a little simplistic, but it will
	     usually be sufficient.
	     Take only the first entry.  */
	  if (TAG_LEN (p) >= sizeof (in_addr))
	    grub_memmove ((char *) &arptable[ARP_GATEWAY].ipaddr, p + 2,
			  sizeof (in_addr));
	}
      else if (c == RFC1533_EXTENSIONPATH)
	extpath = p;
#ifndef	NO_DHCP_SUPPORT
      else if (c == RFC2132_MSG_TYPE)
	{
	  dhcp_reply = *(p + 2);
	}
      else if (c == RFC2132_SRV_ID)
	{
	  grub_memmove ((char *) &dhcp_server, p + 2, sizeof (in_addr));
#ifdef DEBUG
	  etherboot_printf ("dhcp_server = %@\n", dhcp_server.s_addr);
#endif
	}
#endif /* ! NO_DHCP_SUPPORT */
      else if (c == RFC1533_VENDOR_MAGIC
	       && TAG_LEN(p) >= 6
	       && ! grub_memcmp (p + 2, vendorext_magic, 4)
	       && p[6] == RFC1533_VENDOR_MAJOR)
	vendorext_isvalid++;
      /* GRUB now handles its own tag. Get the name of a configuration
	 file from the network. Cool...  */
      else if (c == RFC1533_VENDOR_CONFIGFILE)
	{
	  int l = TAG_LEN (p);
	  
	  /* Eliminate the trailing NULs according to RFC 2132.  */
	  while (*(p + 2 + l - 1) == '\000' && l > 0)
	    l--;
	  
	  /* XXX: Should check if LEN is less than the maximum length
	     of CONFIG_FILE. This kind of robustness will be a goal
	     in GRUB 1.0.  */
	  grub_memmove (config_file, p + 2, l);
	  config_file[l] = 0;
	}
      
      p += TAG_LEN (p) + 2;
    }
  
  extdata = extend = endp;
  
  /* Perhaps we can eliminate this because we doesn't require so
     much information, but I leave this alone.  */
  if (block == 0 && extpath != NULL)
    {
      char fname[64];
      int fnamelen = TAG_LEN (extpath);
      
      while (*(extpath + 2 + fnamelen - 1) == '\000' && fnamelen > 0)
	fnamelen--;
      
      if (fnamelen + 1 > sizeof (fname))
	{
	  grub_printf ("Too long file name for Extensions Path\n");
	  return 0;
	}
      else if (! fnamelen)
	{
	  grub_printf ("Empty file name for Extensions Path\n");
	  return 0;
	}
      
      grub_memmove (fname, extpath + 2, fnamelen);
      fname[fnamelen] = '\000';
      grub_printf ("Loading BOOTP-extension file: %s\n", fname);
      tftp (fname, decode_rfc1533);
    }
  
  /* Proceed with next block.  */
  return -1;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数Checksum IP Header的功能。
*/
/**************************************************************************
IPCHKSUM - Checksum IP Header
**************************************************************************/
static unsigned short 
ipchksum (unsigned short *ip, int len)
{
  unsigned long sum = 0;
  len >>= 1;
  while (len--)
    {
      sum += *(ip++);
      if (sum > 0xFFFF)
	sum -= 0xFFFF;
    }
  return (~sum) & 0x0000FFFF;
}

#define TWO_SECOND_DIVISOR (2147483647l/TICKS_PER_SEC)
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现RFC2131睡眠的功能。
*/
/**************************************************************************
RFC2131_SLEEP_INTERVAL - sleep for expotentially longer times
**************************************************************************/
long
rfc2131_sleep_interval (int base, int exp)
{
  static long seed = 0;
  long q;
  unsigned long tmo;
  
#ifdef BACKOFF_LIMIT
  if (exp > BACKOFF_LIMIT)
    exp = BACKOFF_LIMIT;
#endif
  if (!seed)
    /* Initialize linear congruential generator */
    seed = (currticks () + *((long *) &arptable[ARP_CLIENT].node)
	    + ((short *) arptable[ARP_CLIENT].node)[2]);
  /* simplified version of the LCG given in Bruce Schneier's
     "Applied Cryptography" */
  q = seed / 53668;
  if ((seed = 40014 * (seed - 53668 * q) - 12211 *q ) < 0)
    seed += 2147483563L;
  tmo = (base << exp) + (TICKS_PER_SEC - (seed / TWO_SECOND_DIVISOR));
  return tmo;
}
/**
* @attention 本注释得到了"核高基"科技重大专项2012年课题“开源操作系统内核分析和安全性评估
*（课题编号：2012ZX01039-004）”的资助。
*
* @copyright 注释添加单位：清华大学——03任务（Linux内核相关通用基础软件包分析）承担单位
*
* @author 注释添加人员：谢文学
*
* @date 注释添加日期：2013年6月4日
*
* @note 注释详细内容:
* 
* 该函数实现关闭网路接口的功能。
*/
/**************************************************************************
CLEANUP - shut down networking
**************************************************************************/
void
cleanup_net (void)
{
  if (network_ready)
    {
      /* Stop receiving packets.  */
      eth_disable ();
      network_ready = 0;
    }
}
