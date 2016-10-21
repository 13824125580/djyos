//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// �����Ȩ�����ʹ���߷��������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������
// 3. �������Ϊ�����߲���ϵͳ����ɲ��֣�δ����ǰȡ�õ�������ɣ���������ֲ����
//    �����߲���ϵͳ���������С�

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------

#include <stddef.h>
#include <endian.h>
#include "flash.h"


//-----------------------------------------------------------------------------
//����: ����NAND FLASH��ONFI����ҳ��
//����: OnfiPage -- NandParamterPage
//      Onfi -- Nand�����ṹ��
//      Little -- NandParamterPage�ĸ�ʽ,"1"��ʾ32λС�ˡ�
//����: 0 -- �ɹ�; -1 -- ʧ��;
//��ע: ����ҳ�ľ������ݽṹ,Ҫ��ע����NAND FLASH�Ĳ����ֲ�
//-----------------------------------------------------------------------------
s32 DecodeONFI(const char *OnfiPage, struct NandDescr *Onfi, u8 Little)
{
   u8 i;
   u32 Offset;
   u8 Temp[20] = {0};
   char Signatrue[4] = {'O', 'N', 'F', 'I'};

   //ONFI���
   Offset = 0;
   if(Little)
   {
	   *(u32 *)Signatrue = swapl(*(u32 *)Signatrue);
   }
   for (i = 0; i < 4; i++)
   {
	   if (Signatrue[i] != (char)OnfiPage[Offset+i])
		   return (-1);// ������ݲ���Ҫ��
   }

   //NOFI�汾��
   Offset = 4;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
	   Onfi->Revision = (u16)swapl(*(u32 *)Temp);
   else
	   Onfi->Revision = (u16)(*(u32 *)Temp);

   //Device Manufacturer
   Offset = 32;
   for (i = 0; i < 12; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   for (i = 0; i < 3; i++)
	   {
		   ((u32 *)Temp)[i] = swapl(((u32 *)Temp)[i]);
	   }
   }
   for (i = 0; i < 12; i++)
   {
       Onfi->Manufacturer[i] = (char)Temp[i];
   }
   Onfi->Manufacturer[i] = '\0';

   //Device Model
   Offset = 44;
   for (i = 0; i < 20; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   for (i = 0; i < 5; i++)
	   {
		   ((u32 *)Temp)[i] = swapl(((u32 *)Temp)[i]);
	   }
   }
   for (i = 0; i < 20; i++)
   {
       Onfi->DeviceModel[i] = (char)Temp[i];
   }
   Onfi->DeviceModel[i] = '\0';

   //Number of data bytes per page
   Offset = 80;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->BytesPerPage = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->BytesPerPage = *(u32 *)Temp;
   }

   //Number of spare bytes per page
   Offset = 84;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->OOB_Size = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->OOB_Size = *(u32 *)Temp;
   }

   //Number of pages per block
   Offset = 92;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->PagesPerBlk = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->PagesPerBlk = *(u32 *)Temp;
   }

   //Number of blocks per LUN
   Offset = 96;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->BlksPerLUN = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->BlksPerLUN = *(u32 *)Temp;
   }

   //Number of LUNs
   Offset = 100;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->LUNs = (u8)swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->LUNs = (u8)(*(u32 *)Temp);
   }

#if 0
   //Guaranteed valid block at the beginning of target
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[104+i];
   }
   *(u32 *)Temp = swapl(*(u32 *)Temp);
   Onfi->SafeBlkNo = Temp[3];
#endif

   TraceDrv(FLASH_TRACE_DEBUG, "ONFI revision:0x\%x\r\n", Onfi->Revision);
   TraceDrv(FLASH_TRACE_DEBUG, "Manufacturer:%s\r\n", Onfi->Manufacturer);
   TraceDrv(FLASH_TRACE_DEBUG, "Device Model:%s\r\n", Onfi->DeviceModel);
   TraceDrv(FLASH_TRACE_DEBUG, "bytes per page:0x%x\r\n", Onfi->BytesPerPage);
   TraceDrv(FLASH_TRACE_DEBUG, "oob size:0x%x\r\n", Onfi->OOB_Size);
   TraceDrv(FLASH_TRACE_DEBUG, "pages per block:0x%x\r\n", Onfi->PagesPerBlk);
   TraceDrv(FLASH_TRACE_DEBUG, "blocks per LUN:0x%x\r\n", Onfi->BlksPerLUN);
   TraceDrv(FLASH_TRACE_DEBUG, "LUNs:0x%x\r\n", Onfi->LUNs);
#if 0
   TraceDrv(FLASH_TRACE_DEBUG, "Guaranteed block:0x%x\r\n", Onfi->SafeBlkNo );
#endif

   return (0);
}
//-----------------------------------------------------------------------------
//����: ��������оƬ(���������)
//����:
//����: >=0 -- ���ڵĻ�������-1 -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
s32 EarseWholeChip(struct FlashChip *Chip)
{
    u32 i;
    u32 Blks;
    s32 BadBlks = 0;

    if(NULL == Chip)
    	return (-1);

    if(F_NAND == Chip->Type)
    {
    	Blks = Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs;

		for(i = Chip->Descr.Nand.ReservedBlks; i < Blks; i++)
		{
			if(0 != Chip->Ops.ErsBlk(i))
			{
				// ����ʧ��
				BadBlks++;
				TraceDrv(FLASH_TRACE_ERROR, "block %d cannot be erased!\r\n", i);
			}
		}
    }
    else if(F_NOR == Chip->Type)
    {
    	if(0 == Chip->Descr.Nor.ReservedBlks)
    	{
			if(0 != Chip->Ops.Special.Nor.ErsChip())
			{
				// ����ʧ��
				BadBlks = -1;
				TraceDrv(FLASH_TRACE_ERROR, "chip %d cannot be erased!\r\n", i);
			}
    	}
		else
		{
			for(i = Chip->Descr.Nand.ReservedBlks; i < Chip->Descr.Nor.Blks; i++)
			{
				if(0 != Chip->Ops.ErsBlk(i))
				{
					// ����ʧ��
					BadBlks++;
					TraceDrv(FLASH_TRACE_ERROR, "block %d cannot be erased!\r\n", i);
				}
			}
		}

    }
    else
    {
    	BadBlks = -1;
    }
    return (BadBlks);
}
