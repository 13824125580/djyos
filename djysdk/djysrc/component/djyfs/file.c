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
#include <stdlib.h>
#include <stddef.h>
#include <djyos.h>
#include <systime.h>
#include "file.h"

//-----------------------------------------------------------------------------
//����: ģʽת��
//����: mode -- ģʽ
//����: ��0 -- �ɹ�; 0 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
static u32 __ToMode(const char *mode)
{
	u32 Mode;
	u8 StrEnd = 0; // �ַ���������־

	Mode = M_OPEN; // ���봴���ɹ��棬todo: ֧����������

	switch(mode[0])
	{
		case 'r': Mode |= M_READ;break;
		case 'w': Mode |= M_WRITE | M_CREATE | M_TRUNCATE;break;
		case 'a': Mode |= M_WRITE | M_CREATE | M_APPEND;break;
		default : return (0); // ������ʽ����
	}

	switch(mode[1])
	{
		case '\0': Mode |= M_TEXT; StrEnd = 1;break;
		case 'b': Mode |= M_BIN;break;
		case '+': Mode |= M_WRITE | M_READ;break;
		default : return (0); // ������ʽ����
	}

	if(!StrEnd)
	{
		switch(mode[2])
		{
			case '\0':
				if(!(Mode & M_BIN))
					Mode |= M_TEXT; // todo
				break;
			case 'b':
				if('b' == mode[1])
					return (0); // ��������'b', ���ʽ����
				Mode |= M_BIN;
				break;

			case '+':
				if('+' == mode[1])
					return (0); // ��������'+', ���ʽ����
				Mode |= M_WRITE | M_READ;
				break;
			default : return (0); // ������ʽ����
		}
	}

	return (Mode);

}
//-----------------------------------------------------------------------------
//����: ���ļ�
//����: filename -- �ļ�����·��;
//      mode -- �ļ�����ģʽ;
//����: ��NULL -- �ɹ�; NULL -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
FILE *fopen(const char *filename, const char *mode)
{
	u32 Mode;
	
	if((!filename) || (!mode)  || ('\0' == *filename))
		return (NULL);

	Mode = __ToMode(mode);
	if(Mode)
		return(__Open(filename, Mode));
	else
		return (NULL);
}
//-----------------------------------------------------------------------------
//����: �ر��ļ�
//����: stream -- �ļ���
//����: 0 -- �ɹ�;
//      -1 -- ��������; -2 -- �ļ����ʹ���; ����;
//��ע: todo:�ݴ���,�龰(�ظ��ر�һ���ļ�).
//-----------------------------------------------------------------------------
int fclose(FILE *stream)
{
	s32 Ret;
	
	if(NULL == stream)
		return (-1);

	Ret = stream->VOps->VClose(stream->Private);
	if(!Ret)
		FreeVFile(stream);// �ɹ����ͷž��.

	return (Ret);
}
//-----------------------------------------------------------------------------
//����: ���ļ�
//����: ptr -- ������;
//      size -- ��Ҫ���ĵ�λ��;
//      nmemb -- ��λ��;
//      stream -- �ļ���;
//����: 0 -- ʧ��; >0 -- �ɹ��������ֽ���;
//��ע:
//-----------------------------------------------------------------------------
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	s32 Ret;

	if((NULL == ptr) || (NULL == stream))
		return (0);
	
	if(0 == (size * nmemb))
		return (0);

	Ret = stream->VOps->VRead(ptr, size, nmemb, stream->Private);
	if(Ret < 0)
		return (0);

	return ((size_t)Ret);
}
//-----------------------------------------------------------------------------
//����: д�ļ�
//����: ptr -- ������;
//      size -- ��Ҫд�ĵ�λ��;
//      nmemb -- ��λ��;
//      stream -- �ļ���;
//����:  0 -- ʧ��; >0 -- ʵ��д����ֽ���
//��ע:
//-----------------------------------------------------------------------------
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	s32 Ret;

	if((NULL == ptr) || (NULL == stream))
		return (0);
	
	if(0 == (size * nmemb))
		return (0);
		
	Ret = stream->VOps->VWrite(ptr, size, nmemb, stream->Private);
	if(Ret < 0)
		return (0);

	return((size_t)Ret);
}
//-----------------------------------------------------------------------------
//����: �ƶ���дλ��
//����: stream -- �ļ���;
//      offset -- ���ƶ����ƫ��;
//      whence -- offset�����;
//����: 0 -- �ɹ�; 
//      -1 -- �ļ����ʹ���; -2 -- ��������; -3 -- �ļ�д�����;
//      -4 -- �ļ�ϵͳ��֧��seek;
//��ע: ��֧���豸�ļ�
//-----------------------------------------------------------------------------
int fseek(FILE *stream, long int offset, int whence)
{
	struct FileContext *FileCt;
	struct VCommon *FileCom;
	struct FileBuf *Buf;
	struct FileOperations *FileOps;
	u32 Length;
	s32 Front, Back;

	if(NULL == stream)
		return (-2);
		
	FileCt = (struct FileContext*)stream->Private;
	FileCom = FileCt->CHost;
	if(FileCom->Obj.Type != OB_REG)
		return (-1);// �������ļ�
	
	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileSeek))
		return (-4);
	
	Buf = FileCt->Buf;	
	if(Buf && (BUF_DIRTY == Buf->Status))
	{
		switch(whence)
		{
			case V_SEEK_SET:
			case V_SEEK_END:	
			{
				Length = Buf->Current - Buf->Start;
				if((NULL == FileOps->FileWrite) || 
				   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
					return (-3);
				Buf->Current = Buf->Start;
				Buf->Status = BUF_CLEAN;
				break;
			}	
			case V_SEEK_CUR:
			{
				Front = Buf->Start - Buf->Current;// Buffer���ÿռ�
				Back = Buf->End - Buf->Current;// Bufferδ�ÿռ�
				if((offset >= Front) && (offset <= Back))// ��seek��λ����Buffer��
				{
					Buf->Current += offset;
					return (0);
				}
				else// ��seek��λ�ó���Buffer��Χ,��Buffer������д���豸
				{
					Length = Buf->Current - Buf->Start;
					if((NULL == FileOps->FileWrite) || 
					   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
						return (-3);
					Buf->Current = Buf->Start;
					Buf->Status = BUF_CLEAN;
				}
				break;
			}	
			default:
				return (-2);
		}
	}	
		
	return (FileOps->FileSeek(FileCt, offset, whence));	
}
//-----------------------------------------------------------------------------
//����: ɾ���ļ���Ŀ¼
//����: filename -- �ļ�����·��;
//����: 0 -- �ɹ�; -1 -- ��������; -2 -- �ļ��޷�ɾ��; -3 -- ����;
//��ע: ��֧���豸�ļ�
//-----------------------------------------------------------------------------
int remove(const char *filename)
{
	struct VCommon *FileCom;
	struct PathWalk PathContext;
	struct FileOperations *FileOps;
	u8 IsFound;
	
	if((NULL == filename) || ('\0' == *filename))
		return (-1);

	__PathInit(&PathContext, filename);
	
	IsFound = __VPathLookup(&PathContext, 0);
	
	
	if(IsFound)
	{
		FileCom = Container(PathContext.End, struct VCommon, Obj);
		if(FileCom->Obj.Child || FileCom->Obj.Inuse || FileCom->Context)
			return (-2);// �ļ������ӽڵ�����ڱ�ʹ�ã����ܱ�ɾ��
	}

	FileOps = __FileOps(&PathContext);

	if(NULL == FileOps->FileDelete)
		return (-2);// ���ļ�ϵͳ��֧�ֱ�����
	
	return (FileOps->FileDelete(PathContext.Path, PathContext.Root));
}
//-----------------------------------------------------------------------------
//����: ���ļ�����������д���ļ������豸
//����: stream -- �ļ���;
//����: 0 -- �ɹ�; -1 -- �����������; -2 -- ������֧��; -3 -- ��������;
//��ע: ��֧���豸�ļ�
//-----------------------------------------------------------------------------
int fflush(FILE *stream)
{
	struct FileContext *FileCt;
	struct VCommon *FileCom;
	struct FileBuf *Buf;
	struct FileOperations *FileOps;
	u32 Length;
	
	if(NULL == stream)
		return (-1);
	
	FileCt = (struct FileContext*)stream->Private;
	FileCom = FileCt->CHost;
	
	if(FileCom->Obj.Type != OB_REG)
		return (-1);// �����ļ����ͣ�Ӧ�÷���EOF
		
	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileSync))
		return (-2);
	
	Buf = FileCt->Buf;
	if(Buf && (BUF_DIRTY == Buf->Status))// �Ƚ������Ļ���ˢ���豸
	{
		Length = Buf->Current - Buf->Start;
		if((NULL == FileOps->FileWrite) || 
		   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
			return (-3);// д��ʧ��
		Buf->Current = Buf->Start;
		Buf->Status = BUF_CLEAN;
	}
		
	return (FileOps->FileSync(FileCt));	
}
//-----------------------------------------------------------------------------
//����: ���ļ�
//����:
//����: ��NULL -- �ɹ�; NULL -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
FILE *freopen(const char *filename, const char *mode, FILE *stream)
{
	struct FileContext *FileCt;
	u32 NewProperty = 0;
	u32 Mode;
	u8 IsFound;
	struct PathWalk PathCt;
	s32 Ret = -1;

	if(((NULL == filename) && (NULL == stream)) || (NULL == mode))
		return (NULL);

	if(filename)
	{
		Mode = __ToMode(mode);
		if(0 == Mode)
			return (NULL);

		__PathInit(&PathCt, filename);

		IsFound = __VPathLookup(&PathCt, 1); // �����м���

		if(PathCt.VOps->VOpen) // ϵͳ�ļ���֧�ָú���
			Ret = PathCt.VOps->VOpen(&PathCt, Mode, IsFound);

		if(Ret < 0)
			return (NULL);

		if((stream->VOps)&&(stream->VOps->VClose)) // �ر�stream������
			stream->VOps->VClose(stream->Private);

		if(PathCt.PrivateVHost)
			*(PathCt.PrivateVHost) = stream;
		stream->Private = PathCt.Private; // ʵ����FileContext
		stream->VOps = PathCt.VOps;

		return (stream);
	}

	// δָ�����ļ�����ֱ�ӱ���ļ�����,todo:���ĵײ�����
	FileCt = (struct FileContext *)stream->Private;

	switch(mode[0])
	{
		case 'r': NewProperty |= P_READ;break;
		case 'w': NewProperty |= P_WRITE;break;
		case 'a': NewProperty |= P_WRITE;break;
		default : return (NULL);// ������ʽ����
	}

	switch(mode[1])
	{
		case '\0':
		case 'b' : break;
		case '+' : NewProperty |= P_WRITE | P_READ;break;
		default  : return (NULL);// ������ʽ����
	}

	if('\0' != mode[1])
	{
		switch(mode[2])
		{
			case '\0':
			case 'b' :
				if('b' == mode[1])
					return (NULL);// ��������'b', ���ʽ����
				break;

			case '+' :
				if('+' == mode[1])
					return (NULL);// ��������'+', ���ʽ����
				NewProperty |= P_WRITE | P_READ;
				break;

			default  : return (NULL); // ������ʽ����
		}
	}

	FileCt->Property &= ~(P_WRITE | P_READ);
	FileCt->Property |= NewProperty;
	return (stream);
}
