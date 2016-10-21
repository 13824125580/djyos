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
//����ģ��: CAN SJA1000����ģ��
//����:  zhb.
//�汾��V1.0.0
//�ļ�����: SJA1000�ײ���������������˾SR1210��
//ʱ��:2015.06.09
//------------------------------------------------------
/*�ж�  CAN1---�ⲿ�ж�8 ��ӦCPU����G0
        CAN2--�ⲿ�ж�9 ��ӦCPU����G1
        CAN3---�ⲿ�ж�10 ��ӦCPU����G2
        CAN4--�ⲿ�ж�11��ӦCPU����G3*/
#include "SJA1000Can.h"
#include "2410addr.h"
#include "os.h"
#include "msgqueue.h"
#include "stdlib.h"
#include "stdio.h"
#include "int.h"
#include "string.h"
#include "lock.h"
#include "stdint.h"
#include "int_hard.h"
#include "ring.h"
#include "shell.h"
#include "AT24C128.h"
#include "cpu_peri.h"

#define CN_U32_MAX           0x7FFFFFFF
#define CN_MAX_PKG_NUM_PERI  4000
#define CN_CAN_BUFSIZE (52000)

#define CN_CAN_CONFIG_LEN                       0xA
#define CN_E2PROM_CAN_START_ADDR                0x00
#define CN_E2PROM_CAN_ADDR1_ADDR                0x04

#define CN_E2PROM_CAN_ADDR2_ADDR                0x06
#define CN_E2PROM_CAN_ADDR3_ADDR                0x08
#define CN_CAN_PARA_SUCCESS_FLAG                0xAA      //�����ɹ����ñ�ʶ��

#define CN_CAN_ERROR_MAX                        0xA
//CAN������Ϣ��E2PROM������Ϊ��Byte1~Byte2 ������;Byte3:��ʾ�Ƿ������˵�ַ����
//Byte4:���õĵ�ַ,Byte5:��E2PROM���Ƿ������˲����ʱ�־λ����Ϊ0xAA���ʾ�����ã�
//��ʱCANоƬ����E2PROM��ֵ����оƬ������Ϊ0xAA�����ʾδ���ã���ʱ������Ĭ��ֵ����
//CAN0 ������250;CAN1 ������250;CAN2������125;CAN3������125.

CAN_DevCtrl *CAN_DevCtrlPtr=NULL;
static u8 gs_CANAddrNum=0;
static struct  tagSemaphoreLCB *g_ptCanSemp;
static u8 CAN_RecvStack[0xA000];
static u8 CAN_MonitorStack[0x1000];
static u8 recvbuf[650];
static u32 gs_CANDefaultBaudRate[CN_CAN_CHIP_NUM]={250,250,125,125};
static u32 gs_CANCurBaudRate[CN_CAN_CHIP_NUM]={0,0,0,0};

static u32 ALI_Count=0;
static u8  ALC_Current=0;
static u32 BEI_Count=0;
static u8  BusErrorType=0;
static u32 BusError_BitCount=0;
static u32 BusError_FormCount=0;
static u32 BusError_StuffCount=0;
static u32 BusError_OtherCount=0;
static u32 BusError_Tx=0;
static u32 BusError_Rx=0;
static u8  BusError_Current=0;
static u8  ErrStatusCurrent=0xff;
static u32 BusOff_Count=0;

#define CN_CAN_RECORD_ADDR    0x100
#define CN_CAN_RECORD_LEN     13
#define CN_CAN_RECORD_NUM     1240
static u32 RecordConut=0;
static u8 Hard_Can_Message_Read(u8 byChipNum);
static u32 ISR_CAN( ptu32_t );
ptu32_t CAN_Shell_Module_Install(void);
bool_t Sh_CAN_SetBaudRate(char *param);
bool_t Sh_CAN_SetAddr(char *param);
bool_t  Sh_CAN_GetBaudRate(char *param);
bool_t Sh_CAN_GetAddr(char *param);

bool_t Sh_CAN_GetCANInfor(char *param);
extern int Can2Eth(int interface, u8 *buf, int len);

struct ShellCmdTab const shell_cmd_can_table[]=
{
		{
			"set-baudrate",
			Sh_CAN_SetBaudRate,
			"����CANͨ�Ų�����",
			"COMMAND:set-baudrate+enter"
		},
		{
			"set-addr",
			Sh_CAN_SetAddr,
			"����CANͨ�ŵ�ַ",
			"COMMAND:set-addr+enter"
		},
		{
				"get-baudrate",
				Sh_CAN_GetBaudRate,
				"��ȡָ��CANͨ�Ų�����",
				"COMMAND:get-baudrate+enter"
		},
		{
				"get-addr",
				Sh_CAN_GetAddr,
				"��ȡָ��CANͨ�ŵ�ַ",
				"COMMAND:get-addr+enter"
		},
		{
				"get-infor",
				Sh_CAN_GetCANInfor,
				"��ȡCAN0ͨ���Ĵ���ֵ",
				"COMMAND:get-infor+enter"
		},
};

#define CN_CAN_SHELL_NUM  sizeof(shell_cmd_can_table)/sizeof(struct ShellCmdTab)
static struct ShellCmdRsc tg_can_shell_cmd_rsc[CN_CAN_SHELL_NUM];

/*******************************************************************************
����:CAN���ƿ��ڴ������ʼ����
����:�ա�
����:true--��ʼ���ɹ���false--��ʼ��ʧ�ܡ�
******************************************************************************/

bool_t CAN_MemInit()
{
    CAN_RxTag* can_Rxtag=NULL;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    u32 i;
    struct RingBuf *RingBuf;
    CAN_DevCtrlPtr=(CAN_DevCtrl *)malloc(sizeof(CAN_DevCtrl)*CN_CAN_CHIP_NUM);
    if(!CAN_DevCtrlPtr)
        return false;
    for(i=0;i<CN_CAN_CHIP_NUM;i++)
    {
       CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
       can_Rxtag =(CAN_RxTag* )malloc(sizeof(CAN_RxTag));
       if(!can_Rxtag)
             goto ERRORDeal;
	   RingBuf=(struct tagRingBuf *)malloc(sizeof(struct RingBuf));
	   if(!RingBuf)
	   {
		goto ERRORDeal;
	   }
	   RingBuf->buf=(u8 *)malloc(CN_CAN_BUFSIZE);
	   if(!RingBuf->buf)
	   {
		goto ERRORDeal;
	   }
	    can_Rxtag->rxBuf=RingBuf;
        CAN_DevCtrlTempptr->can_RxTag=can_Rxtag;
    }
    return true;
ERRORDeal:

    for(i=0;i<CN_CAN_CHIP_NUM;i++)
    {
        CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
        if(CAN_DevCtrlTempptr->can_RxTag)
        {
            if(CAN_DevCtrlTempptr->can_RxTag->rxBuf)
                free(CAN_DevCtrlTempptr->can_RxTag->rxBuf);
            free((CAN_DevCtrlTempptr->can_RxTag));
        }
    }
    free(CAN_DevCtrlPtr);
    return false;

}

/*******************************************************************************
����:CAN�豸�жϿ������жϳ�ʼ����
����:�ޡ�
����:�ޡ�
��ע���ĸ�SJA1000���ж�������νӵ�S3C2410��EXT8~EXT11�ϣ����ｫEXT8~EXT11����Ϊ�½�
�ش�����
*********************************************************************************/
 void InitCANExtInt()
{
    u32 tempp;
    // �����ж����ͣ��½��ش���
    SNGS3C_REG_READ(rEXTINT1,tempp);  /* EXT8-11 TO �½����жϴ���*/
    tempp = tempp & 0xFFFF0000;
    tempp |= 0x2222;
    SNGS3C_REG_WRITE(rEXTINT1,tempp);

    // ���ö˿�Ϊ�ж�ģʽ
    SNGS3C_REG_READ(rGPGCON,tempp);  /* PG0-3 TO INT8-11 */
    tempp = ( tempp & 0xFFFFFF00) | 0x000000AA;
    SNGS3C_REG_WRITE(rGPGCON,tempp);

    // ���ö˿�Ϊ�ж�����
    SNGS3C_REG_READ(rEINTMASK,tempp);  /* PG0-1 TO INT8-11 */
    tempp = ( tempp & 0xFFFFF0FF) ;
    SNGS3C_REG_WRITE(rEINTMASK,tempp);

    Int_SetClearType(CN_INT_LINE_EINT8_23,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(CN_INT_LINE_EINT8_23,ISR_CAN);
    Int_SettoAsynSignal(CN_INT_LINE_EINT8_23);
    Int_ClearLine(CN_INT_LINE_EINT8_23);
    Int_RestoreAsynLine(CN_INT_LINE_EINT8_23);
}

 /*******************************************************************************
 ����:����CANͨ�Ų����ʡ�
 ����:para:�����ַ�������Ҫ�����������ÿո����
      byChipNum--CAN��������ţ�0~3��
      baudRate--CANͨ�Ų����ʣ����貨������5��10��20��40��50��80��100��125��200��250��
      400��500��666��800��1000.
 ���:true--���óɹ���false-����ʧ�ܡ�
 *********************************************************************************/
bool_t Sh_CAN_SetBaudRate(char *param)
{
     char *word_ChipNum,*word_BaudRate,*word_trail,*next_param;
     u8 byChipNum;
     u32 BaudRate;
     u32 BaudRate1;
     u8  byBuf[4];
     u32 result;
	 //��ȡ2������
	 word_ChipNum = Sh_GetWord(param,&next_param);
	 word_BaudRate = Sh_GetWord(next_param,&next_param);
	 word_trail = Sh_GetWord(next_param,&next_param);
	 if((word_ChipNum == NULL)||(word_BaudRate == NULL)||(word_trail != NULL))
	 {
		printk("\r\n��ʽ������ȷ��ʽ�ǣ�\r\n>d CAN��������� ������ \r\n");
		return false;
	 }
	 byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	 BaudRate = strtol(word_BaudRate, (char **)NULL, 0);

	 if(byChipNum>=CN_CAN_CHIP_NUM)
	 {
		printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	 }

	 switch(BaudRate)
	 {
	 case 5:
		 BaudRate1=5;
	   break;
	 case 10:
		 BaudRate1=10;
	  break;
	 case 20:
		 BaudRate1=20;
	   break;
	 case 40:
		 BaudRate1=40;
	   break;
	 case 50:
		 BaudRate1=50;
	   break;
	 case 80:
		 BaudRate1=80;
	   break;
	 case 100:
		 BaudRate1=100;
		break;
	 case 125:
		 BaudRate1=125;
		break;
	 case 200:
		 BaudRate1=200;
		break;
	 case 250:
		 BaudRate1=250;
		break;
	 case 400:
		 BaudRate1=400;
		break;
	 case 500:
		 BaudRate1=500;
		break;
	 case 666:
		 BaudRate1=666;
		break;
	 case 800:
		 BaudRate1=800;
		break;
	 case 1000:
		 BaudRate1=1000;
		break;
	 default:
		 printk("The CAN NUM %d :BaudRate is invalid,please set again\r\n",byChipNum);
		 return false;
	 }
	 byBuf[0]=byChipNum;
	 //��д��8λ����д��8λ
     byBuf[1]=BaudRate1;
     byBuf[2]=BaudRate1>>8;
     byBuf[3]=CN_CAN_PARA_SUCCESS_FLAG;
     result=AT24_WriteBytes(CN_E2PROM_CAN_START_ADDR+byChipNum*CN_CAN_CONFIG_LEN,byBuf,4);
	if(!result)
	{
		printk("The BaudRate set failed!Please set again!\r\n");
		//���������óɹ���־λ
		return false;
	}
	 printk("The CAN NUM %d BaudRate set success.\r\n",byChipNum);
	  return true;
}

/*******************************************************************************
����:��ȡCANͨ�Ų����ʡ�
����:para:�����ַ�������Ҫ1������
     byChipNum--CAN��������ţ�0~3
���:true--��ȡ�ɹ���false-��ȡʧ�ܡ�
*********************************************************************************/
bool_t  Sh_CAN_GetBaudRate(char *param)
{
	 char *word_ChipNum,*word_trail,*next_param;
	 u8 byChipNum;
	 u32 result;
	 u8 byBuf[4];
	 u32 temp,baudrate=0x0000;
	//��ȡ1������
	word_ChipNum = Sh_GetWord(param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n��ʽ������ȷ��ʽ�ǣ�\r\n>d CAN���������. \r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	 result=AT24_ReadBytes(CN_E2PROM_CAN_START_ADDR+byChipNum*CN_CAN_CONFIG_LEN, byBuf,4);
	 if(!result)
	 {
       printk("Get the BaudRate failed!\r\n");
       return false;
	 }
	if(byChipNum!=byBuf[0]) //�����ж�CAN����Ƿ���ȷ������ⶼ����ȷ�Ƿ�pass��
	{
		printk("The CAN NUM don't match.\r\n");
		return false;
	}
	else if(CN_CAN_PARA_SUCCESS_FLAG!=byBuf[3])
	{
		printk("The Success Config flag is error.\r\n");
		return false;
	}
	 else
	 {
		if(byBuf[2]==0)
        {
		    baudrate=byBuf[1];
		    printf("The CAN NUM %d:The BaudRate is %d.\r\n",byChipNum,baudrate);
            return true;
        }
        else
        {
		    temp=byBuf[2];
        	baudrate|=(temp<<8);
		    temp=byBuf[1];
        	baudrate|=temp;
		    printf("The CAN NUM %d:The BaudRate is %d.\r\n",byChipNum,baudrate);
        	return true;
        }
	 }
}

/*******************************************************************************
 ����:����CAN��ַ���˹��ܣ����õ�ַ���˺�CAN������ֻ����Ŀ���ַΪ����ַ��CAN֡��
 ����: para:�����ַ�������Ҫ�����������ÿո����
      byChipNum--CAN��������ţ�0~3��
      Addr1--CAN��ַ1����Χ��0~0xFE�������ַ��Ϊ0xFF�����ʾ����Ҫ��ַ���ˣ���������CAN���ߵİ���
 ���:true--���óɹ���false-����ʧ�ܡ�
 ˵�������ĿǰӦ������ֻʵ����չ֡��ʽ�ĵ��˲�����.
*********************************************************************************/
bool_t Sh_CAN_SetAddr(char *param)
{
    char *word_ChipNum,*word_Addr,*word_trail,*next_param;
    u8 byChipNum;
    u32 Addr,result;
    u8 byBuf[2];
    u16 wAddr;
   //��ȡ2������
    word_ChipNum = Sh_GetWord(param,&next_param);
    word_Addr = Sh_GetWord(next_param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
    if(word_ChipNum==NULL||word_Addr==NULL||word_trail!=NULL)
    {
    	printk("\r\n��ʽ������ȷ��ʽ�ǣ�\r\n>d CAN��������� ��ַ.\r\n");
    	return false;
    }
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
		printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	Addr = strtol(word_Addr, (char **)NULL, 16);
	if(Addr>0xFF)
	{
		printk("The CAN addr is invalid,the addr must be 0x00~0xFF");
		return false;
	}
	byBuf[0]=Addr;
	byBuf[1]=CN_CAN_PARA_SUCCESS_FLAG;
    switch(gs_CANAddrNum)
    {
    case 0:
    	wAddr=CN_E2PROM_CAN_ADDR1_ADDR;
    	break;
    case 1:
    	wAddr=CN_E2PROM_CAN_ADDR2_ADDR;
    	break;
    case 2:
    	wAddr=CN_E2PROM_CAN_ADDR3_ADDR;
    	break;
    default:
    	printk("The Addr Num Supported exceed.\r\n");
    	break;
    }
    result=AT24_WriteBytes(wAddr+byChipNum*CN_CAN_CONFIG_LEN, byBuf, 2);
    if(!result)
    {
    	printk("The addr set failed!Please set again!\r\n");
    	return false;
    }
    else
    {
    	gs_CANAddrNum++;
    	printk("The CAN NUM %d addr set success!\r\n",byChipNum);
    	return true;
    }
}

/*******************************************************************************
����:��ȡCANͨ�ŵ�ַ��
����:para:�����ַ�������Ҫ1������
     byChipNum--CAN��������ţ�0~3
���:true--��ȡ�ɹ���false-��ȡʧ�ܡ�
*********************************************************************************/
bool_t  Sh_CAN_GetAddr(char *param)
{
	char *word_ChipNum,*word_trail,*next_param;
	u8 byChipNum;
	u32 result;
	u8 byBuf[6];
	u8 Addr[3]={0,0,0};
	//��ȡ1������
	word_ChipNum = Sh_GetWord(param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n��ʽ������ȷ��ʽ�ǣ�\r\n>d CAN���������.\r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	 result=AT24_ReadBytes(CN_E2PROM_CAN_ADDR1_ADDR+byChipNum*CN_CAN_CONFIG_LEN, byBuf,6);
	 if(!result)
	 {
       printk("Get the addr failed!\r\n");
       return false;
	 }
	 if(byBuf[1]!=CN_CAN_PARA_SUCCESS_FLAG)
	 {
		 printk("The CAN has not set the addr.\r\n");
		 return false;
	 }
	 else
	 {
       Addr[0]=byBuf[0];
       if(byBuf[3]!=CN_CAN_PARA_SUCCESS_FLAG)
        {
    	   printk("The CAN Num %d:The Addr is %d.\r\n",byChipNum,Addr[0]);
    	   return true;
        }
       else
       {
    	   Addr[1]=byBuf[2];
    	   if(byBuf[5]!=CN_CAN_PARA_SUCCESS_FLAG)
    	   {
    		   printk("The CAN Num %d:The Addr is %d and %d.\r\n",byChipNum,Addr[0],Addr[1]);
              return true;
            }
           else
           {
    		   Addr[2]=byBuf[4];
    		   printk("The CAN Num %d:The Addr is %d , %d and %d.\r\n",byChipNum,Addr[0],Addr[1],Addr[2]);
    		   return true;
            }
	    }
	 }
}


/*******************************************************************************
����:��ȡCANͨ�ŵ�ַ��
����:para:�����ַ�������Ҫ1������
     byChipNum--CAN��������ţ�0~3
���:true--��ȡ�ɹ���false-��ȡʧ�ܡ�
*********************************************************************************/
bool_t  Sh_CAN_GetCANInfor(char *param)
{
	 char *word_ChipNum,*word_trail,*next_param;
	 u8 byChipNum;
	 u32 result;
	 u8 byBuf[260];
     u8 i;
	 word_ChipNum = Sh_GetWord(param,&next_param);
     word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n��ʽ������ȷ��ʽ�ǣ�\r\n>d CAN���������. \r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
    if(RecordConut>=19)
    {
       result=AT24_ReadBytes(CN_CAN_RECORD_ADDR+(RecordConut-19)*CN_CAN_RECORD_LEN, byBuf,260);
    }
    else
    {
    	i=20-RecordConut-1;
    	result=AT24_ReadBytes(CN_CAN_RECORD_ADDR+0x4000-i*CN_CAN_RECORD_LEN, byBuf,i*CN_CAN_RECORD_LEN);
    	result=AT24_ReadBytes(CN_CAN_RECORD_ADDR, &byBuf[CN_CAN_RECORD_LEN*i],RecordConut*CN_CAN_RECORD_LEN);
    }
    if(result)
    {
      for(i=0;i<20;i++)
      {
    	  printf("Mode:%x,CMR:%x,SR:%x,IIR:%x,IER:%x,D0:%x,D1:%x,D2:%x,D3:%x,D4:%x,D5:%x,D6:%x,D7:%x,\r\n",
    	    byBuf[13*i+0],byBuf[13*i+1],byBuf[13*i+2],byBuf[13*i+3],byBuf[13*i+4],byBuf[13*i+5],byBuf[13*i+6],
			byBuf[13*i+7],byBuf[13*i+8],byBuf[13*i+9],byBuf[13*i+10],byBuf[13*i+11],byBuf[13*i+12]);
      }
    }
    else
    {
       printf("Read fram failed.\r\n");
    }
    return true;
}
/*******************************************************************************
����:CAN�豸�ĳ�ʼ����������ʼ���жϿ������������ڴ棬��ʼ��������
����:byChipNum--CAN��������ţ�0~3��
     addr--CAN��������ַ��
     baudRate--CANͨ�Ų����ʡ�
���:true--��ʼ���ɹ���false-��ʼ��ʧ�ܡ�
*********************************************************************************/
bool_t Hard_Can_Init( u8 byChipNum,u32 baudRate)
{
    u8    byBTR0, byBTR1,byTemp;
    u32 i;
    volatile u8*   pAddr;
	static u8 flag=0;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;

    if(byChipNum>=CN_CAN_CHIP_NUM)
        return false;

    if(!flag)
    {  
        if(CAN_MemInit()==false)/*�ڴ����룬ֻ����һ��*/
        {
        	printf("CAN Control Block Malloc Failed!\r\n");
        	return false;
        } 
        InitCANExtInt();/*�ж����ã�ֻ����һ��*/
        flag++;
    }
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    CAN_DevCtrlTempptr->BaudRate=baudRate;
    CAN_DevCtrlTempptr->RecvFlag=false;
    switch(byChipNum)
    {
        case 0:
            pAddr = (u8*)(CN_ADDR_CAN1_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN1_BASE;
            break;
        case 1:
            pAddr = (u8*)(CN_ADDR_CAN2_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN2_BASE;
            break;
        case 2:
            pAddr = (u8*)(CN_ADDR_CAN3_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN3_BASE;
            break;
        case 3:
            pAddr = (u8*)(CN_ADDR_CAN4_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN4_BASE;
            break;
        default:
          break;
    }

    switch(baudRate )
    {
        case 5:
            byBTR0 = 0xBF;
            byBTR1 = 0xFF;
            break;      //    5k
        case 10:
            byBTR0 = 0x31;
            byBTR1 = 0x1C;
            break;      //   10k
        case 20:
            byBTR0 = 0x18;
            byBTR1 = 0x1C;
            break;      //   20k
        case 40:
            byBTR0 = 0x87;
            byBTR1 = 0XFF;
            break;      //   40k
        case 50:
            byBTR0 = 0x09;
            byBTR1 = 0X1c;
            break;      //   50k
        case 80:
            byBTR0 = 0x83;
            byBTR1 = 0XFF;
            break;      //  80k
        case 100:
            byBTR0 = 0x04;
            byBTR1 = 0x1C;
            break;      //  100k
        case 125:
            byBTR0 = 0x03;
            byBTR1 = 0x1C;
            break;      //  125k
        case 200:
            byBTR0 = 0x81;
            byBTR1 = 0xFA;
            break;      // 200k
        case 250:
            byBTR0 = 0x01;
            byBTR1 = 0X1C;
            break;      //  250k
        case 400:
            byBTR0 = 0x80;
            byBTR1 = 0XFA;
            break;      //   400k
        case 500:
            byBTR0 = 0x00;
            byBTR1 = 0X1C;
            break;      //  500k
        case 666:
            byBTR0 = 0x80;
            byBTR1 = 0xB6;
            break;      //  666k
        case 800:
            byBTR0 = 0x00;
            byBTR1 = 0x16;
            break;      //  800k
        case 1000:
            byBTR0 = 0x00;
            byBTR1 = 0x14;
            break;      // 1000k
        default:
            byBTR0 = 0x01;
            byBTR1 = 0x1C;
            break;      //   250k
    }
	byTemp = *( pAddr + CN_PCAN_SR );       // Clear Status Register
	byTemp = *( pAddr + CN_PCAN_IIR );      // Clear Interrupt Identify Register
	*( pAddr + CN_PCAN_MODE ) = 0x01;     // Reset mode
	*( pAddr + CN_PCAN_CDR )  = 0x82;     // PeliCAN mode & Clock driver:1000-0101   todo
	*( pAddr + CN_PCAN_CMR )  = 0x0E;     // Command Register
	*( pAddr + CN_PCAN_IER )  = 0x00;     // Interruption disabled
	*( pAddr + CN_PCAN_BTR0 ) = byBTR0;   // Bus timeing register 0
	*( pAddr + CN_PCAN_BTR1 ) = byBTR1;   // Bus timeing register 1
	*( pAddr + CN_PCAN_OCR )  = 0xDA;     // Output Control:110-110-10
	*( pAddr + CN_PCAN_RXERR )= 0x00;     // RX Error counter
	*( pAddr + CN_PCAN_TXERR )= 0x00;     // TX Error counter
	*( pAddr + CN_PCAN_RBSA ) = 0x00;     // RX Buffer Start Address

	//��BMS,��CAN���ߵ�ַΪ0x56(0xE5)
	if(byChipNum<=1)
	{
		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
		*( pAddr + CN_PCAN_ACR1 ) = 0x02;     //
		*( pAddr + CN_PCAN_ACR2 ) = 0xB0;     //
		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR1 ) = 0xF8;     //
		*( pAddr + CN_PCAN_AMR2 ) = 0x07;     //
		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //
	}
	//��ַ������
	else
	{
		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
		*( pAddr + CN_PCAN_ACR1 ) = 0x00;     //
		*( pAddr + CN_PCAN_ACR2 ) = 0x00;     //
		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR1 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR2 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //
	}

//		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
//		*( pAddr + CN_PCAN_ACR1 ) = 0x00;     //
//		*( pAddr + CN_PCAN_ACR2 ) = 0x00;     //
//		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
//		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR1 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR2 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //

	*( pAddr + CN_PCAN_IER )  = 0x7D;  //Interrupt enabled(���ղ����ж�ģʽ�����Ͳ�ʹ���ж�)
	*( pAddr + CN_PCAN_MODE ) = 0x00;     // Normal Operating mode
	while(*( pAddr + CN_PCAN_MODE )&0x01)
	{
		i++;
		if(i>100000)
		{
			break;
		}
	}

    return true;
}

/*******************************************************************************
����:CAN�豸�жϴ����ú�����ISR_CAN����
����:�ޡ�
����ֵ:�ޡ�
��ע������SJA1000 PeriCANģʽ���ж�����˵����
   ���ߴ����жϣ�
��CAN��������⵽���ߴ������ж�ʹ�ܼĴ�����EIR���е�BEIE����λʱ��λ����λ����ǰ�Ĵ���
�������ͨ��������벶׽�Ĵ���(ECC)��ã����п��Է�Ϊ��λ���󡢸�ʽ������������������
   �ٲö�ʧ�жϣ�
��CAN��������ʧ�ٲ�,��Ϊ���������ж�ʹ�ܼĴ�����ALIEΪ����λʱ,��λ����λ��ǰ�ٲö�ʧ
��λ�ÿ���ͨ�����ٲö�ʧ��׽�Ĵ���(ALC)��ã�
   ���������жϣ�
��CAN�����������������״̬��Error Passive��(����һ���������������Э��涨��ֵ127)��
�Ӵ�������״̬��Error Passive���ֽ������״̬(Error Active)�Լ��жϼĴ�����EPIEλ��
��λʱ��λ����1��
   ���󱨾��жϣ�
����״̬λ��״̬�Ĵ�����SR.6λ ES��������״̬λ��״̬�Ĵ�����SR.7λ BS���ĸı���жϼĴ���
��EIEλ����λʱ��λ����1Ӱ�����״̬λ���У�
1�����������������һ�������������
2���������󱨾����ƼĴ���(EWLR)���õ�ֵʱ����״̬λ����λ�������жϡ�EWLR Ӳ
����λ���Ĭ��ֵ��96
3�������ʹ����������������255 ����״̬λ����Ϊ1 ���߹ر�
4���������߹رպ��ٴν����������״̬��Error Active��ʱ�� Ҳ������жϣ�
*********************************************************************************/
static u8 gs_SR=0x00;
void ISR_Deal( u8 byIndex )
{
    u8    byIIR,byTemp;
    volatile u8*    pAddr;
    volatile u8 RxErrCount=0;
    volatile u8 TxErrCount=0;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    if( byIndex >=CN_CAN_CHIP_NUM )
        return ;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byIndex;
    if(!CAN_DevCtrlTempptr)
        return;
    pAddr = (u8*)(CAN_DevCtrlTempptr->CANBaseAddr);
    //��ʱ�����жϣ�Ŀ������Ϊ�ڶ�IIR�Ĵ������жϵĹ����У����SJA1000�ٴδ����жϣ���ʱ����
    //�����ж�ʱ�������̣�CPU�޷���Ч�ɼ������Ӷ�������CPU�޷���Ӧ�ڶ����жϣ���SJA1000�ĵ�
    //�����жϻὫSJA1000��INT�������ͣ�����SJA1000����һֱ���ڵ͵�ƽ״̬����CPU�����ó����½���
    //��������ʱ�ᵼ��CPUһֱ�޷���ӦSJA1000���жϡ�
    byIIR = *(pAddr+CN_PCAN_IIR);
    gs_SR=*(pAddr+CN_PCAN_SR);
    *(pAddr+CN_PCAN_IER)=0x00;

    //����SJA1000��8���жϣ�������SJA1000�ĳ�ʼ��������ֻʹ����7���ж�
    //(�����ж�û��ʹ��)������ֱ����7���жϽ�����Ӧ����
    //�ٲö�ʧ�ж� ��SJA1000�ٲö�ʧ��SJA1000���Զ��˳����ͣ���Ϊ���գ��Ƚ�������Զ����͡�
    if(byIIR&ALI_Bit)
    {
    	ALI_Count++;                      //�ٲö�ʧ�������ۼ�
    	byTemp=*(pAddr + CN_PCAN_ALC);    //���ٲö�ʧ�Ĵ���
    	ALC_Current=byTemp&0x1F;          //��õ�ǰ�ٲö�ʧ��λ��
    	if(ALI_Count>=CN_U32_MAX)
    	{
    		ALI_Count=0;
    	}
    }
    //���ߴ����ж�
    if(byIIR&BEI_Bit)
    {
    	BEI_Count++;                      //���ߴ���������ۼ�
    	byTemp=*(pAddr + CN_PCAN_ECC);    //���ٲö�ʧ�Ĵ���
        //�жϳ����򣬷���or����
    	if(byTemp&0x20)
    	{
    		BusError_Rx++;
    		if(BusError_Rx>=CN_U32_MAX)
    		{
    			BusError_Rx=0;
    		}
    	}
    	else
    	{
    		BusError_Tx++;
    		if(BusError_Tx>=CN_U32_MAX)
			{
				BusError_Tx=0;
			}
    	}
    	//�жϳ������ͣ�λ���󡢸�ʽ������������������
    	BusErrorType=byTemp&0xC0;
    	switch(BusErrorType)
    	{
    	   case 0x00:
    	    	BusError_BitCount++;       //λ����������ۼ�
    	    	break;
    	   case 0x40:
    	    	BusError_FormCount++;      //��ʽ����������ۼ�
    	    	break;
    	   case 0x80:
    	    	BusError_StuffCount++;     //���λ�������ۼ�
    	    	break;
    	   case 0xc0:
    	    	BusError_OtherCount++;     //�����������ۼ�
    	    	break;
    	   default:
    	    	break;
    	}
    	BusError_Current=byTemp&0x1F;     //�����λ�ã�ָCAN֡�����bitλ
        if((BusError_BitCount>=CN_CAN_ERROR_MAX)||(BusError_FormCount>=CN_CAN_ERROR_MAX)||\
           (BusError_StuffCount>=CN_CAN_ERROR_MAX)||(BusError_OtherCount>=CN_CAN_ERROR_MAX))
        {
           Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
           BusError_BitCount=0;
           BusError_FormCount=0;
           BusError_StuffCount=0;
           BusError_OtherCount=0;
           return;
        }
    }
    //���������жϣ�ֻ���¼����״̬�������������ر���
    if(byIIR&EPIE_Bit)
    {
    	RxErrCount=*(pAddr + CN_PCAN_RXERR);
    	TxErrCount=*(pAddr + CN_PCAN_TXERR);
  //���ջ��ʹ����������127ʱ������״̬��Ϊ��������
    	if((RxErrCount>127)||(TxErrCount>127))
    	{
    		ErrStatusCurrent=ERR_PASSIVE;
    	}
  //���ջ��ʹ�������ص�С��127ʱ������״̬��Ϊ��������
    	if((RxErrCount<127)&&(TxErrCount<127))
    	{
    		ErrStatusCurrent=ERR_ACTIVE;
    	}
    }
   //��������ж�,������FIFO���ʱ����ʱֱ���������������ͷŽ��ջ�������
   if(byIIR&DOI_Bit)
   {
	   Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
	   return;
   }
   //���󱨾��ж�,Ŀǰֻ�����߹رմ������˴���
   if(byIIR&EI_Bit)
   {
       if(gs_SR&BS_Bit)  //���߹ر�
       {
          BusOff_Count++;
          Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
          return;
       }
   }

   if(byIIR&RI_Bit)  //�����ж�
   {
	   Hard_Can_Message_Read( byIndex );
   }
   //����ʹ�������ж�
       *(pAddr+CN_PCAN_IER)=0x7D;
}

/*******************************************************************************
����:CAN�豸���жϷ�����
����:��
���:��
*********************************************************************************/
u32 ISR_CAN( ptu32_t IsrNo )
{
    u32   dwValReg;
//��CPU�ж�
    Int_ClearLine(CN_INT_LINE_EINT8_23);
   //��ȡ�ж�pending״̬
    SNGS3C_REG_READ(rEINTPEND,dwValReg);
    if(dwValReg&(1<<8))
    {
    	 SNGS3C_REG_WRITE( rEINTPEND, (1<<8) );
         ISR_Deal( 0 );
    }
     if(dwValReg&(1<<9))
    {
    	SNGS3C_REG_WRITE( rEINTPEND, (1<<9) );
        ISR_Deal( 1 );
    }
     if(dwValReg&(1<<10))
    {
       	SNGS3C_REG_WRITE( rEINTPEND, (1<<10) );
        ISR_Deal( 2 );
    }
     if(dwValReg&(1<<11))
    {
    	SNGS3C_REG_WRITE( rEINTPEND, (1<<11) );
    	ISR_Deal( 3 );
    }
    return 0;
}

/*******************************************************************************
����:��ȡCAN���ջ������е����ݣ�Ϊ����ӿ�
����:byChipNum--CAN������ѡ�񣬵�ǰΪ0~3
     rxBuf---�洢���յ����ĵķ��ͻ�����  len--����������ݵĳ���
���:��ȡ�������ݵĳ���
*********************************************************************************/
u32 CAN_ReadData( u8 byChipNum,u8* rxBuf,u32 len)
{
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    struct tagRingBuf *ring;
    u32 rdLen=0x0000;
    if( byChipNum >=CN_CAN_CHIP_NUM )
        return 0;
    if(!rxBuf)
    {
        return 0;
    }
    if(len==0)
    {
    	return 0;
    }
    if(len>CN_CAN_BUFSIZE)/*����ȡ��������*/
          len=CN_CAN_BUFSIZE;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    if(!CAN_DevCtrlTempptr)
    {
        return 0;
    }
    can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;/*�豸�ջ���Ľṹ��ָ��*/
    if(!can_Rxtag)
    {
        return 0;
    }
    ring=can_Rxtag->rxBuf;
    rdLen=Ring_Read(ring,recvbuf,len);
    return rdLen;
}

/*******************************************************************************
����:��ȡCAN�豸�е����ݵ����ջ������У����ж��е��á�
����:byChipNum--CAN��������ţ�0~3��
���:ʵ�ʴ�Ӳ���ж�ȡ�ĳ��ȡ�
*********************************************************************************/
static u8 Hard_Can_Message_Read( u8 byChipNum )
{
    volatile u8*            pAddr;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    struct tagRingBuf *ring;
    u8 PkgNum=0;
    u8 RecvNum=0;
    u8 i;
    u8 j;
    u8 buf[26];
    if( byChipNum >=CN_CAN_CHIP_NUM )
        return 0;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;/*�豸�ջ���Ľṹ��ָ��*/
    ring=can_Rxtag->rxBuf;
    pAddr = (volatile u8*)(CAN_DevCtrlTempptr->CANBaseAddr);/*�豸�����Ļ���ַ*/
    for(i=0;i<2;i++)
    {
        if(0==(gs_SR&0x01))
    	    break;

		for(j=0;j<13;j++)
		{
		    buf[13*PkgNum+j]=*(pAddr+CN_PCAN_RXD+j);
		}
		PkgNum++;
		*(pAddr+CN_PCAN_CMR)=0x04;  //�ͷŽ��ջ�����
		for(j=0;j<=8;j++)   //�ȴ�1us
		{

		}
		gs_SR=*(pAddr+CN_CAN_SR);

    }
   RecvNum=13*PkgNum;
   if(PkgNum!=0)
   	{
	   Ring_Write(ring,buf,RecvNum);
   	   CAN_DevCtrlTempptr->RecvFlag=true;
   	   Lock_SempPost(g_ptCanSemp);
   	}
   return RecvNum;
}

/*******************************************************************************
����:Tcp/Ip���øú������������ݻ��ڵ���CAN֡ͨ��ָ��CAN�ڷ��͵�ָ����ַ��
����:byChipNum:CAN��������ţ�0~3
    buf: Ҫ���͵�����ָ��
    len: Ҫ���͵����ݳ���
����ֵ:ʵ��ͨ��CAN���߷��ͳ�ȥ���ֽ���
��ע��������չ֡��ʽ������CAN֡���ͣ�����CAN֡����֡ͷ���ֹ�13�ֽڡ�
*********************************************************************************/
u8 Can_Send(u8 byChipNum,u8 * txbuf,u8 len)
{
    volatile u8*  pAddr;
    u8    bySR, byLen, byLoop=0;
    u32 Id=0x0000;

    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    if( byChipNum >=CN_CAN_CHIP_NUM )
    {
#ifdef DEBUG
    	printf("The CAN Chip Num should not bigger than CN_CAN_CHIP_NUM!\r\n");
#endif
         return 0;
    }
    if(len>13)
    {
#ifdef DEBUG
    	printf("The sizes of one Pkg is smaller than 13 bytes!\r\n");
#endif
        len=13;
    }

    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    if(!CAN_DevCtrlTempptr)
    {
#ifdef DEBUG
    	printf("CAN Control Block error occurs!\r\n");
#endif
            return 0;
    }

        pAddr = (volatile u8* )(CAN_DevCtrlTempptr->CANBaseAddr);
        bySR = *( pAddr + CN_CAN_SR );/*��ȡ���ͻ�����״̬���Ƿ���release?*/
        if(0==(bySR&0x04))//ͨ��SJA1000״̬�Ĵ����ķ��ͻ�����״̬λ�ж������Ƿ����
        {
           //��1ms֮���ٴ��ж��Ƿ����
            Djy_EventDelay(10*mS);
            bySR = *( pAddr + CN_CAN_SR );
            if(0==(bySR&0x04))
            {
#ifdef DEBUG
       printf("CAN Transmit Failed because of Tx Buffer status is locked!\r\n");
#endif
              return 0;     // TBS not empty
            }
        }
        byLen=len-5;
        Id|=(u32)txbuf[1]<<24;
        Id|=(u32)txbuf[2]<<16;
        Id|=(u32)txbuf[3]<<8;
        Id|=(u32)txbuf[4];
        Id=Id<<3;
        *( pAddr + CN_PCAN_DSCR0 )   = txbuf[0];
        *( pAddr + CN_PCAN_TXD )   =   Id;
        *( pAddr + CN_PCAN_FRAMENO ) = Id>>8;
        *( pAddr + CN_PCAN_DSCR2 )   = Id>>16;
        *( pAddr + CN_PCAN_DSCR1 )   = Id>>24;
         for( byLoop=0;byLoop<byLen;byLoop++)
                    *(pAddr+(CN_PCAN_TXD+1+byLoop))=txbuf[byLoop+5];
            *(pAddr+CN_PCAN_CMR)=0x01;/*��������*/

        return byLen;
}
/*******************************************************************************
����:��CAN�ڽ��յ�CAN����BMS���߳��ģ���CAN֡��
����:byChipNum:CAN��������ţ�0~3��
    buf: Ҫ���͵�����ָ�룻
    len: Ҫ���͵����ݳ��ȡ�
����ֵ:0.
��ע��ͨ��CAN�ڽ����յ���CAN֡����Can2Eth��֡ͨ��TCP���ϴ���Ӧ�ò㡣
*******************************************************************************/
ptu32_t __Can_Recv(void)
{
    u8 i,j;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    atom_low_t atom;
    u32 PkgNum,temp,byReadBytes=0x0000;
    volatile u32 Id=0x0000;
    struct tagRingBuf *ring;
    while(1)
    {
    	 Lock_SempPend(g_ptCanSemp,CN_TIMEOUT_FOREVER);
    	 for(i=0;i<CN_CAN_CHIP_NUM;i++)
    	 {
    		 CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
    		 can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;
    		 ring=can_Rxtag->rxBuf;
             if((CAN_DevCtrlTempptr->RecvFlag)&&(false==Ring_IsEmpty(ring)))
             {
				 atom = Int_LowAtomStart();
				 byReadBytes=Ring_Check(ring);
				 Int_LowAtomEnd(atom);
				 PkgNum=(u32)(byReadBytes/CN_SIGNAL_PKG_SIZES_PERI);
			  //һ������50��CAN֡
				 if(PkgNum>50)
				 {
					 PkgNum=50;
				 }
				 byReadBytes=PkgNum*CN_SIGNAL_PKG_SIZES_PERI;
				 CAN_ReadData(i,recvbuf,byReadBytes);
				 CAN_DevCtrlTempptr->RecvFlag=false;
				 for(j=0;j<PkgNum;j++)
				 {
					 //����CAN IdŲλת��
					Id|=(u32)recvbuf[1+13*j]<<24;
					Id|=(u32)recvbuf[2+13*j]<<16;
					Id|=(u32)recvbuf[3+13*j]<<8;
					Id|=(u32)recvbuf[4+13*j];
					Id=Id>>3;
					temp=Id;
					recvbuf[4+13*j]=(u8)temp&0xff;
					temp=Id;
					recvbuf[3+13*j]=(u8)((temp>>8)&0xff);
					temp=Id;
					recvbuf[2+13*j]=(u8)((temp>>16)&0xff);
					temp=Id;
					recvbuf[1+13*j]=(u8)((temp>>24)&0xff);
					Id=0x0000;
					temp=0x0000;
				 }
				 Can2Eth(i,recvbuf,byReadBytes);
            }
         }

    }

    return 0;
}

/*******************************************************************************
����:CANͨ�ſڼ���¼���
����:�ޡ�
����ֵ:0.
��ע��ÿ��100ms��ȡһ��4��CAN�ڵ�CAN�Ĵ���.
*******************************************************************************/
ptu32_t __Can_Monitor(void)
{
   u8 RegSr,RegIr,RegRmc;
   volatile u8*    pAddr;
   CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
   CAN_DevCtrlTempptr=CAN_DevCtrlPtr;
   pAddr = (u8*)(CAN_DevCtrlTempptr->CANBaseAddr);
   while(1)
   {
	 RegSr= *(pAddr+CN_PCAN_SR);
	 RegIr= *(pAddr+CN_PCAN_IIR);
	 RegRmc=*(pAddr+CN_PCAN_RXMC);
//	 printf("SR:%x,IIR:%x,RMC:%x\r\n",RegSr,RegIr,RegRmc);
	 Djy_EventDelay(100000);  //100ms
   }
}
/*******************************************************************************
����:��ʼ��CAN��������ʹ��CAN�����ж�
����:��
����ֵ:�ɹ���ʼ������true��ʧ���򷵻�false��
˵������4��CAN������Ӳ���̶�����Ϊ���£�
1.ֻ���շ�����չ֡
2.ͨѶ������Ϊ250kbps
3.4��CAN��������ַ����Ϊ0xF0/0xF1/0xF2/0xF3

*********************************************************************************/
bool_t CAN_Main(void)
{
   u8 i;
   bool_t result;
   uint16_t evtt;
   CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
   CAN_RxTag* can_Rxtag=NULL;
   struct RingBuf *rxRingBuf=NULL;
   u8 ConfigBuf[CN_CAN_CONFIG_LEN];
   u8 temp=0x0000;
   u32 BaudRate=0x0000;

   //����CAN-Shell����
   CAN_Shell_Module_Install();

/*�����ĸ�CAN����������
 * ��ַ��CAN0/CAN1�ֱ��Ե������BMS����ʱCAN0/CAN1�ڵ�ַ����0x56(0xE5)��BMS�ĵ�ַΪ0xF4
 * (���ݹ��ҵ綯������缼���淶  )  ������չ֡��ʽ��������Ϊ250kbps
 * CAN2/CAN3�ֱ�ӵ��������ģ�����У�CAN2/CAN3�ĵ�ַ����Ϊ0xF0/0xF0�����ģ���ַ��
 * ��Ϊ0,1,2,3...(����Ϊ�����Э�顷)
 * ������չ֡��ʽ��������Ϊ125kbps   2015.06.04
 */
   //�ϵ�󣬶�ȡE2PROM��CAN��������������Ϣ
  //��3��bytes����һ��ByteΪ�����ʣ��ڶ������������ֱ�Ϊ��ѡ��ַ(һ����ַ������)

   for(i=0;i<CN_CAN_CHIP_NUM;i++)
   {
	   result=AT24_ReadBytes(CN_E2PROM_CAN_START_ADDR+i*CN_CAN_CONFIG_LEN, ConfigBuf, CN_CAN_CONFIG_LEN);
	   if(!result)
	  {
		 gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	  	 printk("Read from AT24C128 failed!\r\n");
	//��û������CANоƬ�Ĳ����ʣ������Ĭ��ֵ
	   }
	  else
	  {
		 if(ConfigBuf[0]!=i) //ͨ����Ŷ�����ȷ�������û��Ҫ�����ˣ�ֱ�Ӳ���Ĭ�ϲ�����ʼ��
	    	 {
			gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	    	 }
	    	 else
	    	 {
			 if(ConfigBuf[3]!=CN_CAN_PARA_SUCCESS_FLAG)
	    	 {
				 gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	    	 }
			 else
	    	 {
	    	 //��ȡ��ַ���ò�����Ϣ
	    	  temp=ConfigBuf[2];
				 if(temp==0)
	    	  {
					 BaudRate=ConfigBuf[1];
	    	  }
	    	  else   //û�����õ�ַ�������Ĭ�ϵ�ַ
	    	  {
					 BaudRate|=(u32)temp<<8;
					 temp=ConfigBuf[1];
					 BaudRate|=temp;
	    	  }
			   gs_CANCurBaudRate[i]=BaudRate;
			 }
		   }
	    }
       printk("The CAN Num %d:The BaudRate is %d.\r\n",i,gs_CANCurBaudRate[i]);
	   result=Hard_Can_Init(i,gs_CANCurBaudRate[i]);

	   if(!result)
	   {
	      printk("The CAN Num %d init failed.\r\n",i);
		  return false;
	   }
   }
   //��ʼ�����λ�����
   for(i=0;i<CN_CAN_CHIP_NUM;i++)
   {
		CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
		can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;
		rxRingBuf=can_Rxtag->rxBuf;
		Ring_Init(rxRingBuf,rxRingBuf->buf,CN_CAN_BUFSIZE);

 //��ʼ��CAN_MonitorReg���飬����������CANͨ�ų����Ϻ��¼SJA1000��32����ؼĴ���ֵ
   }
   //����һ���ź��������ĸ�CAN�������յ����ݺ��֪�û�ȥ��������
  g_ptCanSemp=Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,"CAN Semp");
  if(g_ptCanSemp==NULL)
  {
	#ifdef DEBUG
		   printk("CAN Semp create failed!\r\n");
	#endif
	return false;
  }

  evtt = Djy_EvttRegist(EN_CORRELATIVE,130,0,0,__Can_Recv,
		            CAN_RecvStack,sizeof(CAN_RecvStack),"CAN Recv function");
  if(evtt!=CN_EVTT_ID_INVALID)
  {

	  //�¼���������������Ϊ0,�����shell����,�������ɼ�shell�����в���
	  Djy_EventPop(evtt,NULL,0,NULL,0,130);
  }
  else
  {
	  Djy_EvttUnregist(evtt);
  }
  evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,__Can_Monitor,
		  CAN_MonitorStack,sizeof(CAN_MonitorStack),"CAN Monitor function");
   if(evtt!=CN_EVTT_ID_INVALID)
   {
	  Djy_EventPop(evtt,NULL,0,NULL,0,0);
   }
   else
   {
	  Djy_EvttUnregist(evtt);
   }
  return true;
}

/*******************************************************************************
����:CAN����������
����:byChipNum--CAN��������ţ�0~3;
    cmd--�����룬�����enum _CAN_CTRL_CMD_;
    data--ÿ����������ָ����Ĳ��������ò����ʣ�������ʽ���enum _CAN_BAUD_RATE_SET_��
����ֵ:�ޡ�
��ע��ͨ����CAN2��CAN3�Ի���CAN2���ͣ�CAN3���գ�����SJA1000�ķ��ͽ��չ��̡�
*********************************************************************************/
ptu32_t CAN_Shell_Module_Install(void)
{

	Sh_InstallCmd(shell_cmd_can_table,tg_can_shell_cmd_rsc,CN_CAN_SHELL_NUM);
	return 1;
}




