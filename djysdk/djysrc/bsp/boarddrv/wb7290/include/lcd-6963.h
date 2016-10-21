// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��LCD.c
// ģ������: LCD��������ʹ����SMC����
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 07/03.2016
// =============================================================================

#ifndef __LCD_H__
#define __LCD_H__


void ModuleInstall_LCD(void);
void LCD_Clr(void);

void LCD_WriteByte(uint8_t dat,uint8_t lcd_write_mode);
void LCD_SetBackLight(uint8_t byCtrl);
void Lcd_wrDISPram(uint32_t dwleft,uint32_t dwtop,uint32_t dwsize,uint8_t * pbysrcadd);

#endif
