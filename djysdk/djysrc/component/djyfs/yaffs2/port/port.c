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
#include <stddef.h>
#include <djyfs/vfile.h>
#include "../yaffs2-583dbd9/yaffs2-583dbd9/direct/yaffsfs.h"

extern struct FileOperations g_tYAF2FileOps; // ./mount.c�ļ��ж���
//-----------------------------------------------------------------------------
//����:
//����: 
//����: !NULL -- �ɹ�; NULL -- ʧ��;
//��ע: 
//-----------------------------------------------------------------------------
struct FileContext *YAF2Open(const char *Path, u32 Mode, const char *Root)
{
	int YFlag = 0;
    int YMode = S_IREAD | S_IWRITE;
	char *YPath = PathCopy(Root, Path, NULL);
	struct FileContext *FileCt;
	struct FileBuf *Buf;
	
	FileCt = AllocContext();
	if(!FileCt)
		return (NULL);
		
	FileCt->FileOps = (void *)&g_tYAF2FileOps;

	if((Mode & M_READ) && (Mode & M_WRITE))
	{
		YFlag |= O_RDWR;
		FileCt->Property = P_READ | P_WRITE;
	}
	else if(Mode & M_READ)
	{
		YFlag |= O_RDONLY;
		FileCt->Property = P_READ;
	}
	else if(Mode & M_WRITE)
	{
		YFlag |= O_WRONLY;
		FileCt->Property = P_WRITE;
	}
	else
		goto FAIL;// ��������
	
	if(Mode & M_CREATE)
	{
		if(Mode & M_OPEN)
			YFlag |= O_CREAT;// ��������Ѵ���
		else
			YFlag |= O_CREAT | O_EXCL;// ������
	}
		
	if(Mode & M_TRUNCATE)
		YFlag |= O_TRUNC;
		
	if(Mode & M_APPEND)
		YFlag |= O_APPEND;	

	if(!(Mode & M_TYPE))// ����δָ������������Ҫ������
		Mode |= M_REG | M_DIR;
		
	if(Mode & M_REG)// �ļ�����
	{
		int Handle;
		Handle = yaffs_open(YPath, YFlag, YMode);
		if(Handle < 0)
			goto FAIL;//todo: �Ѵ����ļ��Ǻν��������

		if(AllocFileBuf(&Buf, 0))// Ϊ�ļ�����������
		{
			yaffs_close(Handle);
			if((Mode & M_CREATE) && (!(Mode & M_OPEN))) // ʧ����ɾ���Ѵ������ļ�
				yaffs_unlink(YPath);
			goto FAIL;
		}
		
		FileCt->Property |= P_REG;
		FileCt->Private = (void *)Handle;
		FileCt->Buf = Buf;
		return (FileCt);// �ɹ�
	}

	if(Mode & M_DIR)// Ŀ¼����
	{
		yaffs_DIR *Dir;
		if(YFlag & O_CREAT)
		{
			int Ret;
			Ret = yaffs_mkdir(YPath, YMode);
			if(Ret < 0)// �ļ��Ѵ���,���ʾ��������;��û�пռ�
				goto FAIL;
		}

		Dir = yaffs_opendir(YPath);
		if(NULL == Dir)
			goto FAIL;

		FileCt->Property |= P_DIR;
		FileCt->Private = (void *)Dir;
		FileCt->Buf = NULL;
		return (FileCt);// �ɹ�
	}

FAIL: //ʧ��
	FreeContext(FileCt);
	return (NULL);
		

}
//-----------------------------------------------------------------------------
//����:
//����: 
//����: 
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Close(struct FileContext *FileCt)
{
	int Ret;
	
	switch(FileCt->Property & P_TYPE)
	{
		case P_REG:
		{
			int File = (int)FileCt->Private;
			Ret = yaffs_close(File);
			if(!Ret)
			{
				FreeFileBuf(FileCt->Buf);
				break;// �ɹ�
			}
			return (-2);// ʧ��
		}
		case P_DIR:
		{
			yaffs_DIR *Dir = (yaffs_DIR*)FileCt->Private;
			Ret = yaffs_closedir(Dir);
			if(!Ret)
				break;// �ɹ�
			return (-2);// ʧ��
		}
		default:
			return (-2);// ʧ��
	}
	
	free(FileCt);
	return (0);
}
//-----------------------------------------------------------------------------
//����:
//����: 
//����: 
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Seek(struct FileContext *FileCt, s64 Offset, s32 Whence)
{
	Y_LOFF_T Ret;
	int Handle = (int)(FileCt->Private);
	
	Ret = yaffs_lseek(Handle, Offset, Whence);
    if (-1 == Ret)
		return (-1);
		
	return (0);
	
}
//-----------------------------------------------------------------------------
//����:
//����: 
//����: 0 -- �ɹ�; -3 -- ʧ��;
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Sync(struct FileContext *FileCt)
{
	int Ret;
	int Handle = (int)FileCt->Private;

	Ret = yaffs_flush(Handle);
	if(Ret)
		return (-3);
	return (0);
}
//-----------------------------------------------------------------------------
//����:
//����: 
//����: 0 -- �ɹ�; -3 -- ����;
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Delete(const char *Path, const char *Root)
{
	int Ret;
	char *YPath = PathCopy(Root, Path, NULL);
	u32 Length;
	Length = strlen(Path);
	
	// ���·���Ƿ�˵��,ɾ������Ŀ¼�����ļ�;���'/'��β����ʾɾ������Ŀ¼
	if(Path[Length] != '/')
	{
		Ret = yaffs_unlink(YPath);
		if(!Ret)
			return (0);// �ɹ�
	}
	
	Ret = yaffs_rmdir(YPath);
	if(!Ret)
		return (0);// �ɹ�
		 
	return (-3);
}

//-----------------------------------------------------------------------------
//����:
//����: Size -- ��֤��Ϊ��
//����: >0 -- �ɹ�; 0 -- ʧ��;
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Write(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	int Ret;
    int File = (int)FileCt->Private;
    u32 AllSize = Size * Nmemb;

    Ret = yaffs_write(File, Buf, AllSize);
	if(-1 == Ret)
		return (0);// ʧ��

    return (Ret/Size);
}
//-----------------------------------------------------------------------------
//����:
//����: Size -- ��֤��Ϊ��
//����: >0 -- �ɹ�; 0 -- ʧ��;
//��ע: 
//-----------------------------------------------------------------------------
s32 YAF2Read(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	int Ret;
    int File = (int)FileCt->Private;
    u32 AllSize = Size * Nmemb;

    Ret = yaffs_read(File, Buf, AllSize);
	if(-1 == Ret)
		return (0);// ʧ��

    return (Ret/Size);
}

//-----------------------------------------------------------------------------
//����: ��ѯ�ļ�״̬
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
s32 YAF2Stat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
	int Ret;
	int File;
	struct yaffs_stat FileInfo;
	char *YPath;

	if(FileCt)// �ļ��Ѿ��ڻ�����
	{
		Buf->Property = FileCt->Property;
		if(FileCt->Property & P_REG)
		{
			File = (int)FileCt->Private;
			Ret = yaffs_fstat(File, &FileInfo);
			if(-1 == Ret)
				return (-1);
			Buf->Size = (long long)FileInfo.st_size;
		}
		else
		{
			Buf->Property = FileCt->Property;
			Buf->Size = 0;
		}
		return (0);

	}

	// �ļ����ڻ�����
	YPath = PathCopy(Root, Path, NULL);
	Ret = yaffs_stat(YPath, &FileInfo);
	if(-1 == Ret)
		return (-1);

	Buf->Size = (long long)FileInfo.st_size;

	Buf->Property = P_READ | P_WRITE;
	if(FileInfo.st_mode & S_IFDIR)
	{
		Buf->Property |= P_DIR;
		Buf->Size = 0; // yaffs2��Ŀ¼��С��Ϊ��
	}
	else if(FileInfo.st_mode & S_IFREG)
		Buf->Property |= P_REG;
	else if(FileInfo.st_mode & S_IFLNK)
		Buf->Property |= P_LINK;

	return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע: todo: off_t���ʹ���
//-----------------------------------------------------------------------------
s32 YAF2Truncate(struct FileContext *FileCt, off_t NewSize)
{
	int File;

	File = (int)FileCt->Private;
	return(yaffs_ftruncate(File, (Y_LOFF_T)NewSize));// todo
}
//-----------------------------------------------------------------------------
//����: ��Ŀ¼����
//����:
//����: 1 -- ����,ȫ������;
//      -1 -- ��������; -2 -- ������֧��; -3 -- ��������;
//      0 -- �ɹ���ȡ����;
//��ע: ÿ��һ��,����һ��Ŀ¼��
//-----------------------------------------------------------------------------
// ��YAFFS2���ƵĽṹ��
struct yaffsfs_DirSearchContext {
    struct yaffs_dirent de;
    YCHAR name[NAME_MAX + 1];
    struct yaffs_obj *dirObj;
    struct yaffs_obj *nextReturn;
    struct list_head others;
    s32 offset:20;
    u8 inUse:1;
};
s32 YAF2DirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	 u32 FileType;
	 struct yaffs_dirent *De;
	 struct yaffsfs_DirSearchContext *Dsc;
	 yaffs_DIR *Dir = (yaffs_DIR*)FileCt->Private;

	 Dsc = (struct yaffsfs_DirSearchContext *)Dir;
	 if(Dsc->inUse && Dsc->nextReturn)
	 {
		 FileType = yaffs_get_obj_type((struct yaffs_obj *)Dsc->nextReturn);
		 switch(FileType)
		 {
		 	 case DT_REG: Content->Property = P_REG; break;
		 	 case DT_DIR: Content->Property = P_DIR; break;
		 	 case DT_LNK: Content->Property = P_LINK; break;
		 	 default : return (-3); // �������Ͳ�֧��
		 }
	 }

	 De = yaffs_readdir(Dir);
	 if(NULL == De)
		 return (1);

	 strcpy(Content->Name, De->d_name);

	 return (0);
}
