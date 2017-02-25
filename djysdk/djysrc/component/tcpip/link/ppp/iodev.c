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
//author:zhangqf
//date  :����4:59:55/2017��1��4��
//usage :
//------------------------------------------------------

//standard includes
#include "iodev.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <os.h>
//add your own specified header here
#include <driver.h>
#include <shell.h>
//-----------------------------------------------------------------------------
//����:weuse,0xthis,0xfunction,0xto open the serial device
//����:
//����:NULL open failed else the device handle
//��ע:
//����:zhangqf@����4:55:51/2017��1��5��
//-----------------------------------------------------------------------------
ptu32_t iodevopen(const char *name)
{
	ptu32_t result = 0;
	result = (ptu32_t)Driver_OpenDevice(name,O_RDWR,0);
	if(0 == result)
	{
		printf("%s:open %s failed\n\r",__FUNCTION__,name);
	}
	else
	{
		printf("%s:open %s success\n\r",__FUNCTION__,name);
	}
	return result;
}
#define CN_UART_RT_TIMEOUT (100*mS)

static u8 gDevIDebugMode = 0;  //0,no debug 1,in ascii mode,2 in hex mode :in debug
static u8 gDevODebugMode = 0;  //0,no debug 1,in ascii mode,2 in hex mode :out debug
//-----------------------------------------------------------------------------
//����:we use this function to read data from the serial device
//����:dev,device handle;buf,data buf to read;len,data len
//����:-1 if the read abort others  the read len
//��ע:do the block read
//����:zhangqf@����4:57:17/2017��1��5��
//-----------------------------------------------------------------------------
int iodevread(ptu32_t dev,u8 *buf,u32 buflen)
{

	int len = 0;
	len = Driver_ReadDevice((struct DjyDevice *)dev,buf,buflen,0,CN_UART_RT_TIMEOUT);
	if((len > 0)&&(gDevIDebugMode > 0))
	{
		time_t printtime;
		printtime = time(NULL);
		printf("[RCV:%s:%d bytes]",ctime(&printtime),len);
		for(int i = 0;i < len;i++)
		{
			if(gDevIDebugMode == 1)
			{
				printf("%c",buf[i]);
			}
			else if(gDevIDebugMode ==2)
			{
				printf("%02x ",buf[i]);
			}
		}
		printf("\n\r");
	}

	return len;
}
//-----------------------------------------------------------------------------
//����:we use this function to write data to the serial device
//����:dev,device handle;buf,data buf to write;len,data len
//����:
//��ע:do the block send
//����:zhangqf@����4:57:17/2017��1��5��
//-----------------------------------------------------------------------------
int iodevwrite(ptu32_t dev,u8 *buf,u32 len)
{
	Driver_WriteDevice((struct DjyDevice *)dev,buf,len,0,CN_BLOCK_BUFFER,CN_UART_RT_TIMEOUT);
	if(gDevODebugMode > 0)
	{
		time_t printtime;
		printtime = time(NULL);
		printf("[SND:%s:%d bytes]",ctime(&printtime),len);
		for(int i = 0;i < len;i++)
		{
			if(gDevODebugMode == 1)
			{
				printf("%c",buf[i]);
			}
			else if(gDevODebugMode ==2)
			{
				printf("%02x ",buf[i]);
			}
		}
		printf("\n\r");
	}
	return len;
}
//-----------------------------------------------------------------------------
//����:use this function to close the sio device
//����:
//����:
//��ע:
//����:zhangqf@����5:05:27/2017��1��5��
//-----------------------------------------------------------------------------
bool_t iodevclose(ptu32_t dev)
{
	Driver_CloseDevice((struct DjyDevice *)dev);
	return true;
}
//usage:this function to set the sdev debug mode
bool_t IoDevDebug(char *param)
{
	char *argv[2];
	int   argc=2;
	int   v1;

	string2arg(&argc,argv,param);
	if(argc == 2)
	{
		v1 = strtol(argv[0],NULL,NULL);
		if(v1 == 0)
		{
			gDevIDebugMode = strtol(argv[1],NULL,NULL);
		}
		else
		{
			gDevODebugMode = strtol(argv[1],NULL,NULL);
		}
	}
	return true;
}




