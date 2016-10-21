//----------------------------------------------------
//Copyright (C), 2004-2011,  ������.
//��Ȩ���� (C), 2004-2011,   ������.
//����ģ��:ʱ��ģ��
//����:  ������
//�汾��V1.0.0
//�ļ�����: windows����ģ��Һ������
//����˵��:
//�޶���ʷ:
// 1. ����: 2012-10-09
//   ����:  ������
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------
#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "gkernel.h"
#include "cpu_peri.h"
#include "os.h"
#include "board-config.h"
#include <gui/gkernel/gk_display.h>
#include <LCM240128C/sr5333/lcm240128c_config.h>

//����ϵ˵��,�������Դ�ƫ�Ƶ�ַ���������ص������ӳ���ϵ,ע�ⵥɫ�ͻҶ���ʾ��
//ÿ�ֽڿ��ܰ�����ֹһ�����ص�,ͼ��û�м�������.
//��ͬ��ϵͳ�в�ͬ�Ķ�Ӧ��ϵ,���ֱ仯Ӱ�� ������ʾ�����ı�д����.
//Ӧ�ó����Լ�gui�������������������ӳ���ϵ.
//�������,���������������Ļֱ��ӳ��ķ�����ʾbmpͼ��,��Ҫ�˽�����ӳ���ϵ.
//----------------------------------------------------->x   ���Ͻ�
// |0x0000,0x0001,0x0002--------------------------xmax-1
// |xmax,xmax+1,xmax+2---------------------------2*max-1
// |----------------------------------------------------
// |----------------------------------------------------
// |----------------------------------------------------
// |(ymax-1)*xmax,(ymax-1)*xmax+1------------ymax*xmax-1    ���½�
//\/

struct DisplayRsc tg_lcd_display;
u16 *pg_frame_buffer;
u8 *CmdPort = (u8*)0x60000000;
u8 *DataPort = (u8*)0x60000001;
u8 ContrastLevel;
#define CN_TONTRAST_LEVEL_DEFAULT   0x68


static const Pin LCD_BLK[] = {
        {PIO_PA10, PIOA, ID_PIOA, PIO_OUTPUT_0,    PIO_DEFAULT}
};

static const Pin LCD_Pins[] = {
    PIN_EBI_DBUS,
    PIN_EBI_DNRD,
    PIN_EBI_DNWE,
    PIN_EBI_ABUS0,
    PIN_EBI_ENCS0,
    PIN_LCD_BACKLED,
};
static void Lcd_PortEBI_Init(void)
{
    PIO_Configure(LCD_Pins,PIO_LISTSIZE(LCD_Pins));
    PMC_EnablePeripheral(ID_SMC ) ;

    SMC->SMC_CS_NUMBER[0].SMC_SETUP = (0
                    | (SMC_SETUP_NWE_SETUP(40))
                    | (SMC_SETUP_NCS_WR_SETUP(20))
                    | (SMC_SETUP_NRD_SETUP(40))
                    | (SMC_SETUP_NCS_RD_SETUP(20))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_PULSE = (0
                    | (SMC_PULSE_NWE_PULSE(50))
                    | (SMC_PULSE_NCS_WR_PULSE(80))

                    | (SMC_PULSE_NRD_PULSE(50))
                    | (SMC_PULSE_NCS_RD_PULSE(80))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_CYCLE = (0
                    | (SMC_CYCLE_NWE_CYCLE(150))
                    | (SMC_CYCLE_NRD_CYCLE(150))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_MODE = (0
                    | (SMC_MODE_DBW_8_BIT)
                    | (SMC_MODE_EXNW_MODE_DISABLED)
                    | (SMC_MODE_BAT_BYTE_WRITE)
                    | (SMC_MODE_READ_MODE)
                    | (SMC_MODE_WRITE_MODE)
                      );
}

//----��ʼ��lcdӲ��------------------------------------------------------------
//����: ����
//����: ��
//����: ��
//��ע: ����������ֲ�ؼ�����.
//----------------------------------------------------------------------------
void InitLCM240128C(void)
{
    ContrastLevel = CN_TONTRAST_LEVEL_DEFAULT;
    *CmdPort = 0xE2;                            // �����¶Ȳ���ϵ��-0.05%/C
    Djy_EventDelay(10*mS);
    *CmdPort = 0x25;                            // �����¶Ȳ���ϵ��-0.05%/C
    *CmdPort = 0x2b;                            // �ڲ�DC-DC
    *CmdPort = 0xc4;                            // LCDӳ��MY=1��MX=0��LC0=0
    *CmdPort = 0xa3;                            // ������ɨ��Ƶ�� FR=263Hz
    *CmdPort = 0xd1;                            // ��ɫ���ݸ�ʽR-G-B
    *CmdPort = 0xd5;                            // ��������λΪ12λRRRR-GGGG-BBBB
    *CmdPort = 0xc8; *CmdPort = 0x18;           // ����M�ź�Ϊ֡��ת
    *CmdPort = 0xeb;                            // ����ƫѹ��1/12
    *CmdPort = 0xa7;                            // ������ʾ
    *CmdPort = 0xa4;                            // ������ʾ
    *CmdPort = 0x81; *CmdPort = ContrastLevel;  // ���öԱȶ�  ��Ӧ��ɫͼ��
    *CmdPort = 0xdf;                            // ����ɨ��ģʽ ����FRC
    *CmdPort = 0xad;                            // ����ʾ  �����Ҷ���ʾ
    *CmdPort = 0xf4; *CmdPort = 0x00;           // ���ò���������߽�
    *CmdPort = 0xf5; *CmdPort = 0x00;           // ���ò��������ϱ߽�
    *CmdPort = 0xf6; *CmdPort = 0x4f;           // ���ò��������ұ߽�
    *CmdPort = 0xf7; *CmdPort = 0x7f;           // ���ò��������±߽�
    *CmdPort = 0xf8;                            // ���ô��ڲ���ʹ��
    *CmdPort = 0x00; *CmdPort = 0x10;           // ������ʼ�е�ַ
    *CmdPort = 0x60; *CmdPort = 0x70;           // ������ʼ�е�ַ
    Djy_EventDelay(50*mS);
}

//----LCD�������------------------------------------------------------------
//����: ��
//����: OnOff��true=������false=Ϩ��
//����: ��
//-----------------------------------------------------------------------------
void __lcd_BackLight(bool_t OnOff)
{
    if(OnOff)
        PIO_Clear(LCD_BLK);
    else
        PIO_Set(LCD_BLK);
}

//----LCD��ʾʹ��------------------------------------------------------------
//����: ��
//����: OnOff��true=ʹ����ʾ��false=�ر���ʾ
//����: ��
//-----------------------------------------------------------------------------
void __lcd_envid_of(bool_t OnOff)
{
    if(OnOff)
        *CmdPort = 0xaf;                // ����ʾ  �����Ҷ���ʾ
    else
        *CmdPort = 0xae;                // ����ʾ  �����Ҷ���ʾ
}

//----����LCD��Դ--------------------------------------------------------------
//����: LCD ��Դ��������ʹ��
//����: OnOff��true=��������Դ��false=�رյ�Դ
//����: ��
//-----------------------------------------------------------------------------
void __lcd_power_enable(bool_t OnOff)
{
    if(OnOff)
        *CmdPort = 0x28;                // ����ʾ  �����Ҷ���ʾ
    else
        *CmdPort = 0x2b;                // ����ʾ  �����Ҷ���ʾ
}
//----������ʾ��---------------------------------------------------------------
//����: ������driver�ṩ�̱�д�ģ�����������ʾ�����ֲ����ĺ���������Ӧ�ó����
//      ���룬�������豸���̶��塣
//����: disp����ʾ��ָ��
//����: true=�ɹ���false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct DisplayRsc *disp)
{
    return true;
}


//----λͼ�л�����-------------------------------------------------------------
//����: ��λͼ�л�һ�����أ�ֻ����bitmap�����ظ�ʽΪcn_custom_pfʱ������Ҫ���ơ�
//      ����Կ�������֧���Զ����ʽ��������ֱ�ӷ��ء�
//����: bitmap��Ŀ��λͼ
//      limit�����ƾ��Σ�ֻ�����ڸþ����ڲ��Ĳ��֡�
//      x��y������
//      color����ͼ�õ���ɫ
//      r2_code����Ԫ��դ������
//����: ��
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_bm(struct RectBitmap *bitmap,
                     s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----λͼ�л���---------------------------------------------------------------
//����: ��λͼ�л�һ��ֱ�ߣ�������(x2,y2)�㣬ֻ������limit�޶��������ڵĲ��֡�
//����: bitmap��Ŀ��λͼ
//      limit�����ƾ��Σ�ֻ�����ڸþ����ڲ��Ĳ��֡�
//      x1,y1,x2,y2������յ�����
//      color����ͼ�õ���ɫ
//      r2_code����Ԫ��դ������
//����: true=�ɹ����ƣ�false=ʧ�ܣ���Ӳ�����ٻ�֧�ְ�r2_code���ߡ�
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----λͼ�л���(���˵�)-------------------------------------------------------
//����: ��λͼ�л�һ��ֱ�ߣ�ֻ������limit�޶��������ڵĲ���
//����: bitmap��Ŀ��λͼ
//      limit�����ƾ��Σ�ֻ�����ڸþ����ڲ��Ĳ��֡�
//      x1,y1,x2,y2������յ�����
//      color����ͼ�õ���ɫ
//      r2_code����Ԫ��դ������
//����: true=�ɹ����ƣ�false=ʧ�ܣ���Ӳ�����ٻ�֧�ְ�r2_code���ߡ�
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----λͼ��������-----------------------------------------------------------
//����: ��λͼ�еľ�����color��ɫ���
//����: bitmap��Ŀ��λͼ
//      dst_rect,�����ľ���
//      color�������ɫ
//����:  true=�ɹ����ƣ�false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_bm(struct RectBitmap *dst_bitmap,
                          struct Rectangle *Target,
                          struct Rectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode)
{
    return false;
}
//----bitmap��λ�鴫��---------------------------------------------------------
//����: ��һ��λͼ�е�һ�����������͵���һ����������λͼ�����ظ�ʽ������ͬ��
//      Դ��Ŀ��λͼ������ͬ
//����: dst_bitmap��Ŀ��λͼ
//      dst_rect��Ŀ�����
//      src_bitmap��Դλͼ
//      xsrc,ysrc��Դλͼ�б����͵��������Ͻ�����
//      pat��patternָ��
//      mask��rop4������Ҫ������
//      rop_code����Ԫ��դ������
//      transparentcolor���ؼ���ɫ
//����: true=�ɹ�,false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_blt_bm_to_bm( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode, u32 HyalineColor)
{
    return false;
}
//----��ȡλͼ�е�����---------------------------------------------------------
//����: ȡλͼ�е�һ�����أ�ֻ����bitmap�����ظ�ʽΪcn_custom_pfʱ������Ҫ��ȡ��
//      ����Կ�������֧���Զ����ʽ��������ֱ�ӷ���false��
//����: bitmap��Ŀ��λͼ
//      x��y������
//����:  true=�ɹ���ȡ��false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_get_pixel_bm(struct RectBitmap *bitmap,s32 x,s32 y)
{
    return false;
}
//----screen�л�����-----------------------------------------------------------
//����: ��screen�л�һ�����أ���frame buffer��driver��ֱ�ӷ��ؼ���
//����: x,y,��������
//      color����ɫ
//      r2_code����Ԫ��դ������
//����: true=�ɹ����ƣ�false=û�л��ƣ����������
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----screen�л���---------------------------------------------------------------
//����: ��screen�л�һ������ֱ�ߣ������˵㣬ֻ������limit�޶��������ڵĲ��֡���
//      ��֧�ֻ��ߵ�Ӳ�����٣�ֱ�ӷ���false��
//����: limit�����ƾ��Σ�ֻ�����ڸþ����ڲ��Ĳ��֡�
//      x1,y1,x2,y2������յ�����
//      color����ͼ�õ���ɫ
//      r2_code����Ԫ��դ������
//����: true=�ɹ����ƣ�false=ʧ�ܣ���Ӳ�����ٻ�֧�ְ�r2_code���ߡ�
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen(struct Rectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen�л���(���˵�)------------------------------------------------------
//����: ��screen�л�һ��ֱ�ߣ�ֻ������limit�޶��������ڵĲ���
//����: limit�����ƾ��Σ�ֻ�����ڸþ����ڲ��Ĳ��֡�
//      x1,y1,x2,y2������յ�����
//      color����ͼ�õ���ɫ
//      r2_code����Ԫ��դ������
//����: true=�ɹ����ƣ�false=ʧ�ܣ���Ӳ�����ٻ�֧�ְ�r2_code���ߡ�
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen��������-----------------------------------------------------------
//����: ��screen�еľ�����color��ɫ��䣬color�ĸ�ʽ��cn_sys_pf_e8r8g8b8
//����: dst_rect,�����ľ���
//      color�������ɫ
//����:  true=�ɹ����ƣ�false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct Rectangle *Target,
                              struct Rectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
    return false;
}

//----bitmap��screenλ�鴫��---------------------------------------------------
//����: ��һ��λͼ�е�һ�����������͵���һ����������λͼ�����ظ�ʽ������ͬ��
//      Դ��Ŀ��λͼ������ͬ
//����: dst_rect��Ŀ�����
//      src_bitmap��Դλͼ
//      xsrc,ysrc��Դλͼ�б����͵��������Ͻ�����
//����: true=�ɹ�,false=ʧ��
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct Rectangle *dst_rect,
                struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    s32 xstart,xend;
    s32 realxsrc;
    s32 loopx,loopy;
    u32 offset;
    u8 c;
    //����LCDģ���Ҫ��x��start��end��Ҫ����Ϊ6�ı���
    xstart = (dst_rect->left/6)*6;
    xend = ((dst_rect->right-1)/6)*6;
    realxsrc = xsrc - (dst_rect->left-xstart);

    *CmdPort = 0xf4; *CmdPort = xstart/3;           // ���ò���������߽�
    *CmdPort = 0xf5; *CmdPort = (u8)dst_rect->top;  // ���ò��������ϱ߽�
    *CmdPort = 0xf6; *CmdPort = xend/3+1;             // ���ò��������ұ߽�
    *CmdPort = 0xf7; *CmdPort = (u8)(dst_rect->bottom-1);  // ���ò��������±߽�
    *CmdPort = 0xf8;                                // ���ô��ڲ���ʹ��
    *CmdPort = xstart/3 & 0x0f;
        *CmdPort = 0x10|(xstart/3 >> 4);            // ������ʼ�е�ַ

    *CmdPort = 0x60 + ((u8)dst_rect->top & 0x0f);
        *CmdPort = 0x70 + ((u8)(dst_rect->top)>> 4);// ������ʼ�е�ַ

    if(realxsrc%2 == 0)
    {
        offset = ysrc * src_bitmap->linebytes + realxsrc/2;
        for(loopy = 0; loopy < dst_rect->bottom - dst_rect->top; loopy++)
        {
            for(loopx = 0; loopx < (xend-xstart+6)/2; loopx++ )
            {
                c = src_bitmap->bm_bits[offset+loopx];
                *DataPort = c;
            }
            offset += src_bitmap->linebytes;
        }
    }
    else
    {

    }
    return true;
}
//----��screen��ȡ����---------------------------------------------------------
//����: ��screen��ȡһ���أ���ת����cn_sys_pf_e8r8g8b8��cn_sys_pf_a8r8g8b8��ʽ
//����: x,y������
//����: ������ɫֵ
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    //x86_windows�£���������windows��ȡ������ȡ��
    return 0;
}

//----��ȡbitmap�о��ο�-------------------------------------------------------
//����: ��һ��λͼ�еľ��ζ�ȡ����һ��λͼ��
//����: rect������ȡ�ľ��Ρ�
//      dest��������ε�λͼ���䳤�������rect��ͬ��
//����: true=�ɹ���ȡ��false=ʧ�ܣ�ԭ��������ṩ�˲�֧�ֵ����ظ�ʽ��
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct Rectangle *rect,struct RectBitmap *dest)
{
    return false;
 }

//----��ʼ��lcd�豸------------------------------------------------------------
//����: ����
//����: ��
//����: ��ʾ����Դָ��
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_LCM240128C(ptu32_t para)
{
    static struct GkWinRsc frame_win;
    static struct RectBitmap FrameBitmap;

    PIO_Configure(LCD_BLK, PIO_LISTSIZE(LCD_BLK));
    __lcd_BackLight(1);
    Lcd_PortEBI_Init();
    InitLCM240128C( );

    pg_frame_buffer = malloc(CN_LCD_XSIZE*CN_LCD_YSIZE/2);
    memset(pg_frame_buffer,0,CN_LCD_XSIZE*CN_LCD_YSIZE/2);
    FrameBitmap.bm_bits = (u8*)pg_frame_buffer;
    FrameBitmap.width = CN_LCD_XSIZE;
    FrameBitmap.height = CN_LCD_YSIZE;
    FrameBitmap.PixelFormat = CN_SYS_PF_GRAY4;
    FrameBitmap.linebytes = CN_LCD_XSIZE/2;
    FrameBitmap.ExColor = 0xffffffff;
    frame_win.wm_bitmap = &FrameBitmap;
    tg_lcd_display.frame_buffer = &frame_win;

    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = CN_LCD_XSIZE;
    tg_lcd_display.height = CN_LCD_YSIZE;
    tg_lcd_display.pixel_format = CN_SYS_PF_GRAY4;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.framebuf_direct = false;
    //�����ʼ��frame_buffer��desktop��z_topmost������Ա

    tg_lcd_display.draw.SetPixelToBitmap = __lcd_set_pixel_bm;
    tg_lcd_display.draw.FillRectToBitmap = __lcd_fill_rect_bm;
    tg_lcd_display.draw.LineToBitmap = __lcd_line_bm;
    tg_lcd_display.draw.LineToBitmapIe = __lcd_line_bm_ie;
    tg_lcd_display.draw.BltBitmapToBitmap = __lcd_blt_bm_to_bm;
//    tg_lcd_display.draw.check_raster = __lcd_check_raster;
    tg_lcd_display.draw.SetPixelToScreen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.LineToScreen = __lcd_line_screen;
    tg_lcd_display.draw.LineToScreenIe = __lcd_line_screen_ie;
    tg_lcd_display.draw.FillRectToScreen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.CopyBitmapToScreen = __lcd_bm_to_screen;
    tg_lcd_display.draw.GetPixelFromScreen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.GetRectFromScreen = __lcd_get_rect_screen;

    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,(char*)para))
        return (ptu32_t)&tg_lcd_display;
    else
        return 0;
}

