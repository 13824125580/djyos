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


/*____��Ӳ����ص�����____*/
#define Mhz 1000000
#define cfg_mclk (400*Mhz)  //��Ƶ
//#define cfg_fclk cfg_mclk  //cm3������������ʱ��
//#define cfg_ahbclk cfg_mclk  //����ʱ��
//#define cfg_pclk2 cfg_mclk  //��������ʱ��
#define cfg_pclk1 (cfg_mclk/2)  //��������ʱ��
#define cfg_extclk 25000000  //�ⲿʱ��=8M
//#define cfg_core_ccb_clk (400*Mhz)
//#define cfg_core_implete_clk (cfg_core_ccb_clk/8)  //tbclk
//#define cfg_rtc_clk 800*Mhz

/*____�弶��������____*/
#define easynorflash_start_sector    	0 	//��������ļ�ϵͳ��ʼ��,��Χ��0~39(����ӳ���),0=hw block 31,39=hw block 70
#define easynorflash_length_sector		40  //�����ļ�ϵͳռ��sector����
#define cn_norflash_bus_width		   	8
//SPI_FLASH_M25P16
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg        //FLASHʹ�õ�SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH���ݵ�SPI�ٶ� 600kHz


#endif









