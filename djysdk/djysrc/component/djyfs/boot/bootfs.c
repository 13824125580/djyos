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
#include <systime.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <djyfs/vfile.h>
#include <djyfs/mount.h>
#include <djyos.h>


static struct FileContext *IAPOpen(const char *Path, u32 Mode, const char *Root);
static s32 IAPClose(struct FileContext *FileCt);
static s32 IAPDelete(const char *Path, const char *Root);
static s32 IAPWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
static s32 IAPRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
static s32 IAPDirentRead(struct FileContext *FileCt, struct Dirent *Content);
static s32 IAPStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private);
static s32 IAP_FS_Uninstall(struct MountInfo *Info);

//
// IAP���ļ�ϵͳ�����ӿ�
//
struct FileOperations IAPFileOps =
{
    .FileOpen       = IAPOpen,
    .FileClose      = IAPClose,
    .FileDelete     = IAPDelete,
    .FileSync       = NULL,
    .FileSeek       = NULL,
    .FileWrite      = IAPWrite,
    .FileRead       = IAPRead,
    .FileStat       = IAPStat,
    .DirRead        = IAPDirentRead,
};
//
// IAP���ļ�ϵͳ����
//
struct FileSysType IAP_FS =
{
    .FileOps = &IAPFileOps,
    .VOps = NULL,
    .Install = IAP_FS_Install,
    .Uninstall = IAP_FS_Uninstall,
    .Format = NULL,
    .Property = MOUNT_FILE,
    .MountList = NULL,
    .Name = "IAP",
};
//
struct IAP{
    u32 Size;
    u32 CRC;
    u32 Reserved;
    u32 Signature;
#define S_APP_DONE			(u32)0x61707000
#define S_APP_PRO           (u32)-1
#define S_DEBUG				  (u32)0x00676264
    u8 FileName[240];
    u8 *HeadSpace;
    u8 *DataSpace;
#define BINOFFSET              (u32)256
};
//
struct IAP_FS{
    struct FlashChip *Super;
    struct IAP Head;
};
//
struct IAPFile
{
    struct IAP_FS *Info;
    u32 End;
    u32 Cur;
};
//
struct IAP_Dir
{
    u8 DirentIndex;
};

static u32 BinSpace;
static u32 BinInfoSpace;
static struct IAP_FS *sp_tFilesInfo;
//-----------------------------------------------------------------------------
//����: �ļ�ϵͳ��װ
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private)
{
    struct DjyDevice *Dev;
    struct EmbdFlashDescr *Embd;
    u16 SizeofFileInfo = sizeof(*sp_tFilesInfo);

    if((sp_tFilesInfo) || (!Info))
        return (-1);

    sp_tFilesInfo = malloc(SizeofFileInfo);
    if(!sp_tFilesInfo)
        return (-1);
    memset(sp_tFilesInfo, 0x0, SizeofFileInfo);

    Dev = Container(Info->Dev, struct DjyDevice, Node);
    sp_tFilesInfo->Super = (struct FlashChip*)Dev->PrivateTag;
    Embd = &sp_tFilesInfo->Super->Descr;
//    BinInfoSpace = Embd->BytesPerPage *
//                     (Embd->PagesPerSmallSect * Embd->SmallSectorsPerPlane +
//                      Embd->PagesPerLargeSect * Embd->LargeSectorsPerPlane);
//    if(Embd->SmallSectorsPerPlane)
//    	BinInfoSpace += (Embd->ReservedSects-1) * Embd->BytesPerPage * Embd->PagesPerNormalSect;
//    else
//    	BinInfoSpace += (Embd->ReservedSects) * Embd->BytesPerPage * Embd->PagesPerNormalSect;
    BinInfoSpace = Embd->ReservedPages  * Embd->BytesPerPage;
    BinSpace = BinInfoSpace + BINOFFSET;

    Info->Mount.Name = NULL;
    Info->Private = NULL;
    return (0);
}

//-----------------------------------------------------------------------------
//����: �ļ�ϵͳж��
//����:
//����: 0 -- �ɹ�;
//��ע:
//-----------------------------------------------------------------------------
static s32 IAP_FS_Uninstall(struct MountInfo *Info)
{
    free(sp_tFilesInfo);
    sp_tFilesInfo = NULL;
    return (0);
}

//-----------------------------------------------------------------------------
//����: ��ʼ��(����)�ļ���Ϣ
//����:
//����:
//��ע: δ���ǻ���
//-----------------------------------------------------------------------------
static struct IAP_FS *__NewOrDelFile(const char *Path)
{
    s32 Ret = 0;
    struct EmbdFlashDescr *Embd = &(sp_tFilesInfo->Super->Descr);
    u32 SectorToErase,Sectors;

//    if(Embd->SmallSectorsPerPlane)
//    {
//    	Sectors = (Embd->NormalSectorsPerPlane+1) * Embd->Planes;//todo
//    }
//    else
//    {
//    	Sectors= (Embd->NormalSectorsPerPlane) * Embd->Planes;
//    }

    SectorToErase = (Embd->ReservedPages)/Embd->PagesPerNormalSect;

    Sectors =  (Embd->LargeSectorsPerPlane * Embd->PagesPerLargeSect +
    		Embd->SmallSectorsPerPlane * Embd->PagesPerSmallSect)
    				/ Embd->PagesPerNormalSect;
    Sectors= (Embd->NormalSectorsPerPlane + Sectors) * Embd->Planes;

    if(strlen(Path) > 239)
        return (NULL); // �ļ������ܴ����趨�ռ�

    for(; SectorToErase < Sectors; SectorToErase++)
    {
        Ret = sp_tFilesInfo->Super->Ops.ErsBlk(SectorToErase);
        if(Ret)
            break;
    }

    if(!Ret)
    {
        sp_tFilesInfo->Head.Size = 0;
        sp_tFilesInfo->Head.CRC = 0;
        sp_tFilesInfo->Head.Reserved = -1;
        sp_tFilesInfo->Head.Signature = S_APP_PRO;
        if(Path)
        	strcpy(sp_tFilesInfo->Head.FileName, Path);
        sp_tFilesInfo->Head.HeadSpace = (u8*)BinInfoSpace;
        sp_tFilesInfo->Head.DataSpace = (u8*)BinSpace;
        return (sp_tFilesInfo);
    }

    return (NULL);
}
//-----------------------------------------------------------------------------
//����: ��ȡ�ļ���Ϣ
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static struct IAP_FS *__LoadFile(const char *Path)
{

    struct IAP *Head = (struct IAP*)BinInfoSpace;

    if(S_APP_DONE == Head->Signature)
    {
        sp_tFilesInfo->Head.Size = Head->Size;
        sp_tFilesInfo->Head.CRC = Head->CRC;
        sp_tFilesInfo->Head.Reserved = Head->Reserved;
        sp_tFilesInfo->Head.Signature = Head->Signature;
        strcpy(sp_tFilesInfo->Head.FileName, Head->FileName);
        sp_tFilesInfo->Head.HeadSpace = (u8*)BinInfoSpace;
        sp_tFilesInfo->Head.DataSpace = (u8*)BinSpace;
    }

    if(Path && strcmp(sp_tFilesInfo->Head.FileName, Path))
        return (NULL);
    else
        return (sp_tFilesInfo);
}
//-----------------------------------------------------------------------------
//����: ���ļ�
//����:
//����: ��NULL -- �ɹ�; NULL -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
static struct FileContext *IAPOpen(const char *Path, u32 Mode, const char *Root)
{
    struct FileContext *FileCt;
    struct FileBuf *Buf;

    if((Mode & M_APPEND) || (Mode & M_LINK))
        return (NULL); // ��ǰ��֧�ֵ�ģʽ

    if((Path) && (Mode & M_DIR)) // ���Ǹ�Ŀ¼,����Ŀ¼���ܴ������
        return (NULL);

    FileCt = AllocContext();
    if(!FileCt)
        return (NULL);

    FileCt->FileOps = (void *)&IAPFileOps;
    if(Mode & M_READ)
        FileCt->Property |= P_READ;

    if(Mode & M_WRITE)
        FileCt->Property |= P_WRITE;

    if(!(Mode & M_TYPE)) // ����δָ��,Ĭ��Ϊ���ļ�
        Mode |= M_REG;

    if(Mode & M_REG) // ���ļ�
    {
        struct IAPFile *IFile;
        struct IAP_FS *Info;

        if(Mode & M_CREATE)
            Info = __NewOrDelFile(Path); // ������½��Ļ�,��ֱ����ԭ�ļ�ɾ��
        else
            Info = __LoadFile(Path);

        if(Info)
        {
            IFile = (struct IAPFile*)malloc(sizeof(*IFile));
            if(IFile)
            {
                IFile->Info = Info;
                IFile->Cur = BINOFFSET;
                IFile->End = Info->Head.Size + IFile->Cur;
				if(!AllocFileBuf(&Buf, 1)) // ǿ������Buf,M7������Flash���ڶ�������
                {
                    FileCt->Property |= P_REG;
                    FileCt->Private = (void *)(IFile);
                    FileCt->Buf = Buf;
                    return (FileCt); // �ɹ�;
                }
            }
            free(IFile); // ʧ��;
        }
    }

    //IAP�ļ�ϵͳ��֧�ִ�Ŀ¼������һ����Ŀ¼.�����������Ŀ¼��ʧ��
    if((Mode & M_DIR) && (!(Mode & M_CREATE)) ) // ��Ŀ¼
    {
        struct IAP_Dir *Root = (struct IAP_Dir*)malloc(sizeof(*Root));
        if(Root)
        {
            Root->DirentIndex = 0;
            FileCt->Property |= P_DIR;
            FileCt->Buf = NULL;
            FileCt->Private = (void *)(Root);
            return (FileCt); // �ɹ�
        }
    }

    // ʧ��
    FreeContext(FileCt);
    return (NULL);
}
//-----------------------------------------------------------------------------
//����: �����ļ���CRCֵ
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static u32 __CalculateFileCRC(struct IAPFile *IFile)
{
    extern u32 crc32( char *buf, u32 len);
    extern u16 crc16( char *buf, u32 len);

    struct IAP_FS *Info = IFile->Info;

    return ((u32)crc32((char *)Info->Head.DataSpace, Info->Head.Size));

}
//-----------------------------------------------------------------------------
//����: �ر��ļ�
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPClose(struct FileContext *FileCt)
{
    if((FileCt->Property & P_REG) && (FileCt->Property & P_WRITE))
    {
        s32 Ret;
        u32 PageNo, Offset;
        struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
        struct FlashChip *Super = IFile->Info->Super;
        struct IAP *FileHead = &(IFile->Info->Head);

        if(FileHead->Signature == S_APP_PRO)
        {
            FileHead->Size = IFile->End - BINOFFSET;
            FileHead->Signature = S_APP_DONE;
            Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
            FileHead->CRC = __CalculateFileCRC(IFile);
            PageNo = ((u32)FileHead->HeadSpace) / Super->Descr.Embd.BytesPerPage;
            Offset = ((u32)FileHead->HeadSpace) % Super->Descr.Embd.BytesPerPage;
            Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
            if(Ret)
            {
                memcpy((Super->Buf+Offset), FileHead, BINOFFSET);
                Ret = Super->Ops.WrPage(PageNo, Super->Buf, NO_ECC);
            }
            Lock_MutexPost((struct MutexLCB*)Super->Lock);
            if(!Ret)
                return (-1);
        }
    }

    if(FileCt->Property & P_DIR)
        free(FileCt->Private);
	if(FileCt->Buf)
		free(FileCt->Buf);

    FreeContext(FileCt);
    return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPSeek(struct FileContext *FileCt, s64 Offset, int Whence)
{
    ;// FTP��Ҫ
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPDelete(const char *Path, const char *Root)
{
	__NewOrDelFile(NULL);
	return (0);
}
//-----------------------------------------------------------------------------
//����: ҳ������,�޳�������ҳ����
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static u32 __PageFix(struct FlashChip *Super, u32 PageNo)
{
//    u32 ResevedPages;
    struct EmbdFlashDescr *Embd = &sp_tFilesInfo->Super->Descr;

//    ResevedPages = (Embd->PagesPerSmallSect * Embd->SmallSectorsPerPlane +
//                   Embd->PagesPerLargeSect * Embd->LargeSectorsPerPlane);
//
//	if(Embd->SmallSectorsPerPlane)
//    {
//		ResevedPages += (Embd->ReservedSects - 1) * Embd->PagesPerNormalSect;
//    }
//    else
//    {
//		ResevedPages = Embd->ReservedSects * Embd->PagesPerNormalSect + ResevedPages;
//    }


    return (PageNo + Embd->ReservedPages);

}
//-----------------------------------------------------------------------------
//����: д�ļ�
//����:
//����: ʵ��д��ĵ�Ԫ��
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
    u32 i, PageNo, Offset;
    s32 Ret;
    u32 LenToWrOrig, LenToWr, OnceToWr;
    struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
    struct FlashChip *Super = IFile->Info->Super;
    u8 *DataBuf = (u8*)Buf;

    LenToWrOrig = LenToWr = Size * Nmemb;
    Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
    PageNo = (IFile->Cur) / Super->Descr.Embd.BytesPerPage;
    PageNo = __PageFix(Super, PageNo);
    Offset = (IFile->Cur) % Super->Descr.Embd.BytesPerPage;
    while(LenToWr > 0)
    {
        OnceToWr = MIN((Super->Descr.Embd.BytesPerPage - Offset), LenToWr);
        if(OnceToWr != Super->Descr.Embd.BytesPerPage)
        {
        	Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
            if(!Ret)
                goto FAILURE; //

            // ���򵥵ļ��
            for(i = Offset; i < (Offset + OnceToWr); i++)
            {
                if((0xFF != Super->Buf[i]) && (Super->Buf[i] != DataBuf[i]))
                {
                    printf("data cannot be writen, offset %d, destination 0x%x, source 0x%x\r\n",
                            i, Super->Buf[i], DataBuf[i]);
                    goto FAILURE; //
                }
            }
        }

        memcpy((Super->Buf + Offset), DataBuf, OnceToWr);
        Ret = Super->Ops.WrPage(PageNo, Super->Buf, NO_ECC);
        if(Ret != Super->Descr.Embd.BytesPerPage)
            goto FAILURE;
        LenToWr -= OnceToWr;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToWr;
        IFile->Cur += OnceToWr;
    }

FAILURE:	
	if(IFile->Cur > IFile->End)
		IFile->End = IFile->Cur;
	Lock_MutexPost((struct MutexLCB*)Super->Lock);	
	
	return ((s32)((LenToWrOrig - LenToWr)/Size));
}
//-----------------------------------------------------------------------------
//����: ���ļ�
//����:
//����: ʵ�ʶ����ĵ�Ԫ��
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
    s32 Ret;
    u32 PageNo, Offset;
    u32 LenToRdOrig, LenToRd, OnceToRd;
    struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
    u8 *DataBuf = (u8*)Buf;
    struct FlashChip *Super = IFile->Info->Super;

    LenToRd = Size * Nmemb;
    if((IFile->Cur + LenToRd) > IFile->End)
        LenToRd = IFile->End - IFile->Cur;

    LenToRdOrig = LenToRd;
    Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
    PageNo = (IFile->Cur) / Super->Descr.Embd.BytesPerPage;
    PageNo = __PageFix(Super, PageNo);
    Offset = (IFile->Cur) % Super->Descr.Embd.BytesPerPage;
    while(LenToRd > 0)
    {
        OnceToRd = MIN((Super->Descr.Embd.BytesPerPage - Offset), LenToRd);
        Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
        if(!Ret)
            goto FAILURE; //
        memcpy(DataBuf, (Super->Buf + Offset), OnceToRd);
        LenToRd -= OnceToRd;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToRd;
        IFile->Cur += OnceToRd;
    }

FAILURE:
    Lock_MutexPost((struct MutexLCB*)Super->Lock);
    return((s32)((LenToRdOrig - LenToRd)/Size));
}
//-----------------------------------------------------------------------------
//����: �ļ���ѯ
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
static s32 IAPStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
    if(FileCt)
    {
        struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
        Buf->Property = FileCt->Property;
        Buf->Size = IFile->End;
        return (0);
    }

    if(Path)
    {
        struct IAP_FS *FileInfo = __LoadFile(Path);

        if(!FileInfo)
            return (-1);

        Buf->Size = FileInfo->Head.Size;
        Buf->Property = P_READ | P_REG;
        return (0);
    }

    return (-1);
}
//-----------------------------------------------------------------------------
//����: ��Ŀ¼����
//����:
//����: 1 -- ����,ȫ������;
//      -1 -- ��������; -2 -- ������֧��; -3 -- ��������;
//      0 -- �ɹ���ȡ����;
//��ע: ÿ��һ��,����һ��Ŀ¼��
//-----------------------------------------------------------------------------
static s32 IAPDirentRead(struct FileContext *FileCt, struct Dirent *Content)
{
    struct IAP_FS *Info = __LoadFile(NULL);
    struct IAP_Dir *Root = (struct IAP_Dir*)FileCt->Private;

    if((!Content) || (!FileCt))
        return (-1);

    if((!Root->DirentIndex) &&
        (S_APP_DONE == Info->Head.Signature))
    {
        Root = (struct IAP_Dir*)FileCt->Private;
        strcpy(Content->Name, Info->Head.FileName);
        Content->Property = P_REG | P_READ;
        Root->DirentIndex += 1;
        return (0);
    }
    return (1);
}
//-----------------------------------------------------------------------------
//����: ��װBOOTFS
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
s32 ModuleInstall_BootFS(void)
{
    s32 Ret;

    RegisterFSType(&IAP_FS);
    Ret = open("/iboot", O_DIRECTORY | O_CREAT | O_RDWR, 0);/* ����/efsĿ¼ */
    if(-1 == Ret)
    {
        printf("create diretionay \"/iboot\" error\r\n");
        return (-1);
    }

    Ret = ModuleInstall_EmbededFlash("embedded flash", FLASH_BUFFERED, 0);
    if(Ret)
    {
        printf("install \"embedded flash\" error\r\n");
        return (-1);
    }

    Ret = Mount("/iboot", "/dev/embedded flash", "IAP", NULL);
    if(Ret < 0)
    {
        printf("mount \"IAP_FS\" error\r\n");
        return (-1);
    }

    return (0);
}
