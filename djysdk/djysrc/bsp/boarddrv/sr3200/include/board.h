// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// �ļ���    ��board.h
// ģ������: �����һЩ��������
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 11/11.2015
// =============================================================================

#ifndef __BOARD__H__
#define __BOARD__H__


void BrdWdt_FeedDog(void);
void BrdWdt_ModuleInit(void);

void Board_IOInit(void);
void RUN_Status(u8 Run);
void LOCK1_Lock(void);
void LOCK1_UnLock(void);
void LOCK2_Lock(void);
void LOCK2_UnLock(void);
u8 KI_GetValue(void);

void FPGA_ModuleInit(void);


void L138COM_ModuleInit(void);
u32	L138COM1_Send(u8 *buf,u32 len);
u32	L138COM1_Receive(u8 *buf,u32 len,u32 wait_ms);
u32	L138COM2_Send(u8 *buf,u32 len);
u32	L138COM2_Receive(u8 *buf,u32 len,u32 wait_ms);





#endif






