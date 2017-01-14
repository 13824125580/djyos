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
//����ģ��: GDD
//����:  zhb.
//�汾��V1.0.0
//�ļ�����: ������̿ؼ�ʵ��
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2016-10-08
//   ����:  zhb.
//   �°汾�ţ�V1.0.0
//   �޸�˵��: ԭʼ�汾
//---------------------------------

#include  "gdd.h"
#include  <gui/gdd/gdd_private.h>
#include "font.h"
#include    <gdd_widget.h>

static u8 gs_KeyBoardRowsDef=4;
static u8 gs_KeyBoardColsDef=3;
static u8 gs_KeySpaceDef=0;
static u8 gs_KeyVtimeLimitDef=10;
static char *gs_KeyValSetsDef="123456789.0��";
static u8 gs_KeyBoardRows;
static u8 gs_KeyBoardCols;
static u8 gs_KeySpace;
static char *gs_KeyValSets;
static u8 gs_KeyWidth,gs_KeyHeight;

static bool_t gs_bKeyDownFlag=false;
static char gs_CurKeyDown;
static s32 gs_KeyBoardId=0;

//-----------------------------------------------------------------------
//���ܣ�������̻�õ�ǰ���µļ�ֵ
//��������
//���أ����µļ�ֵASCIIֵ.
//-----------------------------------------------------------------------------
static u32 __VirKeyBoardRead(void)
{
      return (u32)gs_CurKeyDown;
}

//---------------------------------------------------------------------------
//���ܣ����ⰴ��������Ϣ��Ӧ����
//������pMsg��������Ϣָ��
//���أ��ɹ�����true,ʧ���򷵻�false��
//-----------------------------------------------------------------------------
static bool_t VirKeyBoard_Create(struct WindowMsg *pMsg)
{
    HWND hwnd;
	VirKeyBoard *pVKB;
	struct KeyBoardPrivate *pkbp;
	char *name;
	char buf[5];
	if(pMsg==NULL)
		return false;
	hwnd=pMsg->hwnd;
	if(hwnd==NULL)
		return false;
	if(pMsg->Param1==0)
	{
	   pVKB=(VirKeyBoard *)malloc(sizeof(VirKeyBoard));
	   if(pVKB==NULL)
	       return false;
	   pkbp=(struct KeyBoardPrivate *)malloc(sizeof(struct KeyBoardPrivate));
	   if(pkbp==NULL)
		   return false;
	   pVKB->cols=gs_KeyBoardRowsDef;
	   pVKB->rows=gs_KeyBoardColsDef;
	   pVKB->keyspace=gs_KeySpaceDef;
	   pVKB->keysets=gs_KeyValSetsDef;
	   pVKB->id=gs_KeyBoardId;
	   pkbp->key_bak=0;
	   pkbp->key_now=0;
	   pkbp->vtime_count=0;
	   pkbp->vtime_limit=gs_KeyVtimeLimitDef;
	   pkbp->read_keyboard=__VirKeyBoardRead;
	   pVKB->pKBP=pkbp;
	   SetWindowPrivateData(hwnd,(void *)pVKB);
	   itoa(gs_KeyBoardId,buf,10);
	   name="vir single touch keyboard";
	   Keyboard_InstallDevice(name, pkbp);
	   gs_KeyBoardId++;
	}
	else
	{

	}
    return true;
}

//----------------------------------------------------------------------------
//���ܣ�������̿ؼ���MSG_PAINT��Ϣ��Ӧ����
//������pMsg��������Ϣָ��
//���أ��ɹ�����true,ʧ���򷵻�false��
//-----------------------------------------------------------------------------
static  bool_t VirKeyBoard_Paint(struct WindowMsg *pMsg)
{
	HWND hwnd,keyhwnd;
	HDC hdc;
	RECT rc;
	u8 width,height,i,j,keynum,keycnt=0;
	u8 LeftStart,HighStart;
	VBK vbk;
	char keyval;
	if(pMsg==NULL)
		return false;
	hwnd=pMsg->hwnd;
	if(hwnd==NULL)
		return false;
	hdc =BeginPaint(hwnd);
	if(hdc==NULL)
		return false;
	vbk=(VBK)pMsg->Param1;
	if(vbk==NULL)
	{
		gs_KeyBoardRows=gs_KeyBoardRowsDef;
		gs_KeyBoardCols=gs_KeyBoardColsDef;
		gs_KeySpace=gs_KeySpaceDef;
		gs_KeyValSets=gs_KeyValSetsDef;
	}
	else
	{
		gs_KeyBoardRows=vbk->rows;
		gs_KeyBoardCols=vbk->cols;
		gs_KeySpace=vbk->keyspace;
		gs_KeyValSets=vbk->keysets;
	}

	GetClientRect(hwnd,&rc);
	SetFillColor(hdc,RGB(255,255,255));
	FillRect(hdc,&rc);
	width=RectW(&rc);
	height=RectH(&rc);
	gs_KeyWidth=(width-(gs_KeyBoardCols+1)*gs_KeySpace)/gs_KeyBoardCols;
	gs_KeyHeight=(height-(gs_KeyBoardRows+1)*gs_KeySpace)/gs_KeyBoardRows;
	LeftStart=gs_KeySpace;
	HighStart=gs_KeySpace;
	keynum=strlen(gs_KeyValSetsDef);
	for(i=0;i<gs_KeyBoardRows;i++)
	{
		for(j=0;j<gs_KeyBoardCols;j++)
		{

			keyval=*(gs_KeyValSetsDef+keycnt);
			keyhwnd=CreateButton(&keyval,WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE|\
					BS_SIMPLE|BUTTON,LeftStart+j*gs_KeyWidth,HighStart+i*gs_KeyHeight,gs_KeyWidth,\
					gs_KeyHeight,hwnd,0x1000,&keyval,NULL);
			keycnt++;
			//���һ���Ƿ񴴽��ɹ�
			if(keycnt==gs_KeyBoardRows*gs_KeyBoardCols)
			{
				if(keyhwnd==NULL)
				{
				    #ifdef DEBUG
							printf("VirKeyBoard Create Failed.\r\n");
							while(1);
					#endif
				   DestroyAllChild(hwnd);
				   DestroyWindow(hwnd);
				   EndPaint(hwnd,hdc);
				   return false;
				}
			}

		}
	}

	EndPaint(hwnd,hdc);
	return true;
}

//-----------------------------------------------------------------------------
//���ܣ��ж�ĳ�������Ƿ��ܽ�������������룬ֻ��TextBox��RICHTEXTBOX��LISTBOX��LISTVIEW
//�ȿ��Խ��������������.
//������hwnd�����ھ��.
//���أ����򷵻�true,���򷵻�false.
//-----------------------------------------------------------------------------
//static bool_t __IsKeyBoradInputValidControl(HWND hwnd)
//{
//      u8 style;
//	  bool_t ret=false;
//	  style=(u8)hwnd->Style;     //ȡ��͵�8λ
//	  switch (style)
//	  {
//	  	case BUTTON:
//		case CHECKBOX:
//		case LABEL:
//		case PROGRESSBAR:
//				ret=false;
//				break;
//		case TEXTBOX:
//		case RICHTEXTBOX:
//		case LISTBOX:
//		case LISTVIEW:
//				ret=true;
//				break;
//		default:
//				break;
//
//	  }
//	  return ret;
//}

//-----------------------------------------------------------------
//���ܣ�������̿ؼ�MSG_NOTIFY��Ϣ��Ӧ����
 //������pMsg��������Ϣָ�롣
 //���أ��ɹ�����true,ʧ���򷵻�false��
//-----------------------------------------------------------------------------
static bool_t VirKeyBoard_NotifyHandle(struct WindowMsg *pMsg)
{
    HWND hwnd,keyhwnd;
	u16 event,id;
	u32 time;
	char *keyval;
	if(pMsg==NULL)
		 return false;
	hwnd =pMsg->hwnd;
	if(hwnd==NULL)
		 return false;
	event=HI16(pMsg->Param1);
	keyhwnd=(HWND)pMsg->Param2;
    if(keyhwnd==NULL)
    	return false;
    keyval=GetWindowPrivateData(keyhwnd);
	if(event==MSG_BTN_UP)
	{
		if(gs_bKeyDownFlag)
		{
			gs_CurKeyDown=*keyval;
			PostMessage( hwnd, MSG_KEY_UP, (u32) keyval, time);
		}

	}
	else if(event==MSG_BTN_DOWN)
	{
		gs_bKeyDownFlag=true;
	 	PostMessage( hwnd, MSG_KEY_DOWN, (u32) keyval, time);
	}
	else
	{

	}
	 return true;
}



//Ĭ�����������Ϣ�������������û���������û�д������Ϣ��
static struct MsgProcTable s_gVirKeyBoardMsgProcTable[] =
{
    {MSG_CREATE,VirKeyBoard_Create},
    {MSG_PAINT,VirKeyBoard_Paint},
    {MSG_NOTIFY,VirKeyBoard_NotifyHandle}
};

static struct MsgTableLink  s_gVirKeyBoardMsgLink;
// =============================================================================
// ��������: ������̿ؼ�����������
// �������: Text:������̴���Text;
//           Style:������̷�񣬲μ�gdd.h;
//           x:���������ʼλ��x��������(��λ������);
//           y:���������ʼλ��y��������(��λ������);
//           w:������̿��(��λ������);
//           h:������̸߶�(��λ������);
//           hParent:������̸����ھ��;
//           WinId:������̿ؼ�Id;
//           pdata:������̿ؼ�˽�����ݽṹ;
//           UserMsgTableLink:������̿ؼ��û���Ϣ�б�ṹָ�롣
// �������: �ޡ�
// ����ֵ  :�ɹ��򷵻��ı�������ʧ���򷵻�NULL��
// =============================================================================
HWND CreateVirKeyBoard(const char *Text,u32 Style,
                    s32 x,s32 y,s32 w,s32 h,
                    HWND hParent,u32 WinId,void *pdata,
                    struct MsgTableLink *UserMsgTableLink)
{
    WINDOW *pGddWin=NULL;
    struct MsgTableLink *Current;
    if(UserMsgTableLink != NULL)
    {
        Current = UserMsgTableLink;
        while(Current->LinkNext != NULL)
            Current = Current->LinkNext;
        Current->LinkNext = &s_gVirKeyBoardMsgLink;
        Current = UserMsgTableLink;
    }
    else
        Current = &s_gVirKeyBoardMsgLink;
    s_gVirKeyBoardMsgLink.LinkNext = NULL;
    s_gVirKeyBoardMsgLink.MsgNum = sizeof(s_gVirKeyBoardMsgProcTable) / sizeof(struct MsgProcTable);
    s_gVirKeyBoardMsgLink.myTable = (struct MsgProcTable *)&s_gVirKeyBoardMsgProcTable;
    pGddWin=CreateWindow(Text,WS_CHILD|Style,x,y,w,h,hParent,WinId,CN_WINBUF_PARENT,pdata,Current);
    return pGddWin;
}
