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
// �����Ȩ�����ʹ���߷������¶������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�����������У���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�����������У��Լ�����
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

#ifndef __CONFIG_PRJ_H_
#define __CONFIG_PRJ_H_

#ifdef __cplusplus
extern "C" {
#endif


/*____��Ӳ����ص�����____*/
#define Mhz 1000000
#define CN_CFG_PLL0_SYSCLK1 (400*Mhz)
#define CN_CFG_PLL0_SYSCLK2 (CN_CFG_PLL0_SYSCLK1/2)
#define CN_CFG_PLL0_SYSCLK4 (CN_CFG_PLL0_SYSCLK1/4)
#define CN_CFG_PLL0_SYSCLK6 (CN_CFG_PLL0_SYSCLK1)
#define CN_CFG_PLL0_SYSCLK3 (100*Mhz)
#define CN_CFG_PLL0_SYSCLK7 (50*Mhz)
#define CN_CFG_PLL1_SYSCLK1 (150*Mhz)
#define CN_CFG_PLL1_SYSCLK2 (150*Mhz)
#define CN_CFG_MCLK CN_CFG_PLL0_SYSCLK1  //��Ƶ
#define CN_CFG_AUX_CLK (25*Mhz)  //timer��usb��iic��rtc��mcaspʱ��
#define CN_CFG_TIMER_CLK CN_CFG_AUX_CLK  //��ʱ��ʱ��Դ
#define CN_CFG_DDRII_CLK 150  //��MΪ��λ��omapl138���֧��150M

/*____����tick����____*/
#define CN_CFG_TICK_US 1000  //����ϵͳ�ں�ʱ�����峤�ȣ���usΪ��λ��
#define CN_CFG_TICK_HZ 1000  //�ں�ʱ��Ƶ�ʣ���λΪhz��
#define CN_CFG_FINE_US 0x00000a3d  //40nS,����ʱ�����峤�ȣ���uSΪ��λ��32λ������������С����ռ16λ����Ҳ������ticks�������65535uS
#define CN_CFG_FINE_HZ (25*Mhz)  //�ں˾���ʱ��Ƶ�ʣ���cn_fine_ns�ĵ�����

/*____ʱ���������____*/
#define CN_CFG_TIME_MODULE 1  //�Ƿ����ʱ��ģ��
#define CN_CFG_TIME_ZONE (+8)  //����ʱ��
#define CN_CFG_TIME_SRC_TICK 0  //�ں������������ʱ��׼
#define CN_CFG_TIME_SRC_RTC 1  //Ӳ��rtc����ʱ��׼
#define CN_CFG_TIME_SRC _tick  //ѡ����ʱ��׼

/*____cpu����ģʽ����____*/
#define CN_MODE_TINY 0  //tinyģʽ
#define CN_MODE_SI 1  //��ӳ��ģʽ
#define CN_MODE_DLSP 2  //��̬���ص�����ģʽ
#define CN_MODE_MP 3  //�����ģʽ��ֻ��mmu=true�ſ�ѡ���ģʽ
#define CN_CFG_RUN_MODE CN_MODE_SI

/*____�ַ�����֧��gui kernel���������ò�������____*/
#define CN_CFG_CHARSET_GB2312 1  //gb2312����
#define CN_CFG_CHARSET_ASCII 0  //gb2312�Ѿ�������ascii���룬������֧��
#define CN_CFG_CHARSET_UTF8 0  //unicode�����е�һ�ֱ��뷽ʽ
#define CN_CFG_CHARSET_DEFAULT "gb2312"  //����������C�ļ��ж�����ַ���������ͬ

/*____���壬֧��gui kernel���������ò�������____*/
#define CN_CFG_GB2312_8x16_1616 1  //����8*16�����ascii�ַ���16*16��������庺��
#define CN_CFG_ASCII_8x8 1  //����8*8�����ascii�ַ�
#define CN_CFG_ASCII_8x16 0  //����8*16�����ascii�ַ�gb2312_816_1616���Ѿ����������뵥������
#define CN_CFG_FONT_DEFAULT "gb2312_8_16_1616"  //����������C�ļ���ʹ�õ�������Դ������ͬ,

/*____gui kernel����____*/
#define CN_CFG_GUI_KERNEL 1  //gui kernelģ��
#define CN_CFG_DOWN_CHUNNEL_DEEP 1024  //�����������ȣ��ֽ���
#define CN_CFG_GK_UP_CHUNNEL_DEEP 256  //gui kernel��Ϣ���������ȣ��ֽ���
#define CN_CFG_GUI_PATTERN_MODE 0  //ģʽλͼ�ߴ磬0=�̶�8*8λͼ��1=����bitmap��V1.0.0��֧�֣�

/*____��������____*/
#define CN_CFG_DEVICE_LIMIT 20  //�����豸������
#define CN_CFG_LOCKS_LIMIT 20  //��������п��õ���������������������ָ�û�����semp_create��mutex_create�����������������ں��õ�������
#define CN_CFG_EVENTS_LIMIT 100  //�¼�����
#define CN_CFG_EVTTS_LIMIT 32  //���¼�������
#define CN_CFG_PARAS_LIMIT (CN_CFG_EVENTS_LIMIT+CN_CFG_EVTTS_LIMIT)  //�¼���������������һ������
#define CN_CFG_WDTS_LIMIT 5  //������������
#define CN_CFG_PIPES_LIMIT 5  //�ܵ�����

/*____���ڴ������صĳ���____*/
#define CN_CFG_VIRTUAL_MEMORY 0  //2440Ӳ����Ȼ֧�������ڴ棬��SI�汾������֧�֣���Ϊ0
#define CN_CFG_PHY_MEMORY 0  //�����ڴ�������֧�������ڴ�������塣
#define CN_CFG_MMU_USED 1  //ʹ��MMU
#define CN_CFG_CACHE_USED 1  //ʹ��Cache
#define CN_CFG_PAGE_SIZE 0x1000  //��ҳ�ߴ�Ϊ4Kbytes
#define CN_CFG_PAGE_SIZE_SUFFIX_ZERO 12  //ҳ�ߴ��׺0�ĸ���
#define CN_CFG_MEM_RECYCLE true  //֧���ڴ����----���ù���
#define CN_CFG_MEM_POOLS (10)  //������10���ڴ��
#define CN_CFG_SYSSVC_STACK 1024  //ϵͳ�����¼�ջ����
#define CN_CFG_DYNAMIC_MEM 1  //�Ƿ�֧�ָ߼��ڴ���䣬��ʹ��֧�֣�Ҳ����ʹ��malloc-free�����ڴ棬�������ϸ�ɶԵ��ã�������ڴ�й©
#define CN_CFG_STACK_FILL 'd'  //ջ���ֵ����Ϊ0�����
#define CN_CFG_MAIN_STACK 1024  //main��������ջ����

/*____�ļ�ϵͳ�������____*/
#define CN_CFG_DJYFS 1  //�Ƿ�����ļ�ϵͳ
#define CN_CFG_DJYFS_FLASH 0  //�Ƿ����flash�ļ�ϵͳ������
#define CN_CFG_FAT_FILESYS 0  //�Ƿ����flash�ļ�ϵͳ������
#define CN_CFG_DJYFS_RAM 0  //�Ƿ����ram�ļ�ϵͳ
#define CN_CFG_OPENED_FSNODE_LIMIT 16  //��ͬʱ�򿪵��ļ�������ע��༶·����ÿһ������һ���ļ�������ͬ��·�����ظ���

/*____�������____*/
#define CN_CFG_PAN_DRIVER 1  //�Ƿ�������豸����ģ��
#define CN_CFG_DEBUG_INFO 1  //������Ϣ�ռ������ģ��
#define CN_CFG_SHELL 1  //�Ƿ����shellģ��
#define CN_CFG_PIPE 1  //�Ƿ�����ܵ�ͨ��ģ��
#define CN_CFG_WDT 0  //�Ƿ�������Ź�����
#define CN_CFG_NUDE_TERMINAL 0  //nude terminal��һ��������ʱ�ṩ��������˻��Ի����ܵ�ģ��
#define CN_CFG_DJYIP 0  //djyip ģ��

/*____��׼����ģ������____*/
#define CN_CFG_KEYBOARD 1  //�Ƿ������������
#define CN_CFG_SINGLE_TOUCH 0  //�Ƿ�֧�ֵ��㴥����
#define CN_CFG_MUTI_TOUCH 0  //�Ƿ�֧�ֶ�㴥����
#define CN_CFG_MUTI_KEYBOARD 0  //0=��֧�ֶ���̣���ʹϵͳӵ�ж���̣����м���Ҳ�������ֵ�ʹ��
#define CN_CFG_MUTI_MOUSE_2D 0  //0=��֧�ֶ���꣬��ʹϵͳӵ�ж����꣬�������Ҳ����ͬһ�����ָ�롣
#define CN_CFG_MUTI_MOUSE_3D 0  //0=��֧�ֶ�3d��꣬��ʹϵͳӵ�ж��3d��꣬�������Ҳ����ͬһ�����ָ�롣
#define CN_CFG_STDIN_DEVICE "uart1"  //��׼�����豸
#define CN_CFG_STDIN_DEVICE_LIMIT 3  //��֧�ֵ������豸����

/*____��������____*/
#define CN_CFG_UART0_USED 0
#define CN_CFG_UART1_USED 1
#define CN_CFG_UART2_USED 0

/*____����·������____*/
#define CN_CFG_WORK_PATH "sys:\\"  //����·�������û������õĻ���set_work_path���ǿ��ַ�

#ifdef __cplusplus
}
#endif
#endif

