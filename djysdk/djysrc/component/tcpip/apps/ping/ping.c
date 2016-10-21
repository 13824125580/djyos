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

#include "shell.h"
#include "../../icmp.h"


#define CN_PING_DEFAULT_TIMEOUT   (1*1000*mS)   //1s
#define CN_PING_DEFAULT_COUNTER   (4)
#define CN_PING_DEFAULT_SIZE      (32)
// =============================================================================
// FUNCTION:This function is used to do the ping echo
// PARA  IN:param
//         :must be in the style:host name   times
// PARA OUT:
// RETURN  :
// INSTRUCT:
// =============================================================================
bool_t Ping(char *param)
{
    u32 i;
    u32 ip;
    u32 timeused;
    u32 timestart;
    u32 timeend;
    u8  datapad[CN_PING_DEFAULT_SIZE];
    struct in_addr ipaddr;
    struct hostent *host;
    u32    times;

    int argc = 2;
    char *argv[2];
    if(NULL == param)
    {
        return false;
    }

    string2arg(&argc,argv,param);
    if((argc != 1)&&(argc != 2))
    {
        return false;
    }
    if(argc == 2)
    {
        times = strtol(argv[1],NULL,0);
    }
    else
    {
        times = CN_PING_DEFAULT_COUNTER;
    }

    if(0 == inet_aton(argv[0],&ipaddr))
    {
        //use the dns to get the ip
        host = gethostbyname(argv[0]);
        if(NULL != host)
        {
            memcpy((void *)&ipaddr,host->h_addr_list[0],sizeof(ipaddr));
        }
        else
        {
            printf("%s:Unknown host:%s\n\r",__FUNCTION__,param);
            return false;
        }
    }
    ip = ipaddr.s_addr;

    memset(datapad,0x55,CN_PING_DEFAULT_SIZE);
    for(i = 0; i <times; i++)
    {
        timestart = (u32)DjyGetSysTime();
        if(Icmp_EchoRequest(ip,datapad,CN_PING_DEFAULT_SIZE,CN_PING_DEFAULT_TIMEOUT))
        {
            timeend = (u32)DjyGetSysTime();
            timeused = (u32)(timeend - timestart);
            printf("0x%08x %s reply:Time = %d ms\n\r",i,inet_ntoa(ipaddr),timeused/1000);
            Djy_EventDelay(CN_PING_DEFAULT_TIMEOUT);
        }
        else
        {
            printf("0x%08x %s reply:Timeout\n\r",i,inet_ntoa(ipaddr));
        }
    }
    return true;
}

struct ShellCmdTab  gServicePing[] =
{
    {
        "ping",
        Ping,
        "usage:ping hostname [times]",
        "usage:ping hostname [times]",
    }
};

#define CN_PINGDEBUG_NUM  ((sizeof(gServicePing))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gServicePingCmdRsc[CN_PINGDEBUG_NUM];

//THIS IS PING MODULE FUNCTION
bool_t ServicePingInit(ptu32_t para)
{
    bool_t result;

    result = Sh_InstallCmd(gServicePing,gServicePingCmdRsc,CN_PINGDEBUG_NUM);

    return result;
}

