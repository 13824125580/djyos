#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "os.h"

#include "s29xxx.h"

#define LOCAL static

void taskDelay(int tick)
{
	Djy_EventDelay(tick*20*mS);

}


LOCAL void s29gl_SectorEraseCmd(u32   baseAddr, u32 offset);
LOCAL DEVSTATUS s29gl_PollStatus(u32 baseAddr, u32 offset,u16 *expDataPtr,
		                        u16 *actDataPtr );
LOCAL DEVSTATUS s29gl_StatusGet(u32   baseAddr,u32 offset );
LOCAL void s29gl_WriteBufferAbortResetCmd(u32   baseAddr );
LOCAL DEVSTATUS s29gl_WriteBufferProgramOp(u32   baseAddr,u32 saAddr,u32 offset,
		                                   u8 *dataBuf,u32 len );
LOCAL void s29gl_StatusRegReadCmd(u32   baseAddr,u32 offset);
LOCAL void s29gl_WordProgramCmd(u32   baseAddr,u32 offset, u16 pgm_data_ptr);
LOCAL DEVSTATUS s29gl_WordProgramOp(u32   baseAddr,u32 offset,u16 write_data );

#define NOR_BASE_ADDR   (0x0000000000)
#define NOR_FLASH_SIZE  (32*1024*1024)/*32MB*/
#define ADDR_OFFSET     (0x0010000)
#define SECOTR_SIZE     (0x20000)
#define NOR_PROGBUFF_SIZE  (512)/*Ӳ���ڲ��ı�̻�������С*/

//#define NOR_FLASH_DEBUG
#define NOR_ERROR_LEVEL 0x1
#define NOR_INFO_LEVEL  0x10
#ifdef NOR_FLASH_DEBUG
int norFlashDebug=0x0;
#define NORFLASH_LOGMSG(levl,fmt,a,b,c,d,e,f) \
	if(norFlashDebug>=levl)                        \
{                          \
	logMsg (fmt,a,b,c,d,e,f);    \
}
#else
#define NORFLASH_LOGMSG(x,a,b,c,d,e,f)
#endif /* BETSET_DEBUG */
/* ==========================================================
   �������� :��ʼ��nor flash �豸�������ֲ�ֻ��Ҫ��λnor flash
   �������:baseAddr--����ַ
   �������:void
   ����ֵ:void
   ===========================================================*/
void s29gl_Init(u32  baseAddr)
{
	s29gl_ResetCmd(baseAddr);
}

void s29gl_ResetCmd(u32  baseAddr)
{
	/* Write Software RESET command */
	FLASH_WR(baseAddr, 0, NOR_RESET_CMD);
	taskDelay(1);
}

/* ==========================================================
   �������� :��ȡnor flash��ID
   �������:baseAddr--����ַ
   �������:void
   ����ֵ:��ȡ�����豸ID--0x7e2201
   ===========================================================*/
u32 s29gl_GetDeviceId(u32  baseAddr)
{
	u32 id;
	u32 temp;

	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_AUTOSELECT_CMD);

	temp = (u32)(FLASH_RD(baseAddr, 0x1) & 0xFF);
	id=temp<<16;
	temp= (u32)(FLASH_RD(baseAddr, 0xE) & 0xFF) ;
	id|=temp<<8;
	temp= (u32)(FLASH_RD(baseAddr, 0xF) & 0xFF);
	id|=temp;
	s29gl_ResetCmd(baseAddr);

	return(id);
}

/* ==========================================================
   �������� :nor flash�Ĳ����������ÿ�ε��ý���ɾ��һ������
   �������:baseAddr--nor flash����ַ,offset--����ڻ���ַ��ƫ�ƣ�������
   ����
   �������:void
   ����ֵ:ִ�в��������״̬
   ===========================================================*/
DEVSTATUS s29gl_SectorErase(u32  baseAddr,u32 offset)
{
	u16  expect_data = 0xFFFF;
	u16  actual_data = 0;
	DEVSTATUS  status;

	offset&=~(SECOTR_SIZE-1);/*��ַ��������������*/

	s29gl_SectorEraseCmd(baseAddr,offset);

	status = s29gl_PollStatus(baseAddr,offset, &expect_data, &actual_data);
	if(status!=DEV_NOT_BUSY)
	{
		taskDelay(4);/*��ʽ��ʧ�ܣ��������¸�ʽ��*/
		s29gl_SectorEraseCmd(baseAddr,offset);
		status = s29gl_PollStatus(baseAddr,offset, &expect_data, &actual_data);
	}
	return(status);
}

bool_t s29gl_Erase(u32  baseAddr,u32 offset,u32 len)
{
	u32 elen;
	bool_t result;

	result =true;

	elen = 0;
	while(elen < len)
	{
		if(DEV_NOT_BUSY !=s29gl_SectorErase(baseAddr,elen+offset))
		{
			result = false;
			break;
		}
		elen += SECOTR_SIZE;
	}

	return result;
}


void s29gl_SectorEraseCmd(u32   baseAddr,u32 offset)
{
	/* Issue Sector Erase Command Sequence */
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_ERASE_SETUP_CMD);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	/* Write Sector Erase Command to Offset */
	FLASH_WR(baseAddr, (offset>>1), NOR_SECTOR_ERASE_CMD);
}

/* ==========================================================
   �������� :д�����ݵ�nor flash��,д��֮ǰ��Ҫ��֤��ȷ����
   �������:baseAddr--nor flash����ַ,offset--����ڻ���ַ��ƫ��
   len--д������ݳ��ȣ�dataBuf--д������ݻ�����
   �������:void
   ����ֵ:ִ��д�����������״̬
   ===========================================================*/
int s29gl_WriteData(
		u32   baseAddr,
		u32   offset,     /* address offset from base address  */
		u8   *dataBuf,   /* buffer containing data to program */
		u32   len /* number of BYTES to program        */
		)
{
	u32 flag=0,writeLen=0;
	u32 saAddr,curOffset=0;
	u16 tempRd=0,tempWr=0;
	DEVSTATUS status;

	if(!dataBuf)
	{
		return DEV_PROGRAM_ERROR;
	}

	if((len<=0)||(len>NOR_FLASH_SIZE))
	{
		return DEV_PROGRAM_ERROR;
	}
	if(offset&0x01)/*������ַ*/
	{
		/*�ȶ���д*/
		tempRd=FLASH_RD(baseAddr,((offset-1)>>1));
		tempWr=(tempRd&0xff)+((*dataBuf)<<8);
		status=s29gl_WordProgramOp(baseAddr,(offset-1),tempWr);
		dataBuf++;
		len--;
		offset++;

	}
	if(len&0x01)/*�������ȵĴ���*/
	{
		flag=1;
		len--;
	}
	curOffset=offset;
	saAddr=curOffset&(~(SECOTR_SIZE-1));/*��ȡ������ַ*/

	while(len)/*д����*/
	{
		writeLen=NOR_PROGBUFF_SIZE-(curOffset%NOR_PROGBUFF_SIZE);
		if(len>=writeLen)
		{
			len-=writeLen;
		}
		else
		{
			writeLen=len;
			len=0;
		}
		if((status=s29gl_WriteBufferProgramOp(baseAddr,saAddr,curOffset,
						                    dataBuf,writeLen))!=DEV_NOT_BUSY )
		{
			return status;
		}
		dataBuf+=writeLen;
		curOffset+=writeLen;
		saAddr=curOffset&(~(SECOTR_SIZE-1));/*��ȡ������ַ*/
	}

	if(flag)
	{
		/*�ȶ���д*/
		tempRd=FLASH_RD(baseAddr,(curOffset>>1));
		tempWr=(tempRd&0xff00)+(*dataBuf);
		status=s29gl_WordProgramOp(baseAddr,curOffset,tempWr);
	}
	return status;
}


DEVSTATUS s29gl_WordProgramOp
(
 u32   baseAddr,
 u32 offset,         /* address offset from base address */
 u16 write_data    /* variable containing data to program */
 )
{
	u16 read_data = 0;
	DEVSTATUS status;

	s29gl_WordProgramCmd(baseAddr, offset, write_data);
	status = s29gl_PollStatus(baseAddr,offset,&write_data, &read_data);
	return(status);
}

void s29gl_WordProgramCmd
(
 u32   baseAddr,
 u32 offset,                  /* address offset from base address */
 u16 data          /* variable containing data to program */
 )
{

	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	/* Write Program Command */
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_PROGRAM_CMD);
	/* Write Data */
	FLASH_WR(baseAddr, (offset>>1), data);

}

DEVSTATUS s29gl_WriteBufferProgramOp(u32 baseAddr,u32 saAddr,
		u32 offset,u8 *dataBuf,u32 len)
{
	DEVSTATUS status;
	u16 write_data = 0;
	u16 read_data = 0;
	u32   last_loaded_addr;
	u32   current_offset;
	u32   end_offset;
	u16 wcount;

	current_offset  =offset;
	end_offset      =offset+len-1;
	last_loaded_addr=offset;

	/* ����Ϊ0������ */
	if (!len)
	{
		return(DEV_NOT_BUSY);
	}
	/* ����д���Ӳ�� */
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	FLASH_WR(baseAddr, (saAddr>>1), NOR_WRITE_BUFFER_LOAD_CMD);
	/* д��Ҫ��̵����ݳ��ȣ�����Ϊ��λ*/
	wcount = (u16)(len>>1) - 1;
	FLASH_WR(baseAddr, (saAddr>>1), wcount);
	/* д�����ݵ�Ӳ�������� */
	while(current_offset <= end_offset)
	{
		last_loaded_addr = current_offset;
		write_data = (*dataBuf)+(*(dataBuf+1)<<8);
		/* д����*/
		FLASH_WR(baseAddr, (current_offset>>1), write_data);
		current_offset+=2;
		dataBuf+=2;
	}
	/* ����дȷ������ */
	FLASH_WR(baseAddr, (saAddr>>1), NOR_WRITE_BUFFER_PGM_CONFIRM_CMD);
	status = s29gl_PollStatus(baseAddr, last_loaded_addr, &write_data,
			                &read_data);
	return(status);
}



/* ==========================================================
   �������� :��NOR FLASH�ж�ȡ����
   �������:baseAddr--nor flash����ַ,offset--����ڻ���ַ��ƫ��
   len--��ȡ�����ݳ��ȣ�dataBuf--��ȡ�����ݻ�����
   �������:void
   ����ֵ:void
   ===========================================================*/
void  s29gl_ReadData
(
 u32  baseAddr,
 u32  offset,       /* address offset from base address */
 u8* dataBuf,
 u32  length)//length for byte
{
	u32 tempLen=0,flag=0;
	u16 tempVal=0;
	if(!dataBuf)
	{

		return;
	}
	if((length>NOR_FLASH_SIZE)||(length<=0))
	{
		return;
	}

	if(offset&0x01)/*������ַ����*/
	{
		tempVal=*(volatile u16*)(baseAddr+(offset-1));
		*dataBuf=(tempVal>>8)&0xff;
		dataBuf++;
		length--;
		offset++;/*ż����ַ����*/
	}
	if(length&0x01)
	{
		flag++;
		length--;/*ż������*/
	}

	length=length/2;
	while(tempLen<length)
	{
		tempVal=*(volatile u16*)(baseAddr+(offset));
		*dataBuf++=tempVal&0xff;
		*dataBuf++=(tempVal>>8)&0xff;
		tempLen++;
		offset+=2;
	}

	if(flag)/*�������ȣ�����һ���ֽ�û�ж�ȡ*/
	{
		tempVal=*(volatile u16*)(baseAddr+(offset));
		*dataBuf=tempVal&0xff;
	}
}


DEVSTATUS s29gl_PollStatus
(
 u32  baseAddr,
 u32 offset,                 /* address offset from base address */
 u16 *expDataPtr,        /* expect data */
 u16 *actDataPtr        /* actual data */
 )
{
	DEVSTATUS       status;
	u32    counter = 0x1000000;//0x10000000;
	int i=0;
	for(i=0;i<20;i++);

	if(!expDataPtr||!actDataPtr)
		return DEV_STATUS_UNKNOWN;
	/*��ȡNOR FLASH��״̬*/
	do
	{
		counter--;
		status = s29gl_StatusGet( baseAddr,offset);
	}
	while((status == DEV_BUSY) && counter);
	if(!counter)
		printf("%s:timeout\r\n",__FUNCTION__);

	/*NORFLASH_LOGMSG(NOR_INFO_LEVEL,"s29gl_pollStatus:status =0x%x\n",
	                    status,2,3,4,5,6);*/
	/* ����豸���ط�DEV_NOT_BUSY����ѯ����״̬�������� */
	if(status != DEV_NOT_BUSY)
	{
		if(status == DEV_WRITE_BUFFER_ABORT)
		{
			s29gl_WriteBufferAbortResetCmd(baseAddr);
		}
		else
		{
			/* Issue software reset. */
			s29gl_ResetCmd(baseAddr);
		}
		/* indicate to caller that there was an error */
		return(status);
	}
	else /*�豸����DEV_NOT_BUSY�������ݽ�һ��У�飬��֤������ȷ*/
	{
		/* read the actual data */
		*actDataPtr = FLASH_RD(baseAddr, (offset>>1));
		/* Check that polling location verifies correctly */
		if((*expDataPtr & LLD_DB_READ_MASK) == (*actDataPtr & LLD_DB_READ_MASK))
		{
			status = DEV_NOT_BUSY;
			return(status);
		}
		else
		{
			status = DEV_VERIFY_ERROR;
			return(status);
		}
	}
}


void s29gl_StatusRegReadCmd
(
 u32  baseAddr,
 u32 offset           /* address offset from base address */
 )
{
	FLASH_WR(baseAddr,(offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR1, NOR_STATUS_REG_READ_CMD);
}


DEVSTATUS s29gl_StatusGet
(
 u32  baseAddr,
 u32      offset          /* address offset from base address */
 )
{
	volatile u16 status;

	s29gl_StatusRegReadCmd( baseAddr, (offset>>1) );		  /* Issue status register read command */
	status = FLASH_RD(baseAddr, (offset>>1));         /* read the status register */

	if ( (status & DEV_RDY_MASK) != DEV_RDY_MASK  ) /* Are all devices done bit 7 is 1 */
		return DEV_BUSY ;
	if ( status & DEV_ERASE_MASK )				    /* Any erase error */
		return DEV_ERASE_ERROR;
	if ( status & DEV_PROGRAM_MASK )				/* Any program error */
		return DEV_PROGRAM_ERROR;
	if ( status & DEV_SEC_LOCK_MASK )				/* Any sector lock error */
		return DEV_SECTOR_LOCK;
	return DEV_NOT_BUSY ;
}


void s29gl_WriteBufferAbortResetCmd(u32 baseAddr)
{

	/* Issue Write Buffer Abort Reset Command Sequence */
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
	/* Write to Buffer Abort Reset Command */
	FLASH_WR(baseAddr, LLD_UNLOCK_ADDR1, NOR_WRITE_BUFFER_ABORT_RESET_CMD);

}


#if 0
/////////////////////////////////////////////////////////////////////////////////
/*���Դ���*/
#define DATA_LENGTH  1024

void nor_Test()
{
	u32 deviceId=0;
	u16 s=0;
	int i=0,j,k;
	u8 status=0;
	u8 data[DATA_LENGTH];
	u8 rdData[DATA_LENGTH];
	u32 tick=0;
	unsigned short* pAddr;
	int fd;

	s29gl_Init(NOR_BASE_ADDR);
	deviceId=s29gl_GetDeviceId(NOR_BASE_ADDR);
	printf("device id is %x\n",deviceId);
	//////////////////////////////////////////////////////////
	printf("erase,read and write test begin.....\n");
#if 1//earse chip
	for(i=0;i<128;i++)//ÿ������128kbyte��32Mһ��256������
	{
		status=s29gl_SectorErase( NOR_BASE_ADDR,i*ADDR_OFFSET);
		taskDelay(1);
		if(status!=DEV_NOT_BUSY)
		{
			printf("erase sector is error ,sector num is %d,status is %d\n",i,status);

		}
		printf("sector num is %d,offset addr  is 0x%x\n",i,i*ADDR_OFFSET);
	}
#endif
#if 1//д��Ͷ�ȡ����

#define WRITE_NUM  1024  //д�����
	//set data buff
	memset(data,0xab,sizeof(data));
	for(i=0;i<DATA_LENGTH;i++)
	{
		if(i&0x01)
		{
			data[i]=0xde;
		}
	}
	data[DATA_LENGTH-1]=0xff;
	data[DATA_LENGTH-2]=0x99;
	///////////////////////////////
	printf("write data begin.......\n");
	for(i=0;i<WRITE_NUM;i++)
	{
		status=s29gl_WriteData(NOR_BASE_ADDR,i*sizeof(data),data,sizeof(data) );
		if(status!=DEV_NOT_BUSY)
		{
			printf("write sector is error ,sector num is %d,status is %d\n",i,status);

		}
		memset(rdData,0x0,sizeof(rdData));
		s29gl_ReadData(NOR_BASE_ADDR,i*sizeof(data), (u8*)rdData, sizeof(rdData));
		for(j=0;j<sizeof(rdData);j++)
		{
			if(rdData[j]!=data[j])
			{
				printf("write data and read error,read val=%x,write val=%x\n",
						rdData[j],data[j]);
				break;
			}
		}

		printf("write num is %d,offset addr  is 0x%x\n",i,i*sizeof(data));
	}
#endif
}
#endif



