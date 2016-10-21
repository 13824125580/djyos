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
//
//
//
#include <string.h>
#include <stdlib.h>
#include <lock.h>
#include "mount.h"
#include "vfile.h"


//
//
//
extern struct VCommon g_tRootCom; // rsc.c�ļ��ж���
extern struct VCommon g_tSysCom; // rsc.c�ļ��ж���
extern void __DevModule2FS(void *FileOps); // dev_module.c�ж���
extern void FsShellInstall(void); // fs_shell.c�ж���
//
// ϵͳ����������ϵͳ����ϵͳ��ǰ
//
struct SysEnv g_tSysRoot, g_tSysPWD;
//
// �ļ�ϵͳ�������ӿ�
//
struct VOperations g_tFOps =
{
    .VOpen      = FOpen,
    .VClose     = FClose,
    .VWrite     = FWrite,
    .VRead      = FRead,
    .VCntrl     = NULL, // ���������ļ��������ʱ��ʵ�֣���Ϊ��ר�ŵ�fxxxx�ຯ��
};
//
// RAM�ļ�ϵͳ�߼�
//
static struct FileContext *RamOpen(const char *Path, u32 Mode, const char *Root);
static s32 RamClose(struct FileContext *FileCt);
static s32 RamStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
static s32 RamDirRead(struct FileContext *FileCt, struct Dirent *Content);

static struct FileOperations s_tRamFileOps =
{
    .FileOpen = RamOpen,
    .FileClose = RamClose,
	.FileStat = RamStat,
	.DirRead = RamDirRead,
    // todo: ��ǰֻʵ��Ŀ¼�Ĵ�����������ʵ��
};

//
// �����ļ�ϵͳ�ĸ��ļ�ϵͳ���ص�
//
static struct VMount s_tRootMount =
{
	.VOps 		= &g_tFOps,
	.OldObj 	= NULL,
	.Property 	= MOUNT_ON_DIR | MOUNT_FILE,
	.Context 	= NULL,
	.Private 	= (void*)&s_tRamFileOps,
};
//
// �����ļ�ϵͳ��"sys"�ļ�ϵͳ���ص�
//
static struct VMount s_tSysMount =
{
    .VOps       = NULL,
    .OldObj     = NULL,
    .Property   = MOUNT_ON_DIR,
    .Context    = NULL,
    .Private    = NULL,
};

struct VFilePool *g_ptVFilePool;// todo:���ǵ�/dev�����Ļ�������ռ����ٿ��Ǵ��λ��
struct MutexLCB *g_ptVFMutex;// �����ļ�ϵͳ������
struct MutexLCB *g_ptFSTypeMutex;// �ļ�ϵͳ���͹�������
struct MutexLCB *g_ptVFilePoolMutex;// �ļ����������
char g_cPathSegment[257]; // ·����ĳ�Σ����������̬����;
//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
s32  ModuleInstall_FileSystem(void)
{
    // �ļ�ϵͳ��ػ�����
    g_ptVFMutex = Lock_MutexCreate("virtual file system");
    if(!g_ptVFMutex)
        goto FAIL;

    g_ptFSTypeMutex = Lock_MutexCreate("file system type");
    if(!g_ptFSTypeMutex)
        goto FAIL;

    g_ptVFilePoolMutex = Lock_MutexCreate("virtual file pool");
    if(!g_ptVFilePoolMutex)
        goto FAIL;

    g_ptVFilePool = __ExtendVFilePool();// Ԥ����һ��VFile Pool,ע�������������������
    if(NULL == g_ptVFilePool)
        goto FAIL;

    // ��ϵͳ����Ϊ�����ļ�ϵͳ�߼�
    g_tRootCom.Obj.Type = OB_DIR | OB_MOUNT;
    g_tRootCom.Obj.Inuse = 2;// ��Ϊ����Ὣ���ڵ���ΪĬ�ϵĸ��͵�ǰĿ¼
    g_tRootCom.Context = (void*)(&s_tRootMount);
    s_tRootMount.Name = g_tRootCom.Obj.Name;

    // "/sys"�ڵ�
    g_tSysCom.Obj.Type = OB_DIR | OB_MOUNT;
    g_tSysCom.Context = (void*)&s_tSysMount;
    s_tSysMount.Name = g_tSysCom.Obj.Name;

    // "/dev"�ڵ�
    __DevModule2FS((void*)(&s_tRamFileOps));

    // ����ϵͳ���͵�ǰĿ¼
    g_tSysRoot.Obj = g_tSysPWD.Obj = &(g_tRootCom.Obj);
    g_tSysRoot.VOps = g_tSysPWD.VOps = &g_tFOps;

    // shell���װ
    FsShellInstall();

    return (0);

FAIL:
    if(g_ptVFMutex)
        Lock_MutexDelete(g_ptVFMutex);
    if(g_ptFSTypeMutex)
        Lock_MutexDelete(g_ptFSTypeMutex);
    if(g_ptVFilePoolMutex)
        Lock_MutexDelete(g_ptVFilePoolMutex);
    return (-1);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����: NULL -- ��ʧ��; ��NULL -- �򿪳ɹ���
//��ע: ֻ֧��Ŀ¼
//-----------------------------------------------------------------------------
static struct FileContext *RamOpen(const char *Path, u32 Mode, const char *Root)
{
    struct FileContext *FileCt;
    char *PathNext;
    struct Object *Current, *Parent;
    char Temp;
    char *PathCur = PathCopy(Root, Path, NULL);

    if((Mode & M_APPEND) || (Mode & M_LINK) || (Mode & M_REG))
        return (NULL);// ��ǰ��֧�ֵ�ģʽ

    if((Mode & M_CREATE) && (0 == (Mode & M_DIR)))// Ŀǰֻ֧�ִ���Ŀ¼
    	return (NULL);
#if 0
    if(NULL == Root)
    	Current = g_tSysPWD.Obj;
    else
    	Current = g_tSysRoot.Obj;
#else
    Current = g_tSysRoot.Obj;
#endif

    // todo: �˴������߼����Ż�,��Ϊ�ظ���
    while(1)
	{
		while(*PathCur == '/')
			PathCur++;// ���˶����'/'

		if((*PathCur == '\0') || (PathCur == NULL))
			break;// ����·��������·��������

		Parent = Current;
		// �ҵ���һ���ļ���Ŀ¼������PathCurָ���ļ�������ʼ��PathNextָ���ļ�������������һ���ַ���
		PathNext = PathCur;
		while(1)
		{
			if((*PathNext == '/') || (*PathNext == '\0'))
				break;
			PathNext++;
		}

		if(PathCur[0] == '.' && PathCur[1] == '.')
		{
			// �����ǰ�ڵ�ĸ�Ϊ��㣬��ǰ�ڵ㲻��.�ݴ�������丸Ϊ�յĻ���current�ڵ㲻��.
			if(Current->Parent)
				Current = Current->Parent;
		}
		else if(PathCur[0] != '.')
		{
			// �����ַ�������Ҫ�ַ�����ʽ������Ϊ�˽�ʡ�ռ䣬����ԭ�ƻ�ֱ����ԭ�ַ����ϸģ���ʵ�ֲ���,��Ϊconst
			if(*PathNext != '\0')
			{

                Temp = *PathNext;
                *PathNext = '\0';
                Current = OBJ_SearchChild(Current, PathCur);
                *PathNext = Temp;
			}
			else
                Current = OBJ_SearchChild(Current, PathCur);
		}

		if(NULL == Current)
		{
			if(*PathNext != '\0')
				return (NULL); // ���ڶ�������ڽڵ�
			break; // �����жϣ�·���ϳ��ֲ�����(����)�Ľڵ�
		}

		PathCur = PathNext;
	}

    if((NULL == Current) && (0 == (Mode & M_CREATE)))
    	return (NULL); // �������ҽڵ㲻����

    FileCt = AllocContext();
    if(!FileCt)
        return (NULL);

    FileCt->Property = P_DIR;
    FileCt->FileOps = (void*)&s_tRamFileOps;
    return (FileCt);
}

//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�;
//��ע: ɾ�������ļ�
//-----------------------------------------------------------------------------
static s32 RamClose(struct FileContext *FileCt)
{
	struct VCommon *FileObj = FileCt->CHost;

	if((FileObj) && (0 == FileObj->Obj.Inuse))
		FileObj->Obj.Inuse = 1; // �ر��ڴ��е��ļ�����Ŀ¼������ɾ��

	FreeContext(FileCt);
    return (0);
}

//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע: ��ǰ�ڴ��ļ�ϵͳֻ֧��Ŀ¼
//-----------------------------------------------------------------------------
static s32 RamStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
//	if(!FileCt)
//		return (-1); // �ݴ��߼�
	memset(Buf, 0, sizeof(*Buf));
	Buf->Property = P_DIR;
	Buf->Size = 0;
	return (0);
}
//-----------------------------------------------------------------------------
//����: ��Ŀ¼��
//����:
//����: 1 -- ����,ȫ������;
//      -1 -- ��������; -2 -- ������֧��; -3 -- ��������;
//      0 -- �ɹ���ȡ����;
//��ע: �ں����ڲ����ܷ����ļ��ڵ�ɾ��
//-----------------------------------------------------------------------------
static s32 RamDirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	u16 Type;
	struct Object *Child, *Current = (struct Object*)FileCt->Private;
	struct VCommon *FileCom = FileCt->CHost;

	Child = OBJ_Child(&(FileCom->Obj));
	if(NULL == Child)
		return (1);

	if(Current && (OBJ_Next(Current) == Child))
	{
		Put(Current);
		FileCt->Private = NULL;
		return (1);
	}

	if(Current)
	{
		Put(Current); //
		Current = FileCt->Private = (void*)OBJ_Next(Current);
	}
	else
	{
		Current = FileCt->Private = (void*)Child;
	}

	Get(Current); // ��ֹ�ڵ���ʹ�ù����б�ɾ��
	strcpy(Content->Name, OBJ_Name(Current));
	Type = OBJ_Type(Current) & (~OB_FILE);
	if(Type & OB_REG)
		Content->Property = P_REG;
	if(Type & OB_DIR)
		Content->Property |= P_DIR;
	if(Type & OB_LINK)
		Content->Property |= P_LINK;

	return (0);
}

