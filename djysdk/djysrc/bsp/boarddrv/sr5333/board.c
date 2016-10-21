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
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���     ��board.c
// ģ������: �����ز��ֳ�ʼ�������õ�
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 5/11.2015
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "os.h"
#include "cpu_peri.h"


//�������Ŷ���  DEBUG
static const Pin uart1_pin[]={
        {PIO_PA5C_URXD1,PIOA,ID_PIOA,PIO_PERIPH_C,PIO_DEFAULT},
        {PIO_PA6C_UTXD1,PIOA,ID_PIOA,PIO_PERIPH_C,PIO_DEFAULT}};

//232
static const Pin usart0_Pin[]={
		{PIO_PB0C_RXD0,PIOB,ID_PIOB,PIO_PERIPH_C,PIO_DEFAULT},
		{PIO_PB1C_TXD0,PIOB,ID_PIOB,PIO_PERIPH_C,PIO_DEFAULT}
};

//���ʹ��485����   485_1
static const Pin usart1_pin[] = {
    {PIO_PA21A_RXD1, 	PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PB4D_TXD1,		PIOB, ID_PIOB, PIO_PERIPH_D, PIO_PULLUP},
	{PIO_PA24, 			PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}	//485 RX TX
};

//485_2
static const Pin usart2_pin[] = {
    {PIO_PD15B_RXD2, 	PIOD, ID_PIOD, PIO_PERIPH_B, PIO_PULLUP},
    {PIO_PD16B_TXD2,	PIOD, ID_PIOD, PIO_PERIPH_B, PIO_PULLUP},
	{PIO_PD18, 			PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT}	//485 RX TX
};


//AT45�õ�QSPI��SPI MODE  (spi flash)
static const Pin Qspi_pin[] = {
		{PIO_PA11A_QCS,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA13A_QIO0,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA12A_QIO1,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA14A_QSCK,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
};


//at45db321д������������
static const Pin AT45_WP_pin [] = {
		{PIO_PD14, PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT},
};


//����IIC��GPIO��������   ������AT24CL128B(EEPROM iic�ӿ�)ͨ��
static const Pin Twi0_Pins[]={
        {PIO_PA3A_TWD0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT},
        {PIO_PA4A_TWCK0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT}
};

//AT24CL128B д������������
static const Pin AT24_WP_pin [] = {
		{PIO_PA2,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_DEFAULT},
};

//Һ����LCM240128Cͨ��
static const Pin Spi1_pin[] = {
        {PIO_PC26C_SPI1_MISO,PIOC, ID_PIOC, PIO_PERIPH_C, PIO_DEFAULT},
        {PIO_PC27C_SPI1_MOSI,PIOC, ID_PIOC, PIO_PERIPH_C, PIO_DEFAULT},
        {PIO_PC25C_SPI1_NPCS0,PIOC, ID_PIOC, PIO_PERIPH_C, PIO_DEFAULT},
        {PIO_PC24C_SPI1_SPCK,PIOC, ID_PIOC, PIO_PERIPH_C, PIO_DEFAULT},
};

//��������GMAC
static const Pin Gmac_pin[] = {
		{(PIO_PD0A_GTXCK | PIO_PD1A_GTXEN | PIO_PD2A_GTX0 | PIO_PD3A_GTX1
		| PIO_PD4A_GRXDV | PIO_PD5A_GRX0 | PIO_PD6A_GRX1 | PIO_PD7A_GRXER
		| PIO_PD8A_GMDC | PIO_PD9A_GMDIO ),PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT},
};


#define PWM_OUT1  {PIO_PD27A_PWMC0_PWML3,PIOD,ID_PIOD,PIO_PERIPH_A,PIO_DEFAULT}
#define PWM_OUT2 {PIO_PA15C_PWMC0_PWML3, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT}

const Pin pwm_pin[] = {
		PWM_OUT1,
		PWM_OUT2,
};


//����ADS8664(spi0 �ӿڵ�ADC����оƬ ADS8664)
static const Pin Spi0_pin[] = {
		{PIO_PD20B_SPI0_MISO,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
		{PIO_PD21B_SPI0_MOSI,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
		{PIO_PD22B_SPI0_SPCK,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
};

const Pin ADRST[] = {
    {PIO_PB5, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT},//ADRST
};

const Pin ADCS[] = {
        {PIO_PA31, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT},//ADCS
};


// ============================================================================
// ���ܣ����ô��ڱ��ΪSerialNo�Ĵ���Ϊ��˫�����͹���
// ������SerialNo,���ں�
// ���أ���
// ============================================================================
void Board_UartHalfDuplexSend(u8 SerialNo)
{
	switch(SerialNo)
	{
	case CN_USART1:
		PIO_Set(&usart1_pin[2]);
		break;
	case CN_USART2:
		PIO_Set(&usart2_pin[2]);
		break;
	default:
		break;
	}
}

// ============================================================================
// ���ܣ����ô��ڱ��ΪSerialNo�Ĵ���Ϊ��˫�����չ���
// ������SerialNo,���ں�
// ���أ���
// ============================================================================
void Board_UartHalfDuplexRecv(u8 SerialNo)
{
	//����Ϊ485����
	switch(SerialNo)
	{
	case CN_USART1:
		PIO_Clear(&usart1_pin[2]);
		break;
	case CN_USART2:
		PIO_Clear(&usart2_pin[2]);
		break;
	default:
		break;
	}
}

// =============================================================================
// ���ܣ���ʼ���������GPIO���ܣ�������ͨGPIO�ڣ���������IO(�紮�ڣ����ڵ�)
// ��������
// ���أ�true
// ˵�����ú����������������蹦������ǰ���ã�������module-trim��Sys_ModuleInit��һ��
//      ��������
// =============================================================================
bool_t Board_GpioInit(void)
{
    PIO_Configure(uart1_pin,PIO_LISTSIZE(uart1_pin));

    PIO_Configure(usart0_Pin,PIO_LISTSIZE(usart0_Pin));    //232T/232R

    MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

    PIO_Configure(usart1_pin,PIO_LISTSIZE(usart1_pin));   //485
    PIO_Configure(usart2_pin, PIO_LISTSIZE(usart2_pin));

    PIO_Configure(Qspi_pin, PIO_LISTSIZE(Qspi_pin));		//AT45
   	PIO_Configure(AT45_WP_pin, PIO_LISTSIZE(AT45_WP_pin));
   	PIO_Set(AT45_WP_pin);

   	PIO_Configure(Twi0_Pins,PIO_LISTSIZE(Twi0_Pins));  //AT24CL128B  �洢��ֵ
	PIO_Configure(AT24_WP_pin,PIO_LISTSIZE(AT24_WP_pin));
	PIO_Clear(AT24_WP_pin);

	PIO_Configure(Gmac_pin, PIO_LISTSIZE(Gmac_pin));		//GMAC

	PIO_Configure(Spi1_pin,PIO_LISTSIZE(Spi1_pin));

	PIO_Configure(Spi0_pin,PIO_LISTSIZE(Spi0_pin));

	PIO_Configure(ADRST,PIO_LISTSIZE(ADRST));

    PIO_Configure(ADCS,1);


	PIO_Configure(pwm_pin,PIO_LISTSIZE(pwm_pin));          //PWM





    return true;
}

