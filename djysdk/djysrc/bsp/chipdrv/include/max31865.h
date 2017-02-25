// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��max31865.h
// ģ������: �¶ȼ��оƬmax31865������
// ģ��汾: V1.00
// ������Ա:
// ����ʱ��: 2/9.2017
// =============================================================================

#ifndef _CHIP_MAX31865_H_
#define _CHIP_MAX31865_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX31865_CONTINOUS_MODE      0		//��������ģʽ
#define MAX31865_SINGLE_MODE	 	 1		//���δ�������ģʽ

bool_t ModuleInstall_Max31865(char *BusName,u8 SampleMode);
float Max31865_TemperatureGet(void);

#ifdef __cplusplus
}
#endif

#endif





