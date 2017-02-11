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
// �ļ���     ��sysinit.c
// ģ������: ϵͳ��ʼ�����֣���Ҫ���ں�������صĳ�ʼ������ʱ�ӣ����ߵ�
// ģ��汾:
// ������Ա:
// ����ʱ��:
// =============================================================================

#include "stdint.h"
#include "cpu_peri.h"
#include "board-config.h"

// ===================== PLL��������ĺ궨�� =====================================
//PLL_N:��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
//PLL_M:��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//PLL_P:ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
//PLL_Q:USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.
//VCOƵ��=PLL*(PLL_N/PLL_M);
//ϵͳʱ��Ƶ��=VCOƵ��/PLL_P=PLL*(PLL_N/(PLL_M*PLL_P));
//USB,SDIO,RNG�ȵ�ʱ��Ƶ��=VCOƵ��/PLL_Q=PLL*(PLL_N/(PLL_M*PLL_Q));
// =============================================================================
/*����  VCOƵ��=432Mhz ϵͳʱ��Ƶ��=216Mhz USB,SDIO,RNG�ȵ�ʱ��Ƶ��=48Mhz*/
#define PLL_N      ((CN_CFG_MCLK/Mhz)*2)

#define PLL_M      25//��Ƶϵ�������ⲿ����

#define PLL_P      2
#define PLL_Q      9

#define PLLSAI_N      192
#define PLLSAI_R      5
#define PLLSAI_P      2
#define PLLSAI_Q      2
//0 1 2 3��Ӧ2 4 8 16��Ƶ
#define PLLSAIDIVR    0
// =============================================================================
// ���ܣ��ú���ʵ��ϵͳʱ�ӵĳ�ʼ������Ҫ������1��ϵͳʱ�Ӵ��ڲ�ʱ���л����ⲿʱ�ӣ�2������
//       HCLK��PCLK1��PCLK2��MCLK��Ƶϵ����3����PLLΪϵͳʱ��
//       ��������ʱ�����ã�������board-config.h�е�CN_CFG_MCLK�ȳ�������һ�¡�
// ��������
// ���أ�true false
// =============================================================================
bool_t SysClockInit(void)
{

//��Ӧ�Ĵ�����ʼ��
    RCC->CR|=0x00000001;//�����ڲ�ʱ��ʹ��
    RCC->CFGR=0x00000000;
    RCC->CR&=0xFEF6FFFF;
    RCC->PLLCFGR=0x24003010;
    RCC->CR&=~(1<<18);
    RCC->CIR=0x00000000;//��ֹʱ���ж�
    RCC->CR|=1<<16;//��HSE
    while((RCC->CR&(1<<17))==0);

//����PLL PLLSAI
    RCC->APB1ENR|=1<<28;    //��Դ�ӿ�ʱ��ʹ��
    PWR->CR1|=3<<14;        //�л���������ģʽ
    RCC->PLLCFGR=PLL_M|(PLL_N<<6)|(((PLL_P>>1)-1)<<16)|(PLL_Q<<24)|(1<<22);//������PLL
    RCC->PLLSAICFGR=(PLLSAI_R<<28)|(PLLSAI_Q<<24)|(((PLL_P>>1)-1)<<16)|( PLLSAI_N<<6); //������SAI
    RCC->DCKCFGR1 |=(PLLSAIDIVR <<16);
    RCC->CR |=(1<<24);//�ر���PLL

//�л���������ģʽ
    PWR->CR1|=1<<16;        //ʹ�ܹ�����,Ƶ�ʿɵ�216Mhz
    while(0==(PWR->CSR1&(1<<16)));
    PWR->CR1|=1<<17;        //ʹ�ܹ������л�
    while((PWR->CSR1&(1<<17))==0);

//FLASH ʱ������ APB1 APB2��Ƶϵ��
    FLASH->ACR|=7<<0;       //7��CPU�ȴ�����.
    FLASH->ACR|=1<<8;       //ָ��Ԥȡʹ��.
    FLASH->ACR|=1<<9;       //ʹ��ART Accelerator
    RCC->CFGR|=(0<<4)|(5<<10)|(4<<13);//HCLK ����Ƶ;APB1 4��Ƶ;APB2 2��Ƶ.

//�л�ϵͳʱ��
    RCC->CR|=1<<24;         //����PLL
    while((RCC->CR&(1<<25))==0);//�ȴ�PLL׼����
    RCC->CFGR&=~(3<<0);     //����
    RCC->CFGR|=2<<0;        //ѡ����PLL��Ϊϵͳʱ��
    while((RCC->CFGR&(3<<2))!=(2<<2));//�ȴ���PLL��Ϊϵͳʱ�ӳɹ�.
    return true;
}

// =============================================================================
// ���ܣ�Ƭ��SRAM��ʼ��������GPIO��FMC�������ĳ�ʼ��
// ��������
// ���أ���
// =============================================================================
static void SRAM_GpioInit(void)
{
    GPIO_PowerOn(GPIO_D);//ʹ��P-DEFGHIʱ��
    GPIO_PowerOn(GPIO_E);
    GPIO_PowerOn(GPIO_F);
    GPIO_PowerOn(GPIO_G);
    GPIO_PowerOn(GPIO_H);
    GPIO_PowerOn(GPIO_I);
//��ʼ������IO
    GPIO_CfgPinFunc(GPIO_D,
            PIN0|PIN1|PIN4|PIN5|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PC0/2/3

    GPIO_CfgPinFunc(GPIO_E,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);     //PD0/1/8/9/10/14/15

    GPIO_CfgPinFunc(GPIO_F,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PE0/1/7~15

    GPIO_CfgPinFunc(GPIO_G,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN10,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);     //PG0~5/11~15

    GPIO_CfgPinFunc(GPIO_H,
            PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PC0/2/3

    GPIO_CfgPinFunc(GPIO_I,
            PIN0|PIN1|PIN3|PIN6|PIN7|PIN9|PIN10,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE); //PF0~2/4/5/8/15
//////////////////////////////////////
    GPIO_AFSet(GPIO_D,
            PIN0|PIN1|PIN4|PIN5|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);            //PC0/2/3

    GPIO_AFSet(GPIO_E,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);        //PD0/1/8/9/10/14/15

    GPIO_AFSet(GPIO_F,
                    PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN12|PIN13|PIN14|PIN15,AF12);            //PE0/1/7~15

    GPIO_AFSet(GPIO_G,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN10,AF12);      //PG0~5/11~15

    GPIO_AFSet(GPIO_H,
            PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);            //PC0/2/3

    GPIO_AFSet(GPIO_I,
            PIN0|PIN1|PIN3|PIN6|PIN7|PIN9|PIN10,AF12);  //PF0~2/4/5/8/15
//����I/o


}
static void SRAM_FmcInit(void)
{

    RCC->AHB3ENR|=1<<0;         //ʹ��FMCʱ��
      /* Configure and enable Bank1_SRAM3��NE3 */
    //bank1��NE1~4,ÿһ����һ��BCR+TCR�������ܹ��˸��Ĵ�����

    FMC_Bank1->BTCR[4]=0X00000000;
    FMC_Bank1->BTCR[5]=0X00000000;
    FMC_Bank1E->BWTR[4]=0X00000000;
    //����BCR�Ĵ��� ʹ���첽ģʽ,ģʽA(��д����һ��ʱ��Ĵ���)
    //BTCR[ż��]:BCR�Ĵ���;BTCR[����]:BTR�Ĵ���
    FMC_Bank1->BTCR[4]|=1<<12;//�洢��дʹ��
    FMC_Bank1->BTCR[4]|=1<<4; //�洢�����ݿ��Ϊ16bit
    //����BTR�Ĵ���         ��HCLK=216M/2, 1��HCLK=2*4.629ns
    FMC_Bank1->BTCR[5]|=8<<5; //���ݱ���ʱ�䣨DATAST��
    FMC_Bank1->BTCR[5]|=0<<3; //��ַ����ʱ�䣨ADDHLD��δ�õ�
    FMC_Bank1->BTCR[5]|=6<<0; //��ַ����ʱ�䣨ADDSET��Ϊ0��HCLK 0ns
    //����дʱ��Ĵ���
    FMC_Bank1E->BWTR[4]=0x0FFFFFFF;//Ĭ��ֵ
    //ʹ��BANK1����3
    FMC_Bank1->BTCR[4]|=1<<0;
}

//SDRAM��ʼ��
void SRAM_Init(void)
{
    SRAM_GpioInit();
    SRAM_FmcInit();

}


