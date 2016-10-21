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
#ifndef __LCM240128C_IODEFINE_H__
#define __LCM240128C_IODEFINE_H__

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//EBI Data Bus pins
#define PIN_EBI_DBUS            {0xFF,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI pins for address bus
//#define PIN_EBI_ABUS          {0x1f<<18,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//ע���ַ����
#define PIN_EBI_ABUS0           {1<<18,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS1           {1<<19,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS2           {1<<20,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS3           {1<<21,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS4           {1<<22,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//

//EBI NRD pin(PC11)
#define PIN_EBI_DNRD            {1<<11, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI NWE pin(PC8)
#define PIN_EBI_DNWE            {1<<8,  PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI NCS0 pin(PC14)
#define PIN_EBI_ENCS0           {1<<14, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//XCS1 nCS0 pin

//PIN_LCD_BACKLIGHT
#define PIN_LCD_BACKLED         {PIO_PD23, PIOD,ID_PIOD,PIO_OUTPUT_0,PIO_PULLUP}

#define SPI_BUS_NAME           "SPI1"


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __LCM240128C_IODEFINE_H__ */

