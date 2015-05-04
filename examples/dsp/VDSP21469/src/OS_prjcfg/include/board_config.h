//----------------------------------------------------
//Copyright (C), 2013-2020,  �����߲���ϵͳ�����Ŷ�
//��Ȩ���� (C), 2013-2020,   �����߲���ϵͳ�����Ŷ�
//����ģ��:s29glxxx����
//���ߣ�hm
//�汾��V1.0.1
//�ļ�����:���������ص�����
//����˵��:
//�޶���ʷ:
//1. ����: 2014-04-25
//   ����: hm
//   �°汾��: V1.0.0
//   �޸�˵��: ԭʼ�汾
//------------------------------------------------------

#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H
#include "config-prj.h"
/*____��Ӳ����ص�����____*/
#define Mhz 1000000
#define CN_CFG_MCLK (400*Mhz)  //��Ƶ
//#define CN_CFG_FCLK CN_CFG_MCLK  //cm3������������ʱ��
//#define CN_CFG_AHBCLK CN_CFG_MCLK  //����ʱ��
//#define CN_CFG_PCLK2 CN_CFG_MCLK  //��������ʱ��
#define CN_CFG_PCLK1 (CN_CFG_MCLK/2)  //��������ʱ��
#define CN_CFG_EXTCLK 25000000  //�ⲿʱ��=8M
//#define cfg_core_ccb_clk (400*Mhz)
//#define cfg_core_implete_clk (cfg_core_ccb_clk/8)  //tbclk
//#define cfg_rtc_clk 800*Mhz

#if(cn_cfg_board == CN_BOARD_P1020RDB)
#define CN_CFG_EASYNOR_START_SECTOR       0 //��������ļ�ϵͳ��ʼ��
#define CN_CFG_EASYNOR_LENGTH_SECTOR      50  //�����ļ�ϵͳռ��sector����
#define cn_norflash_bus_width		   16
#elif(cn_cfg_board == CN_BOARD_WB8298)

/*�����ļ�ϵͳ��ʼ�ͽ������*/
#define CN_CFG_EASYNOR_START_SECTOR       0   //��������ļ�ϵͳ��ʼ��
#define CN_CFG_EASYNOR_LENGTH_SECTOR      50  //�����ļ�ϵͳռ��sector����

#define cn_norflash_bus_width		   16 /*����flash���߿��*/
#define cn_norflash_buf_size           512//����norflash��������С

//RTC_DS1390
#define CFG_RTC_SPI_BUS			g_ptSpi_Reg //RTCʹ�õ�SPI
#define CFG_RTC_SPI_CS			0			//CS0
#define CFG_RTC_SPI_SPEED		600000      //RTC���ݵ�SPI�ٶ� 600kHz

#elif(cn_cfg_board == CN_BOARD_WB9260)
/*____�弶��������____*/
#define CN_CFG_EASYNOR_START_SECTOR       40 //��������ļ�ϵͳ��ʼ��
#define CN_CFG_EASYNOR_LENGTH_SECTOR      192  //�����ļ�ϵͳռ��sector����
#define cn_norflash_bus_width		   8
#define cn_norflash_buf_size            64//����norflash��������С
//RTC_DS1390
#define CFG_RTC_SPI_BUS			g_ptSpi0Reg //RTCʹ�õ�SPIx
#define CFG_RTC_SPI_CS			2			//SS2
#define CFG_RTC_SPI_SPEED		600000      //RTC���ݵ�SPI�ٶ� 600kHz
//SPI_FLASH_AT45DB321D
#define AT45_Page_Size			512
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg //FLASHʹ�õ�SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH���ݵ�SPI�ٶ� 600kHz

#elif (cn_cfg_board == ADSP21469_EZ_Board)
/*____�弶��������____*/
#define easynorflash_start_sector    	0 	//��������ļ�ϵͳ��ʼ��,��Χ��0~39(����ӳ���),0=hw block 31,39=hw block 70
#define easynorflash_length_sector		40  //�����ļ�ϵͳռ��sector����
#define cn_norflash_bus_width		   	8
//SPI_FLASH_M25P16
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg        //FLASHʹ�õ�SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH���ݵ�SPI�ٶ� 600kHz
#endif

#endif









