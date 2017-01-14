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
//����:  LiuWei.
//�汾��V1.0.0
//�ļ�����: ���ڼ��ؼ����Ƽ�����
//����˵��:
//�޶���ʷ:
//2. ...
//1. ����: 2014-11-10
//   ����:  LiuWei.
//   �°汾�ţ�V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------

#include    <gui/gdd/gdd_private.h>


/*
typedef struct  __WNDCLASS  //���������ݽṹ
{
    WNDPROC *WinProc;
    u32 DrawColor;
    u32 FillColor;
    u32 TextColor;
}WNDCLASS;
*/

extern HWND g_CursorHwnd;         //��괰��
extern void GK_ApiSetVisible(struct GkWinRsc *gkwin, u32 visible,u32 SyncTime);

/*============================================================================*/

static  HWND HWND_Desktop=NULL;
static  HWND HWND_Focus=NULL;

//----��������------------------------------------------------------------------
//����: ��������,���ڶԴ��ڻ������,�ú�������TRUEʱ,�������HWND_Unlock����;
//      �����ú�������FALSE,���������HWND_Unlock.
//������hwnd:���ھ��
//���أ��ɹ�:TRUE; ʧ��:FLASE;
//------------------------------------------------------------------------------
bool_t    HWND_Lock(HWND hwnd)
{
    return GDD_Lock();
}

//----ȡ���ھ��---------------------------------------------------------------
//���ܣ�����gkwinָ�룬ȥ������gdd���ھ��
//������gkwin��gkwinָ��
//���أ�gdd���ھ��
//---------------------------------------------------------------------------
HWND GetGddHwnd(struct GkWinRsc *gkwin)
{
    return (HWND)GK_ApiGetUserTag(gkwin);
}
//----��������------------------------------------------------------------------
//����: �����������ɹ���,�ɸú������н�������.
//������hwnd:���ھ��
//���أ���
//------------------------------------------------------------------------------
void    HWND_Unlock(HWND hwnd)
{
    GDD_Unlock();
}

//----��Ļ����ת�ͻ�����---------------------------------------------------------
//����: ��һ����Ļ����ֵ��ת��Ϊ���ڿͻ������������
//������hwnd:���ھ��
//      pt:  ��Ҫת������Ļ�����ָ��
//      count: ��Ҫת�������������
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    ScreenToClient(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            for(i=0;i<count;i++)
            {
                pt[i].x -= hwnd->CliRect.left;
                pt[i].y -= hwnd->CliRect.top;
            }
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return  FALSE;
}

//----�ͻ�����ת��Ļ����---------------------------------------------------------
//����: ��
//������hwnd:���ھ��
//      pt:  ��Ҫת���������ָ��
//      count: ��Ҫת�������������
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    ClientToScreen(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            for(i=0;i<count;i++)
            {
                pt[i].x += hwnd->CliRect.left;
                pt[i].y += hwnd->CliRect.top;
            }
            HWND_Unlock(hwnd);
            return  TRUE;
        }

    }
    return  FALSE;
}

//----��Ļ����ת��������---------------------------------------------------------
//����: ��
//������hwnd:���ھ��
//      pt:  ��Ҫת���������ָ��
//      count: ��Ҫת�������������
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    ScreenToWindow(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    RECT rc;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            GK_ApiGetArea(hwnd->pGkWin, &rc);
            for(i=0;i<count;i++)
            {
                pt[i].x -= rc.left;
                pt[i].y -= rc.top;
            }
            HWND_Unlock(hwnd);
            return  TRUE;
        }
    }
    return  FALSE;
}

//----��������ת��Ļ����---------------------------------------------------------
//����: ��
//������hwnd:���ھ��
//      pt:  ��Ҫת���������ָ��
//      count: ��Ҫת�������������
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    WindowToScreen(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    RECT rc;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            GK_ApiGetArea(hwnd->pGkWin, &rc);
            for(i=0;i<count;i++)
            {
                pt->x += rc.left;
                pt->y += rc.top;
            }
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return FALSE;
}

//----������洰�ھ��-----------------------------------------------------------
//����: ��
//��������
//���أ����洰�ھ��.
//------------------------------------------------------------------------------
HWND    GetDesktopWindow(void)
{
    HWND hwnd;

    hwnd =NULL;
    if(HWND_Lock(HWND_Desktop))
    {
        hwnd =HWND_Desktop;
        HWND_Unlock(HWND_Desktop);
    }
    return hwnd;
}

//----��ô��ھ�����-----------------------------------------------------------
//����: ��ô��ھ�����,����Ϊ��Ļ����.
//������hwnd:���ھ��
//      prc:���ڱ���������ݵ�ָ��.
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    GetWindowRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        GK_ApiGetArea(hwnd->pGkWin, prc);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----��ô��ڿͻ�������---------------------------------------------------------
//����: ��ô��ڿͻ�������,����Ϊ�ͻ�����.
//������hwnd:���ھ��
//      prc:���ڱ���������ݵ�ָ��.
//���أ�TRUE:�ɹ�; FALSE:ʧ��
//------------------------------------------------------------------------------
bool_t    GetClientRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        prc->left =0;
        prc->top =0;
        prc->right =RectW(&hwnd->CliRect);
        prc->bottom =RectH(&hwnd->CliRect);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----��ô��ڿͻ���������ת��Ϊ��Ļ����------------------------------------------
//����: ��
//������hwnd:���ھ��
//      prc:���ڱ���������ݵ�ָ��.
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    GetClientRectToScreen(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    {
        if(HWND_Lock(hwnd))
        {
            CopyRect(prc,&hwnd->CliRect);
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return FALSE;
}

//----��ô�����Ϣ����-----------------------------------------------------------
//����: �ú���Ϊ�ڲ�����,����麯�������Ϸ���,���̰߳�ȫ
//������hwnd:���ھ��
//���أ�������Ϣ����ָ��.
//------------------------------------------------------------------------------
struct WindowMsgQ*   __GetWindowMsgQ(HWND hwnd)
{
    return hwnd->pMsgQ;
}

//----��ô����߳�Id-------------------------------------------------------------
//����: �ú���Ϊ�ڲ�����,����麯�������Ϸ���,���̰߳�ȫ
//������hwnd:���ھ��
//���أ������߳�Id.
//------------------------------------------------------------------------------
u32 __GetWindowEvent(HWND hwnd)
{
    return hwnd->EventID;
}

//----��ô��ڷ��---------------------------------------------------------------
//����: �ú���Ϊ�ڲ�����,����麯�������ĺϷ�,���̰߳�ȫ
//������hwnd:���ھ��
//���أ����ڷ��.
//------------------------------------------------------------------------------
u32 GetWindowStyle(HWND hwnd)
{
    u32 style=0;

    if(HWND_Lock(hwnd))
    {
        style =hwnd->Style;
        HWND_Unlock(hwnd);
    }
    return style;
}

//----��ô���˽������-----------------------------------------------------------
//����: ��
//������hwnd:���ھ��
//���أ�����˽������.
//------------------------------------------------------------------------------
void* GetWindowPrivateData(HWND hwnd)
{
    void *data;

    data=NULL;
    if(HWND_Lock(hwnd))
    {
        data = hwnd->PrivateData;
        HWND_Unlock(hwnd);
    }
    return data;
}

//----���ô���˽������-----------------------------------------------------------
//����: ��
//������hwnd:���ھ��
//      data: ˽������
//���أ���.
//------------------------------------------------------------------------------
void SetWindowPrivateData(HWND hwnd,void *data)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->PrivateData=data;
        HWND_Unlock(hwnd);
    }
}

//----ȡ������-----------------------------------------------------------------
//���ܣ�ȡ���ڵĸ����ھ��
//������hwnd�����ھ��
//���أ������ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND Gdd_GetWindowParent(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetParent(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}
//----ȡ�Ӵ���-----------------------------------------------------------------
//���ܣ�ȡ���ڵ��Ӵ��ھ��
//������hwnd�����ھ��
//���أ��Ӵ��ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND GetWindowChild(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetChild(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----ȡһ��ǰ����-------------------------------------------------------------
//���ܣ�ȡ���ڵ�ǰ���ھ��
//������hwnd�����ھ��
//���أ�ǰ���ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND GetWindowPrevious(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetPrevious(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----ȡһ���󴰿�-------------------------------------------------------------
//���ܣ�ȡ���ڵĺ󴰿ھ��
//������hwnd�����ھ��
//���أ��󴰿ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND GetWindowNext(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetNext(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----ȡһ����ǰ����-------------------------------------------------------------
//���ܣ�ȡ����ͬ�������е���ǰ���ھ��
//������hwnd�����ھ��
//���أ���ǰ���ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND GetWindowFirst(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetFirst(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----ȡһ����󴰿�-------------------------------------------------------------
//���ܣ�ȡ���ڵ�ͬ����������󴰿ھ��
//������hwnd�����ھ��
//���أ���󴰿ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
HWND GetWindowLast(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetLast(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----ȡһ����󴰿�-------------------------------------------------------------
//���ܣ�ȡ���ڵ�ͬ����������󴰿ھ��
//������hwnd�����ھ��
//���أ���󴰿ھ�������򷵻�NULL
//-----------------------------------------------------------------------------
char *GetWindowText(HWND hwnd)
{
    return hwnd->Text;
}

//----��Id���Ҵ����е��Ӵ���(�ؼ�)-----------------------------------------------
//����: ��.
//������hwnd:���ھ��
//     Id: ��Ҫ���ҵ��Ӵ���(�ؼ�)Id.
//���أ����ҵ����Ӵ���(�ؼ�)���.
//------------------------------------------------------------------------------
HWND    GetDlgItem(HWND hwnd,u16 Id)
{
    HWND Current = hwnd;
    if(HWND_Lock(hwnd))
    {
        do
        {
            Current = (HWND)GK_ApiGetUserTag(GK_ApiTraveChild(hwnd->pGkWin,Current->pGkWin));
            if(Current->WinId == Id)
                break;
        }while(Current != NULL);
        HWND_Unlock(hwnd);
    }
    return Current;
}

//----��������Ҵ���(�����ڻ��Ӵ���)-------------------------------------------
//����: ��.
//������pt:�����
//���أ����ҵ��Ĵ��ھ��.
//------------------------------------------------------------------------------
HWND    GetWindowFromPoint(struct GkWinRsc *desktop, POINT *pt)
{
    struct GkWinRsc *GkWin;
    if((NULL == desktop) || (NULL == pt))
        return NULL;
    if(GDD_Lock( ))
    {
        GkWin = GK_ApiGetWinFromPt(desktop, pt);
        GDD_Unlock( );
    }
    if(GkWin != NULL)
        return (HWND)GK_ApiGetUserTag(GkWin);
    else
        return NULL;
}

//----���ô�������-------------------------------------------------------------
//����: ��.
//������hwnd: �����õĴ���
//      NewName���µ�����
//���أ��ɵĽ��㴰��.
//------------------------------------------------------------------------------
void SetWindowName(HWND hwnd, char *NewName)
{
    GK_ApiSetName(hwnd->pGkWin, NewName);
    return ;
}

//----���ý��㴰��---------------------------------------------------------------
//����: ��.
//������hwnd: �µĽ��㴰��
//���أ��ɵĽ��㴰��.
//------------------------------------------------------------------------------
HWND    SetFocusWindow(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock())
    {
        wnd =HWND_Focus;
        HWND_Focus =hwnd;
        GDD_Unlock();
    }
    else
    {
        return NULL;
    }

    if(wnd!=NULL)
    {
        SendMessage(wnd,MSG_KILLFOCUS,0,0);
    }
    if(hwnd!=NULL)
    {
        SendMessage(hwnd,MSG_SETFOCUS,0,0);
    }

    return wnd;

}

//----��ý��㴰��---------------------------------------------------------------
//����: ��.
//��������.
//���أ����㴰��.
//------------------------------------------------------------------------------
HWND    GetFocusWindow(void)
{
    HWND wnd=NULL;

    if(GDD_Lock())
    {
        wnd =HWND_Focus;
        GDD_Unlock();
    }
    return wnd;
}

//----�ж��Ƿ�Ϊ���㴰��---------------------------------------------------------
//����: ��.
//��������.
//���أ����ָ���Ĵ����ǽ��㴰��,������TRUE,���򷵻�FALSE.
//------------------------------------------------------------------------------
bool_t    IsFocusWindow(HWND hwnd)
{
    bool_t res=FALSE;

    if(HWND_Lock(hwnd))
    {
        if(GDD_Lock())
        {
            if(hwnd == HWND_Focus)
            {
                res=TRUE;
            }
            GDD_Unlock();
        }
        HWND_Unlock(hwnd);
    }
    return res;

}

//----��ʼ���������ݽṹ---------------------------------------------------------
//����: �ú���Ϊ�ڲ�����.
//������pwin:�������ݽṹָ��.
//      proc:���ڹ��̻ص�����ָ�룮
//      Text:������������ָ��.
//      Style:���ڷ��.
//      x,y,w,h:����λ�úʹ�С.
//      hParent:�����ھ��.
//      WinId: ����Id.
//���أ���.
//------------------------------------------------------------------------------
static  void    __InitWindow(WINDOW *pwin,u32 Style,u32 WinId)
{
    RECT rc;

    pwin->Flag =0;

    pwin->Style     =Style;
    pwin->WinId     =WinId&0x0000FFFF;
    pwin->Text = pwin->pGkWin->win_name;
    dListInit(&pwin->list_timer);
    dListInit(&pwin->node_msg_close);
    dListInit(&pwin->node_msg_ncpaint);
    dListInit(&pwin->node_msg_paint);
    GK_ApiGetArea(pwin->pGkWin, &rc);

    if(Style&WS_BORDER)
    {
        pwin->BorderSize =DEF_BORDER_SIZE;
        __InflateRectEx(&rc, -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE);

    }
    else
    {
        pwin->BorderSize =0;
    }

    if(Style&WS_DLGFRAME)
    {
//      pwin->DlgFrameSize =DEF_DLGFRAME_SIZE;
        __InflateRectEx(&rc, -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE);
    }
//    else
//    {
//        pwin->DlgFrameSize =0;
//    }

    if(Style&WS_CAPTION)
    {
        pwin->CaptionSize =DEF_CAPTION_SIZE;
        __InflateRectEx(&rc,0,-DEF_CAPTION_SIZE,0,0);
    }
    else
    {
        pwin->CaptionSize =0;
    }
    CopyRect(&pwin->CliRect,&rc);

}


//������ƺ���
bool_t DesktopPaint(struct WindowMsg *pMsg)
{
    HDC hdc;
    RECT rc;

    HWND hwnd=pMsg->hwnd;
    hdc =BeginPaint(hwnd);

    GetClientRect(hwnd,&rc);

    GradientFillRect(hdc,&rc,RGB(120,120,255),RGB(20,20,80),CN_FILLRECT_MODE_UD);

    EndPaint(hwnd,hdc);
    return true;
}


//----�����ƺ���-------------------------------------------------------------
//���ܣ����ǰ�ť�ؼ���MSG_PAINT��Ϣ��Ӧ����
//������pMsg����Ϣָ��
//���أ��̶�true
//-----------------------------------------------------------------------------
static bool_t __Cursor_TmrHandle(struct WinTimer *pTmr)
{
    bool_t ret,bIsStart;
    bIsStart=pTmr->Flag;
    if(bIsStart&TMR_START)
    {
        ret=IsWindowVisible(g_CursorHwnd);
        if(ret)
        {
             SetWindowHide(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
        else
        {
             SetWindowShow(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
    }
    else
    {
        ret=IsWindowVisible(g_CursorHwnd);
        if(ret)
        {
             SetWindowHide(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
    }
    return true;
}

//----�����ƺ���-------------------------------------------------------------
//���ܣ����ǰ�ť�ؼ���MSG_PAINT��Ϣ��Ӧ����
//������pMsg����Ϣָ��
//���أ��̶�true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrHandle(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    switch(TmrId)
    {
       case CN_CURSOR_TIMER_ID:
           __Cursor_TmrHandle(pTmr);
       break;
       default:
          break;
    }

    return true;
}
//----�����ƺ���-------------------------------------------------------------
//���ܣ����ǰ�ť�ؼ���MSG_PAINT��Ϣ��Ӧ����
//������pMsg����Ϣָ��
//���أ��̶�true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrStart(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    pTmr->Flag|=TMR_START;
    return true;
}

//----�����ƺ���-------------------------------------------------------------
//���ܣ����ǰ�ť�ؼ���MSG_PAINT��Ϣ��Ӧ����
//������pMsg����Ϣָ��
//���أ��̶�true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrStop(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    pTmr->Flag&=~TMR_START;
    return true;
}


//������Ϣ��������
static struct MsgProcTable s_gDesktopMsgProcTable[] =
{
    {MSG_PAINT,DesktopPaint},
    {MSG_TIMER,DesktopTmrHandle},
    {MSG_TIMER_START,DesktopTmrStart},
    {MSG_TIMER_STOP,DesktopTmrStop}
};

static struct MsgTableLink  s_gDesktopMsgLink;

//----��ʼ�����洰��-----------------------------------------------------------
//����: ��gk��winת��Ϊgdd��window.
//������proc: ���洰�ڹ��̻ص�����
//      desktop,���ʼ��������ָ��
//      pdata: �û��Զ��帽������.
//���أ����洰�ھ��.
//------------------------------------------------------------------------------
HWND    InitGddDesktop(struct GkWinRsc *desktop)
{
    WINDOW *pGddWin=NULL;
    u32 Style;

    pGddWin=malloc(sizeof(WINDOW));
    if(NULL!=pGddWin)
    {

        Style = 0;
        s_gDesktopMsgLink.LinkNext = NULL;
        s_gDesktopMsgLink.MsgNum = sizeof(s_gDesktopMsgProcTable) / sizeof(struct MsgProcTable);
        s_gDesktopMsgLink.myTable = s_gDesktopMsgProcTable;
        pGddWin->pGkWin     = desktop;
        pGddWin->PrivateData = NULL;
        pGddWin->MyMsgTableLink = &s_gDesktopMsgLink;
        pGddWin->mutex_lock =Lock_MutexCreate(NULL);
        pGddWin->pMsgQ      =GUI_CreateMsgQ(32);
        pGddWin->EventID   = Djy_MyEventId();
        pGddWin->DrawColor = CN_DEF_DRAW_COLOR;
        pGddWin->FillColor = CN_DEF_FILL_COLOR;
        pGddWin->TextColor = CN_DEF_TEXT_COLOR;
        if((pGddWin->mutex_lock==NULL)||(pGddWin->pMsgQ==NULL))
        {
            if(pGddWin->mutex_lock!=NULL)
            {
                Lock_MutexDelete(pGddWin->mutex_lock);
            }
            if(pGddWin->pMsgQ!=NULL)
            {
                GUI_DeleteMsgQ(pGddWin->pMsgQ);
            }
            free(pGddWin);
            return NULL;
        }

        GK_ApiSetUserTag(desktop,pGddWin);
        __InitWindow(pGddWin,Style,0);
        HWND_Desktop = pGddWin;
        SendMessage(pGddWin,MSG_CREATE,0,0);
    }

        return pGddWin;
}

//----��������-----------------------------------------------------------------
//����: �ú������Դ��������ں��Ӵ���(�ؼ�)
//������proc: ���ڹ��̻ص�����
//      Style: ���ڷ��(�ο� WS_CHILD �峣��)
//      x,y,w,h: ����λ�úʹ�С,����ڸ�����
//      hParent: �����ھ��.�����NULL,��Ĭ������Ϊ������.
//      WinId: ����Id.�����������,�ò���������.
//      BufProperty�����ڵ�buf���ԣ�ȡֵΪ CN_WINBUF_PARENT�ȡ�
//      pdata: �û��Զ��帽������.
//      pUserMsgTableLink:������Ӧ����Ϣ��
//���أ����ھ��.
//-----------------------------------------------------------------------------
HWND    CreateWindow(const char *Text,u32 Style,
                     s32 x,s32 y,s32 w,s32 h,
                     HWND hParent,u32 WinId,
                     u32 BufProperty,void *pdata,
                     struct MsgTableLink *pUserMsgTableLink)
{
    WINDOW *pGddWin=NULL;
    struct GkWinRsc *pGkWin=NULL;
    struct RopGroup RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };

    if(NULL==hParent)
    {
        hParent = HWND_Desktop;
    }

    if(HWND_Lock(hParent))
    {
        pGddWin=M_Malloc(sizeof(WINDOW) + sizeof(struct GkWinRsc),100*mS);
        if(NULL!=pGddWin)
        {
            pGkWin = (struct GkWinRsc*)(pGddWin+1);

            if(!GK_ApiCreateGkwin(hParent->pGkWin, pGkWin,x,y,x+w,y+h,
                                RGB(0,0,0), BufProperty, Text,
                                CN_SYS_PF_DISPLAY, 0,RGB(255, 255, 255),RopCode))
            {
                free(pGddWin);
                pGddWin = NULL;
            }
            else
            {
                if(Style&WS_CHILD)
                {
                    pGddWin->EventID   = hParent->EventID;
                    pGddWin->mutex_lock =hParent->mutex_lock;  //�Ӵ���ʹ�ø�������
                    pGddWin->pMsgQ      =hParent->pMsgQ;       //�Ӵ���ʹ�ø�������Ϣ����
                    pGddWin->DrawColor = hParent->DrawColor;
                    pGddWin->FillColor = hParent->FillColor;
                    pGddWin->TextColor = hParent->TextColor;
                }
                else
                {
                    pGddWin->EventID   = Djy_MyEventId();
                    pGddWin->mutex_lock =Lock_MutexCreate(NULL);
                    pGddWin->pMsgQ      =GUI_CreateMsgQ(32);
                    pGddWin->DrawColor = CN_DEF_DRAW_COLOR;
                    pGddWin->FillColor = CN_DEF_FILL_COLOR;
                    pGddWin->TextColor = CN_DEF_TEXT_COLOR;

                    if((pGddWin->mutex_lock==NULL)||(pGddWin->pMsgQ==NULL))
                    {
                        Lock_MutexDelete(pGddWin->mutex_lock);
                        GUI_DeleteMsgQ(pGddWin->pMsgQ);
                        GK_ApiDestroyWin(pGkWin);
                        free(pGkWin);
                        HWND_Unlock(hParent);
                        return NULL;
                    }

                }
                pGddWin->pGkWin = pGkWin;
                pGddWin->PrivateData = pdata;
                pGddWin->MyMsgTableLink = pUserMsgTableLink;
                GK_ApiSetUserTag(pGkWin,pGddWin);
                //��ʼ����������
                __InitWindow(pGddWin,Style,WinId);
                GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
                //���´�����ӵ�������
            }

        }
        HWND_Unlock(hParent);

        if(NULL!=pGddWin)
        {
            SendMessage(pGddWin,MSG_CREATE,(u32)pdata,0);
            PostMessage(pGddWin,MSG_NCPAINT,0,0);
            InvalidateWindow(pGddWin,TRUE);
        }

    }
    return pGddWin;
}

//----ɾ��һ���Ӵ������ݽṹ---------------------------------------------------
//����:�ú���Ϊ�ڲ�����,�����ͷŴ��ڽṹ�ڴ�,�����Լ��Ը�����ȥ�Ƴ�.
//��������Ҫɾ�����Ӵ��ھ��
//���أ���.
//------------------------------------------------------------------------------
static void __DeleteChildWindowData(HWND hwnd)
{
    dListRemove(&hwnd->node_msg_close);
    dListRemove(&hwnd->node_msg_ncpaint);
    dListRemove(&hwnd->node_msg_paint);
    __RemoveWindowTimer(hwnd);
    GK_ApiDestroyWin(hwnd->pGkWin);
    free(hwnd->pGkWin);
    hwnd->pGkWin =NULL;

    hwnd->mutex_lock =NULL; //�Ӵ���û��˽�е� mutex_lock,�����ͷ�.

    free(hwnd);

}
//----ɾ��һ�����������ݽṹ---------------------------------------------------
//����:�ú���Ϊ�ڲ�����,�����ͷŴ��ڽṹ�ڴ�,�����Լ��Ը�����ȥ�Ƴ�.
//��������Ҫɾ���������ھ��
//���أ���.
//------------------------------------------------------------------------------
void __DeleteMainWindowData(HWND hwnd)
{
    dListRemove(&hwnd->node_msg_close);
    dListRemove(&hwnd->node_msg_ncpaint);
    dListRemove(&hwnd->node_msg_paint);

    __RemoveWindowTimer(hwnd);
    GUI_DeleteMsgQ(hwnd->pMsgQ);
    GK_ApiDestroyWin(hwnd->pGkWin);
    UpdateDisplay(CN_TIMEOUT_FOREVER);
    free(hwnd->pGkWin);
    hwnd->pGkWin =NULL;

    Lock_MutexDelete(hwnd->mutex_lock);
    hwnd->mutex_lock =NULL;
    free(hwnd);

}

//----����һ������--------------------------------------------------------------
//����: �����������ں��Ӵ���,����ɾ�������ڽṹ,��������WSG_DESTROY��Ϣ��������.
//������hwnd:��Ҫ���ٵĴ��ھ��
//���أ���.
//------------------------------------------------------------------------------
void    DestroyWindow(HWND hwnd)
{
    struct GkWinRsc *Ancestor, *Current;

    SetWindowHide(hwnd);

    if(IsFocusWindow(hwnd))
    {
        SetFocusWindow(NULL);
    }

    GDD_Lock();
    if(HWND_Lock(hwnd))
    {
        Ancestor = hwnd->pGkWin;
        Current = Ancestor;
        Current = GK_ApiTraveScion(Ancestor,Current);
        while(Current != NULL)
        {
            __DeleteChildWindowData((HWND)GK_ApiGetUserTag(Current));
            Current = GK_ApiTraveScion(Ancestor,Current);
        }

        SendMessage(hwnd,MSG_DESTROY,0,0);
        if(hwnd->Style&WS_CHILD)
        {
            __DeleteChildWindowData(hwnd);   //���������ڣ����ᱻ����
        }
        HWND_Unlock(hwnd);

    }
    GDD_Unlock();

}

//----����ȫ���Ӵ���-----------------------------------------------------------
//����: �����������ں��Ӵ���,����ɾ�������ڽṹ,��������WSG_DESTROY��Ϣ��������.
//������hwnd:��Ҫ���ٵĴ��ھ��
//���أ���.
//------------------------------------------------------------------------------
void    DestroyAllChild(HWND hwnd)
{
    struct GkWinRsc *Ancestor, *Current;
    GDD_Lock();
    if(HWND_Lock(hwnd))
    {
        Ancestor = hwnd->pGkWin;
        Current = Container(OBJ_GetTwig(&Ancestor->node),struct GkWinRsc,node);
        while(Current != NULL)
        {
            __DeleteChildWindowData((HWND)GK_ApiGetUserTag(Current));
            Current = Container(OBJ_GetTwig(&Ancestor->node),struct GkWinRsc,node);
        }

        HWND_Unlock(hwnd);

    }
    GDD_Unlock( );

}

//----ƫ�ƴ���------------------------------------------------------------------
//����: ƫ��һ������λ��,�����Ӵ���.�ú���Ϊ�ڲ�����,���̰߳�ȫ
//������hwnd:���ھ��.
//      dx,dy: ˮƽ�ʹ�ֱ�����ƫ����.
//���أ���.
//------------------------------------------------------------------------------
static  void    __OffsetWindow(HWND hwnd,s32 dx,s32 dy)
{
    HWND wnd;
    RECT rc;
    struct GkWinRsc *Ancestor, *Current;

    Ancestor = hwnd->pGkWin;
    Current = Ancestor;
    Current = GK_ApiTraveScion(Ancestor,Current);
    __OffsetRect(&hwnd->CliRect,dx,dy);
    while(Current != NULL)
    {
        wnd = (HWND)GK_ApiGetUserTag(Current);
        OffsetRect(&wnd->CliRect,dx,dy);
        Current = GK_ApiTraveScion(Ancestor,Current);
    }
    GK_ApiGetArea(hwnd->pGkWin,&rc);
    GK_ApiMoveWin(  hwnd->pGkWin,rc.left+dx,rc.top+dy,0);

}

//----ƫ�ƴ���------------------------------------------------------------------
//����: ƫ��һ������λ��,�����Ӵ���.
//������hwnd:���ھ��.
//      dx,dy: ˮƽ�ʹ�ֱ�����ƫ����.
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    OffsetWindow(HWND hwnd,s32 dx,s32 dy)
{
    if(hwnd == HWND_Desktop)
    {//���治�����ƶ�
        return false;
    }
    if(HWND_Lock(hwnd))
    {
        __OffsetWindow(hwnd,dx,dy);
        HWND_Unlock(hwnd);
        return true;
    }
    return FALSE;

}

//----�ƶ�����------------------------------------------------------------------
//����: �ƶ�һ������λ��,�����Ӵ���.�ú���Ϊ�ڲ�����.���̰߳�ȫ.
//������hwnd:���ھ��.
//      x,y: ����ڸ����ڿͻ�������λ��.
//���أ���.
//------------------------------------------------------------------------------
static void __MoveWindow(HWND hwnd,s32 x,s32 y)
{
    HWND wnd;
    struct GkWinRsc *Ancestor, *Current;
    struct Rectangle rc;
    s32 dx,dy;
    POINT point;
    point.x=x;
    point.y=y;
    Ancestor = hwnd->pGkWin;
    Current = Ancestor;
    GK_ApiGetArea(Ancestor, &rc);//��ʾ����,��ʾ���ľ�������
    wnd=Gdd_GetWindowParent(hwnd);
    ScreenToClient(wnd,(POINT *)&rc,2);//ת��Ϊ�ͻ���
    dx = x - rc.left;
    dy = y - rc.top;
    Current = GK_ApiTraveScion(Ancestor,Current);
    __OffsetRect(&hwnd->CliRect, dx, dy);
    while(Current != NULL)
    {
        wnd = (HWND)GK_ApiGetUserTag(Current);
        __OffsetRect(&wnd->CliRect,dx,dy);
        Current = GK_ApiTraveScion(Ancestor,Current);
    }
    ClientToScreen(wnd,&point,1);
    ScreenToWindow(wnd,&point,1);
    GK_ApiMoveWin(  hwnd->pGkWin,point.x,point.y,0);
}

//----�ƶ�����------------------------------------------------------------------
//����: �ƶ�һ������λ��,�����Ӵ���.
//������hwnd:���ھ��.
//      x,y: ����ڸ����ڿͻ�������λ��.
//���أ�TRUE:�ɹ�;FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    MoveWindow(HWND hwnd,s32 x,s32 y)
{
    if(hwnd == HWND_Desktop)
    {//���治�����ƶ�
        return false;
    }
    if(HWND_Lock(hwnd))
    {
        __MoveWindow(hwnd,x,y);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----�жϴ����Ƿ�ɼ�-----------------------------------------------------------
//����: ��
//������hwnd:���ھ��.
//���أ�TRUE:���ڿɼ�;FALSE:���ڲ��ɼ�.
//------------------------------------------------------------------------------
bool_t    IsWindowVisible(HWND hwnd)
{
    bool_t res=FALSE;

    if(HWND_Lock(hwnd))
    {
        res = GK_ApiIsWinVisible(hwnd->pGkWin);
        HWND_Unlock(hwnd);
    }
    return res;
}

//----���ô���Ϊ��Ч-----------------------------------------------------------
//����: ���ô���Ϊ��Ч����˼��:������Ҫ�ػ�.�����ػ���Ϣ���,�ú���Ϊ�ڲ�����.
//������hwnd:���ھ��.
//      bErase: �Ƿ��������.
//���أ���
//------------------------------------------------------------------------------
void    __InvalidateWindow(HWND hwnd,bool_t bErase)
{
    if(IsWindowVisible(hwnd))
    {
        //if(bErase)
        {
            PostMessage(hwnd,MSG_PAINT,bErase,0);
        }

    }
}

//----���ô���Ϊ��Ч-------------------------------------------------------------
//����: ��
//������hwnd:���ھ��.
//      bErase��true= ��Ҫ����������false=����Ҫ��������
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    InvalidateWindow(HWND hwnd,bool_t bErase)
{
    if(HWND_Lock(hwnd))
    {
        __InvalidateWindow(hwnd,bErase);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}


//----��ʾ����-------------------------------------------------------------
//����: ���ô���Ϊ��ʾ(�����Ӵ���)
//������hwnd:���ھ��.
//���أ�TRUE:�ɹ�;FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t SetWindowShow(HWND hwnd)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->Style |= WS_VISIBLE;
        GK_ApiSetVisible(hwnd->pGkWin,CN_GKWIN_VISIBLE,0);
        HWND_Unlock(hwnd);
        return TRUE;
    }
     return FALSE;
}
//----���ش���-------------------------------------------------------------
//����: ���ô���Ϊ����(�����Ӵ���)
//������hwnd:���ھ��.
//���أ�TRUE:�ɹ�;FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t SetWindowHide(HWND hwnd)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->Style |=~ WS_VISIBLE;
        GK_ApiSetVisible(hwnd->pGkWin,CN_GKWIN_HIDE,0);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----��ô��ڻ�ͼ������(DC)-----------------------------------------------------
//����: �ú�����õ�DC,�������������ڷ�Χ�ڻ�ͼ���(�ǿͻ���+�ͻ���).
//������hwnd:���ھ��.
//���أ�DC���.
//------------------------------------------------------------------------------
HDC GetWindowDC(HWND hwnd)
{
    DC *pdc;

    pdc =malloc(sizeof(DC));
    if(pdc!=NULL)
    {
        __InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_WINDOW);
    }
    return DC2HDC(pdc);

}

//----��ô��ڿͻ�����ͼ������(DC)-----------------------------------------------
//����: �ú�����õ�DC,ֻ���ڴ��ڿͻ�����Χ�ڻ�ͼ���).
//������hwnd:���ھ��.
//���أ�DC���.
//------------------------------------------------------------------------------
HDC GetDC(HWND hwnd)
{
    DC *pdc;

    pdc =malloc(sizeof(DC));
    if(pdc!=NULL)
    {
        __InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_CLIENT);
    }
    return DC2HDC(pdc);

}

//----��ô��ڿͻ�����ͼ������(DC)-----------------------------------------------
//����: �ú�����õ�DC,ֻ���ڴ��ڿͻ�����Χ�ڻ�ͼ���).
//������hwnd:���ھ��.
//���أ�DC���.
//------------------------------------------------------------------------------
bool_t    ReleaseDC(HWND hwnd,HDC hdc)
{
    return  DeleteDC(hdc);
}

//----ָ�����ڿ�ʼ��ͼ-----------------------------------------------------------
//����: �ú���ֻ����MSG_PAINT�е���,������EndPaint�ɶ�ʹ��.
//������hwnd: ���ھ��.
//���أ�DC���.
//------------------------------------------------------------------------------
HDC BeginPaint(HWND hwnd)
{
    HDC hdc;
    struct WindowMsg msg;

    hdc =malloc(sizeof(DC));
    if(hdc!=NULL)
    {
        __InitDC(hdc,hwnd->pGkWin,hwnd,DC_TYPE_PAINT);

        if(HWND_Lock(hwnd))
        {
            if(hwnd->Flag&WF_ERASEBKGND)
            {
                hwnd->Flag &= ~WF_ERASEBKGND;

                msg.hwnd =hwnd;
                msg.Code =MSG_ERASEBKGND;
                msg.Param1 =(ptu32_t)hdc;
                msg.Param2 =0;
                msg.ExData =NULL;
                WinMsgProc(&msg);
            }

            HWND_Unlock(hwnd);
        }
        return hdc;
    }

    return NULL;
}

//----ָ�����ڽ�����ͼ-----------------------------------------------------------
//����: �ú���ֻ����MSG_PAINT�е���,������BeginPaint�ɶ�ʹ��.
//������hwnd: ���ھ��.
//      hdc: DC���,������BeginPaint���ص�DC���.
//���أ�TRUE:�ɹ�; FALSE:ʧ��.
//------------------------------------------------------------------------------
bool_t    EndPaint(HWND hwnd,HDC hdc)
{
    UpdateDisplay(CN_TIMEOUT_FOREVER);

    return  DeleteDC(hdc);
}

//----Ĭ�ϵĴ��ڷǿͻ���������Ϣ������---------------------------------------
//����: ���ƴ��ڵķǿͻ���.
//������pMsg: ��Ϣָ��.
//���أ���.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_NCPAINT(struct WindowMsg *pMsg)
{
    HWND hwnd=pMsg->hwnd;
    HDC hdc;
    RECT rc;

    if(HWND_Lock(hwnd))
    {
        hdc =GetWindowDC(hwnd);
        if(NULL!=hdc)
        {
            GetWindowRect(hwnd,&rc);
            ScreenToWindow(hwnd,(POINT*)&rc,2);

            if(hwnd->Style&WS_BORDER)
            {
                SetDrawColor(hdc,WINDOW_BORDER_COLOR);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);
            }

            if(hwnd->Style&WS_DLGFRAME)
            {
                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR1);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR2);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR3);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

            }

            if(hwnd->Style&WS_CAPTION)
            {
                rc.bottom =rc.top+hwnd->CaptionSize;

                GradientFillRect(hdc,&rc,RGB(0,100,200),RGB(0,30,100),CN_FILLRECT_MODE_UD);

                SetTextColor(hdc,WINDOW_CAPTION_TEXT_COLOR);
                InflateRect(&rc,-1,-1);
                DrawText(hdc, hwnd->Text, -1, &rc, DT_LEFT | DT_VCENTER);
            }

            ReleaseDC(hwnd,hdc);
        }

        HWND_Unlock(hwnd);
    }
    return true;
}

//----Ĭ�ϵĴ��ڿͻ�������������Ϣ������-------------------------------------
//����: ʹ�ô��ڵ�Ĭ�ϱ�����ɫ����䴰�ڿͻ�������.
//������pMsg: ��Ϣָ��.
//���أ���.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_ERASEBKGND(struct WindowMsg *pMsg)
{
    HDC hdc;
    RECT rc;

    hdc =(HDC)pMsg->Param1;
    if(hdc!=NULL)
    {
        GetClientRect(pMsg->hwnd,&rc);
        FillRect(hdc,&rc);
        return TRUE;
    }
    return FALSE;
}

//----Ĭ�ϵĴ��ڿͻ���������Ϣ������-------------------------------------
//����: ����ûʲô�������ȿ��š�.
//������pMsg: ��Ϣָ��.
//���أ���.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_PAINT(struct WindowMsg *pMsg)
{
    return true;
}

//----Ĭ�ϵĴ��ڹر���Ϣ������-------------------------------------
//����: �رմ���
//������pMsg: ��Ϣָ��.
//���أ���.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_CLOSE(struct WindowMsg *pMsg)
{
    DestroyWindow(pMsg->hwnd);
    return TRUE;
}

//----Ĭ�ϵ����ٴ�����Ϣ������-------------------------------------
//����: ����.
//������pMsg: ��Ϣָ��.
//���أ���.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_DESTROY(struct WindowMsg *pMsg)
{
    PostQuitMessage(pMsg->hwnd,0);
    return TRUE;
}

//Ĭ�ϴ�����Ϣ�������������û����ڹ���û�д������Ϣ��
static struct MsgProcTable s_gDefWindowMsgProcTable[] =
{
    {MSG_NCPAINT,DefWindowProc_NCPAINT},
    {MSG_ERASEBKGND,DefWindowProc_ERASEBKGND},
    {MSG_PAINT,DefWindowProc_PAINT},
    {MSG_CLOSE,DefWindowProc_CLOSE},
    {MSG_DESTROY,DefWindowProc_DESTROY},
};

//----����Ϣ���������в��Ҷ�Ӧ����Ϣ-----------------------------------------
//���ܣ���
//������pMsg��Ŀ����Ϣ
//      MsgTable��Դ����0������
//���أ����ҵ����򷵻���Ϣ��MsgTable�е�ƫ�ƣ����򷵻�-1
//-----------------------------------------------------------------------------
s32 GetWinMsgFunc(struct WindowMsg *pMsg,struct MsgProcTable *MsgTable,u32 Num)
{
    u32 loop = 0;
    u32 Code,CodeTab;
    if((pMsg == NULL) || (MsgTable == NULL))
        return -1;
    Code = pMsg->Code;
    for(loop = 0; loop < Num; loop++)
    {
        CodeTab = MsgTable[loop].MsgCode;
        if(Code == CodeTab)
        {
            break;
        }
        else if(CodeTab == 0)
        {
            loop = -1;
            break;
        }
    }
    if(loop >= Num)
        loop = -1;
    return loop;

}
//----������Ϣ������---------------------------------------------------------
//����: ��������Ϣ����Ϣ����hwnd->MyMsgTableLink��Աʵ�ָ��ǣ�ֻҪ���Ӵ�����
//      ʵ��ͬ����Ϣ�������ڵĸ���Ϣ�������ͻᱻ���滻������
//������pMsg: ��Ϣָ��.
//���أ���Ϣ������.
//-----------------------------------------------------------------------------
ptu32_t WinMsgProc(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u32 offset;
    struct MsgTableLink *MyTable;

    hwnd = pMsg->hwnd;
    MyTable = hwnd->MyMsgTableLink;
    //����һ������Ϣ����̳л��ƣ��ȴ����һ���̳г���
    //�Կؼ���paint��Ϣ����Ϊ��˵��һ�£�
    //һ����˵��MyMsgTableLinkֱ��ָ������û�����Ϣ��������û������ҵ���
    //      paint��Ϣ����������ú󷵻ء�����
    //      MyMsgTableLink->LinkNextָ����ǿؼ�����Ϣ������һ�����ҵ� paint
    //      ��Ϣ�����������ú󷵻ء�����
    //      �Ӵ���ϵͳ��Ĭ����Ϣ�������� s_gDefWindowMsgProcTable���ҡ�
    //���������C++�ļ̳л��ơ��൱���û��̳пؼ����ؼ��̳д���ϵͳ��
    while(MyTable != NULL)
    {
        if(MyTable->myTable != NULL)
        {
            offset = GetWinMsgFunc(pMsg, MyTable->myTable, MyTable->MsgNum);
            if(offset != -1)
            {
                return MyTable->myTable[offset].MsgProc(pMsg);
            }
        }
        MyTable = MyTable->LinkNext;
    }

    offset = GetWinMsgFunc(pMsg, s_gDefWindowMsgProcTable,
                sizeof(s_gDefWindowMsgProcTable) / sizeof(struct MsgProcTable));
    if(offset != -1)
    {
        return s_gDefWindowMsgProcTable[offset].MsgProc(pMsg);
    }
    else
        return false;

}

//----GDD����ģ���ʼ������------------------------------------------------------
//����: �ú���Ϊ�ڲ�����.
//��������
//���أ���
//------------------------------------------------------------------------------
void    GDD_WindowInit(void)
{
}

