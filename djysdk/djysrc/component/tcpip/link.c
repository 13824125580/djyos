//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 ����Ȩ����������������޹�˾���С�����Ȩ�˱���һ��Ȩ����
//
// �����Ȩ�����ʹ���߷��������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������
// 3. �������Ϊ�����߲���ϵͳ����ɲ��֣�δ����ǰȡ�õ�������ɣ���������ֲ����
//    �����߲���ϵͳ���������С�

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------
#include <sys/socket.h>
#include "link.h"
#include "rout.h"
#include "arp.h"
#include "ip.h"

#pragma pack(1)
typedef struct EthernetHdr
{
    u8 macdst[CN_MACADDR_LEN];
    u8 macsrc[CN_MACADDR_LEN];
    u16 type;
}tagEthernetHdr;
#pragma pack()
#define CN_ETHERNET_HEADLEN  sizeof(tagEthernetHdr)

//the internet proto deal function we need implement
extern bool_t IpPushPkg(tagNetDev *netdev, tagNetPkg *pkg);
extern bool_t ArpPushPkg(tagNetDev *netdev, tagNetPkg *pkg);

//use this function to make an ethernet package and send it to the netdev
bool_t __EthernetSend(tagNetDev *dev,tagNetPkg *pkg,u32 framlen,u32 devtask,u16 proto,\
                     enum_ipv_t ver,ipaddr_t ip)
{
    bool_t            result;
    tagEthernetHdr   *hdr;
    tagNetPkg        *ethpkg;

    result = false;
    ethpkg = PkgMalloc(CN_ETHERNET_HEADLEN,0);
    if(NULL != ethpkg)
    {
        hdr = (tagEthernetHdr  *)(ethpkg->buf + ethpkg->offset);
        memcpy(hdr->macsrc, NetDevGetMac(dev), CN_MACADDR_LEN);
        hdr->type = htons(proto);
        if(ver == EN_IPV_4)
        {
            result = ArpFindMac(ip,hdr->macdst);  //the ipv4 support
        }
        else
        {
            //not support the ipv6 yet
        }
        if(result)
        {
            ethpkg->datalen = CN_ETHERNET_HEADLEN;
            ethpkg->partnext = pkg;
            framlen += CN_ETHERNET_HEADLEN;
            result = dev->ifsend((ptu32_t)dev,ethpkg,framlen, devtask);
        }
        else
        {
             //can't find the mac
        }
        PkgTryFreePart(ethpkg);
    }
    return result;
}

// =============================================================================
// FUNCTION   :the driver should call this function to post the package here
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t LinkPost(ptu32_t devhandle,tagNetPkg *pkg)
{
    bool_t          result=false;
    tagNetDev      *dev;
    tagEthernetHdr *hdr;
    u16             proto;

    dev = (tagNetDev *)devhandle;
    if((NULL != dev)&&(NULL != pkg)&&(pkg->datalen > 0))
    {
        switch(dev->iftype)
        {
            case EN_LINK_ETHERNET:
                //we analyze the ethernet header first, which type it has
                hdr = (tagEthernetHdr *)(pkg->buf + pkg->offset);
                pkg->offset += CN_ETHERNET_HEADLEN;
                pkg->datalen -= CN_ETHERNET_HEADLEN;
                proto = ntohs(hdr->type);
                switch(proto)
                {
                 //uptil now, we support the ipv4
                    case EN_NET_PROTO_IP:
                        result = IpPushPkg((tagNetDev *)devhandle,pkg);
                        break;
                    case EN_NET_PROTO_ARP:
                        result = ArpPushPkg((tagNetDev *)devhandle,pkg);
                        break;
                    default:
                        break;
                }
                break;
            case EN_LINK_RAW:      //we could send the package to the net device directly
                //this only support the ip frame,for the raw type, no mac need
                result = IpPushPkg((tagNetDev *)devhandle,pkg);
                break;
            default:
                break;
        }
    }
    TCPIP_DEBUG_INC(dev->frcv);
    if(result == false)
    {
        TCPIP_DEBUG_INC(dev->frcverr);
    }
    return result;
}

// =============================================================================
// FUNCTION   :you could use this function to send the package
// PARAMS IN  :para, the net device function parameters as tagNetDevPara defined
// PARAMS OUT :NULL
// RETURN     :the dev handle
// DESCRIPTION:NO DUPLICATION NAME PERMITTED
// =============================================================================
bool_t LinkSend(tagRout *rout,tagNetPkg *pkg,u32 framlen,u32 devtask,u16 proto,\
                enum_ipv_t ver,ipaddr_t ip)
{
    bool_t result = false;
    tagNetDev *dev;

    if((NULL != rout) &&(NULL != rout->dev))
    {
        dev = rout->dev;
        switch(dev->iftype)
        {
            case EN_LINK_ETHERNET: // we need make a ethernet header
                //first we need to know the
                result = __EthernetSend(dev,pkg,framlen,devtask,proto,ver,ip);
                break;
            case EN_LINK_RAW:      // we could send the package to the net device directly
            	result = dev->ifsend((ptu32_t)dev,pkg,framlen,devtask);
                break;
            default:
                break;
        }

        TCPIP_DEBUG_INC(dev->fsnd);
        if(result == false)
        {
            TCPIP_DEBUG_INC(dev->fsnderr);
        }
    }
    return result;
}

bool_t LinkSendRaw(tagRout *rout,tagNetPkg *pkg,u32 framlen,u32 devtask)
{
    bool_t result = false;
    tagNetDev *dev;

    if((NULL != rout) &&(NULL != rout->dev))
    {
        dev = rout->dev;
    	result = dev->ifsend((ptu32_t)dev,pkg,framlen,devtask);
        TCPIP_DEBUG_INC(dev->fsnd);
        if(result == false)
        {
            TCPIP_DEBUG_INC(dev->fsnderr);
        }
    }
    return result;
}

// =============================================================================
// FUNCTION   :this is the link module initialize function
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :
// DESCRIPTION:uptils now, we do nothing here
// =============================================================================
bool_t LinkInit(void)
{
    return true;
}


