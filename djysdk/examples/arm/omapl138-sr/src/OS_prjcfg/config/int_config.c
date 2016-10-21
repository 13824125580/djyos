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
//����ģ��:��������
//���ߣ�lst
//�汾��V1.0.0
//�ļ�����: ���ļ������˾��幤�������õ��жϣ���ʹ���˵�ÿ���жϣ��������һ��
//      struct int_line�ṹ��������ڴ�Ƚϳ����ϵͳ��˵������Ϊ������������ϵ
//      ͳ����ϵͳ�����ж��߶����ϾͿ����ˡ�
//          �������ڴ�ǳ��������ж����ڶ�ĵ�Ƭ����˵�����ܸ��ж��Ƿ�ʹ�ã���
//      ����һ��struct int_line�ṹ�Ļ����ڴ��ʾ����ɽ��ʵ�ʹ��������õ�����
//      �����������࣬������ֻ��ʹ�õ��˵��ж�������һ�¾ͺá�
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 20130907
//   ����: lst
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------
#include "os_inc.h"
#include "cpu_peri.h"

const ufast_t tg_int_used[] =
{
    cn_int_line_commtx          ,
    cn_int_line_commrx          ,
    cn_int_line_nint            ,
    cn_int_line_pru_ev0         ,
    cn_int_line_pru_ev1         ,
    cn_int_line_pru_ev2         ,
    cn_int_line_pru_ev3         ,
    cn_int_line_pru_ev4         ,
    cn_int_line_pru_ev5         ,
    cn_int_line_pru_ev6         ,
    cn_int_line_pru_ev7         ,
    cn_int_line_edma3_0_cc0     ,
    cn_int_line_edma3_0_cc0_err ,
    cn_int_line_edma3_0_tc0_err ,
    cn_int_line_emifa           ,
    cn_int_line_iic0            ,
    cn_int_line_mmcsd0_int0     ,
    cn_int_line_mmcsd0_int1     ,
    cn_int_line_psc0_allint     ,
    cn_int_line_rtc_irqs10      ,
    cn_int_line_spi0            ,
    cn_int_line_t64p0_tint12    ,
    cn_int_line_t64p0_tint34    ,
    cn_int_line_t64p1_tint12    ,
    cn_int_line_t64p1_tint34    ,
    cn_int_line_uart0           ,
    cn_int_line_reserved1       ,
    cn_int_line_proterr         ,
    cn_int_line_sfg_chip0       ,
    cn_int_line_sfg_chip1       ,
    cn_int_line_sfg_chip2       ,
    cn_int_line_sfg_chip3       ,
    cn_int_line_edma3_0_tc1_err ,
    cn_int_line_emac_c0rxthresh ,
    cn_int_line_emac_c0rx       , 
    cn_int_line_emac_c0tx       ,
    cn_int_line_emac_c0misc     ,
    cn_int_line_emac_c1rxthresh ,
    cn_int_line_emac_c1rx       ,
    cn_int_line_emac_c1tx       ,
    cn_int_line_c1misc          ,
    cn_int_line_memerr          ,
    cn_int_line_gpiob0          ,
    cn_int_line_gpiob1          ,
    cn_int_line_gpiob2          ,
    cn_int_line_gpiob3          ,
    cn_int_line_gpiob4          ,
    cn_int_line_gpiob5          ,
    cn_int_line_gpiob6          ,
    cn_int_line_gpiob7          ,
    cn_int_line_gpiob8          ,
    cn_int_line_iic1            ,
    cn_int_line_lcdc            ,
    cn_int_line_uart1           ,
    cn_int_line_mcasp           ,
    cn_int_line_psc1_allint     ,
    cn_int_line_spi1            ,
    cn_int_line_uhpi_arm        ,
    cn_int_line_usb0            ,
    cn_int_line_usb1_hc         ,
    cn_int_line_usb1_r_wakeup   ,
    cn_int_line_uart2           ,
    cn_int_line_reserved2       ,
    cn_int_line_ehrpwm0         ,
    cn_int_line_ehrpwm0tz       ,
    cn_int_line_ehrpwm1         ,
    cn_int_line_ehrpwm1tz       ,
    cn_int_line_sata            ,
    cn_int_line_t64p2_all       ,
    cn_int_line_ecap0           ,
    cn_int_line_ecap1           ,
    cn_int_line_ecap2           ,
    cn_int_line_mmcad1_int0     ,
    cn_int_line_mmcsd1_int1     ,
    cn_int_line_t64p0_cmp0      ,
    cn_int_line_t64p0_cmp1      ,
    cn_int_line_t64p0_cmp2      ,
    cn_int_line_t64p0_cmp3      ,
    cn_int_line_t64p0_cmp4      ,
    cn_int_line_t64p0_cmp5      ,
    cn_int_line_t64p0_cmp6      ,
    cn_int_line_t64p0_cmp7      ,
    cn_int_line_t64p1_cmp0      ,
    cn_int_line_t64p1_cmp1      ,
    cn_int_line_t64p1_cmp2      ,
    cn_int_line_t64p1_cmp3      ,
    cn_int_line_t64p1_cmp4      ,
    cn_int_line_t64p1_cmp5      ,
    cn_int_line_t64p1_cmp6      ,
    cn_int_line_t64p1_cmp7      ,
    cn_int_line_armclkstopreq   ,
    cn_int_line_upp_all         ,
    cn_int_line_vpif_all        ,
    cn_int_line_edma3_1_cc0     ,
    cn_int_line_edma3_1_cc0_err ,
    cn_int_line_edma3_1_tc0_err ,
    cn_int_line_t64p3_all       ,
    cn_int_line_mcbsp0_r        ,
    cn_int_line_mcbsp0_x        ,
    cn_int_line_mcbsp1_r        ,
    cn_int_line_mcbsp1_x        
};

const ufast_t ufg_int_used_num = sizeof(tg_int_used)/sizeof(ufast_t);
//�����ж��߿������ݽṹ
struct tagIntLine tg_int_table[sizeof(tg_int_used)/sizeof(ufast_t)];


