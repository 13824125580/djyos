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


#include <string.h>
#include <driver.h>
#include <djyos.h>
#include "fat_port.h"
#include "./drivers/fat_drivers.h"
//
// FAT���ļ�ϵͳ�����ӿ�
//
struct FileOperations g_tFATFileOps =
{
    .FileOpen       = FATOpen,
    .FileClose      = FATClose,
    .FileDelete     = FATDelete,
    .FileSync       = FATSync,
    .FileSeek       = FATSeek,
    .FileWrite      = FATWrite,
    .FileRead       = FATRead,
    .FileStat       = FATStat,
    .FileTruncate   = FATTruncate,
	.DirRead        = FATDirRead
};
//
// FAT���ļ�ϵͳ����
//
struct FileSysType FAT =
{
    .FileOps = &g_tFATFileOps,
    .VOps = NULL,
    .Install = FATInstall,
    .Uninstall = FATUninstall,
    .Property = MOUNT_FILE,
    .MountList = NULL,
    .Name = "FAT",
};

//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
#define INSTALL_MAKE_NEW            (0x1)
#define INSTALL_DELAY				(0x2)
s32 FATInstall(struct MountInfo *Info, void *Private)
{
    char *DiskName, *VolName;
    FATFS *NewFAT;
    FRESULT Ret;
    u32 Length;
    s32 Vol;
    struct DjyDevice *Dev;
    u32 Delay = (*(u32*)Private) & INSTALL_DELAY;

    Dev = Container(Info->Dev, struct DjyDevice, Node);
    Length = strlen(Info->Dev->Name);
    VolName = DiskName = malloc(Length + 2);// �ļ�ϵͳ����,FAT�¾��Ƿ�����
    if(NULL == DiskName)
        return (-1);

    strcpy(DiskName, Info->Dev->Name);
    DiskName[Length] = ':';
    DiskName[Length+1] = '\0';

    NewFAT = malloc(sizeof(FATFS));
    if(NULL == NewFAT)
        return (-1);

    Vol = get_ldnumber((const char**)&VolName); // ��Ҫffconf.h�ж���"_VOLUME_STRS"
    if (Vol < 0)
        return (-1); //

    if(FatDrvInitialize(LD2PD(Vol), (struct FatDrvFuns*)(Dev->PrivateTag)))
        return (-1); // ��װ����

    Ret = f_mount(NewFAT, DiskName, Delay); // ����
    if ((FR_NO_FILESYSTEM == Ret) && (*((u32*)Private) & INSTALL_MAKE_NEW))
        Ret = f_mkfs(DiskName, 1, 0); // �豸�ϲ������ļ�ϵͳ�����½�FAT

    if(FR_OK != Ret) // ʧ��
    {
        free(NewFAT);
        return (-1);
    }

    // �ɹ�
    Info->Mount.Name = DiskName;
    Info->Private = (void*)NewFAT;
    return (0);
}

//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�;
//��ע:
//-----------------------------------------------------------------------------
s32 FATUninstall(struct MountInfo *Info)
{

    free(Info->Mount.Name);
    free(Info->Private);
    return (0);
}
