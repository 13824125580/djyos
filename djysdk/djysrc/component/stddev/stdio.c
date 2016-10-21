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
//����ģ�飺��׼IOģ��
//����:  ������.
//�汾��V1.0.0
//�ļ�����: ��׼�����������
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2015-09-11
//   ����:  ������.
//   �°汾��: V1.0.0
//   �޸�˵��: �����ļ�
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ring.h"
#include "object.h"
#include "driver.h"
#include "systime.h"

void NULL_Func(void){ }
struct Object g_tStdNotInit = {0};
s32 (* PutStrDirect)(const char *buf,u32 len) = (s32 (* )(const char *,u32 ))NULL_Func;
char (* GetCharDirect)(void) = (char (* )(void))NULL_Func;
#define CN_STDIN_DEV_BUFLIMIT   64  //���stdin���豸�������仺�����ߴ�
struct RingBuf *g_ptStdinDevBuf = NULL;

//StdNotInit��stdio.h�ж��壬�������˸�С���ɣ���stdin/out/err��һ������������
//ʵ�ʵ�FILEָ���ظ���ֵ��ָ������������ص㡣
FILE *stdin = StdNotInit;
FILE *stderr= StdNotInit;
FILE *stdout= StdNotInit;

s32 skip_atoi(const char **s)
{
    register s32 i=0;
    while (isdigit((s32)**s)) i = i*10 + *((*s)++) - '0';

    return i;
}

bool_t OpenStdin(const char *Name)
{
    stdin = fopen(Name,"r");
    if (stdin != NULL)
    {
        if (IS_DEV(stdin)) //Name��һ���豸��
        {
            if (g_ptStdinDevBuf == NULL)     //�豸���뻺������δ��ʼ��
            {
                g_ptStdinDevBuf = (struct RingBuf*)malloc(
                                    sizeof(struct RingBuf)+CN_STDIN_DEV_BUFLIMIT);
                Ring_Init(g_ptStdinDevBuf,(u8*)(g_ptStdinDevBuf+1),CN_STDIN_DEV_BUFLIMIT);
            }
            else
                Ring_Init(g_ptStdinDevBuf,(u8*)(g_ptStdinDevBuf+1),CN_STDIN_DEV_BUFLIMIT);
        }
        return true;
    }
    else
        return false;
}

bool_t OpenStdout(const char *Name)
{
    stdout = fopen(Name,"a");
    if (stdout != NULL)
    {
        return true;
    }
    else
        return false;
}
//todo ����������޸ĵİ�ȫ�ԡ�
bool_t OpenStderr(const char *Name)
{
    stderr = fopen(Name,"a");
    if (stderr != NULL)
    {
        return true;
    }
    else
        return false;
}

