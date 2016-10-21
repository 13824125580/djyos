// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��AD7606.c
// ģ������: AD7606�ɼ�ģ��ĵײ�������������SPI�ӿ�ͨ�ź�����ȡAD�ɼ�ֵ
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 09/04.2015
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "djyos.h"
#include "spibus.h"
#include "cpu_peri.h"
#include "iodefine.h"
// =============================================================================
#define AD7606_SSP_BAUD  10000000       //AD7606��SSP�����ٶ�
#define AD7606_SSP_TIMEOUT (5*1000)     //ad7606 ��ʱ����Ϊ5mS

struct SPI_Device s_AD7606_Dev;

// =============================================================================
// ���ܣ�AD7606��CPU�ӿں���������SPIͨ�ţ�Ӳ��ע����������Ƭѡ
// ������wBuf,д������
//      wLen,д���ݳ��ȣ��ֽڵ�λ
//      rBuf,��������
//      rLen,���ֽ���
// ���أ���
// =============================================================================
void __AD7606_SpiInterfalce(u8* wBuf,u16 wLen,u8* rBuf,u16 rLen)
{
    struct SPI_DataFrame frame;

    frame.RecvBuf = rBuf;
    frame.RecvLen = rLen;
    frame.RecvOff = wLen;
    frame.SendBuf = wBuf;
    frame.SendLen = wLen;

    GPIO_SetToLow( ADCS_PORT, (1 << ADCS_PIN));     // ����Ƭѡ�ź�
    SPI_Transfer(&s_AD7606_Dev,&frame,1,AD7606_SSP_TIMEOUT);
    GPIO_SetToHigh( ADCS_PORT, (1 << ADCS_PIN));        // ����Ƭѡ�ź�
}
// =============================================================================
// ���ܣ���ʼ��AD7606�������������úͳ�ʼ�����ã��ӿڵĳ�ʼ����
// ��������
// ���أ���
// =============================================================================
void AD7606_Init(void)
{
    //����AD7606�Ƿ���ù�����
    GPIO_CfgPinFunc(ADOS0_PORT,ADOS0_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADOS1_PORT,ADOS1_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADOS2_PORT,ADOS2_PIN,CN_GPIO_MODE_OD_PULLUP);

	//�رչ���������000
	GPIO_SetToOut(ADOS0_PORT,(1 << ADOS0_PIN));
	GPIO_SetToOut(ADOS1_PORT,(1 << ADOS1_PIN));
	GPIO_SetToOut(ADOS2_PORT,(1 << ADOS2_PIN));

	GPIO_SetToLow(ADOS2_PORT, (1 << ADOS2_PIN));
	GPIO_SetToLow(ADOS1_PORT, (1 << ADOS1_PIN));
	GPIO_SetToLow(ADOS0_PORT, (1 << ADOS0_PIN));

    //GPIO��ʼ��������RST��SH��BUSY���������������
    GPIO_CfgPinFunc(ADRST_PORT,ADRST_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADCONVS_PORT,ADCONVS_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADBUSY_PORT,ADBUSY_PIN,CN_GPIO_MODE_OD_PULLUP);

    GPIO_SetToOut(ADRST_PORT,(1 << ADRST_PIN));         //��ʼ������RST״̬
    GPIO_SetToHigh(ADRST_PORT, (1 << ADRST_PIN));

    GPIO_SetToOut( ADCONVS_PORT, (1 << ADCONVS_PIN));       //����=0������ADת��
    GPIO_SetToLow( ADCONVS_PORT, (1 << ADCONVS_PIN));

    GPIO_SetToIn( ADBUSY_PORT, (1 << ADBUSY_PIN));      //BUSY�ܽ�Ϊ����

    GPIO_SetToLow(ADRST_PORT,(1 << ADRST_PIN));

    // ���³�ʼ����SSP���
    //CS��ʼ��
    GPIO_CfgPinFunc(ADCS_PORT,ADCS_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_SetToOut(ADCS_PORT,(1 << ADCS_PIN));

    //SSP��������ʼ��
    s_AD7606_Dev.Cs = 1;
    s_AD7606_Dev.AutoCs = false;
    s_AD7606_Dev.CharLen = 8;
    s_AD7606_Dev.Mode = SPI_MODE_3;
    s_AD7606_Dev.ShiftDir = SPI_SHIFT_MSB;
    s_AD7606_Dev.Freq = AD7606_SSP_BAUD;

	if(NULL != SPI_DevAdd_s(&s_AD7606_Dev,"SSP0","SPI_Dev_AD7606"))
	{
		SPI_BusCtrl(&s_AD7606_Dev,CN_SPI_SET_POLL,0,0);
		SPI_BusCtrl(&s_AD7606_Dev,CN_SPI_SET_CLK,AD7606_SSP_BAUD,0);
	}
}

// =============================================================================
// ���ܣ�����AD��ʼ������ͨ������IO���ŵĸߵ͵�ƽ�����ж�BUSYλ
// ��������
// ���أ���
// =============================================================================
void __AD7606_Start(void)
{
    GPIO_SetToHigh( ADCONVS_PORT, (1 << ADCONVS_PIN));
    while( GPIO_DataGet( ADBUSY_PORT) & (1 << ADBUSY_PIN));
}

// =============================================================================
// ���ܣ�ֹͣADת����ͨ������IO��ƽ�ķ�ʽ����
// ��������
// ���أ���
// =============================================================================
void __AD7606_Stop(void)
{
    GPIO_SetToLow( ADCONVS_PORT, (1 << ADCONVS_PIN));
}

// =============================================================================
// ���ܣ��ж�AD7606�Ƿ��Ѿ�ת��������AD7606�����ù�������ʱ��Ϊ4us��64�����Ĺ��������ʱ��
//      Ϊ315us
// ��������
// ���أ���
// =============================================================================
static bool_t _AD7606_ConvFinished(void)
{
    u32 timeout = 500;
    u32 data;

	Djy_DelayUs(4);			//��������������ʱ����315uS

    while(1)
    {
        data = GPIO_DataGet(ADBUSY_PORT);
        if( (!(data & (1 << ADBUSY_PIN))) || (timeout == 0) )
            break;
        Djy_DelayUs(1);
        timeout--;
    }
    if(timeout)
        return true;
    else
        return false;
}

// =============================================================================
// ���ܣ���ȡAD7606�Ĳɼ�ֵ����������ת����ת���������ٶ�ȡADת����ֵ
// ��������
// ���أ���
// =============================================================================
bool_t AD7606_GetValue(uint16_t *AinBuf)
{
    bool_t result = false;
    u8 *pAinBuf = (u8*)AinBuf;
    u8 temp,i;

    __AD7606_Start();
    if(_AD7606_ConvFinished())
    {
        __AD7606_SpiInterfalce(NULL,0,pAinBuf,16);
        result = true;
    }
    __AD7606_Stop();
    if(result == true)
    {
        if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)   //��С��ת��
        {
            for(i = 0; i < 8; i ++)
            {
                temp = pAinBuf[2*i];
                pAinBuf[2*i] = pAinBuf[2*i + 1];
                pAinBuf[2*i + 1] = temp;
            }
        }
    }

    return result;
}
