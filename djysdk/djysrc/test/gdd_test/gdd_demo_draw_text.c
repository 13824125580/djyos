#include <stdint.h>
#include <stddef.h>
#include <gkernel.h>
#include <gdd.h>
#include <widget.h>

/*============================================================================*/
#define ID_CLOSE    0x1000
#define ID_BORDER   0x1100
#define ID_BKGND    0x1101
#define ID_COLOR    0x1102

static const struct   {
    const char *text;
    u32 flag;
}DrawText_Cfg_Tbl[]={
		{"���������+���Ķ��˶���"           ,DT_LEFT|DT_TOP},
		{"���������+���Ĵ�ֱ����"           ,DT_LEFT|DT_VCENTER},
		{"���������+���ĵײ�����"           ,DT_LEFT|DT_BOTTOM},

		{"ˮƽ����+���Ķ��˶���"             ,DT_CENTER|DT_TOP},
		{"ˮƽ����+���Ĵ�ֱ����"             ,DT_CENTER|DT_VCENTER},
		{"ˮƽ����+���ĵײ�����"             ,DT_CENTER|DT_BOTTOM},

		{"ˮƽ�Ҷ���+���Ķ��˶���"            ,DT_RIGHT|DT_TOP},
		{"ˮƽ�Ҷ���+���Ĵ�ֱ����"            ,DT_RIGHT|DT_VCENTER},
		{"ˮƽ�Ҷ���+���ĵײ�����"            ,DT_RIGHT|DT_BOTTOM},

		{"������ʾ1\r\n���������+���Ķ��˶���"  ,DT_LEFT|DT_TOP},
		{"������ʾ2\r\n���������+���Ĵ�ֱ����"  ,DT_LEFT|DT_VCENTER},
		{"������ʾ3\r\n���������+���ĵײ�����"  ,DT_LEFT|DT_BOTTOM},

		{"������ʾ4\r\nˮƽ����+���Ķ��˶���"   ,DT_CENTER|DT_TOP},
		{"������ʾ5\r\nˮƽ����+���Ĵ�ֱ����"   ,DT_CENTER|DT_VCENTER},
		{"������ʾ6\r\nˮƽ�Ҷ���+���ĵײ�����"  ,DT_CENTER|DT_BOTTOM},

		{"������ʾ7\r\nˮƽ�Ҷ���+���Ķ��˶���"  ,DT_RIGHT|DT_TOP},
		{"������ʾ8\r\nˮƽ�Ҷ���+��ֱ����"     ,DT_RIGHT|DT_VCENTER},
		{"������ʾ9\r\nˮƽ�Ҷ���+���ĵײ�����"  ,DT_RIGHT|DT_BOTTOM},

        {NULL,NULL},
};

static const struct   {
    u32 text_color;
    u32 bd_color;
    u32 bk_color;
}DrawText_Color_Tbl[8]={

        {RGB(255,255,255),RGB(255,255,255),RGB(200,0,240)},
        {RGB(1,1,1),RGB(0,0,0),RGB(255,255,255)},
        {RGB(255,0,0),RGB(255,0,0),RGB(0,0,0)},
        {RGB(0,255,0),RGB(0,255,0),RGB(0,0,0)},
        {RGB(0,100,255),RGB(0,100,255),RGB(0,0,0)},
        {RGB(1,1,1),RGB(0,0,0),RGB(240,200,0)},
        {RGB(255,255,0),RGB(0,0,0),RGB(0,100,100)},
        {RGB(255,0,255),RGB(0,0,0),RGB(100,0,100)},
};

static  s8 cfg_idx=0;
static  s8 color_idx=0;
static  s8 bBorder=FALSE;
static  s8 bBKGND=FALSE;
static  s8 bColor=FALSE;
//static  ptu32_t ButtonL_Up(struct WindowMsg *pMsg)
//


#include <stdio.h>
//����������
static ptu32_t HmiCreate(struct WindowMsg *pMsg)
{
    HWND hwnd;
    RECT rc0;
    u32 i;
    hwnd =pMsg->hwnd;
    cfg_idx =0;

    GetClientRect(hwnd,&rc0);
    CreateButton("�ر�",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)+4,60,24,hwnd,ID_CLOSE,NULL,NULL);
    i=RectH(&rc0)-(3*30);
    CreateButton("���Ʊ߿�",WS_CHILD|BS_HOLD|WS_BORDER|WS_VISIBLE,4,i+0*32,128,24,hwnd,ID_BORDER,NULL,NULL);
    CreateButton("���Ʊ���",WS_CHILD|BS_HOLD|WS_BORDER|WS_VISIBLE,4,i+1*32,128,24,hwnd,ID_BKGND,NULL,NULL);
    CreateButton("�ı���ɫ",WS_CHILD|BS_HOLD|WS_BORDER|WS_VISIBLE,4,i+2*32,128,24,hwnd,ID_COLOR,NULL,NULL);
    GDD_CreateTimer(hwnd,1,3000,TMR_START);
    GDD_CreateTimer(hwnd,2,1000,TMR_START);
    return true;
}

//�ӿؼ����͵�֪ͨ
static ptu32_t HmiNotify(struct WindowMsg *pMsg)
{
        HWND hwnd;
        u16 event,id;

        hwnd =pMsg->hwnd;

        event =HI16(pMsg->Param1);
        id =LO16(pMsg->Param1);

        switch (id)
        {
            case ID_CLOSE://�ر�
                    if(event==BTN_UP)
                        {
                            PostMessage(hwnd,MSG_CLOSE,0,0);
                        }
                        InvalidateWindow(hwnd,FALSE);
                    break;
            case ID_BORDER ://���Ʊ߿�
                    if(event==CBN_SELECTED)
                        {
                            bBorder =TRUE;
                        }
                        if(event==CBN_UNSELECTED)
                        {
                            bBorder =FALSE;
                        }
                        InvalidateWindow(hwnd,FALSE);
                    break;
            case ID_BKGND://���Ʊ���
                    if(event==CBN_SELECTED)
                        {
                            bBKGND =TRUE;
                        }
                        if(event==CBN_UNSELECTED)
                        {
                            bBKGND =FALSE;
                        }
                        InvalidateWindow(hwnd,FALSE);
                    break;
            case ID_COLOR://�ı���ɫ
                if(event==CBN_SELECTED)
                    {
                        bColor =TRUE;
                    }
                    if(event==CBN_UNSELECTED)
                    {
                        bColor =FALSE;
                    }
                    InvalidateWindow(hwnd,FALSE);
                    break;
            default:break;
        }
    return true;
}
//��ͼ��Ϣ
static ptu32_t HmiPaint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc0,rc;
    u32 i;
    hwnd =pMsg->hwnd;

    hdc = BeginPaint(hwnd);
    GetClientRect(hwnd,&rc0);

    SetRect(&rc,2,10,RectW(&rc0)-2*2,90);
    SetFillColor(hdc,RGB(000,200,200));
    FillRect(hdc,&rc);

    SetTextColor(hdc,DrawText_Color_Tbl[color_idx].text_color);
    SetDrawColor(hdc,DrawText_Color_Tbl[color_idx].bd_color);
    SetFillColor(hdc,DrawText_Color_Tbl[color_idx].bk_color);

    i=DrawText_Cfg_Tbl[cfg_idx].flag;
    if(bBorder)
    {
        i |= DT_BORDER;
    }
    if(bBKGND)
    {
        i |= DT_BKGND;
    }
    DrawText(hdc,DrawText_Cfg_Tbl[cfg_idx].text,-1,&rc,i);
    EndPaint(hwnd,hdc);

    return true;
}
//���ڱ�������

static ptu32_t HmiErasebkgnd(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc0;

    hwnd =pMsg->hwnd;
    hdc =(HDC)pMsg->Param1;
    GetClientRect(hwnd,&rc0);
    SetFillColor(hdc,RGB(200,200,200));
    FillRect(hdc,&rc0);

    return true;
}

//��ʱ��������Ϣ
static  u32 HmiTimer(struct WindowMsg *pMsg)
{

    HWND hwnd;
    hwnd =pMsg->hwnd;


    switch(pMsg->Param1)
       {
           case    1:
                   cfg_idx++;
                   if(DrawText_Cfg_Tbl[cfg_idx].text==NULL)
                   {
                       cfg_idx=0;
                   }
                   InvalidateWindow(hwnd,true);
                   break;
                   /////
           case    2:
                   if(bColor!=FALSE)
                   {
                       color_idx++;
                       if(color_idx>=8)
                       {
                           color_idx=0;
                       }
                       InvalidateWindow(hwnd,true);
                   }
                   break;
           default:break;
       }
     return  0;
}


//��Ϣ��������
static struct MsgProcTable s_gHmiMsgTextTable[] =
{
    {MSG_CREATE,HmiCreate},         //�����ڴ�����Ϣ
    {MSG_TIMER,HmiTimer},           //��ʱ����Ϣ
    {MSG_NOTIFY,HmiNotify},         //�ӿؼ�������֪ͨ��Ϣ
    {MSG_PAINT,HmiPaint},           //������Ϣ
    {MSG_ERASEBKGND,HmiErasebkgnd}, ///���ڱ�������; Param1:��ͼ������;
};
static struct MsgTableLink  s_gHmiMsgLink;

void    GDD_Demo_DrawText(void)
{
    HWND g_ptMainHwnd;
    struct WindowMsg msg;
    RECT rc;
    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-5,-5);

   s_gHmiMsgLink.LinkNext = NULL;
   s_gHmiMsgLink.MsgNum = sizeof(s_gHmiMsgTextTable) / sizeof(struct MsgProcTable);
   s_gHmiMsgLink.myTable = (struct MsgProcTable *)&s_gHmiMsgTextTable;
   g_ptMainHwnd = CreateWindow("DrawText(�ַ�����)",WS_MAIN_WINDOW,
                               rc.left,rc.top,RectW(&rc),RectH(&rc),
                               NULL,0x0000, CN_WINBUF_PARENT,NULL,&s_gHmiMsgLink);
   SetFocusWindow(g_ptMainHwnd);

    //����������
    ShowWindow(g_ptMainHwnd,TRUE);  //��ʾ����

    while(GetMessage(&msg,g_ptMainHwnd))
    {
        DispatchMessage(&msg);
    }

    printf("win_exit.\r\n");

}
