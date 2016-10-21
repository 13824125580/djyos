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
// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��crtouch.c
// ģ������: ����оƬCRTOUCH����
// ģ��汾: V1.10
// ������Ա: hm
// ����ʱ��: 15/10.2014
// =============================================================================

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"
#include "stdlib.h"

#include "int_hard.h"
#include "int.h"
#include "cpu_peri.h"
#include "Touch.h"
#include "spibus.h"
#include "gkernel.h"
#include "systime.h"
#include <LCM240128C/sr5333/lcm240128c_config.h>
#include <driver/flash/flash.h>


struct AdjustValue
{
	s16 XAdjustLeft;
	s16 YAdjustTop;
	s16 XAdjustRight;
	s16 YAdjustBottom;
};


#define CN_ADJUST_ADDR   0x5ffff8
static struct AdjustValue* s_ptAdjustVaule=(struct AdjustValue*)(CN_ADJUST_ADDR);

static const Pin PenStatus[] = {
        {PIO_PC28, PIOC, ID_PIOC, PIO_INPUT, PIO_PULLUP}
};

//����SPIBUS�ܹ��µ�SPI�豸�ṹ
static struct SPI_Device s_tgTouch_Dev;

extern  s32 EEFC_PageProgram(u32 Page, u8 *Data, u32 Flags);
extern  s32 EEFC_PageRead(u32 Page, u8 *Data, u32 Flags);
extern  s32 EEFC_SectorEarse(u32 SectorNo);


// =============================================================================
// ���ܣ�����У׼�õ���У׼ֵд��Ƭ��Flash�����8���ֽ��С�
// ������pAdjustValue,У׼ֵ�ṹ��ָ��
// ���أ�true,д�ɹ�;false,дʧ��
// =============================================================================
static bool_t Touch_WriteAdjustValue(struct AdjustValue* pAdjustValue)
{
    u16 left,right,top,bottom;
    u8 buf[512];
    s32 ret;
    if(pAdjustValue==NULL)
    	return false;
    left=pAdjustValue->XAdjustLeft;
    right=pAdjustValue->XAdjustRight;
    top=pAdjustValue->YAdjustTop;
    bottom=pAdjustValue->YAdjustBottom;
    //�ȶ����һҳ
    ret=EEFC_PageRead(4095,buf,0);
    if(ret==-1)
    	return false;
    //Ȼ���������Sector������ֻ����SectorΪ��λ
    ret=EEFC_SectorEarse(15);
    if(ret==-1)
        return false;
    buf[504]=left;
    buf[505]=left>>8;
    buf[506]=top;
    buf[507]=top>>8;
    buf[508]=right;
    buf[509]=right>>8;
    buf[510]=bottom;
    buf[511]=bottom>>8;
    ret=EEFC_PageProgram(4095,buf,0);
    if(ret==-1)
        return false;
    return true;
}


// =============================================================================
// ���ܣ���ȡ������AD����ֵ���ֱ�ΪXֵ��Yֵ��δ��У��������
// ������X,Y������������洢��ȡ�������ݵ�ַ
// ���أ�true,��ȡ�ɹ�;false,��ȡʧ��
// =============================================================================
static bool_t Touch_ReadXY(s16 *X, s16 *Y)
{
    struct SPI_DataFrame data;
    u8 xybuf[] = {  0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00};
    u16 maxx=0,minx=65535,maxy=0,miny=65535;
    s16 x0[4],y0[4];
    u32 loop;
    s32 result;
    s32 tempx=0,tempy=0;

#if 1
    data.SendBuf = xybuf;
    data.SendLen = 24;
    data.RecvBuf = xybuf;
    data.RecvLen = 23;
    data.RecvOff = 1;
    //��4���������ֵ
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);
#else

    data.SendBuf = xybuf;
    data.SendLen = 1;
    data.RecvBuf = xybuf;
    data.RecvLen = 2;
    data.RecvOff = 1;
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);

    data.SendBuf = xybuf+2;
    data.SendLen = 1;
    data.RecvBuf = xybuf+2;
    data.RecvLen = 2;
    data.RecvOff = 1;
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);
#endif
    //������һ�������㣨��׼����ȥ������3�������㣬����ȡ�����Сֵ
    for(loop = 1;loop<4;loop++)
    {
        x0[loop] = (xybuf[loop *6] << 8) +xybuf[loop*6+1];
        x0[loop] >>=3;
        y0[loop] = (xybuf[loop*6+3] << 8) +xybuf[loop*6+4];
        y0[loop] >>=3;
        if(x0[loop] > maxx)
            maxx = x0[loop];
        if(x0[loop] < minx)
            minx = x0[loop];
        if(y0[loop] > maxy)
            maxy = y0[loop];
        if(y0[loop] < miny)
            miny = y0[loop];
    }

    //�����Сֵ֮�����20�Ļ���˵������ֵ���ȶ��������ڶ�����
    if( (maxx - minx > 20) || (maxy - miny > 20) )
        return false;
    else
    {
        for(loop = 1;loop<4;loop++)
        {
            tempx +=x0[loop];
            tempy +=y0[loop];
        }
        tempx -= minx;      //�������ֵ����Сֵ
        tempx -= maxx;
        tempy -= miny;
        tempy -= maxy;
        *X = tempx / 1;
        *Y = tempy / 1;
        return true;
    }
}

// =============================================================================
// ���ܣ���ȡ��ʱ�Ƿ��������д���
// ��������
// ���أ�1,����;0,δ����
// =============================================================================
static bool_t Touch_Touched(void)
{
    u8 TouchS;
    TouchS = PIO_Get(PenStatus);
    if(TouchS ==0)
        return true;
    else
        return false;
}

static s32 xp,yp,z;
//----��ȡ����������-----------------------------------------------------------
//����: ��ȡstmpe811�ɼ����Ĵ��������꣬����ж�㣬��ƽ��֮
//����: touch_data���ɼ���������
//����: 1=�����ʰ��£�0=����������
//-----------------------------------------------------------------------------
static ufast_t ReadTouch(struct SingleTouchMsg *touch_data)
{
    atom_low_t atom;
    touch_data->display = NULL;
    atom = Int_LowAtomStart( );
    touch_data->x =xp;
    touch_data->y =yp;
    touch_data->z = z;
    Int_LowAtomEnd(atom);
    return z;

}

//----��ȡ����������-----------------------------------------------------------
//����: ��ȡstmpe811�ɼ����Ĵ��������꣬����ж�㣬��ƽ��֮
//����: touch_data���ɼ���������
//����: 1=�����ʰ��£�0=����������
//-----------------------------------------------------------------------------
static ptu32_t ScanTouch(void)
{
    bool_t trans;
    atom_low_t atom;
    s16 x,y;
    s32 count =  0;
    s16 s_s16gXAdjustLeft,s_s16gXAdjustRight;
    s16 s_s16gYAdjustTop,s_s16gYAdjustBottom;
    while(1)
    {
        trans = Touch_Touched( );
        if(trans)
        {
            if(count++ > 2)
            {
                trans = Touch_ReadXY(&x,&y);
                trans &= Touch_Touched( );
                if(trans)
                {
                	s_s16gXAdjustLeft=s_ptAdjustVaule->XAdjustLeft;
                	s_s16gXAdjustRight=s_ptAdjustVaule->XAdjustRight;
                	s_s16gYAdjustTop=s_ptAdjustVaule->YAdjustTop;
                	s_s16gYAdjustBottom=s_ptAdjustVaule->YAdjustBottom;
                    x = 200 * (s32)(x - s_s16gXAdjustLeft)/(s_s16gXAdjustRight - s_s16gXAdjustLeft)+20;
                    y = 88 * (s32)(y - s_s16gYAdjustTop)/(s_s16gYAdjustBottom - s_s16gYAdjustTop)+20;

                    if( ( ( (xp -x)*(xp-x) + (yp-y)*(yp-y) ) >= 36) && (count >5 ) )
                    {
                    	//��������������� �β���֮���ƶ�����ﵽ6�����أ�˵������ʹ����е����,������
                    }
                    else
                    {
						atom = Int_LowAtomStart( );
						xp = x;
						yp = y;
						z = 1;
						Int_LowAtomEnd(atom);
                    }
                    count = 5;
                }
                else
                    count = 0;
            }
        }
        else
        {
            if(count-- < -2)
            {
                count = -3;
                z = 0;
            }
        }
        Djy_EventDelay(10*mS);
    }
    return 0;
}

static  void draw_cursor(struct GkWinRsc *desktop,int x,int y)
{
    GK_ApiLineto(desktop,x,y-9,x,y+9,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //��
    GK_ApiLineto(desktop,x-9,y,x+9,y,CN_COLOR_WHITE,CN_R2_COPYPEN,0);  //��
    GK_ApiSyncShow(1000*mS);
}

static  void clr_cursor(struct GkWinRsc *desktop,s32 x,s32 y)
{
    GK_ApiLineto(desktop,x,y-9,x,y+9,CN_COLOR_BLACK,CN_R2_COPYPEN,0); //��
    GK_ApiLineto(desktop,x-9,y,x+9,y,CN_COLOR_BLACK,CN_R2_COPYPEN,0);  //��
    GK_ApiSyncShow(1000*mS);

}



void touch_ratio_adjust(struct GkWinRsc *desktop)
{
    u32 loop;
    s32 tempx,tempy;
    u16 maxx=0,minx=65535,maxy=0,miny=65535;
    s16 x[7],y[7];
    bool_t b_AdjustFlag=false;
    struct AdjustValue* pAdjustValue;
    s64 temp=0;
    s64 adjustvalue=0;
    bool_t result=false;
    s16 s_s16gXAdjustLeft,s_s16gXAdjustRight;
    s16 s_s16gYAdjustTop,s_s16gYAdjustBottom;

    pAdjustValue=(struct AdjustValue*)malloc(sizeof(struct AdjustValue));
    if(pAdjustValue==NULL)
    	return;
	tempx = 0;
	tempy = 0;
	GK_ApiFillWin(desktop,CN_COLOR_BLACK,0);

	temp=s_ptAdjustVaule->XAdjustLeft;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->XAdjustRight;
	temp=temp<<16;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->YAdjustBottom;
	temp=temp<<32;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->YAdjustTop;
	temp=temp<<48;
	adjustvalue|=temp;

	//���ж�Flash���Ƿ��Ѵ洢��Ч��У׼ֵ   ����ҪУ��
	if(adjustvalue==0xffffffffffffffff)
	{
		GK_ApiDrawText(desktop,NULL,NULL,40,20,
						"���������� ",11,CN_COLOR_WHITE,CN_R2_COPYPEN,0);
		GK_ApiDrawText(desktop,NULL,NULL,40,40,
						"��׼ȷ���ʮ�ֽ����",20,CN_COLOR_WHITE,CN_R2_COPYPEN,0);

		draw_cursor(desktop, 20, 20);
		//    for(loop =10; loop < 128;)
		//    {
		//      GK_ApiLineto(desktop,0,loop,240,loop,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //��
		//      loop +=10;
		//    }
		//    for(loop =10; loop < 240;)
		//    {
		//        GK_ApiLineto(desktop,loop,0,loop,128,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //��
		//      loop+=10;
		//    }
		//    GK_ApiSyncShow(1000*mS);

		while(Touch_Touched() == false);
		Djy_EventDelay(100*mS);

		for(loop = 0;loop < 50; )
		{
			if(Touch_Touched())
			{
				loop++;
				Djy_DelayUs(30000);
			}
		}
		for(loop = 0;loop < 7; )
		{
			if(Touch_Touched())
			{
				if(Touch_ReadXY(&x[loop], &y[loop]))
				{
					if(Touch_Touched())
					{
						tempx +=x[loop];
						tempy +=y[loop];
						if(x[loop] > maxx)
							maxx = x[loop];
						if(x[loop] < minx)
							minx = x[loop];
						if(y[loop] > maxy)
							maxy = y[loop];
						if(y[loop] < miny)
							miny = y[loop];
						loop++;
					}
				}
				Djy_DelayUs(30000);
			}
		}
		tempx -= maxx + minx;
		tempy -= maxy + miny;
		s_s16gXAdjustLeft = tempx / 5;
		s_s16gYAdjustTop = tempy / 5;


		//    printk("x=%d,y=%d\n\r",s_s16gXAdjustLeft,s_s16gYAdjustTop);
		//    s_s16gXAdjustLeft = 0;
		//    s_s16gYAdjustTop = 0;
		//    maxx=0,minx=65535,maxy=0,miny=65535,tempx=0,tempy=0;
		//    Djy_DelayUs(300*mS);
		//    }
		clr_cursor(desktop, 20, 20);
		draw_cursor(desktop, 220, 108);
		while(Touch_Touched() == true);
		Djy_EventDelay(300*mS);
		while(Touch_Touched() == false);
		Djy_EventDelay(100*mS);
		maxx=0;
		minx=65535;
		maxy=0;
		miny=65535;
		tempx = 0;
		tempy = 0;
		for(loop = 0;loop < 50; )
		{
			if(Touch_Touched())
			{
				loop++;
				Djy_DelayUs(30000);
			}
		}
		for(loop = 0;loop < 7; )
		{
			if(Touch_Touched())
			{
				if(Touch_ReadXY(&x[loop], &y[loop]))
				{
					if(Touch_Touched())
					{
						tempx +=x[loop];
						tempy +=y[loop];
						if(x[loop] > maxx)
							maxx = x[loop];
						if(x[loop] < minx)
							minx = x[loop];
						if(y[loop] > maxy)
							maxy = y[loop];
						if(y[loop] < miny)
							miny = y[loop];
						loop++;
					}
				}
				Djy_DelayUs(30000);
			}
		}
		tempx -= maxx +minx;
		tempy -= maxy +miny;
		s_s16gXAdjustRight = tempx / 5;
		s_s16gYAdjustBottom = tempy / 5;

		pAdjustValue->XAdjustLeft=s_s16gXAdjustLeft;
		pAdjustValue->XAdjustRight=s_s16gXAdjustRight;
		pAdjustValue->YAdjustBottom=s_s16gYAdjustBottom;
		pAdjustValue->YAdjustTop=s_s16gYAdjustTop;
		result=Touch_WriteAdjustValue(pAdjustValue);
		if(result)
		{
			GK_ApiDrawText(desktop,NULL,NULL,50,600,"У׼�ɹ�",
										8,CN_COLOR_WHITE,CN_R2_COPYPEN,0);
			Djy_EventDelay(200*mS);
		}

		clr_cursor(desktop, 220, 108);

	}

}
//----��ʼ��������ģ��---------------------------------------------------------
//����:
//����: display_dev_name:����������Ӧ����ʾ����(��Դ��)
//      touch_dev_name:�������豸��.
//����: ��
//-----------------------------------------------------------------------------
bool_t ModuleInstall_LCM240128Touch(struct GkWinRsc *desktop,const char *touch_dev_name)
{
    static struct SingleTouchPrivate touch_dev;
    u16 evtt_id,event_id;
    s32 Ret;

    PIO_Configure(PenStatus,PIO_LISTSIZE(PenStatus));

    s_tgTouch_Dev.AutoCs = false;
    s_tgTouch_Dev.CharLen = 8;
    s_tgTouch_Dev.Cs = 0;
    s_tgTouch_Dev.Freq = 1000000;
    s_tgTouch_Dev.Mode = SPI_MODE_1;
    s_tgTouch_Dev.ShiftDir = SPI_SHIFT_MSB;

    if(NULL != SPI_DevAdd_s(&s_tgTouch_Dev,SPI_BUS_NAME,touch_dev_name))
    {
        SPI_BusCtrl(&s_tgTouch_Dev,CN_SPI_SET_POLL,0,0);
    }
    else
        printf("��װ������SPI��������\n\r");

extern s32 ModuleInstall_EmbededFlash(const char *ChipName, u32 Flags, u16 Start);
    Ret = ModuleInstall_EmbededFlash("embedded flash", FLASH_BUFFERED, 0);
    if(Ret)
    {
	   printf("install \"embedded flash\" error\r\n");
	   return (-1);
    }

    touch_ratio_adjust(desktop);

    touch_dev.read_touch = ReadTouch;
    touch_dev.touch_loc.display = NULL;
    Touch_InstallDevice(touch_dev_name,&touch_dev);

    evtt_id = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                ScanTouch,NULL,1024,
                                "touch scaning");
    //�¼���������������Ϊ0?�����shell����?�������ɼ�shell�����в���
    Djy_EventPop(evtt_id,NULL,0,0,0,0);

    return true;
}


