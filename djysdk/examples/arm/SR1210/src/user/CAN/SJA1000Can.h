#ifndef _SJA1000_CAN_H_
#define _SJA1000_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "ring.h"

// Define the address of CAN Ports
#define  CN_ADDR_CAN1_BASE        (0x10800000)  // CAN��1
#define  CN_ADDR_CAN2_BASE        (0x10A00000)  // CAN��2
#define  CN_ADDR_CAN3_BASE        (0x10C00000)  // CAN��3
#define  CN_ADDR_CAN4_BASE        (0x10E00000)  // CAN��4
#define  CN_CAN_CHIP_NUM            (4)        //    4��CAN������
#define  CN_SIGNAL_PKG_SIZES_PERI         (13)

//============================================
// SJA1000 Registers for both BasicCAN & PeliCAN Mode
#define CN_CAN_CMR           ( 1)        // Command register
#define CN_CAN_SR            ( 2)        // Status  register
#define CN_CAN_IIR           ( 3)        // Interrupt identification register

//============================================
// SJA1000 Registers BasicCAN Mode
#define CN_BCAN_CR           ( 0)        // Control register ( including IER )
#define CN_BCAN_CMR          ( 1)        // Command register
#define CN_BCAN_SR           ( 2)        // Status  register
#define CN_BCAN_IIR          ( 3)        // Interrupt identification register
#define CN_BCAN_ACR          ( 4)        // Acceptance code register
#define CN_BCAN_AMR          ( 5)        // Acceptance mask register
#define CN_BCAN_BTR0         ( 6)        // Bus timing register 0
#define CN_BCAN_BTR1         ( 7)        // Bus timing register 1
#define CN_BCAN_OCR          ( 8)        // Output control register
#define CN_BCAN_TEST         ( 9)        // Test register
#define CN_BCAN_DSCR0        (10)        // Transmit buffer: Descriptor 0 (Identifier)
#define CN_BCAN_DSCR1        (11)        // Transmit buffer: Descriptor 1 (Controller)
#define CN_BCAN_FRAMENO      (12)        // Transmit buffer: FrameNo
#define CN_BCAN_TXD          (13)        // Transmit buffer: Data (13-19)
#define CN_BCAN_RXD          (20)        // Receive buffer: (20-29))
#define CN_BCAN_RX_LEN       (21)        // RX frame Length register
#define CN_BCAN_CDR          (31)        // Clock divider

//============================================
// SJA1000 Register PeliCAN mode
#define CN_PCAN_MODE         ( 0)        // Mode register
#define CN_PCAN_CMR          ( 1)        // Command register
#define CN_PCAN_SR           ( 2)        // Status register
#define CN_PCAN_IIR          ( 3)        // Interrupt identification register
#define CN_PCAN_IER          ( 4)        // Interrupt setting register
#define CN_PCAN_BTR0         ( 6)        // Bus timing register 0
#define CN_PCAN_BTR1         ( 7)        // Bus timing register 1
#define CN_PCAN_OCR          ( 8)        // Output control register
#define CN_PCAN_TEST         ( 9)        // Test register
#define CN_PCAN_ALC          (11)        // Arbitration lost capture register
#define CN_PCAN_ECC          (12)        // Error code capture register
#define CN_PCAN_EWL          (13)        // Error warning limit register
#define CN_PCAN_RXERR        (14)        // RX error counter register
#define CN_PCAN_TXERR        (15)        // TX error counter register
#define CN_PCAN_ACR0         (16)        // Acceptance code 0/RX(TX) frame information SFF(std) or EFF(Ext)
#define CN_PCAN_ACR1         (17)        // Acceptance code 1/RX(TX) identifier 1
#define CN_PCAN_ACR2         (18)        // Acceptance code 2/RX(TX) identifier 2
#define CN_PCAN_ACR3         (19)        // Acceptance code 3/RX(TX) data 1(SFF) or RX(TX) identifier 3(EFF)
#define CN_PCAN_AMR0         (20)        // Acceptance mask 0/RX(TX) data 2(SFF) or RX(TX) identifier 4(EFF)
#define CN_PCAN_AMR1         (21)        // Acceptance mask 1/RX(TX) data 3(SFF) or RX(TX) data 1
#define CN_PCAN_AMR2         (22)        // Acceptance mask 2/RX(TX) data 4(SFF) or RX(TX) data 2
#define CN_PCAN_AMR3         (23)        // Acceptance mask 3/RX(TX) data 5(SFF) or RX(TX) data 3
#define CN_PCAN_RXD          (16)        // Receive buffer [16-26(SFF) or 16-28(EFF)]
#define CN_PCAN_RX_LEN       (16)        // RX frame Length register(SFF)
#define CN_PCAN_DSCR0        (16)        // Transmit buffer: Descriptor 0 (Length)
#define CN_PCAN_DSCR1        (17)        // Transmit buffer: Descriptor 1 (Identifier)
#define CN_PCAN_DSCR2        (18)        // Transmit buffer: Descriptor 2 (controller)
#define CN_PCAN_FRAMENO      (19)        // Transmit buffer: FrameNo
#define CN_PCAN_TXD          (20)        // Transmit buffer: Data [20-26(SFF) or 22-28(EFF)]
#define CN_PCAN_RXMC         (29)        // RX message counter register
#define CN_PCAN_RBSA         (30)        // RX buffer start address register
#define CN_PCAN_CDR          (31)        // Clock divider register

//����SJA1000ģʽ�Ϳ��ƼĴ���MODλ
#define RM_RR_Bit   0x01     //��λģʽ����λ
#define LOM_Bit     0x02     //����ģʽλ
#define STM_Bit     0x04     //���Ҳ���ģʽλ
#define AFM_Bit     0x08     //�����˲���ģʽλ
#define SM_Bit      0x10     //��������ģʽλ

//����Ĵ���CMRλ����
#define TR_Bit    0x01      //��������λ
#define AT_Bit    0x02      //��ֹ����λ
#define RRB_Bit   0x04      //�ͷŽ��ջ�����λ
#define CDO_Bit   0x08      //����������λ
#define SRR_Bit   0x10      //�Խ�������λ

//״̬�Ĵ���SRλ����
#define RBS_Bit   0x01      //���ջ�����״̬λ
#define DOS_Bit   0x02      //�������״̬λ
#define TBS_Bit   0x04      //���ͻ�����״̬λ
#define TCS_Bit   0x08      //�������״̬λ
#define RS_Bit    0x10      //����״̬λ
#define TS_Bit    0x20      //����״̬λ
#define ES_Bit    0x40      //����״̬λ
#define BS_Bit    0x80      //����״̬λ

//�жϼĴ���IRλ����
#define RI_Bit   0x01       //�����ж�״̬λ
#define TI_Bit   0x02       //�����ж�״̬λ
#define EI_Bit   0x04       //���󱨾��ж�״̬λ
#define DOI_Bit  0x08       //��������ж�״̬λ
#define WUI_Bit  0x10       //�����ж�״̬λ
#define EPI_Bit  0x20       //���������ж�״̬λ
#define ALI_Bit  0x40       //�ٲö�ʧ�ж�״̬λ
#define BEI_Bit  0x80       //���ߴ����ж�״̬λ


//�ж�ʹ�ܼĴ���IERλ����
#define RIE_Bit   0x01      //�����ж�ʹ��λ
#define TIE_Bit   0x02      //�����ж�ʹ��λ
#define EIE_Bit   0x04      //���󱨾��ж�ʹ��λ
#define DOIE_Bit  0x08      //��������ж�ʹ��λ
#define WUIE_Bit  0x10      //�����ж�ʹ��λ
#define EPIE_Bit  0x20      //���������ж�ʹ��λ
#define ALIE_Bit  0x40      //��ö�ʧ�ж�ʹ��λ
#define BEIE_Bit  0x80      //���ߴ����ж�ʹ��λ

//����CAN����״̬����
#define ERR_ACTIVE    0x0   //��������
#define ERR_PASSIVE   0x1   //��������
#define ERR_BUS_OFF   0x2   //���߹ر�



enum _CAN_BAUD_RATE_SET_ {                      // ����CAN�������ʳ���
    EN_CAN_BAUD_5K = 0,                     //    5 Kbps (0)
    EN_CAN_BAUD_10K,                        //   10 Kbps (1)
    EN_CAN_BAUD_20K,                        //   20 Kbps (2)
    EN_CAN_BAUD_40K,                        //   40 Kbps (3)
    EN_CAN_BAUD_50K,                        //   50 Kbps (4)
    EN_CAN_BAUD_80K,                        //   80 Kbps (5)
    EN_CAN_BAUD_100K,                     //   100 Kbps (6)
    EN_CAN_BAUD_125K,                     //   125 Kbps (7)
    EN_CAN_BAUD_200K,                     //   200 Kbps (8)
    EN_CAN_BAUD_250K,                     //   250 Kbps (9)
    EN_CAN_BAUD_400K,                     //   400 Kbps (10)
    EN_CAN_BAUD_500K,                    //    500 Kbps (11)
    EN_CAN_BAUD_666K,                    //    660 Kbps (12)
    EN_CAN_BAUD_800K,                    //    800 Kbps (13)
    EN_CAN_BAUD_1000K                   //  1000 Kbps (14)
};

enum _CAN_MODE_SET_{                           // ����CAN ����ģʽ����
    EN_MODE_PELI_CAN = 0,                   // PeliCAN  ģʽ
    EN_MODE_BASIC_CAN                       // BasicCAN ģʽ
};


enum _CAN_CTRL_CMD_{
	EN_SET_BAUD,
	EN_SET_ADDR_FILTER,
};

typedef struct {
	struct tagRingBuf *rxBuf;
}CAN_RxTag;



typedef struct {
    CAN_RxTag* can_RxTag;
    u32 CANBaseAddr;
    u32 BaudRate;
    bool_t RecvFlag;
}CAN_DevCtrl;



bool_t CAN_Main(void);
u8 Can_Send(u8 byChipNum,u8 * txbuf,u8 len);


#ifdef __cplusplus
}
#endif

#endif /* _CAN_SJA1000_H_ */
