

#include "stddef.h"
#include <stdint.h>
#include <stdio.h>
#include <djyos.h>



extern u32 g_bRunModeFlag;

void Led1_Init(void)
{
	volatile u32 *gpbcon = (u32*)0x56000010;
	*gpbcon &= ~(3<<10);
	*gpbcon |= (1<<10);
}

void Led1_On(void)
{
	volatile u32 *gpbdat = (u32*)0x56000014;
	*gpbdat &= ~(1<<5);
}

void Led1_Off(void)
{
	volatile u32 *gpbdat = (u32*)0x56000014;
	*gpbdat |= (1<<5);
}

#include <IAP.h>
#include <cfg/Iboot_config.h>
// iboot needed function below
// iboot ����Ҫ�õ������ų�ʼ��
void IAP_GpioPinInit(void)
{
	//tq2416 ��key1���ϵ�ʱ�����Ƿ�ǿ�ƽ���iboot
	//������key1���ϵ磬��ǿ�ƽ���ibootģʽ
	volatile u32 *gpfcon = (u32*)0x56000050;

	*gpfcon &= ~(3<<14);
	*gpfcon |= (0<<14);			//input
}
// �Ƿ���Ӳ��ǿ�ƣ���ĳ�����ߣ�����iboot
bool_t IAP_IsForceIboot(void)
{
	volatile u32 *gpfdat = (u32*)0x56000054;
	u32 delay = 1000,i;

	IAP_GpioPinInit();
	for(i = 0; i < delay; i++)
	{

	}

	if(!(*gpfdat & (1<<7)))		//����ʱ��Ϊ�͵�ƽ��������Ϊ�ߵ�ƽ
	{
		return true;
	}
	return false;
}

ptu32_t djy_main(void)
{
	IAP_LoadAPPFromFile();

	extern void Sh_GetStatus(char *param);
	extern void Sh_GetRunMode(char *param);
	Sh_GetRunMode(NULL);
	Sh_GetStatus(NULL);
	printf(">>\r\n");
	Led1_Init();
	while(1)
	{
		Led1_On();
		Djy_EventDelay(1000000);
		Led1_Off();
		Djy_EventDelay(1000000);
	}
}

