//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

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
// �����Ȩ�����ʹ���߷������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------
//����ģ��: Iboot
//����:  ������.
//�汾��V1.0.0
//�ļ�����: ��Ӧ�ñ�̵ĺ����ļ�
//����˵��:
//�޶���ʷ:
//1. ����: 2016-06-17
//   ����:  ������.
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------

#include "stdint.h"
#include "stdio.h"
#include "driver.h"
#include "IAP.h"
#include "shell.h"
#include "djyos.h"
#include <cfg/Iboot_config.h>
#include "verify.h"
#include <stdlib.h>
#include "string.h"
#include "version.h"
#include "cpu_peri.h"
#include "cpu-optional.h"
#if(CN_CPU_OPTIONAL_CACHE==1)
#include "set-cache.h"
#endif
#define CN_IBOOT_FLAG               "RunIboot"
#define CN_APP_CTRL_APP_FLAG        0x61707000     //�ַ���"app"
#define CN_APP_CTRL_DBG_FLAG        0x00676264    //�ַ���"dbg"
#define CN_APP_CTRL_RELEASE_FLAG    0x72656173    //�ַ���"reas"
extern char g_cIbootFlag[];
extern struct IbootCtrl gc_ptIbootCtrl;
extern void reset(void);

extern const char *g_pcIbootDir;


void  Sh_RunIBoot(char *param);
void  Sh_RunAPP(char *param);
void Sh_RunReleaseAPP(char *param);
void Sh_GetAPPInfor(char *param);
void Sh_GetStatus(char *param);
void Sh_GetIAPVersion(char *param);
void Sh_GetRunMode(char *param);


extern u8 g_IbootStaus;
u32 g_bRunModeFlag;

extern struct AppInfo   gc_ptAppInfo;
extern u32 g_pAppCodeStartRamAddr;
extern u32 g_pAppCodeEndRamAddr;


extern void __AppPreLoad(void);


u32 IAP_GetAPPStartCodeRamAddr(void)
{
	return ((u32)&g_pAppCodeStartRamAddr);
}
u32 IAP_GetAPPCodeRamSize(void)
{
   return ((u32)&g_pAppCodeEndRamAddr - (u32)&g_pAppCodeStartRamAddr);
}
u32 IAP_GetAPPStartAddr(void)
{
	return (u32)(&gc_ptAppInfo.RomStartAddr);
}

bool_t IAP_APPIsDebug(void)
{
	u8 *dbgflg;

	dbgflg = (u8*)&gc_ptIbootCtrl.flag;
	if( (dbgflg[0] == 'd') && (dbgflg[1] == 'b') && (dbgflg[2] == 'g') )
	{
		return true;
	}
	return false;
}
u32 IAP_GetAPPSize(void)
{
   return (gc_ptAppInfo.AppSize);
}
u32 IAP_GetAPPCRC(void)
{
    return (gc_ptIbootCtrl.Iap_crc);
}

void  Sh_RunIBoot(char *param)
{
	 g_cIbootFlag[0]=0x52;   //R
	 g_cIbootFlag[1]=0x75;   //u
	 g_cIbootFlag[2]=0x6E;   //n
	 g_cIbootFlag[3]=0x49;   //i
	 g_cIbootFlag[4]=0x62;   //b
	 g_cIbootFlag[5]=0x6F;   //o
	 g_cIbootFlag[6]=0x6F;   //o
	 g_cIbootFlag[7]=0x74;   //t
	 g_cIbootFlag[8]=0x0;    // \0
#if(CN_CPU_OPTIONAL_CACHE==1)
	 Cache_CleanData();
	 Cache_InvalidInst();
#endif
	 reset();
}

void  Sh_RunAPP(char *param)
{
	g_cIbootFlag[0]=0x52;   //R
	g_cIbootFlag[1]=0x75;   //u
	g_cIbootFlag[2]=0x6E;   //n
	g_cIbootFlag[3]=0x41;   //a
	g_cIbootFlag[4]=0x50;   //p
	g_cIbootFlag[5]=0x50;   //p
	g_cIbootFlag[6]=0x00;   // \0
	g_cIbootFlag[7]=0x00;
#if(CN_CPU_OPTIONAL_CACHE==1)
	 Cache_CleanData();
	 Cache_InvalidInst();
#endif
	reset();
}
void Sh_RunReleaseAPP(char *param)
{
//	gc_ptIbootCtrl.flag=CN_APP_CTRL_RELEASE_FLAG;
//#if(CN_CPU_OPTIONAL_CACHE==1)
//	 Cache_CleanData();
//	 Cache_InvalidInst();
//#endif
//	reset();
}

void Sh_GetAPPInfor(char *param)
{
    u32 size,crc32;
    char *filename;
	size=gc_ptIbootCtrl.AppSize;
	crc32=gc_ptIbootCtrl.Iap_crc;
    filename=gc_ptIbootCtrl.filename;
    printf("size:0x%08x,crc32:0x%08x,filename:%s.\r\n",size,crc32,filename);
}
//-----------------------------------------------------------------
void Sh_GetStatus(char *param)
{
	switch (g_IbootStaus)
	{
		case 0:
			printf("IAP status:No Err.\r\n");
			break;
		case 1:
			printf("IAP status:Force Run Iboot.\r\n");
			break;
		case 2:
			printf("IAP status:RAM Iboot flag.\r\n");
			break;
		case 3:
			printf("IAP status:Load from data mode.\r\n");
			break;
		case 4:
			printf("IAP status:Ecc Err.\r\n");
			break;
		case 5:
			printf("IAP status:APP Flag Err.\r\n");
			break;
		case 6:
			printf("IAP Err:APP File Not Exist Err.\r\n");
			break;
		case 7:
			printf("IAP Err:APP File Size Invalid.\r\n");
			break;
		case 8:
			printf("IAP Err:Iboot/App Ids MisMatch..\r\n");
			break;
		default:
			break;
	}
}

#define CN_VERSION_CHAR_LEN   100
void Sh_GetIAPVersion(char *param)
{
   char *str;
   str=malloc(CN_VERSION_CHAR_LEN);
   if(str!=NULL)
   {
	   str=djyos_kernel_version;
	   printf("Version:%s.\r\n",str);
   }
}
void Sh_GetRunMode(char *param)
{
	if(g_bRunModeFlag==0x12345678)
   {
		   printf("Run Mode:Iboot.\r\n");
   }
	else
		printf("Run Mode:APP.\r\n");
}


struct ShellCmdTab const shell_cmd_iap_table[]=
    {
		{
			"runiboot",
			Sh_RunIBoot,
        "�л���Iboot(���ڲ�ȡ�ڴ��ʾȷ��������Ŀ��APP�м���DJYOS shellģ��ʱ��Ч)",
        "COMMAND:runiboot+enter"
		},

		{
           "runapp",
		   Sh_RunAPP,
       "ֱ������APP(���ڲ�ȡ�ڴ��ʾȷ��������Ŀʱ��Ч)",
		   "COMMAND:runapp+enter"
    },
    {
    	"runreapp",
		Sh_RunReleaseAPP,
		"����ReleaseAPP",
		"COMMAND:runreapp+enter"
    },
	{
    	"appinfo",
		Sh_GetAPPInfor,
		"��ȡ����bin�ļ���Ϣ",
		"COMMAND:appinfor+enter"
    },
	{
		"iapstatus",
		Sh_GetStatus,
		"��ȡIboot״̬��Ϣ",
		"COMMAND:iaperr+enter"
	},
	{
		 "iapver",
			  Sh_GetIAPVersion,
			  "��ȡIAP�汾��Ϣ",
			  "COMMAND:iapversion+enter"
		},
		{
      "iapmode",
	  Sh_GetRunMode,
	  "��ȡ��ǰ����ģʽ(Iboot or APP)",
	  "COMMAND:iapsta+enter"
	}
};
#define CN_IAP_SHELL_NUM  sizeof(shell_cmd_iap_table)/sizeof(struct ShellCmdTab)
static struct ShellCmdRsc tg_iap_shell_cmd_rsc[CN_IAP_SHELL_NUM];
ptu32_t IAP_Shell_Module_Install(void)
{
	Sh_InstallCmd(shell_cmd_iap_table,tg_iap_shell_cmd_rsc,CN_IAP_SHELL_NUM);
	return 1;
}

//----��װIAP���-----------------------------------------------------------
//���ܣ���װIAP�����
//����: �ޡ�
//����: �ޡ�
//----------------------------------------------------------------------------
ptu32_t ModuleInstall_IAP(void)
{
	IAP_Shell_Module_Install();
	printf("IAP ModuleInstall success.\r\n");
	return 0;
}


//----�ж��Ƿ���Ҫ���ļ�ϵͳ�м���APP��ִ��---------------------------------------
//���ܣ����ļ��м���APP�����С�
//����: �ޡ�
//����: �ޡ�
//˵��:��IAP����ģʽΪLoadFromDataʱ��ֻ��������������������1.����APP��2.IAP����ģ
//      ʽΪLoadFromData��3.APP�ļ�����.�Ż���ļ��м��ص�RAM�����С�
//----------------------------------------------------------------------------
bool_t IAP_LoadAPPFromFile(void)
{
	bool_t result;
	char *Buf;
	u32 crc,len,addr;
	FILE *fp;
    struct stat FpInfo;

	//�����ж��Ƿ���Ҫ����APP״̬
	result=IAP_IsForceIboot();
	if(result)
	{
		g_IbootStaus=EN_FORCE_IBOOT;
		result = false;  //����Iboot
	}
	else
	{
		result=IAP_IsRamIbootFlag();
		if(result)
		{
			g_IbootStaus=EN_RAM_IBOOT_FLAG;
			result = false;   //����Iboot
		}
		else
		{
			if(g_IbootType==0)
			{
			   fp=fopen(g_pcIbootDir,"r");
			   if(fp==NULL)
			   {
				   result = false;
				   goto LOAD_FILE_EXIT;
			   }

			   addr=IAP_GetAPPStartAddr() - 0x100;
			   Buf=(char *)addr;

			   fread(Buf,0x100,1,fp);			//read the 256 byte file info

			   fseek(fp,0,0);

			   if(IAP_APPIsDebug())				//dbg not need crc
			   {
					if(0 == stat(g_pcIbootDir,&FpInfo))
					{
						len = (u32)FpInfo.st_size;
					}
					else
					{
						fclose(fp);
						result = false;
						goto LOAD_FILE_EXIT;
					}
					fread(Buf,len,1,fp);
			   }
			   else
			   {
				   len=IAP_GetAPPSize();
				   fread(Buf,len,1,fp);
				   crc=crc32(Buf,len);
				   if(crc!=gc_ptIbootCtrl.Iap_crc)
				   {
					   fclose(fp);
					   result = false;
					   goto LOAD_FILE_EXIT;
				   }
			   }
				__AppPreLoad();
				return true;		//clear compile warning
			}
		}

	}
LOAD_FILE_EXIT:
	return result;
}





