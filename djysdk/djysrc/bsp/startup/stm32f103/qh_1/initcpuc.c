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
//����ģ��:  CPU��ʼ��
//���ߣ�     lst
//�汾��      V1.0.0
//��ʼ�汾������ڣ�2009-08-05
//�ļ�����:  cpuʱ�ӳ�ʼ��
//����˵��:  ��
//�޶���ʷ:
//2. ...
//1. ����: 2009-08-05
//   ����: lst
//   �°汾�ţ�V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------
#include "arch_feature.h"
#include "stdint.h"
#include "int.h"
#include "cpu.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "lowpower.h"
#include "djyos.h"
#include "stm32f10x.h"
#include "core_cmFunc.h"

extern   uint32_t   msp_top[ ];
extern void __set_PSP(uint32_t topOfProcStack);
extern void __set_PRIMASK(uint32_t priMask);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __set_CONTROL(uint32_t control);

extern void SRAM_Init(void);
extern void IAP_SelectLoadProgam(void);
extern char g_cIbootFlag[];

struct ScbReg volatile * const startup_scb_reg
                        = (struct ScbReg *)0xe000ed00;
void Startup_NMI(void)
{
    while(1);
}
void Startup_Hardfault(void)
{
    while(1);
}
void Init_Cpu(void);
const u32 gc_u32StartupExpTable[4] =
{
    (u32)msp_top,
    (u32)Init_Cpu,
    (u32) Startup_NMI,
    (u32) Startup_Hardfault
};

void Init_Cpu(void)
{
    __set_PSP((uint32_t)msp_top);
    __set_PRIMASK(1);
    __set_FAULTMASK(1);
    __set_CONTROL(0);
    switch(startup_scb_reg->CPUID)
    {
        case CN_M3_REVISION_R0P0:
            break;    //�г�û�а汾0��оƬ
        case CN_M3_REVISION_R1P0:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R1P1:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R2P0:break;    //����ûʲôҪ����
    }

    FLASH->ACR &= ~(u32)0x1f;
    FLASH->ACR |= (CN_CFG_MCLK-1)/24000000;   //���õȴ����ڡ�
    FLASH->ACR |= 0x10;       //����Ԥȡ

    if(((RCC->CR & cn_cr_check_mask) != cn_cr_check)
                || ((RCC->CFGR & cn_cfgr_check_mask) != cn_cfgr_check))
    {
        //��ʼ��ʼ��ʱ��
        //step1:��λʱ�ӿ��ƼĴ���
    	RCC->CR |= (uint32_t)0x00000001;
        // ��λ SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0] MCO[2:0] λ
    	RCC->CFGR &= (uint32_t)0xF8FF0000;
        // ��λ HSEON, CSSON and PLLON λ
    	RCC->CR &= (uint32_t)0xFEF6FFFF;
        // ��λ HSEBYP λ
    	RCC->CR &= (uint32_t)0xFFFBFFFF;
        // ��λ PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE λ
    	RCC->CFGR &= (uint32_t)0xFF80FFFF;
        // ��ֹ�����ж�
    	RCC->CIR = 0x00000000;

        //step2:���ø�ʱ�ӿ���λ�Լ���Ƶ����Ƶֵ
    	RCC->CFGR = cn_cfgr_set+(7<<24);   // set clock configuration register
    	RCC->CR   = cn_cr_set;     // set clock control register

        while(bb_rcc_cr_hserdy ==0);
        while(bb_rcc_cr_pllrdy ==0);
    }
    SRAM_Init();
//    __LP_BSP_HardInit( );
//    if( __LP_BSP_GetSleepLevel() == CN_SLEEP_L3)
//    {
//        __LP_BSP_RestoreRamL3();
//
//        HardExp_Init();
//        Int_ContactTrunk();
//        cm_cpsie_f();
//
//        __asm_start_thread(g_ptEventRunning->vm);
//    }
//    else
//      Load_Preload( );
    IAP_SelectLoadProgam();
}


//-----------------------------------------------------------------
//���ܣ�IAP�����������ģʽ��GPIO���ų�ʼ��SW4-GPIOC_10
//��������
//���أ��ޡ�
//˵���� ����ѡ���ĸ������ɾ�����ȷ����ѡ����ֵ�ƽ��Ӧ��������״̬Ҳ����ȷ����
//ѡ��ԭ���ǣ���GPIO������������²������ã�ֻ����APP��������쳣�ܷɣ��޷�ͨ��shell
//�����л���IBOOT״̬ʱ������������ð���ø�GPIO���š������Ǹߵ�ƽ����Iboot���ǵ͵�ƽ
//����IBOOT,��Ӳ��������

void IAP_GpioPinInit(void)
{
//   bb_rcc_apb2enr_iopcen=1;//ʹ��gpioc
//   pg_gpio_regc->CRH  =(pg_gpio_regc->CRH &(~(0xf <<((10-8)<<2))))
//                                  | ((CN_GPIO_MODE_IN_PULLUP & 0xf)<< ((10-8)<<2));
//   pg_gpio_regc->BSRR = 1<<10;//��������
//     GPIO_PowerOn(CN_GPIO_C);
//     GPIO_CfgPinFunc(CN_GPIO_C, 10, CN_GPIO_MODE_IN_PULLUP);
}

bool_t IAP_IsForceIboot(void)
{
//    u32 flag;
//    flag=pg_gpio_regc->IDR&(1<<10);
//    if(flag==0)
//        return false;
//    return true;
	return false;

}


bool_t IAP_IsRamIbootFlag(void)
{
    u8 i;
    char bootflag[8]="RunIboot";
    for(i=0;i<8;i++)
    {
        if(g_cIbootFlag[i]!=bootflag[i])
        {
            return false;
        }
    }
    return true;
}
