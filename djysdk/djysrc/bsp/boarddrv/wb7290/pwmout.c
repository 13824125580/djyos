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
// �ļ���     ��cpu_peri_pwm.c
// ģ������: PWM����
// ģ��汾: V1.00
// ������Ա: zhb
// ����ʱ��: 8/24.2016
// =============================================================================

#include "string.h"
#include "cpu_peri.h"
#include "pwmout.h"

static const Pin pinPwm[] = {
		{PIO_PD25A_PWMC0_PWML1, PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA15C_PWMC0_PWML3, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT} };
// =========================================================================
// �������ܣ�����PWM
// ���������idx,PWM���,������·�����Ϊ0��1.
//          cycle,����,��λus
//          duty,ռ�ձ�,25%��ռ�ձȣ�ֱ��25����
// �����������
// ����ֵ  ����
// =========================================================================
void PWM_Config(u8 idx,uint32_t cycle,uint32_t duty)
{
	uint32_t temp;

	PIO_Configure(pinPwm,2);

	if(idx > 3)
		return ;
	PMC_EnablePeripheral(ID_PWM0);

	PWM0->PWM_DIS = (0x01<<idx);

	PWM0->PWM_CLK = PWM_CLK_DIVA(75) | PWM_CLK_PREA_CLK_DIV2;
	PWM0->PWM_CH_NUM[idx].PWM_CMR = PWM_CMR_CPRE_CLKA;

	if( (PWM0->PWM_WPSR & PWM_WPSR_WPSWS3) | (PWM0->PWM_WPSR & PWM_WPSR_WPHWS3) )
	{

	}
	{
		temp = (uint32_t)(1 * cycle);
		PWM0->PWM_CH_NUM[idx].PWM_CPRD = PWM_CPRD_CPRD(temp);
	}

	temp = temp * duty / 100;
	PWM0->PWM_CH_NUM[idx].PWM_CDTY = PWM_CDTY_CDTY(temp);

	PWM0->PWM_ENA = (0x01<<idx);
}

