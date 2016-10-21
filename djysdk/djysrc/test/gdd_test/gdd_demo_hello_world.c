
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <gkernel.h>
#include <gdd.h>
#include <widget.h>

static HWND hwnd;
static RECT rc0;
#define ID_CLOSE    0x1000
static ptu32_t HmiCreate(struct WindowMsg *pMsg)
{

    hwnd=pMsg->hwnd;

    GetClientRect(hwnd,&rc0);
   //����һ����ť��
    CreateButton("�ر�",     //��ť��
    WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,    //��ť���
    RectW(&rc0)-64,RectH(&rc0)-28,60,24,    //��ťλ�úʹ�С
    hwnd,ID_CLOSE,NULL,NULL);    //��ť�����ĸ����ڣ�ID,��������
    return true;
}

static ptu32_t HmiNotify(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u16 event,id;
    hwnd =pMsg->hwnd;

    event =HI16(pMsg->Param1);
    id =LO16(pMsg->Param1);

    if(event==BTN_UP && id==ID_CLOSE)
    {
        PostMessage(hwnd,MSG_CLOSE,0,0);
    }
    return true;
}
static ptu32_t HmiPaint(struct WindowMsg *pMsg)
{
    RECT rc0;
    HWND hwnd;
    hwnd=pMsg->hwnd;
    HDC hdc;
    hdc =BeginPaint(hwnd);  //��ʼ��ͼ����û�ͼ������(hdc)��
    GetClientRect(hwnd,&rc0);           //��ô��ڿͻ�������
    SetFillColor(hdc,RGB(255,255,255)); //�������ɫ��
    FillRect(hdc,&rc0);                 //�����Ρ�
    SetTextColor(hdc,RGB(255,0,0));     //����������ɫ��
    TextOut(hdc,10,20,"Hello World!",-1);  //�ڿͻ���10,20��λ��������֡�
    EndPaint(hwnd,hdc); //�������Ʋ��ͷŻ�ͼ�����ġ�
    return true;

}
//���ڹر���Ϣ������
static ptu32_t HmiClose(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;

    printf("win2_close.\r\n");
    PostQuitMessage(hwnd,0);
    return true;
}
//��Ϣ��������
static struct MsgProcTable s_gHmiWordTable[] =
{
    {MSG_CREATE,HmiCreate},         //��ʱ����Ϣ
    {MSG_NOTIFY,HmiNotify},         //�ӿؼ�������֪ͨ��Ϣ
    {MSG_PAINT,HmiPaint},           //������Ϣ
    {MSG_CLOSE,HmiClose}      //���ڹر���Ϣ
};

static struct MsgTableLink  s_gHmiMsgLink;

void Hello_Word(void)
{
    HWND hwnd;
    struct WindowMsg msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);
    InflateRect(&rc,-7,-7);
    //����������
    s_gHmiMsgLink.LinkNext = NULL;
    s_gHmiMsgLink.MsgNum = sizeof(s_gHmiWordTable) / sizeof(struct MsgProcTable);
    s_gHmiMsgLink.myTable = (struct MsgProcTable *)&s_gHmiWordTable;
    hwnd = CreateWindow("Hello World",      //���ڱ�������
                            WS_MAIN_WINDOW,     //���ڷ�������ڡ�
							rc.left,rc.top,RectW(&rc),RectH(&rc),      //���ڵ�λ�ü���С��
                            NULL,0x0000, CN_WINBUF_PARENT,NULL,&s_gHmiMsgLink); //���ڵĸ����ڣ�ID,�������ݡ�);
    SetFocusWindow(hwnd);
    ShowWindow(hwnd,TRUE);  //��ʾ����

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win2_exit hello world.\r\n");
}







