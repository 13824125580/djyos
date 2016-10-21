//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// �����Ȩ�����ʹ���߷��������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������
// 3. �������Ϊ�����߲���ϵͳ����ɲ��֣�δ����ǰȡ�õ�������ɣ���������ֲ����
//    �����߲���ϵͳ���������С�

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------

//����ģ��: gui
//����:  ������.
//�汾��V1.0.0
//�ļ�����: gui kernel���岿��
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2009-11-21
//   ����:  ������.
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------
#ifndef __GK_WIN_H__
#define __GK_WIN_H__
#include "errno.h"
#include "ring.h"
#ifdef __cplusplus
extern "C" {
#endif

struct GkscParaCreateDesktop;
struct GkscParaCreateGkwin;
struct GkscParaSetRopCode;
struct GkscParaSetPatBuf;
struct GkscParaSetPrio;
struct GkscParaSetHyalineColor;
struct GkscParaSetBitmsk;
struct GkscParaDestroyWin;
struct GkscParaSetBoundMode;
struct GkscParaMoveWin;
struct GkscParaChangeWinArea;
struct GkscParaSetDirectScreen;
struct GkscParaUnsetDirectScreen;

struct DisplayRsc;

//�û�������Ϣ���в�������
#define CN_USERCALL_MSGQ_SIZE   16  //�û�������Ϣ�������ɵ������Ϣ����
#define CN_USERCALL_MSG_SIZE    64  //�û�������Ϣ���е�ÿ����Ϣ����ֽ���

struct GkChunnel       //�û�����(ͨ����gui windows) �� gui kernel��ͨ�Źܵ�
{
    struct RingBuf ring_syscall;         //gui ϵͳ���û��λ�����
    u8 *syscall_buf;                        //Ϊϵͳ���û��λ����������ڴ�

    struct MutexLCB *syscall_mutex;      //������,���ڷ�ֹ����д
    struct SemaphoreLCB *syscall_semp;   //�ź�����дʱ�������������Ⱥ�

    struct SemaphoreLCB *usercall_semp;  //�ź�����дʱ�������������Ⱥ�
    struct MsgQueue *usercall_msgq;      //usercall��Ϣ����

};

ptu32_t ModuleInstall_GK(ptu32_t para);
struct GkWinRsc *__GK_GetZsectionStart(struct GkWinRsc *gkwin);
struct GkWinRsc *__GK_GetZsectionEnd(struct GkWinRsc *gkwin);
struct GkWinRsc *__GK_GetZsectionAllStart(
                                                    struct GkWinRsc *gkwin);
struct GkWinRsc *__GK_GetZsectionAllEnd(
                                                    struct GkWinRsc *gkwin);
struct GkWinRsc *GK_CreateDesktop(struct GkscParaCreateDesktop *para);
struct GkWinRsc *GK_CreateWin(struct GkscParaCreateGkwin *para);
void GK_SetName(struct GkWinRsc *gkwin,char *name);
void GK_SetUnlock(struct GkWinRsc *gkwin);
void GK_SetLock(struct GkWinRsc *gkwin);
void GK_SetHide(struct GkWinRsc *gkwin);
void GK_SetShow(struct GkWinRsc *gkwin);
void GK_MoveWin(struct GkscParaMoveWin *para);
void __GK_SetBound(struct GkWinRsc *gkwin);
void GK_SetBoundMode(struct GkscParaSetBoundMode *para);
void GK_SetPrio(struct GkscParaSetPrio *para);
bool_t GK_SetRopCode(struct GkscParaSetRopCode *para);
bool_t GK_SetPatBuf(struct GkscParaSetPatBuf *para);
bool_t GK_SetHyalineColor(
                                struct GkscParaSetHyalineColor *para);
bool_t GK_SetBitMsk(struct GkscParaSetBitmsk *para);
void GK_SetDirectScreen(struct GkWinRsc *gkwin);
void GK_UnSetDirectScreen(struct GkWinRsc *gkwin);
void __GK_DestroyWinChild(struct GkWinRsc *gkwin);
void GK_DestroyWin(struct GkWinRsc *gkwin);
bool_t GK_ChangeWinArea(struct GkscParaChangeWinArea *para);
void GK_SetCanvasColor(struct GkWinRsc *gkwin,u32 color);
void GK_GetCanvasColor(struct GkWinRsc *gkwin,u32 *color);
void __GK_OutputRedraw(struct DisplayRsc *display);
void __GK_OutputFrameBuffer(struct DisplayRsc *display);
u16 GK_SyscallChunnel(u16 command,u32 sync_time,void *param1,u16 size1,
                                                void *param2,u16 size2);
bool_t  GK_PostUsercall(u16 usercall_id,void *pdata,u16 size);

ptu32_t GK_UsercallServer(void);
ptu32_t GK_Server(void);

#ifdef __cplusplus
}
#endif

#endif //__GK_WIN_H__

