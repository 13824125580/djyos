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
#include "os_inc.h"
#include "cpu_peri.h"
#include "font_songti_16x16.h"
#include "key_hard.h"

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    return djy_main();
}


//----�����ʼ��2-----------------------------------------------------------
//���ܣ���ѡ�����ʼ����������3��:
//      1��sys_module_init����sys_init.c�У��μ��ú���ע��
//      2��prj_module_init���������ʼ����ģ�飬��Ҫ���ø����幤����صĴ��룬��
//         �����ģ�飬���˵���module_init_keyboard�����⣬��Ҫ���ø�Ӳ����ص�
//         module_init_keyboard_hard������
//      ǰ������ʼ��ʱ�����������䷽ʽ��û�г�ʼ��������������õ���̬���䣬ʹ��
//      ����׼��̬���䣬����׼��̬�����˵�����μ�mems.c�ļ���
//      3��run_module_init���μ��ú���ע�͡�
//��������
//���أ���
//-----------------------------------------------------------------------------
void prj_module_init(void)
{
    djy_handle_t char_term_lhdl;
    uint16_t evtt_main;

#if (CN_CFG_GUI_KERNEL == 1)
    static struct tagGkWinRsc desktop;
    struct tagDisplayRsc *lcd_omapl138;
#endif


#if (CN_CFG_TIME_MODULE == 1)
    TM_ModuleInit(0);
#if(CN_CFG_TIME_SRC == CN_CFG_TIME_SRC_RTC)
    RTC_ModuleInit(0);
#endif
#endif


#if (CN_CFG_WDT == 1)
    module_init_wdt(0);
#endif

#if ((CN_CFG_DJYFS == 1) && (CN_CFG_RUN_MODE != CN_MODE_TINY))
    Djyfs_ModuleInit(0);
#if (CN_CFG_DJYFS_FLASH == 1)
    DFFSD_ModuleInit(0);
    module_init_fs_nandflash(0);
#endif
#endif


#if (CN_CFG_DJYIP == 1)
    module_init_djyip(0);
#endif

#if (CN_CFG_KEYBOARD == 1)
    Keyboard_ModuleInit(0);
    module_init_keyboard_hard(0);
#endif

//��ʼ��gui kernelģ��
#if ((CN_CFG_GUI_KERNEL == 1) && (CN_CFG_RUN_MODE != CN_MODE_TINY))
    GK_ModuleInit(0);
    lcd_omapl138 = (struct tagDisplayRsc*)module_init_lcd((ptu32_t)"lcd_omapl138");

    GK_ApiCreateDesktop(lcd_omapl138,&desktop,0,0,
                        CN_COLOR_RED+CN_COLOR_GREEN,CN_WINBUF_BUF);

    //���³�ʼ���ַ�����������
#if (CN_CFG_CHARSET_GB2312 == 1)       //��ʼ��gb2312�ַ���
extern ptu32_t Gk_Gb2312EncodeModuleInit(ptu32_t para);
    Gk_Gb2312EncodeModuleInit(0);
#endif

#if (CN_CFG_CHARSET_ASCII == 1)       //��ʼ��ascii�ַ���
    Gk_AsciiModuleInit(0);
#endif

#if (CN_CFG_CHARSET_UTF8 == 1)       //��ʼ��utf8�ַ���
    Gk_Utf8EncodeModuleInit(0);
#endif

//���³�ʼ��������������
#if (CN_CFG_GB2312_8x16_1616 == 1)     //��ʼ��gb2312�����ֿ�
extern ptu32_t Gk_Gb2312_816_1616FontModuleInit(ptu32_t para);
    Gk_Gb2312_816_1616FontModuleInit((ptu32_t)cs_gb2312_song_16);
#endif

#if (CN_CFG_ASCII_8x16 == 1)           //��ʼ��8*16 ascii�����ֿ�
    Gk_Ascii8x16FontModuleInit(0);
#endif

#if (CN_CFG_ASCII_8x8 == 1)           //��ʼ��8*8 ascii�����ֿ�
extern ptu32_t Gk_Ascii8x8FontModuleInit(ptu32_t para);
    Gk_Ascii8x8FontModuleInit(0);
#endif
extern ptu32_t Gk_NlsModuleInit(ptu32_t para);
    Gk_NlsModuleInit(0);
#endif		//for #if (CN_CFG_GUI_KERNEL == 1)


#if(CN_CFG_UART0_USED == 1)
    module_init_uart0(0);
#endif
#if(CN_CFG_UART1_USED == 1)
    module_init_uart1(0);
#endif
#if(CN_CFG_UART2_USED == 1)
    module_init_uart2(0);
#endif

#if (CN_CFG_SHELL == 1)
    Sh_MoudleInit(0);
    char_term_lhdl = Driver_DevOpenLeft(CN_CFG_STDIN_DEVICE,0);
    if(char_term_lhdl != NULL)
    {
        Driver_DevCtrlLeft(char_term_lhdl,CN_UART_START,0,0);
        //���ô��ڲ�����Ϊ57600��
        Driver_DevCtrlLeft(char_term_lhdl,CN_UART_SET_BAUD,57600,0);
        CharTerm_ModuleInit((ptu32_t)char_term_lhdl);
        Driver_DevCtrlLeft(char_term_lhdl,cn_uart_connect_recv_evtt,
                            Djy_GetEvttId("Term_Scan"),0);
    }
#endif
    
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2, 
                                __djy_main,CN_CFG_MAIN_STACK,"main function");
    //�¼���������������Ϊ0�������shell�������������ɼ�shell�����в���
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,0);
    return 1;
}

//----�����ʼ��3-----------------------------------------------------------
//���ܣ���ѡ�����ʼ����������3��:
//      1��sys_module_init����sys_init.c�У��μ��ú���ע��
//      2��prj_module_init���μ��ú���ע�͡�
//      3��run_module_init����������djy_main������ʼ�����ã���Ҫ����ϵͳ���¼�
//          ���ȿ�ʼ�󣬻�����Ҫ�������ڴ�����ʼ������ܳ�ʼ����ģ�顣
//��������
//���أ���
//-----------------------------------------------------------------------------
void run_module_init(void)
{
    return;
}


