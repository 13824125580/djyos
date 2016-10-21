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
#ifndef __VFILE_H__
#define __VFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>   //added by zqf
#include <object.h>
//
// �ⲿ����,������fs_module.c
//
extern struct VFilePool *g_ptVFilePool;
extern struct MutexLCB *g_ptVFilePoolMutex;
extern struct MutexLCB *g_ptVFMutex;
extern struct MutexLCB *g_ptFSTypeMutex;
extern struct SysEnv g_tSysRoot, g_tSysPWD;
extern struct VOperations g_tFOps;

//
// �ļ�ϵͳ������룬��enum������cn_fs_error��ʼ������1.
//
enum _FS_ERROR_CODE_
{
    EN_FS_NO_ERROR = CN_FS_NO_ERROR,    //û�д���
    EN_FS_NAME_INVALID_CHAR,            //���ֺ��зǷ��ַ�
    EN_FS_NAME_LEN_ERROR,               //���ֳ��ȴ���
    EN_FS_OBJECT_OPENED,                //�����Ѿ���
    EN_FS_OBJECT_NONENTITY,             //���󲻴���
    EN_FS_OBJECT_READONLY,              //ֻ������
    EN_FS_FOLDER_UNBLANK,               //Ŀ¼�ǿ�
    EN_FS_CREAT_PTT_ERROR,              //������������
    EN_FS_CREAT_ROOT_FOLDER_ERROR,      //������Ŀ¼����
    EN_FS_FORMAT_PTT_ERROR,             //��ʽ������
    EN_FS_REMOVE_ERROR,                 //ɾ���������
    EN_FS_PTT_ERROR,                    //ɾ����������
    EN_FS_RENAME_ERROR,                 //�����������
    EN_FS_PTT_NAME_REPEAT,              //������������
    EN_FS_PTT_NO_FORMATED,              //����δ��ʽ��
    EN_FS_READFILE_ERROR,               //���ļ�����
    EN_FS_READFILE_EOF                  //���ļ������ļ�����
};

//
//
//
struct Stat
{
    long long       Size;// �ļ���С
    unsigned int    Property;// �ļ�����
};

//
// �ļ�����ģʽMode
//
#define M_TYPE              (u32)(0xF0000000)// MASK
#define M_REG               (u32)(0x10000000)// �ļ�
#define M_LINK              (u32)(0x20000000)// ����
#define M_DIR               (u32)(0x40000000)// Ŀ¼

        
#define M_READ              (u32)(0x1)// ��
#define M_WRITE             (u32)(0x2)// д
#define M_OPEN              (u32)(0x10)// ��(��ע:����c�����,���봴���ǿ��Թ����,��ioȴ���ܹ��档���M_OPENλ��ʾ�ļ��ǿ����Ѵ��ڵ�)  
#define M_CREATE            (u32)(0x100)// ����
#define M_TRUNCATE          (u32)(0x200)// �ضϣ���ͷ��ʼ��
#define M_APPEND            (u32)(0x800)// ׷�ӣ���β��ʼ��
#define M_TEXT              (u32)(0x4000)// �ı���ʽ
#define M_BIN               (u32)(0x8000)// �����Ƹ�ʽ

//
//
//
#define V_SEEK_SET          (0)
#define V_SEEK_CUR          (1)
#define V_SEEK_END          (2)
 
//
//
//
struct PathInfo; 
struct VFile;
struct VOperations;

//
//
//
struct VFile
{
    struct VOperations *VOps;
    void *Private;
};
//
//
//
#define VFILE_NUM           (128) 
#define VFILE_SIZE          (sizeof(struct VFile))
#define VFILE_POOL_SIZE     (VFILE_NUM * VFILE_SIZE) 

struct VFilePool
{
    struct VFile *Space; // VFile�ؿռ�
    u16 Used; // ��ʹ������
    u16 ID; // VFile��ID
    struct MemCellPool *Management; // VFile�ع���ṹ
    struct VFilePool *Next; // VFile������,��NULL��ʾ�������
};
 

//
// ·������ʱ��������
//
struct PathWalk
{
    char *Path; // �ļ�ϵͳ�ľ���·��������������Ŀ¼
    char *UncoveredPath; // δ������ļ�·��
    char *Root; // �ļ�ϵͳ�ĸ���
    struct Object *End; // ����·���Ľ����ļ�
    struct VOperations *VOps; // ����ֵ���ṩ��VFile
    void *FileOps; // ��ϵͳ�Ĳ�������������struct FileOperations
    void *Private; // ����ֵ���ṩ��VFile,��ǰΪstruct FileContext
    struct VFile **PrivateVHost; // ����Private��VFile֮���ӳ���ϵ
};
//
// Ŀ¼��
//
struct Dirent{
	char Name[257];
	u32 Property;
};
//
//
//
struct VOperations
{
    s32 (*VOpen)(struct PathWalk *Path, u32 Mode, u8 IsFound);
    s32 (*VClose)(void *Private);
    s32 (*VWrite)(const void *Buf, u32 Size, u32 Nmemb, void *Private);
    s32 (*VRead)(void *Buf, u32 Size, u32 Nmemb, void *Private);
    s32 (*VCntrl)(u32 Command, void *Param0, void *Param1, void *Private);// todo
};
//
//
//
struct FileOperations
{
    struct FileContext *(*FileOpen)(const char *Path, u32 Mode, const char *Root);
    s32 (*FileClose)(struct FileContext *FileCt);
    s32 (*FileWrite)(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
    s32 (*FileRead)(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
    s32 (*FileSeek)(struct FileContext *FileCt, s64 Offset, s32 whence);
    s32 (*FileDelete)(const char *Path, const char *Root);
    s32 (*FileSync)(struct FileContext *FileCt);
    s32 (*FileStat)(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
    s32 (*FileTruncate)(struct FileContext *FileCt, off_t NewSize);//todo: off_tǣ�浽C��������ʵ��Ӧ����long�ͣ�������ʵ���г�ͻ
	s32 (*DirRead)(struct FileContext *FileCt, struct Dirent *Content);
};
//
// �ļ��������Ľṹ
//
#define P_READ              (0x1)
#define P_WRITE             (0x2)
#define P_TYPE              (0xF0)// MASK
#define P_REG               (0x10)// �ļ�
#define P_DIR               (0x20)// Ŀ¼
#define P_LINK              (0x40)// ����
#define P_DEV               (0X80)// �豸
struct FileContextList
{
    struct FileContextList *Pre;
    struct FileContextList *Next;
};
struct FileContext
{
    struct VCommon *CHost;// ָ�򻺳��е�VCommon
    struct VFile *FHost;// ָ����
    struct FileBuf *Buf;// NULL��ʾ�����ļ�ϵͳ����
    void *Private; // ��ͬ�ļ�ϵͳ������
    void *FileOps; // �ļ��Ĳ���������,����struct FileOperations
    u32 Property;// �ļ�����,�ɾ����ļ�ϵͳ����
    struct FileContextList List; // ΪNULL��ʾͷ��β,�ļ��Ķ�δ򿪵�����
};
//
//
//
#define FS_BUFFERED        	(0x1)
#define BUF_DIRTY       	(0x00000001)
#define BUF_CLEAN       	(0x00000002)
#define BUF_SIZE            (512)
struct FileBuf
{
    u8 *Start, *Current, *End;
    u32 Status;
};

//
//
//
struct SysEnv
{
    struct VOperations *VOps;
    struct Object *Obj;
    char *Path; // ������ĳ���ļ�ϵͳ�ڲ�ʱ,ָ��ȫ·��(���ǵ����������ܲ�֧�ֵ�ǰ·���ȵ�����);
    struct FileOpertions *FileOps; // ������ĳ���ļ�ϵͳ�ڲ�ʱ,ָ���ļ��Ĳ�����ʽ(���ǵ����������ܲ�֧�ֵ�ǰ·���ȵ�����);
};


//
//
//
s32 FOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound);
s32 FClose(void *File);
s32 FWrite(const void *Buf, u32 Size, u32 Nmemb, void *Private);
s32 FRead(void *Buf, u32 Size, u32 Nmemb, void *Private);
//
//
//
void __PathInit(struct PathWalk *PathContext, const char *Path);
struct VFile *AllocVFile(void);
void FreeVFile(struct VFile *File);
struct VFilePool *__ExtendVFilePool(void);
struct VCommon *__CoverPath(const char *Path, struct Object *Obj);
u8 __VPathLookup(struct PathWalk *PathContext, u8 Open);
struct VFilePool *PoolOf(struct VFile *Fp);
struct VFile *__Open(const char *Path, u32 Mode);
struct VFile *__OpenUnlock(const char *Path, u32 Mode);
char *PathCopy(const char *Root, const char *RelPath, char(*Rebuild)(char *Path));
void FreeFileBuf(struct FileBuf *Buf);
s32 AllocFileBuf(struct FileBuf **BufRet, u8 Force);
struct FileContext *AllocContext(void);
void FreeContext(struct FileContext * FileCt);
void __FlushDir(struct VFile *Fp);
struct Object *__FileObj(struct VFile *Fp);
s32 __CacheDir(struct VFile *Fp);
void *__FileOps(struct PathWalk *PathCt);
s32 ReadDir(struct VFile *Dir, struct Dirent *DirentInfo);
bool_t IS_FILE(struct VFile *File);
bool_t IS_DEV(struct VFile *File);
struct DjyDevice *ToDev(struct VFile *Fp); 

#ifdef __cplusplus
}
#endif

#endif /* __VFILE_H__ */
