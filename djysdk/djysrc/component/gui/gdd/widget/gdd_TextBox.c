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
//�ļ�����: ��ť�ؼ�ʵ��
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2016-6-14
//   ����:  zhb.
//   �°汾�ţ�V1.0.0
//   �޸�˵��: ԭʼ�汾
//---------------------------------

#if 0
#include    "gdd.h"
#include  <gui/gdd/gdd_private.h>
#include "font.h"
#include    <widget.h>

#define CN_CHAR_NUM_LIMIT   128
#define CN_BYTE_NUM_LIMIT   256
#define CN_HEIGHT_MIN  10
//#define TXT_BOX_FIX_SIZE      (0<<0)
//#define TXT_BOX_ADJUST_SIZE   (1<<0)


typedef struct
{
   u16 height;             //������
   u16 width;              //������
   u8 len;               //TextBox�������ַ����ֽ���
   char *str;            //TextBox�����е��ַ���ɵ��ַ���
   u8 chnum;           //�ַ���
   WINDOW *hwnd;         //�ؼ����
}TextBox;




/*============================================================================*/
// =============================================================================
// ��������: ��ȡ�ַ�����Ч�ַ���,TextBoxֻ����ʾ������Ϣ�����ȼ���ַ���,�������
//           \n(���з�)����ֻ��ʾ��һ���ַ�
// �������: char *str:�ַ���
// �������: �ޡ�
// ����ֵ  :��һ���ַ�������
// =============================================================================
u8 __GetValidTextCount(char *str)
{
    u8 linecount=0;
    u8 count=0;
    char ch;
    u8 src_len=0;
    s32 len;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc;
    linecount=GetStrLineCount(str);
    if(linecount>1)
    {
        while(1)
        {
            ch=*str;
            if(ch!='\n')
            {
                src_len++;
                str++;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        src_len=strlen(str);
    }

    //�����ַ������ַ���
    cur_font = Font_GetCurFont();
    cur_enc = Charset_NlsGetCurCharset();
    for(; src_len > 0;)
    {
        len= cur_enc->MbToUcs4(&wc, str, src_len);
        if(len == -1)
        { // ��Ч�ַ�
            src_len--;
            str++;
        }
        else if(len == 0)
        {
            break;
        }
        else
        { // ��Ч�ַ�
            str += len;
            src_len -= len;
            count++;
            continue;
        }
     }

    return count;
}

// =============================================================================
// ��������: ��ȡ�ַ�����ָ���ַ��ĵ�ַ
// �������: char *str:�ַ���
// �������: �ޡ�
// ����ֵ  :��һ���ַ�������
// =============================================================================
char * __GetCharAddr(char *str,u8 index)
{
    char *ch;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc;
    u8 src_len=0;
    s32 len;
    u8 count=0;
    src_len=strlen(str);

    if(index==1)
        return str;

    for(; src_len > 0;)
    {
        len= cur_enc->MbToUcs4(&wc, str, src_len);
        if(len == -1)
        { // ��Ч�ַ�
            src_len--;
            str++;
        }
        else if(len == 0)
        {
            break;
        }
        else
        { // ��Ч�ַ�
            str += len;
            src_len -= len;
            count++;
            if(count==index-1)
            {
                return str;
            }
            continue;
        }
     }
}
// =============================================================================
// ��������: label�ؼ����ƺ���
// �������: pMsg,������Ϣ�ṹ��ָ��
// �������: �ޡ�
// ����ֵ  :�ޡ�
// =============================================================================
static  void TextBox_paint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc,rc0;
    hwnd=pMsg->hwnd;
    u8 linecount;
    char *str;
    u8 count;
    u32 flag;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        //����TextBoxֻ����ʾ������Ϣ,������ж�һ���ַ����Ƿ��������,��Ϊ����,��
        //ֻ��ʾ��һ����Ϣ,��������Ϣֱ�Ӻ���.
       str=hwnd->Text;
       count=__GetValidTextCount(str);
       GetClientRect(hwnd,&rc);
       SetFillColor(hdc,RGB(255,255,255));
       FillRect(hdc,&rc);
       if(hwnd->Style&WS_BORDER)
       {
             if(hwnd->Style&BORDER_FIXED3D)
             {
                 SetDrawColor(hdc,RGB(173,173,173));
                 DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
                 SetDrawColor(hdc,RGB(234,234,234));
                 DrawLine(hdc,0,0,RectW(&rc)-1,0);   //U
                 DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
                 DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D
             }
             else
             {
                 SetDrawColor(hdc,RGB(169,169,169));
                 DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
                 DrawLine(hdc,0,0,RectW(&rc)-1,0);   //U
                 DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
                 DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D
             }
        }
        SetTextColor(hdc,RGB(255,255,0));
        DrawText(hdc,str,count,&rc,DT_VCENTER|DT_CENTER);
        EndPaint(hwnd,hdc);
      }
}
// =============================================================================
// ��������:��TextBox��Text���Ը�ֵ
// �������: TextBox * pTxtBox
// �������: �ޡ�
// ����ֵ  :�ޡ�
// ˵�����ڵ�ǰ
// =============================================================================
static void TextBox_SetText(TextBox * pTxtBox,char *str)
{
    WINDOW *hwnd;
    hwnd=pTxtBox->hwnd;
    u8 len,i;
    len=strlen(str);
    if(len>256)
    {
        len=256;
    }
    for(i=0;i<len;i++)
    {
        hwnd->Text[i]=*(str+i);
    }
    PostMessage(hwnd,MSG_PAINT,0,0);
}

// =============================================================================
// ��������:��TextBox������ַ���
// �������: pMsg,������Ϣ�ṹ��ָ��
// �������: �ޡ�
// ����ֵ  :�ޡ�
// ˵�����ڵ�ǰ
// =============================================================================
static void TextBox_AddText(TextBox * pTxtBox,char *str,u8 len)
{
     WINDOW *hwnd;
     char *text;
     u8 Len,i;
     char ch;
     u8 str_len=0;
     text=hwnd->Text;
     for(i=0;i<len;i++)
     {
         ch=*str;
         if(ch!='\n')
         {
             str_len++;
             str++;
         }
         else
         {
             break;
         }
     }
     //��ȡԭ�ַ�������
     Len=pTxtBox->len;
     //���ַ�������ԭ���ַ�������
     for(i=0;i<str_len;i++)
     {
        *(text+Len+i)=*(str+i);
     }
     PostMessage(hwnd,MSG_PAINT,0,0);
}

// =============================================================================
// ��������:��TextBoxָ��λ�ÿ�ʼɾ���ַ���
// �������: pTxtBox,TextBox�ṹ��ָ�룻
//          index,�ַ���ţ���1��ʼ��
//          count,�ַ���.
// �������: �ޡ�
// ����ֵ  :�ޡ�
// ˵����
// =============================================================================
static void TextBox_DeleteChar(TextBox * pTxtBox,u8 index,u8 count)
{
     WINDOW *hwnd;
     char *str;
     char *temp_str;
     u8 str_len=0;
     u8 i;
     u8 f_len,last_len;
     str=hwnd->Text;
     char *start_ch;
     char *end_ch;
     char *text;
     str_len=strlen(str);   //�ַ���ԭ�е��ֽ���
     //��ȡ��ָ����ŵ��ַ�����ʼ��ַ
     start_ch=__GetCharAddr(str,index);
     f_len=start_ch-str;
     for(i=0;i<f_len;i++)
     {
         *(text+i)=*(str+i);
     }
     end_ch=__GetCharAddr(str,index+count);
     last_len=str+str_len-end_ch;
     for(i=0;i<last_len;i++)
     {
         *(text+f_len+i)=*(end_ch+i);
     }
     *(text+f_len+last_len)='\0';
     PostMessage(hwnd,MSG_PAINT,0,0);
}

// =============================================================================
// ��������:��TextBoxָ��λ�ÿ�ʼ�����ַ���
// �������: pMsg,������Ϣ�ṹ��ָ��
// �������: �ޡ�
// ����ֵ  :�ޡ�
// ˵�����ڵ�ǰ
// =============================================================================
static bool_t TextBox_InsertChar(TextBox *pRxtBox,u8 index,char *pText,u8 count)
{
     WINDOW *hwnd;
     char temp_str[256];
     char *str;
     u8 str_len=0;
     u8 i,ch_num;
     u8 ch_count;
     u8 f_len,last_len;
     str=hwnd->Text;
     char *start_ch;
     char *end_ch;
     char *text;
     struct FontRsc* cur_font;
     struct Charset* cur_enc;
     str_len=strlen(str);   //�ַ���ԭ�е��ֽ���
     ch_num=pRxtBox->chnum;

     if(ch_num+count>CN_CHAR_NUM_LIMIT)
     {
         //��ȡ��ָ����ŵ��ַ�����ʼ��ַ
         start_ch=__GetCharAddr(str,index);

     }

//   last_len=str+str_len-start_ch;
//   temp_str=(char *)malloc(last_len);
//   if(temp_str!=NULL)
//   {
//       for(i=0;i<last_len;i++)
//       {
//           *(temp_str+i)=*(start_ch+i);
//       }
//   }
//
//   //��ָ��λ�ý��ַ�������
//   for(i=0;i<len;i++)
//   {
//       *(start_ch+i)=*(pText+i);
//   }
//   //��ԭ�б�������ַ�������
//   for(i=0;i<last_len;i++)
//   {
//       *(start_ch+len+i)=*(temp_str+i);
//   }


}

// =============================================================================
// ��������: label�ؼ���Ϣ��Ӧ����
// �������: pMsg,������Ϣ�ṹ��ָ��
// �������: �ޡ�
// ����ֵ  :�ޡ�
// =============================================================================
u32 TextBox_proc(struct WindowMsg *pMsg)
{
    HWND hwnd;
    RECT rc;
    HDC hdc;
    hwnd =pMsg->hwnd;
    GetWindowRect(hwnd,&rc);
    hdc =BeginPaint(hwnd);
    switch(pMsg->Code)
    {
        case    MSG_CREATE:
                printf("textbox[%04XH]: MSG_CREATE.\r\n",hwnd->WinId);
                return 1;
                ////
        case    MSG_LBUTTON_DOWN:
                {
                     SetDrawColor(hdc,RGB(220,220,220));
                     DrawDottedLine(hdc,0,0,0,RectH(&rc)); //L
                     DrawDottedLine(hdc,0,0,RectW(&rc),0);   //U
                     DrawDottedLine(hdc,RectW(&rc),0,RectW(&rc),RectH(&rc)); //R
                     DrawDottedLine(hdc,0,RectH(&rc),RectW(&rc),RectH(&rc)); //D
                     InvalidateWindow(hwnd,FALSE);
                }
                break;
                ////
        case    MSG_LBUTTON_UP:
                {
//                    switch(_get_button_type(hwnd))
//                    {
//                        case    BS_NORMAL:
//                                hwnd->Style &= ~BS_PUSHED;
//                                PostMessage(Gdd_GetWindowParent(hwnd),MSG_NOTIFY,(BTN_UP<<16)|(hwnd->WinId),(ptu32_t)hwnd);
//                                break;
//                                ////
//                        case    BS_HOLD:
//                                break;
//                                ////
//                    }
                    InvalidateWindow(hwnd,TRUE);
                }
                break;
                ////
        case    MSG_PAINT:
                TextBox_paint(pMsg);
                return 1;
                ////

        case    MSG_DESTROY:
                printf("textbox[%04XH]: MSG_DESTROY.\r\n",hwnd->WinId);
                return 1;
                ////

        default:
                return DefWindowProc(pMsg);

    }
    return 0;

}
#endif



