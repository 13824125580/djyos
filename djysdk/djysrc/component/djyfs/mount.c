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
#include <stdlib.h>
#include <int_hard.h>
#include <int.h>
#include <systime.h>
#include <driver.h>
#include <djyos.h>
#include "mount.h"
#include "iofile.h"
#include "file.h"

//
//
//
extern char *g_pWokingPath; // shellʹ��
//
// ���ر���
//
static struct FileSysTypeInfo *s_ptFSTypeList;// �ļ�ϵͳ��������
static char s_cPWDRoot[257];
//
// ��������
//
s32 Get(struct Object *Obj);
void Put(struct Object *Obj);
static struct FileSysType *FindFSType(const char *Type);
static char *__PWDFixRootPath(struct Object *Obj);
static s32 __PathLen(struct Object *Obj, u8 Opts);
s32 __WorkingPath(char *Path);

//-----------------------------------------------------------------------------
//����: ��װһ���ļ�ϵͳ.
//����: MountPath -- ���ص�·��;
//      DevPath -- �ļ�ϵͳ�����豸·��;
//      Type -- ׼�����ص��ļ�ϵͳ����;
//      Private -- ���ݸ���װ����;
//����:  0 -- �ɹ�.
//      -1 -- �����������; -2 -- �ļ�ϵͳ���Ͳ�����;
//      -3 -- �豸�޷���; -4 -- ����Ŀ¼������;
//      -5 -- �ļ�ϵͳ�޷���װ���豸;
//      -6 -- �ڴ治��;
//��ע:
//-----------------------------------------------------------------------------
s32 Mount(const char *MountPath, const char *DevPath, const char *Type, void *Private)
{
    struct FileSysType *FSType;
    struct MountInfo *NewMountInfo;
    struct VCommon *FileCom;
    struct FileContext *FileCt;
    struct DjyDevice *Dev;
    struct VFile *DevFp = NULL, *DirFp = NULL;
    s32 Ret = 0;


    if(!MountPath || !DevPath || !Type)
        return (-1);

    FSType = FindFSType(Type);
    if(!FSType)
        return (-2);// Ŀ���ļ�ϵͳ������δע��

    DevFp = __Open(DevPath, M_READ | M_WRITE | M_OPEN);// ���豸
    if (!DevFp)
        return (-3);

    FileCt = (struct FileContext *)(DevFp->Private);
    Dev = (struct DjyDevice *)(FileCt->Private);

    if(Get(&Dev->Node))// �豸�������ļ�ϵͳ�ˣ��Ӵ˲��ܸ��豸ɾ���������ļ�ϵͳж��
    {
        // ���������������ǲ�����ֵ�
        Ret = -3;
        goto FAIL;
    }

    DirFp = __Open(MountPath, M_READ | M_DIR | M_OPEN);// �򿪹��ص�Ŀ¼
    if(!DirFp)
    {
        Ret = -4;
        goto FAIL;
    }

    // ���ص�����У��
    FileCt = (struct FileContext*)(DirFp->Private);
    FileCom = FileCt->CHost;
    if(!(FileCom->Obj.Type & OB_DIR ))// DIR�Ѿ���O_MOUNTҲ������
    {
        Ret = -4;
        goto FAIL;
    }

    NewMountInfo = malloc(sizeof(struct MountInfo));
    if(!NewMountInfo)
    {
        Ret = -6;
        goto FAIL;
    }

    NewMountInfo->Type = FSType;
    NewMountInfo->Dev = &(Dev->Node);
    Ret = FSType->Install(NewMountInfo, Private);// �����ļ�ϵͳ
    if(Ret)
    {
        Ret = -5;
        goto FAIL;
    }

    // ��ʼ����װ��
    NewMountInfo->Mount.OldObj = FileCom->Obj.Child;
    NewMountInfo->Mount.Context = FileCom->Context;
    if(NULL == NewMountInfo->Mount.Name)// �û��������Լ���install������ָ��������FAT
        NewMountInfo->Mount.Name = Dev->Node.Name;// ��װ����(Ŀǰ�о��ļ����ļ�ϵͳ���������豸����Ϊ�ļ�ϵͳ��������)

    if(FSType->Property == MOUNT_FILE)// ��ǰֻ֧���ļ����͵İ�װ
    {
        NewMountInfo->Mount.Private = (void *)FSType->FileOps;// �ļ�ϵͳ�ľ��������ʽ
        NewMountInfo->Mount.Property = MOUNT_FILE;
        NewMountInfo->Mount.VOps = &g_tFOps;

        if(FileCom->Obj.Type & OB_MOUNT)
            NewMountInfo->Mount.Property |= MOUNT_ON_MOUNT;
        else
            NewMountInfo->Mount.Property |= MOUNT_ON_DIR;

        Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);
        FileCom->Context = &(NewMountInfo->Mount);// File����������������Ϊ�µ�Mount
        FileCom->Obj.Type |= OB_MOUNT;
        FileCom->Obj.Child = NULL;
        Lock_MutexPost(g_ptVFMutex);

        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
        if(FSType->MountList)// �µĹ�����Ϣ����
        {
            NewMountInfo->Next = FSType->MountList;
            FSType->MountList = NewMountInfo;
        }
        else
        {
            FSType->MountList = NewMountInfo;// ����ĵ�һ��
            NewMountInfo->Next = NULL;
        }
        Lock_MutexPost(g_ptFSTypeMutex);
    }

FAIL:
    if(DirFp)
        fclose(DirFp);
    if(DevFp)
        fclose(DevFp);


    return (Ret);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����: -1 -- ��װ�㲻����; -2 -- �ļ�ϵͳ�����ļ���ʹ��;
//      -3 -- ���ش���,���ڰ�װ��Ϣ��������;
//      -4 -- �豸��Ϣ����
//��ע: ��ǰֻ֧�ְ�װ��·��ж��
//-----------------------------------------------------------------------------
s32 Unmount(const char *Path)
{
    struct VFile *Fp;
    struct VCommon *FileCom;
    struct FileContext *FileCt;
    struct VMount *Mount;
    struct FileSysType *FSType;
    struct MountInfo *Temp;
    struct MountInfo *MountInfo;

    s32 Ret = 0;

    Fp = __Open(Path, M_READ | M_DIR | M_OPEN);
    if(!Fp)
        return -1;

    FileCt = (struct FileContext*)Fp->Private;
    FileCom = FileCt->CHost;

    if(!(FileCom->Obj.Type & OB_MOUNT))// ����У��
    {
        Ret = -1;
        goto FAIL;
    }


    if(FileCom->Obj.Child == NULL)
    {
        Mount = (struct VMount*)FileCom->Context;

        Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);
        FileCom->Obj.Child = Mount->OldObj;// �ָ�ԭ�ļ�ϵͳ�µ�����
        FileCom->Context = Mount->Context;
        if(Mount->Property == MOUNT_ON_DIR)
            FileCom->Obj.Type &= (~OB_MOUNT);// û�к���MOUNT
        Lock_MutexPost(g_ptVFMutex);
    }
    else
    {
        Ret = -2;
        goto FAIL;// �ļ�ϵͳ�����ļ�����ʹ����
    }

    MountInfo = Container(Mount, struct MountInfo, Mount);

    FSType = MountInfo->Type;

    // ����ǰ��װ������������ļ�ϵͳ���͵�������ɾ��
    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);

    Temp = FSType->MountList;

    if(MountInfo == Temp)
        FSType->MountList = MountInfo->Next;// ��һ��
    else
    {
        while((Temp->Next) && (Temp->Next != MountInfo))
            Temp = Temp->Next;
        if(Temp->Next)
            Temp->Next = MountInfo->Next;
        else// ���������һ����Ϊ���ֱ���װ����Ϣ���϶���ϵͳ��������,�����Ȳ��ͷű��ṹ
        {
            Ret = -3;
            Lock_MutexPost(g_ptFSTypeMutex);
            goto FAIL;
        }
    }

    Lock_MutexPost(g_ptFSTypeMutex);

    if(FSType->Uninstall(MountInfo->Private))
        return (-4);

    Put(MountInfo->Dev);

    free(MountInfo);

FAIL:
    if(Fp)
        fclose(Fp);

    return (Ret);
}

//-----------------------------------------------------------------------------
//����: ע���ļ�ϵͳ����
//����:
//����: 0  -- �ɹ�;
//      -1 -- �����������; -2 -- Ŀ���ļ�ϵͳ�����Ѵ���;-3 -- �ڴ治��;
//��ע:
//-----------------------------------------------------------------------------
s32 RegisterFSType(struct FileSysType *NewType)
{
    struct FileSysTypeInfo *New;

    if(NULL == NewType)
        return (-1);

    if(FindFSType(NewType->Name))
        return (-2);

    New = malloc(sizeof(struct FileSysTypeInfo));
    if(NULL == New)
        return (-3);
    New->Type = NewType;

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(s_ptFSTypeList)
    {
        New->Next = s_ptFSTypeList->Next;
        s_ptFSTypeList = New;
    }
    else
    {
        New->Next = NULL;
        s_ptFSTypeList = New;
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    return (0);
}
//-----------------------------------------------------------------------------
//����: ע���ļ�ϵͳ����
//����:
//����: 0  -- �ɹ�;
//      -1 -- �����������; -2 -- �ļ�ϵͳ�����Ѵ���;-3 -- Ŀ���ļ�ϵͳ������;
//��ע:
//-----------------------------------------------------------------------------
s32 UnRegisterFSType(const char *Type)
{
    struct FileSysTypeInfo *Cur, *Pre;
    char *Name;

    if(NULL == Type)
        return (-1);

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(!s_ptFSTypeList)
    {
        Lock_MutexPost(g_ptFSTypeMutex);
        return (-2);
    }

    Cur = Pre = s_ptFSTypeList;
    while(1)
    {
        Name = Cur->Type->Name;
        if(!strcmp(Name, Type))
        {
            if(Cur == s_ptFSTypeList)
                s_ptFSTypeList = NULL;
            else
                Pre->Next = Cur->Next;
        }
        else
        {
            if(!Cur->Next)
                return (-3);
            Pre = Cur;
            Cur = Cur->Next;
        }
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    free(Cur);

    return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����: ��NULL -- �ɹ�;
//      NULL -- ����������� �� �ļ�ϵͳ���Ͳ�����;
//��ע:
//-----------------------------------------------------------------------------
static struct FileSysType *FindFSType(const char *Type)
{
    struct FileSysTypeInfo *Cur;
    struct FileSysType *Ret = NULL;
    char *Name;

    if(NULL == Type)
        return (NULL);

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(s_ptFSTypeList)
    {
        Cur = s_ptFSTypeList;
        while(1)
        {
            Name = Cur->Type->Name;
            if(0 == strcmp(Name, Type))
            {
                Ret = Cur->Type;
                break;// �ҵ�
            }
            else if(NULL == Cur->Next)
            {
                break;// ���һ���ڵ���
            }
            Cur = Cur->Next;
        }
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    return (Ret);

}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
s32 Get(struct Object *Obj)
{
    s32 Ret = 0;
    atom_low_t   ALock;

    ALock = Int_LowAtomStart();
    if(NULL == Obj)
        Ret = -1;
    else if(Obj->Inuse == -1)
        Ret = -2;
    else
        Obj->Inuse++;
    Int_LowAtomEnd(ALock);
    return (Ret);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
void Put(struct Object *Obj)
{
    if(Obj)
        Obj->Inuse--;
}
//-----------------------------------------------------------------------------
//����: ���û�������,ϵͳ��
//����:
//����: 0 -- �ɹ�; -1 -- Ŀ¼�޷���; -2 -- Ŀ¼�޷�ʹ��;
//��ע:
//-----------------------------------------------------------------------------
s32 SetRoot(const char *Path)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    s32 Ret;
    struct VFile *Dir = __Open(Path, M_READ | M_DIR | M_OPEN);// ��Ŀ¼�������𵽷�ֹ�ļ�������ɾ����Ч��

    if(NULL == Dir)
        return -1;

    FileCt = (struct FileContext *)(Dir->Private);
    FileCom = FileCt->CHost;
    Ret = Get(&(FileCom->Obj));
    if(!Ret)
    {
        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // ������
        Put(g_tSysRoot.Obj);
        g_tSysRoot.Obj = &(FileCom->Obj);
        g_tSysRoot.VOps = Dir->VOps;
        Lock_MutexPost(g_ptFSTypeMutex); // ������
    }

    fclose(Dir);// �ر��ļ�
    return (Ret);
}
//-----------------------------------------------------------------------------
//����: ���û�������,ϵͳ��ǰ����·��
//����:
//����: 0 -- �ɹ�; -1 -- Ŀ¼�޷���; -2 -- Ŀ¼�޷�ʹ��;
//��ע:
//-----------------------------------------------------------------------------
s32 SetPWD(const char *Path)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    s32 Ret;
    struct VFile *Dir = __Open(Path, M_READ | M_DIR | M_OPEN);// ��Ŀ¼�������𵽷�ֹ�ļ�������ɾ����Ч��

    if(NULL == Dir)
        return (-1);

    FileCt = Dir->Private;
    FileCom = FileCt->CHost;
    Ret = Get(&(FileCom->Obj));
    if(!Ret)
    {
        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // ������
        Put(g_tSysPWD.Obj);
        g_tSysPWD.Obj = &(FileCom->Obj);
        g_tSysPWD.VOps = Dir->VOps;
        g_tSysPWD.Path = __PWDFixRootPath(g_tSysPWD.Obj);
        g_tSysPWD.FileOps = FileCt->FileOps;
        Lock_MutexPost(g_ptFSTypeMutex); // ������
    }

    fclose(Dir);

    __WorkingPath(g_pWokingPath); // ����shell

    return (Ret);
}

//-----------------------------------------------------------------------------
//����: ��ȡ��ǰ����·���ַ�������(��������)
//����:
//����: ����-- �ɹ�; �� -- ʧ�ܣ�δ���õ�ǰ����·��;
//��ע:
//-----------------------------------------------------------------------------
s32 CurWorkPathLen(void)
{
#if 0
    struct Object *Obj;
    u32 PathLen = 0;
    u32 PathSegment = 0;

    Obj = g_tSysPWD.Obj;
    if(NULL == Obj)
        return (0); // ��ǰ����·��δ����

    for(;;)
    {
        PathLen += strlen(OBJ_Name(Obj));
        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
            break;
        PathSegment++;
    }

    if(0 == PathSegment)
        PathSegment = 1; // �˴����龰��"/"Ϊ��ǰ·��,��1��ʾ�ַ���������

    return (PathLen + PathSegment);
#else
    return (__PathLen(g_tSysPWD.Obj, 0));
#endif
}


//-----------------------------------------------------------------------------
//����: ����·���ַ�������(��������)
//����: Obj -- ׷�ݵ���ʼ�ڵ�
//      Opts -- 0 -- ���ݵ����ڵ����
//              1 -- ���ݵ���һ�����ص����
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 __PathLen(struct Object *Obj, u8 Opts)
{
    struct VCommon *FileCom;
    struct VMount *Mount;
    u32 PathLen = 1;
    u32 PathSegment = 0;

    if(NULL == Obj)
        return (0); // ��ǰ����·��δ����

    for(;;)
    {
        PathLen += strlen(OBJ_Name(Obj));
        Obj = OBJ_Parent(Obj);
        switch(Opts)
        {
            case 0: if(NULL == Obj)
                    {
                        if(PathSegment)
                            PathSegment--; // ȥ����Ŀ¼�ظ�ͳ��
                        goto DONE;
                    }
                    else
                        break;
            case 1: if(OB_MOUNT & Obj->Type)
                    {
                        FileCom = Container(Obj, struct VCommon, Obj);
                        Mount = (struct VMount*)(FileCom->Context);
                        PathLen += strlen(Mount->Name);
                        if((0 == PathSegment) &&
                           (('/' != Mount->Name[0]) && ('\0' != Mount->Name[1]))) // �ų���Ŀ¼�߼�
                            PathSegment++; // �龰,���ص��һ��Ŀ¼��
                        goto DONE;
                    }
                    else
                        break;
            default: goto DONE;
        }
        PathSegment++;
    }

DONE:
    return (PathLen + PathSegment);
}

//-----------------------------------------------------------------------------
//����: ��ȡ��ǰ����·��
//����: Buf -- ��ǰ����·��
//      BufSize -- ·�����ȣ������ַ���������
//����: 0 -- �ɹ�; -1 -- δ���õ�ǰ����·��; -2 -- ��������; -3 -- �ڴ治��;
//      -4 -- ����;
//��ע:
//-----------------------------------------------------------------------------
s32 CurWorkPath(char *Buf, u32 BufSize)
{
    struct Object *Obj;
    u32 ObjNameLen, Offset;
    char *PathTemp, *ObjName;
    s32 Ret = 0;

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // ������(��ֹ�������̵�ǰ����·��������)
    Offset = CurWorkPathLen();
    if(0 == Offset)
    {
        Ret = -1; // ��ǰ·��δ����
        goto FAIL;
    }

    if((NULL == Buf) || (BufSize < Offset))
    {
        Ret = -2; // ��������
        goto FAIL;
    }

    PathTemp = (char*)malloc(Offset);
    if(NULL == PathTemp)
    {
        Ret = -3; // �ڴ治��
        goto FAIL;
    }

    Offset = Offset - 1;
    PathTemp[Offset] = '\0'; // ·���Ľ�����
    Obj = g_tSysPWD.Obj; // ·�����һ���ڵ�

    for(;;)
    {
        ObjName = OBJ_Name(Obj);
        ObjNameLen = strlen(ObjName);
        Offset = Offset - ObjNameLen;
        memcpy((PathTemp + Offset), ObjName, ObjNameLen);
        if(Offset) // ȥ���������
        {
            Offset--;
            PathTemp[Offset] = '/'; // ·��֮��ķָ������Ǹ�
        }

        if(0 == Offset)
        {
            strcpy(Buf, PathTemp);
            break; // ����
        }
        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
        {
            Ret = -4;
            break;
        }
    }

FAIL:
    if(PathTemp)
        free(PathTemp);
    Lock_MutexPost(g_ptFSTypeMutex); // ������
    return (Ret);
}
//-----------------------------------------------------------------------------
//����: ����shell��ӡ��ǰ����Ŀ¼
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
s32 __WorkingPath(char *Path)
{
    u16 Len;

    Len = CurWorkPathLen();
    if(257 < Len)
        return (-1); // ·������

    if(CurWorkPath(Path, Len))
        return (-1);

    return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
#if 0
struct Object *GetMountByName(struct FileSysType *FSType, const char *MountName)
{
    struct MountInfo *MountInfo = FSType->MountList;

    while(MountInfo)
    {
        if(strcmp(MountInfo->Mount.Name, MountName))
            MountInfo = MountInfo->Next;
        else
            return (MountInfo->Dir);
    }
    return (NULL);
}
#endif
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static char *__PWDFixRootPath(struct Object *Obj)
{
    struct VCommon *FileCom;
    struct VMount* Mount;
    char *ObjName;
    u16 ObjNameLen;
    u32 Len;
    char *PathTemp;

    if(OB_MOUNT & Obj->Type)
        return (NULL); // todo:��ǰ�ڵ��ǹ��ص�ʱ,��ִ��,ϵͳ�������߼��ɸ���

    Len = __PathLen(Obj, 1);
    PathTemp = s_cPWDRoot;
    Len = Len - 1;
    PathTemp[Len] = '\0'; // ·���Ľ�����

    for(;;)
    {
        if(OB_MOUNT & OBJ_Type(Obj))
        {
            FileCom = Container(Obj, struct VCommon, Obj);
            Mount = (struct VMount*)(FileCom->Context);
            ObjName = Mount->Name;
        }
        else
        {
            ObjName = OBJ_Name(Obj);
        }
        ObjNameLen = strlen(ObjName);
        Len = Len - ObjNameLen;
        memcpy((PathTemp + Len), ObjName, ObjNameLen);
        if(Len)
        {
            Len--;
            PathTemp[Len] = '/'; // ·��֮��ķָ������Ǹ�
        }

        if(0 == Len)
            break; // ����

        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
        {
            return (NULL); // ��Ӧ�ó����������
        }
    }

    return(PathTemp);
}
//-----------------------------------------------------------------------------
//����: ��ʽ���ļ�ϵͳ
//����:
//����: -1 -- ��������; -2 -- �ļ�ϵͳ�����ļ����ڱ�ʹ��; -3 -- ��ʽ��ʧ��;
//      0 -- �ɹ�;
//��ע:
//-----------------------------------------------------------------------------
s32 Format(const char *MountPath)
{

    struct VFile *Fp;
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct VMount *Mount;
    struct MountInfo *MountInfos;
    struct FileSysType *Type;
    s32 Ret = -2;

    if(!MountPath)
        return (-1);

    Fp = __Open(MountPath, M_READ | M_DIR | M_OPEN);
    if(!Fp)
        return (-1);

    FileCt = (struct FileContext*)(Fp->Private);
    FileCom = FileCt->CHost;
    if(!(FileCom->Obj.Type & OB_MOUNT))
        return (-1);

    if(NULL == OBJ_Child(&FileCom->Obj))
    {

        Mount = (struct VMount*)FileCom->Context;
        MountInfos = Container(Mount, struct MountInfo, Mount);
        Type = MountInfos->Type;
        if(Type && Type->Format)
            Ret = Type->Format(MountInfos->Private);
        else
            Ret = -3; // ����δע��;
    }

    fclose(Fp);
    return (Ret);
}
