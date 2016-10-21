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

#include <driver.h>
#include <djyos.h>
#include "../../mount.h"
#include "../../vfile.h"
#include "./yaffs2_port.h"
#include "./drivers/yaffs_drv_generic.h"

//
// YAFFS2�ļ�ϵͳ�ӿ�
//
struct FileOperations g_tYAF2FileOps =
{
    .FileOpen       = YAF2Open,
    .FileClose      = YAF2Close,
    .FileDelete     = YAF2Delete,
    .FileSync       = YAF2Sync,
    .FileSeek       = YAF2Seek,
    .FileWrite      = YAF2Write,
    .FileRead       = YAF2Read,
    .FileStat       = YAF2Stat,
    .FileTruncate   = YAF2Truncate,
    .DirRead        = YAF2DirRead,
};
//
// FAT���ļ�ϵͳ����
//
struct FileSysType YAF2 =
{
    .FileOps   = &g_tYAF2FileOps,
    .VOps      = NULL,
    .Install   = YAF2Install,
    .Uninstall = YAF2Uninstall,
    .Property  = MOUNT_FILE,
    .MountList = NULL,
    .Name      = "YAF2",
};

//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
#define ECC_ON                  ((u32)0x1) // ����ECC
#define FORMAT                  ((u32)0x2) // ��ʽ������
s32 YAF2Install(struct MountInfo *Info, void *Private)
{
    struct yaffs_dev *YDev;
    struct yaffs_param *Param;
    struct FlashChip *Chip;
    struct DjyDevice *DevInfo;
    int Ret;
    unsigned int PageIndex;

    DevInfo = Container(Info->Dev, struct DjyDevice, Node);

    Chip = (struct FlashChip*)DevInfo->PrivateTag;

    if(*(u32*)Private & FORMAT)
    {
        Ret = EarseWholeChip(Chip);
        if(-1 == Ret)
            return (-1);
    }

    yaffs_start_up();

    YDev = malloc(sizeof(struct yaffs_dev)); // "yaffs_dev"������superblock����
    if(NULL == YDev)
        return (-1); // ����;

    memset(YDev, 0, sizeof(*YDev));

    Param = &YDev->param;
    Param->name = Info->Dev->Name;

    Param->n_reserved_blocks = 6;
    Param->is_yaffs2 = 1;
    Param->n_caches = 0; // 0 -- ��ʹ��cache�߼�
    Param->use_nand_ecc = 0; // ���ع���δʹ��ECC�������һ����оƬ����Ҫ���߼�
    Param->disable_summary = 1; // ��ȥsummary���ܣ�todo
    if(F_NAND == Chip->Type)
    {
        Param->total_bytes_per_chunk = Chip->Descr.Nand.BytesPerPage; // todo: ����nandҳС��512�����
        Param->chunks_per_block = Chip->Descr.Nand.PagesPerBlk;
        Param->start_block = Chip->Descr.Nand.ReservedBlks;
        Param->end_block = (Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs) - 1; // �����ܿ���2048,��Χ"0-2047"

        Ret = yaffs_nand_install_drv(YDev, Chip); // ��ʼ��YAFFS2������ӿ�
    }
    else if(F_NOR == Chip->Type)
    {
        // �ļ�ͷ�ṹ�������512Bytes���⻹��Tags��16Bytes,
        if(Chip->Descr.Nor.BytesPerPage > 512+16)
        {
            Param->total_bytes_per_chunk = Chip->Descr.Nor.BytesPerPage;
            Param->chunks_per_block = Chip->Descr.Nor.PagesPerSector * Chip->Descr.Nor.PagesPerSector;
        }
        else
        {
            Param->total_bytes_per_chunk = 1024;
            PageIndex = Param->total_bytes_per_chunk / Chip->Descr.Nor.BytesPerPage; // todo: �˴�ת��Ϊλ����
            Param->chunks_per_block = Chip->Descr.Nor.PagesPerSector * Chip->Descr.Nor.PagesPerSector / PageIndex;
        }

        Param->start_block = Chip->Descr.Nor.ReservedBlks;
        Param->end_block = Chip->Descr.Nor.Blks - 1;
        Param->inband_tags = 1; // tag�������ҳ

        Ret = yaffs_nor_install_drv(YDev, Chip); // ��ʼ��YAFFS2������ӿ�
    }
    else
    {
        Ret = YAFFS_FAIL; // �����豸�ݲ�֧��
    }


    if (YAFFS_FAIL == Ret)
    {
        free(YDev);
        return (-1);
    }

    yaffs_add_device(YDev); // ��"yaffs_dev"ע���yaffs2

    Ret = yaffs_mount_reldev(YDev); // ����
    if(-1 == Ret)
    {
        yaffs_remove_device(YDev);
        free(YDev);
        return (-1);
    }

    // �����������֧��ECC�������������һ���ж�
    if(*(u32*)Private & ECC_ON)
        Param->use_nand_ecc = 1;// ʹ��ECC��ֻ�ܷŵ�������ɺ�


    Info->Mount.Name = Chip->Name;
    Info->Private = (void*)YDev;

    return (0); // ����yaffs2�ļ�ϵͳ�ɹ�
}
//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
s32 YAF2Uninstall(struct MountInfo *Info)
{
    struct yaffs_dev *YDev;
    int Ret;

    YDev = (struct yaffs_dev*)(Info->Private);
    Ret = yaffs_unmount_reldev(YDev);
    if(!Ret)// �ɹ�,�����ͷ�"yaffs_dev"
    {
        yaffs_remove_device(YDev);
        free(YDev);
    }

    return (Ret);
}
//-----------------------------------------------------------------------------
//����: ʵ����OS��ص�һЩ�߼�;
//����:
//����: "0" -- �ɹ���
//      "-1" -- ʧ�ܡ�
//��ע: ���û�ʵ�ֵĴ���.
//-----------------------------------------------------------------------------
int yaffs_start_up(void)
{
    static int start_up_called = 0;

    if(start_up_called)
        return (0);
    start_up_called = 1;

    yaffsfs_OSInitialisation();

    return (0);
}

