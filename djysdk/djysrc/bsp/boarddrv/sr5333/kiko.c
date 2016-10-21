// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��KiKo.c
// ģ������: ���뿪������
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 10/05.2016
// =============================================================================

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "os.h"
#include "cpu_peri.h"
#include "kiko.h"

// BITλ����
#define DB0                 (0x01)
#define DB1                 (0x02)
#define DB2                 (0x04)
#define DB3                 (0x08)
#define DB4                 (0x10)
#define DB5                 (0x20)
#define DB6                 (0x40)
#define DB7                 (0x80)
#define DB8                 (0x100)
#define DB9                 (0x200)
#define DB10                (0x400)
#define DB11                (0x800)
#define DB12                (0x1000)
#define DB13                (0x2000)
#define DB14                (0x4000)
#define DB15                (0x8000)
#define DB16                (0x10000)
#define DB17                (0x20000)
#define DB18                (0x40000)
#define DB19                (0x80000)
#define DB20                (0x100000)
#define DB21                (0x200000)
#define DB22                (0x400000)
#define DB23                (0x800000)
#define DB24                (0x1000000)
#define DB25                (0x2000000)
#define DB26                (0x4000000)
#define DB27                (0x8000000)
#define DB28                (0x10000000)
#define DB29                (0x20000000)
#define DB30                (0x40000000)
#define DB31                (0x80000000)

static const u32 Bits[]={
		DB0,
		DB1,
		DB2,
		DB3,
		DB4,
		DB5,
		DB6,
		DB7
};

#define KO1           {PIO_PD10, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define KO2           {PIO_PD11, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define KO3           {PIO_PD13, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_LED1		{PIO_PD31, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_LED2		{PIO_PD17, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_LED3		{PIO_PD29, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_LED4		{PIO_PA9, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}


#define KI1       {PIO_PA23, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define KI2       {PIO_PA18, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define KI3       {PIO_PA25, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define KI4       {PIO_PE4,  PIOE, ID_PIOE, PIO_INPUT, PIO_DEFAULT}
#define KI5       {PIO_PA0,  PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define EJT_KI    {PIO_PE0,  PIOE, ID_PIOE, PIO_INPUT, PIO_DEFAULT}
#define HES_KI    {PIO_PE3,  PIOE, ID_PIOE, PIO_INPUT, PIO_DEFAULT}

#define RUN_DBG_LED  {PIO_PA27, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOOTMODE_LED {PIO_PA28, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define RUN_LED {PIO_PA20,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_DEFAULT}





static const Pin KI_Pins[] = {
		KI1,
		KI2,
		KI3,
		KI4,
		KI5,
		EJT_KI,
		HES_KI
};

static const Pin KO_Pins[] = {
		KO1,
		KO2,
		KO3
};

static const Pin LED_KO_Pins[] = {
		CPU_LED1,
		CPU_LED2,
		CPU_LED3,
		CPU_LED4,
};

static const Pin CPU_CTRL_LED_Pins[]={
		RUN_LED,
		RUN_DBG_LED,
		BOOTMODE_LED
};
static const Pin EN_KO_Pin={PIO_PA29, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT};


#define MAX_KO_NUM   3     //��󿪳���
#define MAX_KI_NUM   7      //�������
#define MAX_LED_NUM  4

// =========================================================================
// �������ܣ��������ų�ʼ��
// �����������
// �����������
// ����ֵ  ����
// =========================================================================
void KO_Init(void)
{
    PIO_Configure(KO_Pins,  PIO_LISTSIZE(KO_Pins));
    PIO_Configure(LED_KO_Pins, PIO_LISTSIZE(LED_KO_Pins));
    PIO_Configure(&EN_KO_Pin, PIO_LISTSIZE(EN_KO_Pin));
    PIO_Configure(CPU_CTRL_LED_Pins, PIO_LISTSIZE(CPU_CTRL_LED_Pins));
}

// =========================================================================
// �������ܣ�д����ֵ������ֵ�ӵ͵�����һ��32λ����������
// �����������
// �����������
// ����ֵ  ����
// =========================================================================
void KO_Write(u8 dwKoValue)
{
	u8 i;
	for(i=0;i<MAX_KO_NUM;i++)
	{
       if(dwKoValue&Bits[i])
       {
    	   PIO_Set(&KO_Pins[i]);
       }
       else
       {
    	   PIO_Clear(&KO_Pins[i]);
       }
	}
}

// =========================================================================
// �������ܣ�CPU��led����
// ����������� �ر�
// �����������
// ����ֵ  ����
// =========================================================================
void Led_KO_Ctrl(u8 idx,u8 flag)
{
	if( flag )
	{
		PIO_Set(&LED_KO_Pins[idx]);
	}
	else
	{
		PIO_Clear(&LED_KO_Pins[idx]);
	}
}

// =========================================================================
// �������ܣ���ʼ��IO����
// ���������
// ���������
// ����ֵ  ��
// =========================================================================
void KI_Init(void)
{
    PIO_Configure (KI_Pins, PIO_LISTSIZE(KI_Pins));   //���ߺ�Ƭѡ��ʼ��
}

// =============================================================================
// ���ܣ�����Ӧ�ۺŵĿ���ֵ
// ������SlotNo,�ۺ�
// ���أ������Ŀ���ֵ������32bit�ĵ�8λ
// =============================================================================
void KI_Read(u8 *buf)
{
	u8 i;
	u8 temp=0;
	u8 Buf=0;

	for(i=0;i<MAX_KI_NUM;i++)
	{
		temp=PIO_Get(&KI_Pins[i]);
		temp=temp<<i;
		Buf|=temp;
	}

	*buf=Buf;
}

// =============================================================================
// ���ܣ�RUN LED���ƣ�������Ϩ��
// ������flag,1������0,Ϩ��.
// ���أ���.
// =============================================================================
void CPU_Led_KO_Ctrl(u8 idx, u8 flag)
{
	if( flag )
	{
		PIO_Set(&CPU_CTRL_LED_Pins[idx]);
	}
	else
	{
		PIO_Clear(&CPU_CTRL_LED_Pins[idx]);
	}
}

// =========================================================================
// �������ܣ�����ʹ�ܡ�
// ����������� �ر�
// �����������
// ����ֵ  ����
// =========================================================================
void KO_StartRly(u8 flag)
{
	if( flag )
	{
		PIO_Set(&EN_KO_Pin);
	}
	else
	{
		PIO_Clear(&EN_KO_Pin);
	}
}
