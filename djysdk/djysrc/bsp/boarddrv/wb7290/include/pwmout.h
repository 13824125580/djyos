// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��LED.H
// ģ������: led����ͷ�ļ�
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 10/19.2016
// =============================================================================

#ifndef __PWM_OUT_H__
#define __PWM_OUT_H__

#include <stdint.h>

#define PWM_OUT_0	1
#define PWM_OUT_1	3


void PWM_Config(u8 idx,uint32_t cycle,uint32_t duty);

#endif









