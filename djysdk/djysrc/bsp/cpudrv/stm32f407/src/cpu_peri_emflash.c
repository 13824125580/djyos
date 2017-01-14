//-----------------------------------------------------------------------------
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

#include <stdlib.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <cpu_peri.h>
#include <int.h>

static struct EmbdFlashDescr *sp_tEmFlashDesrc;
//-----------------------------------------------------------------------------
//����: ��ȡ����FLASH����Ϣ
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 EmFlash_GetDescr(struct EmbdFlashDescr *Description)
{
	Description->BytesPerPage = 512;
	Description->PagesPerSmallSect = 32;
	Description->PagesPerLargeSect = 128;
	Description->PagesPerNormalSect = 256;
	Description->SmallSectorsPerPlane = 4;
	Description->LargeSectorsPerPlane = 1;
	Description->NormalSectorsPerPlane = 7;
	Description->Planes = 1;
	Description->ReservedPages = 512;
	Description->MappedStAddr = 0x08000000;
	return (0);
}
//-----------------------------------------------------------------------------
//����: ������
//����: BlkNo -- ���;
//����: "0" -- �ɹ�;"-1" -- ʧ��;
//��ע:
//-----------------------------------------------------------------------------
static s32 EmFlash_SectorEarse(u32 SectorNo)
{
	s32 Ret;
	u32 sector;

	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);

	sector = SectorNo * 0x08;
	Ret = FLASH_EraseSector(sector, VoltageRange_3);

	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
	if(Ret != FLASH_COMPLETE)
		Ret = -1;
	else
		Ret = 0;
	return (Ret);
}
//-----------------------------------------------------------------------------
//����: дĳҳ
//����: PageNo -- ҳ��;
//      Data -- д����;
//      Flags -- д��������λ;
//����: ">0" -- ������д����ֽ���;
//      "-1" -- ��������;
//      "-2" -- дʧ��;
//��ע:
//-----------------------------------------------------------------------------
static s32 EmFlash_PageProgram(u32 Page, u8 *Data, u32 Flags)
{
	u32 Ret,i;
	u32 *pData = (u32*)Data;

	u32 Addr = Page * sp_tEmFlashDesrc->BytesPerPage + sp_tEmFlashDesrc->MappedStAddr;

	if(!Data)
		return (-1);

	FLASH_Unlock();

	for(i = 0; i < sp_tEmFlashDesrc->BytesPerPage;)
	{
    	if(*(u32*)Addr != *pData)
    	{
    		Ret = FLASH_ProgramWord(Addr,*pData);
    		if(Ret != FLASH_COMPLETE)
    			break;
    	}
		pData++;
		i += 4;
		Addr += 4;
	}
	FLASH_Lock();

	return i;
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 EmFlash_PageRead(u32 Page, u8 *Data, u32 Flags)
{
	u32 Addr = Page * sp_tEmFlashDesrc->BytesPerPage + sp_tEmFlashDesrc->MappedStAddr;

	if(!Data)
		return (-1);

	memcpy(Data, (u8*)Addr, sp_tEmFlashDesrc->BytesPerPage);
	return (sp_tEmFlashDesrc->BytesPerPage);

}

//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
static s32 EmFlash_PageToSector(u32 PageNo, u32 *Remains, u32 *SectorNo)
{

	u32 PagesLeft, PagesDone;
	u32 i;
	u32 Sector;

	if((!Remains) || (!SectorNo))
		return (-1);

	Sector = 0;
	PagesDone = 0;
	PagesLeft = sp_tEmFlashDesrc->PagesPerSmallSect -
			   (PageNo % sp_tEmFlashDesrc->PagesPerSmallSect);
	for(i = 1; i <= sp_tEmFlashDesrc->SmallSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + sp_tEmFlashDesrc->PagesPerSmallSect * i))
			goto DONE;
		Sector++;
	}

	PagesDone += sp_tEmFlashDesrc->SmallSectorsPerPlane *
				 sp_tEmFlashDesrc->PagesPerSmallSect;
	PagesLeft = sp_tEmFlashDesrc->PagesPerLargeSect -
				   (PageNo % sp_tEmFlashDesrc->PagesPerLargeSect);
	for(i = 1; i <= sp_tEmFlashDesrc->LargeSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + sp_tEmFlashDesrc->PagesPerLargeSect * i))
			goto DONE;
		Sector++;
	}

	PagesDone += sp_tEmFlashDesrc->LargeSectorsPerPlane *
					 sp_tEmFlashDesrc->PagesPerLargeSect;
	PagesLeft = sp_tEmFlashDesrc->PagesPerNormalSect -
				   (PageNo % sp_tEmFlashDesrc->PagesPerNormalSect);
	for(i = 1; i <= sp_tEmFlashDesrc->NormalSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + sp_tEmFlashDesrc->PagesPerNormalSect * i))
			goto DONE;
		Sector++;
	}

	return (-1);

DONE:
	*SectorNo = Sector; // ��sector���
	*Remains = PagesLeft; //
	return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
s32 ModuleInstall_EmbededFlash(const char *ChipName, u32 Flags, u16 Start)
{
	u32 Len;
    struct FlashChip *Chip;
	struct MutexLCB *EmFlashLock;
	u8 *Buf;
	s32 Ret = 0;

	if (!ChipName)
		return (-1);

	if(sp_tEmFlashDesrc)
		return (-4); // �豸��ע��

	sp_tEmFlashDesrc = malloc(sizeof(*sp_tEmFlashDesrc));
	if(!sp_tEmFlashDesrc)
		return (-1);

    // ��ȡFLASH��Ϣ
    if(EmFlash_GetDescr(sp_tEmFlashDesrc))
    {
        TraceDrv(FLASH_TRACE_ERROR, "��������FLASH��Ϣʧ��\r\n");
		Ret = -3;
		goto FAILURE;
    }

    if(Start > ((sp_tEmFlashDesrc->NormalSectorsPerPlane+1)*
    		sp_tEmFlashDesrc->Planes))
	{
    	Ret = -1;
		goto FAILURE;
    }

	sp_tEmFlashDesrc->ReservedPages += Start * sp_tEmFlashDesrc->PagesPerNormalSect;
    Len = strlen (ChipName) + 1;
    Chip = (struct FlashChip*)malloc(sizeof(struct FlashChip) + Len);
	if (NULL == Chip)
	{
		TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
		Ret = -2;
		goto FAILURE;
	}

	memset(Chip, 0x00, sizeof(*Chip));
	Chip->Type                    = F_ALIEN;
	Chip->Descr.Embd              = *sp_tEmFlashDesrc;
	Chip->Ops.ErsBlk              = EmFlash_SectorEarse;
	Chip->Ops.WrPage              = EmFlash_PageProgram;
	Chip->Ops.RdPage              = EmFlash_PageRead;
	Chip->Ops.PageToBlk			  = EmFlash_PageToSector;
	Chip->Ops.PageToBlk			  = EmFlash_PageToSector;
	strcpy(Chip->Name, ChipName); // �豸��
	if(Flags & FLASH_BUFFERED)
	{
		Buf = (u8*)malloc(sp_tEmFlashDesrc->BytesPerPage); // NAND�ײ㻺��
		if(!Buf)
		{
			TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
			Ret = -2;
			goto FAILURE;
		}

		EmFlashLock = Lock_MutexCreate("Embedded Flash Lock");
		if(!EmFlashLock)
		{
			Ret = -3;
			goto FAILURE;
		}

		Chip->Buf = Buf;
		Chip->Lock =(void*)EmFlashLock;
	}

    Driver_DeviceCreate(NULL, Chip->Name, NULL, NULL, NULL, NULL, NULL, NULL, (ptu32_t)Chip); // �豸����"/dev"
    if(Flags & FLASH_ERASE_ALL)
		EarseWholeChip(Chip);

FAILURE:
	if(Ret)
	{
		if(sp_tEmFlashDesrc)
			free(sp_tEmFlashDesrc);
		if(EmFlashLock)
			Lock_MutexDelete(EmFlashLock);
		if(Buf)
			free(Buf);
		if(Chip)
			free(Chip);
	}
	return (Ret);
}
