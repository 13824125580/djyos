// =============================================================================
// Copyright (C) 2012-2020 ��԰�̱��Զ������޹�˾ All Rights Reserved
// ģ������:sys_space_config.h/sharc21469
// ģ��汾: V1.0.0
// ������Ա: liq
// ����ʱ��: 3:11:11 PM/Mar 28, 2014
// =============================================================================
// �����޸ļ�¼(���µķ�����ǰ��):
// <�汾��> <�޸�����>, <�޸���Ա>: <�޸Ĺ��ܸ���>
// =============================================================================
//maybe this is the most important files,which defines the whole sys space
//

#ifndef SYS_SPACE_CONFIG_H_
#define SYS_SPACE_CONFIG_H_

//----------------------------------------------------------------------------//
#ifdef MB
#undef MB
#endif
#define MB                      0x100000

//we has devided the ddr into several blocks as we need


//we has divided the norflash into several blocks as we need
//nor_flash START(0x04000000) END(0x05FFFFFF)      //32M-8bit
#define cn_sys_nor_flash_base              0x04000000


//defines for the text code in the norflash

//reserved 1MB for the exception record




//define for the system to run



#endif /* SYS_SPACE_CONFIG_H_ */
