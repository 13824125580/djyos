
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
// �����Ȩ�����ʹ���߷������¶������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�����������У���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�����������У��Լ�����
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
// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// ģ������: main.c
// ģ��汾: V1.00
// ������Ա: Administrator
// ����ʱ��: 9:50:10 AM/Aug 15, 2014
// =============================================================================
#include "djyos.h"
#include "gkernel.h"
#include "cpu_peri.h"
#include "systime.h"

const unsigned long led_mask[] = { 1UL<<28, 1UL<<29, 1UL<<31, 1UL<< 2,
                                   1UL<< 3, 1UL<< 4, 1UL<< 5, 1UL<< 6 };

u16 ledofftask;
u16 ledontask;
u8 flag = 0;
void LED_Init (void)
{
    LPC_SC->PCONP     |= (1 << 15);            // enable power to GPIO & IOCON

    LPC_GPIO1->FIODIR |= 0xB0000000;           // LEDs on PORT1 are output
    LPC_GPIO2->FIODIR |= 0x0000007C;           // LEDs on PORT2 are output
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN |=  led_mask[num];
  else         LPC_GPIO2->FIOPIN |=  led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN &= ~led_mask[num];
  else         LPC_GPIO2->FIOPIN &= ~led_mask[num];
}

ptu32_t LedAllOn(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_On(i);
            Djy_EventDelay(100*mS);
        }
        Djy_EventPop(ledofftask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}
ptu32_t LedAllOff(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_Off(i);
            Djy_EventDelay(100*mS);
        }
        Djy_EventPop(ledontask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}

void Led_Task(void)
{
    LED_Init();

    ledontask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOn,NULL,0x300,"LedAllOn function");
    if(ledontask == CN_EVTT_ID_INVALID)
    {
        printk("ledontask create failed!\r\n");
    }

    ledofftask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOff,NULL,0x300,"LedAllOff function");
    if(ledofftask == CN_EVTT_ID_INVALID)
    {
        printk("ledofftask create failed!\r\n");
    }
    Djy_EventPop(ledontask,NULL,0,NULL,0,200);

}
extern u32 g_bRunModeFlag;
void djy_main(void)
{
	Led_Task();
	g_bRunModeFlag=0x12345678;
	printf("Run Mode:Iboot.\r\n");
//	Timer_Test();
//	mcb1700_display();
    while(1)
    {
    	Djy_EventDelay(500*mS);
//    	GDD_Demo();
    }
}

