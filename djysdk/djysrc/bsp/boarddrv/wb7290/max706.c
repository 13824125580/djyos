// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��brdwdt.c
// ģ������: SR3200�Ŀ��Ź���������ʼ����ι������
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 09/04.2015
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "djyos.h"
#include "cpu_peri.h"
#include "int.h"

// BOARD WDT
// =============================================================================
static const Pin wdt_pin[] = {
		{PIO_PA26, PIOA, ID_PIOA, PIO_OUTPUT_1,    PIO_DEFAULT}
};
#define WDT_FEED_CYCLE  (1600*1000) //1.6S
static u32 sBootDogFeedTime = 0;
// =============================================================================
// ���ܣ����Ͽ��Ź�ι������,max706��ι��ʱ����1.6s,�����ֲᣬι���Ǳ��ش���
// ��������
// ���أ���
// =============================================================================
bool_t BrdWdt_FeedDog(void)
{
    if( PIO_Get(wdt_pin) )
    {
        PIO_Clear(wdt_pin);
    }
    else
    {
        PIO_Set(wdt_pin);
    }

    return true;
}

bool_t BrdBoot_FeedEnd(void);
u32 RttFeedDog_Isr(ptu32_t intline)
{
	static u8 cnt = 0;

	if(RTT->RTT_SR & RTT_SR_RTTINC)
	{
		BrdWdt_FeedDog();
		if(cnt * 1000*1000 > sBootDogFeedTime)
		{
			BrdBoot_FeedEnd();
		}
		else
		{
			cnt ++;
		}
	}
	Int_ClearLine(intline);
	return 1;
}
bool_t BrdBoot_FeedStart(u32 bootfeedtime)
{
	u8 irqline = CN_INT_LINE_RTT;

	sBootDogFeedTime = bootfeedtime;

	PMC_EnablePeripheral(ID_RTT);
	RTT->RTT_MR = (32768 |  RTT_MR_RTTRST | RTT_MR_RTTINCIEN);

    Int_Register(irqline);
    Int_IsrConnect(irqline,RttFeedDog_Isr);
    Int_SettoReal(irqline);
    Int_ClearLine(irqline);
    Int_ContactLine(irqline);
	return true;
}

bool_t BrdBoot_FeedEnd(void)
{
	u8 irqline = CN_INT_LINE_RTT;

	RTT->RTT_MR = RTT_MR_RTTDIS;

    Int_CutLine(irqline);
    Int_IsrDisConnect(irqline);
    Int_UnRegister(irqline);
    PMC_DisablePeripheral(ID_RTT);
	return true;
}
// =============================================================================
// ���ܣ����Ͽ��Ź�оƬ��ʼ��
// ��������
// ���أ���
// =============================================================================
void ModuleInstall_BrdWdt(void)
{
	PIO_Configure(wdt_pin, PIO_LISTSIZE(wdt_pin));

    BrdWdt_FeedDog();

    WdtHal_RegisterWdtChip("Wdt_MAX706",WDT_FEED_CYCLE,BrdWdt_FeedDog,
    		BrdBoot_FeedStart,BrdBoot_FeedEnd);
}







