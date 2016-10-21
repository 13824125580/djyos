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
// ģ������: timer��Ӳ���ӿ�
// ģ��汾: V1.00
// ������Ա: zhangqf_cyg
// ����ʱ��: 1:53:42 PM/Mar 4, 2014
// =============================================================================
// �����޸ļ�¼(���µķ�����ǰ��):
// <�汾��> <�޸�����>, <�޸���Ա>: <�޸Ĺ��ܸ���>
// =============================================================================
// ��ע�����ļ���������ʹ�ö�ʱ��������һ��ͳһ�Ľӿڣ�ʵ�ֻ�����timer_driver.c�ļ�
#include "timer_hard.h"
#include "stddef.h"
static struct TimerChip   s_tTimerHardChip = {NULL};//ע���оƬ����
// =============================================================================
// �������ܣ�TimerHard_RegisterChip
//          ע�ᶨʱ��оƬ��ϵͳ��ʱ��ģ��
// �������:timerchip,��ʱ��оƬ
// �����������
// ����ֵ  ��true �ɹ� falseʧ�ܣ�����ʵ��
// =============================================================================
bool_t  TimerHard_RegisterChip(struct TimerChip *timerchip)
{
    if(NULL != timerchip)
    {
        //�������帳ֵ����ʱmemcpyû�м��أ����帳ֵ���ӵ���memcpy��
        s_tTimerHardChip.chipname  = timerchip->chipname;
        s_tTimerHardChip.TimerHardAlloc = timerchip->TimerHardAlloc;
        s_tTimerHardChip.TimerHardFree =  timerchip->TimerHardFree;
        s_tTimerHardChip.TimerHardGetFreq = timerchip->TimerHardGetFreq;
        s_tTimerHardChip.TimerHardCtrl = timerchip->TimerHardCtrl;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// �������ܣ�TimerHard_UnRegisterChip
//          ��ʱ��оƬע��
// �����������
// �����������
// ����ֵ  ��true �ɹ� falseʧ��
// ˵��    :Ŀǰ������û���ã���Ҫ��register֮���Ȼ��unregister������ʵ��
// =============================================================================
bool_t TimerHard_UnRegisterChip(void)
{
    s_tTimerHardChip.TimerHardAlloc = NULL;
    s_tTimerHardChip.TimerHardFree = NULL;
    s_tTimerHardChip.TimerHardGetFreq = NULL;
    s_tTimerHardChip.TimerHardCtrl = NULL;
    return true;
}
// =============================================================================
// �������ܣ�TimerHard_Alloc
//          HAL��ʱ������
// ���������
//     timerisr,����Ķ�ʱ�����жϷ�����,�ж��е���
// �����������
// ����ֵ  ��-1 = ���䲻�ɹ������򷵻ض�ʱ��������þ���Ľṹ�ɶ�ʱ��оƬ�Լ�����
// ˵��    :��һ����Ըշ���Ķ�ʱ�������ʱֹͣ���жϽ�ֹ������оƬ����ʵ��
//        �տ�ʼ����Ķ�ʱ��Ӧ���Ǹ������Զ��رյģ�������Ա����Լ���������
//        Ĭ�ϣ�ֹͣ�������첽�жϣ�reload,�жϽ�ֹ
// =============================================================================
ptu32_t TimerHard_Alloc(fntTimerIsr timerisr)
{
    if(NULL == s_tTimerHardChip.TimerHardAlloc)
    {
        return 0;
    }
    else
    {
        return s_tTimerHardChip.TimerHardAlloc(timerisr);
    }
}

// =============================================================================
// �������ܣ�TimerHard_Free
//          HAL��ʱ���ͷ�
// ���������timerhandle�����ͷŶ�ʱ��
// �����������
// ����ֵ  ��true �ɹ� falseʧ�� 
// ˵��    : ����оƬ����ʵ��
// =============================================================================
bool_t  TimerHard_Free(ptu32_t timerhandle)
{
    if(NULL == s_tTimerHardChip.TimerHardFree)
    {
        return false;
    }
    else
    {
        return s_tTimerHardChip.TimerHardFree(timerhandle);
    }
}

// =============================================================================
// �������ܣ�ȥ��ʱ��ʱ��ԴƵ��
// ���������timerhandle�����ͷŶ�ʱ��
// �����������
// ����ֵ  ��ʱ��ԴƵ�ʣ�Hz 
// ˵��    : ����оƬ����ʵ��
// =============================================================================
u32  TimerHard_GetFreq(ptu32_t timerhandle)
{
    if(NULL == s_tTimerHardChip.TimerHardGetFreq)
    {
        return false;
    }
    else
    {
        return s_tTimerHardChip.TimerHardGetFreq(timerhandle);
    }
}

// =============================================================================
// �������ܣ�TimerHard_Ctrl
//          ����Ӳ����ʱ��
// ���������timerhard�������ö�ʱ���� 
//          ctrlcmd, ��������μ�fnTimerHardCtrl����������ע��
// ���������inoutpara������������������ݲ�ͬ���������
// ����ֵ  ��true �����ɹ� false����ʧ�� 
// ˵����ctrlcmd��Ӧ��inoutpara�����Զ���˵��
// =============================================================================
bool_t TimerHard_Ctrl(ptu32_t timerhandle, \
                      enum TimerCmdCode ctrlcmd, \
                      ptu32_t inoutpara)
{
    bool_t result;
    if(NULL == s_tTimerHardChip.TimerHardCtrl)
    {
        result = false;
    }
    else
    {
        result = s_tTimerHardChip.TimerHardCtrl(timerhandle, ctrlcmd, inoutpara);
    }
    return result;
}
