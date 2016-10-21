//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

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
// �����Ȩ�����ʹ���߷������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "board-config.h"
#include "config-prj.h"
#include "driver.h"
#include "cpu_peri.h"
#include "cpu_peri_uart.h"
#include "uartctrl.h"
#include "gkernel.h"
#include "djyos.h"
#include "timer.h"
#include "lowpower.h"
extern ptu32_t ModuleInstall_DebugInfo(ptu32_t para);

const u32 gc_u32CfgTimeZone = 8;     //����ʱ��Ϊ��8��(����)

const char *gc_pCfgDefaultCharsetName = "gb2312";  //Ĭ���ַ���
const char *gc_pCfgDefaultFontName = "gb2312_8_16_1616";  //Ĭ������

const u32 gc_u32CfgGuiCmdDeep = 1024;   //gkernel����������(�ֽ���,����������)
const u32 gc_u32CfgGuiStsDeep = 256;    //gkernel״̬���������(�ֽ���,����������)

const u32 gc_u32CfgDeviceLimit = 50;     //�����豸������
const u32 gc_u32CfgLockLimit = 100;      //�������������û�����semp_create��mutex_create�����������������ں��õ�������
const u32 gc_u32CfgEventLimit = 15;     //�¼�����
const u32 gc_u32CfgEvttLimit = 15;      //���¼�������
const u32 gc_u32CfgWdtLimit = 15;       //������������
const u32 gc_u32CfgPipeLimit = 20;       //�ܵ�����
const u32 gc_u32CfgMemPoolLimit = 20;    //������10���ڴ��
const u32 gc_u32CfgStdinDeviceLimit = 2;
const u32 gc_u32CfgMainStackLen = 8192;

const u32 gc_u32CfgOpenedfileLimit = 50;
const u32 gc_u32CfgTimerLimit = 20;

const char *gc_pCfgStdinName = "/dev/UART0";    //��׼�����豸
const char *gc_pCfgStdoutName = "/dev/UART0";   //��׼����豸
const char *gc_pCfgStderrName = "/dev/UART0";   //��׼��������豸
const char *gc_pCfgWorkPath = "sys:\\";         //����·������

//Э��ջ����
const u32 gNetPkgMemSize = 10*1024*1024;     //10MB

u32 SR1210_EntrySleepReCall(u32 SleepLevel);
u32 SR1210_ExitSleepReCall(u32 SleepLevel);

extern  void _start(void);

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    volatile u32 a;
    a =&_start;
    djy_main();
    return 0;
}

//----����ϵͳ���в�������-----------------------------------------------------
//����: ���ò���ϵͳ���в���,������������/�¼�������.ʹ���ı��ļ�����ʱʹ�á�
//����: ��
//����: ��
//-----------------------------------------------------------------------------
void Sys_ConfigRunParam(char *CfgfileName)
{
}

//----����ϵͳ�ں��������-----------------------------------------------------
//���ܣ����úͳ�ʼ����ѡ������������û�����Ŀ¼�б���ʵ�ֱ����������ں˳�ʼ��
//      �׶ε��á�
//      ������ʵ���ں˲ü����ܣ�����ֻҪע�͵�
//          ModuleInstall_Multiplex(0);
//      ��һ�У���·��������ͱ��ü����ˡ�
//      �û��ɴ�example��copy���ļ����Ѳ�Ҫ�����ע�͵���ǿ�ҽ��飬��Ҫɾ��,Ҳ
//      ��Ҫ�޸ĵ���˳�򡣿��԰��û�ģ��ĳ�ʼ������Ҳ���뵽��������,�������
//      ϵͳģ���ʼ������.
//      ��Щ�����������ϵ,�ü�ʱ,ע���Ķ�ע��.
//��������
//���أ���
//---------------------------------------------------------------------------
void Sys_ModuleInit(void)
{
    uint16_t evtt_main;

    //��ʼ��ֱ������������Ӳ����Ϊstdio.c�ж���� PutStrDirect��GetCharDirect
    //����ָ�븳ֵ
    //��������bsp�ĺ�����һ���Ǵ�������,BSPû�ṩ�Ļ����Ͳ�Ҫ���ã��ᵼ��Ӧ�ó�����벻ͨ����
    Stdio_KnlInOutInit( 0 );
   //shellģ��,����:��
    ModuleInstall_Sh(0);
    //�ļ�ϵͳģ��,����:shell
//    ModuleInstall_Djyfs(0);
    //�豸����ģ��
    ModuleInstall_Driver(0);
    //��·����ģ��,�ṩ����Linux��epoll��select�Ĺ���
    ModuleInstall_Multiplex(0);
    //��Ϣ����ģ��
    extern ptu32_t ModuleInstall_MsgQ (ptu32_t para);
    ModuleInstall_MsgQ(0);
    //�ṩ��shell������ں���Ϣ�Ĺ���,����:shellģ��
    ModuleInstall_DebugInfo(0);
    //�쳣����ģ��,����:shellģ��
    ModuleInstall_Exp(0);
    //flash�ļ�ϵͳ��ʼ��,����:�ļ�ϵͳģ��,shellģ��
    //���ù���·��,����:�ļ�ϵͳ,�����·������.
    ModuleInstall_FileSystem();
    ModuleInstall_UART(CN_UART0);


   //����Э��ջ�������ο�tcpipĿ¼�µ�boardnetcfg.cfg�ļ���ʵ����������
   //�����øð�������磬�������ʹ������Ļ� 
//    ModuleInstall_NetStaticIP(0);//ʹ�þ�̬IP
//    Module_BoardNetInstall_Dynamic(0); //ʹ�ö�̬����IP

    //enable the os update module
    //install the os netupdate module
    //install the os netupdate module
    extern u32    sst39DrvIdentify(u32 baseaddr);
    extern bool_t sst39DrvErase(u32 baseaddr, u32 offset, u32 len);
    extern bool_t sst39DrvWrite(u32 baseaddr,u32 offset, void *buffer, int length);
    extern bool_t ModuleInstall_OsUpdate(void *fnflashId, void *fnflasherase, void *fnflashwite);
    ModuleInstall_OsUpdate((void *)sst39DrvIdentify,\
                           (void *)sst39DrvErase,(void *)sst39DrvWrite);
//��װwebserver
//  unsigned int errno =0;
//  extern bool_t ModuleInstall_GoAhead(void);
//  ModuleInstall_GoAhead();

//��װJSON
//  extern int JSON_main (int argc, const char * argv[]);
//  JSON_main(0,NULL);

//��װIE
    extern bool_t ModuleInstallIe(ptu32_t param);
    ModuleInstallIe(0);

    //��׼IO��ʼ��,��Ӱ��printk����,�˺�,printf��scanf��ʹ��stdin/out�������
    //��,����õ�,��һֱ��PutChark,Putsk,GetChark,GetsKִ��IO
    //����: ��stdin/out/err���ļ�,�������ļ�ϵͳ
    //      �����豸,�������豸����
    //      ͬʱ,���������������Ϣ����ʩ,���紮��,LCD��
    OpenStdin(gc_pCfgStdinName);
    OpenStdout(gc_pCfgStdoutName);
    OpenStderr(gc_pCfgStderrName);
    //һ���������������stdin��stdout��stderrʹ�ò�ͬ�Ĵ��ڻ�����IOͨ������Ҫ
    //�ֱ��趨������
    Driver_CtrlDevice(ToDev(stdin),CN_UART_START,0,0);
//    Driver_CtrlDevice(ToDev(stdin),CN_UART_DMA_USED,0,0);
    //���ô��ڲ�����Ϊ115200��
    Driver_CtrlDevice(ToDev(stdin),CN_UART_SET_BAUD,115200,0);

    //��װ�˻���������ģ�飬������̡�����
    ModuleInstall_HmiIn( 0 );

    //djybusģ��
    ModuleInstall_DjyBus(0);
    //IIC����ģ��,����:djybus
    ModuleInstall_IICBus(0);
    //SPI����ģ��,����:djybus
    ModuleInstall_SPIBus(0);

    //����ʱ��ģ��
    ModuleInstall_TM(0);

    //if the APP, THEN LOAD THE APP MODULE
#ifdef APP_PRO
    //APP INIT
    extern bool_t EthCan_Main(void);
    EthCan_Main();

    extern bool_t CAN_Main(void);
    CAN_Main();
#endif

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                __djy_main,NULL,gc_u32CfgMainStackLen,
                                "main function");
    //�¼���������������Ϊ0,�����shell����,�������ɼ�shell�����в���
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0);

    //�û��Լ�����ĳ�ʼ��ģ��,��������￪ʼ����
#if CN_CFG_DYNAMIC_MEM == 1     //CN_CFG_DYNAMIC_MEM�����libOSʱ��������ر���һ��
    Heap_DynamicModuleInit(0);  //�Դ�malloc����ִ�п������㷨
#endif
    //����,��ʼ��ʱʹ�õ�ջ,�Ѿ���ϵͳ����,�������ʹ˽���,��������ú��ѿ�

    return ;
}


