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

#define CN_LOOP_MTU      8*1024        //8KB
#define CN_LOOP_DEVICE   "loop"
static  ptu32_t          LoopHandle = NULL;
// =============================================================================
// FUNCTION��this function is the loop device to send the message
// PARA  IN��
// PARA OUT��
// RETURN  ��true success while false failed
// INSTRUCT:cpy the function to an new package and post it to the stack
// =============================================================================
bool_t __LoopSnd(ptu32_t dev,tagNetPkg *pkg,u32 framlen,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    tagNetPkg *sndpkg;
    u8 *src;
    u8 *dst;

    result = false;
    if((LoopHandle == dev)&&(NULL != pkg))
    {
        sndpkg = PkgMalloc(framlen,0);
        if(NULL != sndpkg)
        {
            tmp = pkg;
            while(NULL != tmp)
            {
                src = (u8 *)(tmp->buf + tmp->offset);
                dst = (u8 *)(sndpkg->buf + sndpkg->offset + sndpkg->datalen);
                memcpy(dst, src, tmp->datalen);
                sndpkg->datalen += tmp->datalen;
                if(tmp->pkgflag & CN_PKLGLST_END)
                {
                    tmp = NULL;
                }
                else
                {
                    tmp = tmp->partnext;
                }
            }
            result = LinkPost(LoopHandle,sndpkg);
            PkgTryFreePart(sndpkg);
        }
    }
    return result;
}

// =============================================================================
// FUNCTION��this function is used to add an loop device and a loop link
// PARA  IN��
// PARA OUT��
// RETURN  ��true success while false failed
// INSTRUCT:if success, then the stack could  send to or receive from itself
// =============================================================================
bool_t LoopInit(void)
{
    bool_t  result = false;
    tagNetDevPara   devpara;
    tagHostAddrV4   devaddr;

    memset((void *)&devpara,0,sizeof(devpara));
    devpara.ifsend = __LoopSnd;
    devpara.iftype = EN_LINK_RAW;
    devpara.name = CN_LOOP_DEVICE;
    devpara.private = 0;
    devpara.mtu = CN_LOOP_MTU;
    devpara.devfunc = CN_IPDEV_ALL;
    memcpy(devpara.mac,CN_MAC_BROAD,CN_MACADDR_LEN);

    LoopHandle = NetDevInstall(&devpara);
    if(0 != LoopHandle)
    {
        devaddr.ip      = inet_addr("127.0.0.1");
        devaddr.gatway  = inet_addr("127.0.0.1");
        devaddr.submask = inet_addr("255.255.255.0");
        devaddr.dns     = inet_addr("127.0.0.1");
        devaddr.broad   = inet_addr("127.0.0.255");
        //loop could receive the broad mail,but never send the broad mail
        result = RoutCreate(CN_LOOP_DEVICE,EN_IPV_4,(void *)&devaddr,CN_ROUT_NONE);
    }
    return result;
}


