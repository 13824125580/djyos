//-----------------------------------------------------------------------------
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

#include <stdio.h>
#include <string.h>
#include <systime.h>
#include <driver.h>
#include <object.h>
#include "vfile.h"
#include "mount.h"

extern struct VCommon g_tDevCom; // driver.c�ж���

static s32 DOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound);
static s32 DClose(void *Device);
static s32 DWrite(const void *Buf, u32 Size, u32 Nmemb, void *Device);
static s32 DRead(void *Buf, u32 Size, u32 Nmemb, void *Device);
static s32 DCntrl(u32 Command, void *Param0, void *Param1, void *Device);
//
// "/dev"Ŀ¼����һ����ϵ,���Ǹ�Ŀ¼��ϵ�³�Ա��ǰ���ɲ���
//
static struct VOperations s_tDOps =
{
    .VOpen      = DOpen,
    .VClose     = DClose,
    .VWrite     = DWrite,
    .VRead      = DRead,
    .VCntrl     = DCntrl,
};

//
// "/dev"Ŀ¼����һ����ϵ,���Ǹ�Ŀ¼��ϵ�³�Ա��ǰ���ֿɲ���
//
static struct VMount s_tDevMount =
{
    .VOps       = &s_tDOps,
    .OldObj     = NULL,
    .Property   = MOUNT_ON_DIR | MOUNT_DEV,
    .Context    = NULL,
    .Private    = NULL,
};

//-----------------------------------------------------------------------------
//����: Devģ��ת�ļ�ϵͳģʽ
//����: �ļ�ϵͳ��������
//����:
//��ע:
//-----------------------------------------------------------------------------
void __DevModule2FS(void *FileOps)
{
    g_tDevCom.Context = (void*)(&s_tDevMount);
    g_tDevCom.Obj.Type = OB_MOUNT | OB_DIR;
    s_tDevMount.Name = g_tDevCom.Obj.Name;
}
//-----------------------------------------------------------------------------
//����: ���豸
//����: 
//����: 1 -- �ɹ�; 
//      -1 -- �����������; -2 -- ��������;
//��ע: 
//-----------------------------------------------------------------------------
static s32 DOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound)
{
    struct DjyDevice *Dev;
    struct FileContext *FileCt;
    u32 DevMode = 0;
#if 0
    if((!IsFound) || (Mode & M_CREATE))
        return (-1);// ��ǰ�߼�,�豸�ǲ���ͨ��Open������

    // todo: ����У��
    Mode &= ~M_OPEN;// �豸��֧������open����
#else
    if(!IsFound)
    	return (-1);
#endif
    FileCt = AllocContext();
    if(!FileCt)
        return (-2);// �ڴ治��

    FileCt->Property |= P_DEV;// �豸�ļ�

    if((Mode & M_READ) && (Mode & M_WRITE))
    {
        DevMode = O_RDWR;
        FileCt->Property |= (P_READ | P_WRITE);
    }
    else if(Mode & M_READ)
    {
        DevMode = O_RDONLY;
        FileCt->Property |= P_READ;
    }
    else if(Mode & M_WRITE)
    {
        DevMode = O_WRONLY;
        FileCt->Property |= P_WRITE;
    }   

#if 0
    if(Mode)    
        return (-1);// ����ģʽ��ǰ��֧��
#endif

    Dev = Driver_OpenDevice(PathContext->End->Name, DevMode, CN_TIMEOUT_FOREVER);
    if(NULL == Dev)
	{
		FreeContext(FileCt);
        return (-2);// ���ش���
	}
    FileCt->Private = (void*)Dev;

    PathContext->Private = (void*)FileCt;// ���ݸ�VFile
    PathContext->PrivateVHost = &(FileCt->FHost);
    return (1);
}

//-----------------------------------------------------------------------------
//����: �ر��豸
//����: Device -- �豸������;
//����: 0 -- �ɹ�; -2 -- ����;  
//��ע: 
//-----------------------------------------------------------------------------
static s32 DClose(void *Device)
{
    bool_t Ret;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_CloseDevice(Dev);
    if(FALSE == Ret)
        return (-2);
    
    FreeContext(DevCt);

    return (0);
}

//-----------------------------------------------------------------------------
//����: д�豸;
//����: 
//����: >0 -- �ɹ�; 0 -- ����;  
//��ע: 
//-----------------------------------------------------------------------------
static s32 DWrite(const void *Buf, u32 Size, u32 Nmemb, void *Device)
{
    u32 Ret;
    u32 DSize = Size * Nmemb;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    

    Ret = Driver_WriteDevice(Dev, (u8*)Buf, DSize, 0, CN_BLOCK_BUFFER, CN_TIMEOUT_FOREVER);
    if(-1 == Ret)
        return (0);
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//����: ���豸
//����: 
//����: >0 -- �ɹ�; 0 -- ����;  
//��ע: 
//-----------------------------------------------------------------------------
static s32 DRead(void *Buf, u32 Size, u32 Nmemb, void *Device)
{
    u32 Ret;
    u32 DSize = Size * Nmemb;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_ReadDevice(Dev, (u8*)Buf, DSize, 0, CN_TIMEOUT_FOREVER);
    if(-1 == Ret)
        return (0);
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//����: �����豸.
//����: 
//����: -1 -- ����;  
//��ע: 
//-----------------------------------------------------------------------------
static s32 DCntrl(u32 Command, void *Param0, void *Param1, void *Device)
{
    u32 Ret;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_CtrlDevice(Dev, Command, (ptu32_t)Param0, (ptu32_t)Param1);
    
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
struct DjyDevice *ToDev(struct VFile *Fp)
{
	struct FileContext *DevCt;

	if((NULL == Fp) || (NULL == Fp->Private))
		return (NULL);

	DevCt = (struct FileContext *)Fp->Private;

	return ((struct DjyDevice *)(DevCt->Private));
}
