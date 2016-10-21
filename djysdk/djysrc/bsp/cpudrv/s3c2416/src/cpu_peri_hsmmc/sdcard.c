#include <driver/card/card.h>
#include <stdlib.h>
#include <djyfs/fat/port/drivers/fat_drivers.h>
#include <djyfs/fat/ff11/src/integer.h>
#include <djyfs/fat/ff11/src/diskio.h>


//-----------------------------------------------------------------------------
//����:
//����:
//���:
//����:
//-----------------------------------------------------------------------------
int MMC_disk_status(void)
{
    return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע: ���Խ��豸��ʼ����������;
//-----------------------------------------------------------------------------
int MMC_disk_initialize(void)
{
	static BYTE Done = 0;

    if(1 == Done)
        return (0);

    Done = 1;
    return (0);
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
int MMC_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	u32 *Buf = (u32*)buff;
	extern s32 S3c2416_BlkRead(const u32 *Buf, u32 BlkNo, u32 Count);

	if(S3c2416_BlkRead(Buf, sector, count))
    	return (-1);// ʧ��

    return (0);// ��ȷ
}
//-----------------------------------------------------------------------------
//����:
//����: buff -- д�����ݡ�
//     sector -- Ŀ��ҳ�š�
//     count -- ҳ������
//����: 0 -- ��ȷ;-1 -- ����;
//��ע:
//-----------------------------------------------------------------------------
int MMC_disk_write(BYTE *buff, DWORD sector, UINT count)
{

	u32 *Buf = (u32*)buff;
	extern s32 S3c2416_BlkWrite(const u32 *Buf, u32 BlkNo, u32 Count);

	if(S3c2416_BlkWrite(Buf, sector, count))
		return (-1);// ʧ��
    return (0);// ��ȷ
}
//-----------------------------------------------------------------------------
//����:
//����:
//����:
//��ע:
//-----------------------------------------------------------------------------
int MMC_ioctl( BYTE cmd, void *buff)
{

	extern struct CardDescription Card;
    switch(cmd)
    {
        case GET_SECTOR_SIZE:
            *(DWORD *)(buff) = (1 << Card.BlkLenSettings);//
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)(buff) = Card.CapacityInBytes >> Card.BlkLenSettings;//
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)(buff) = 1;// todo
            break;
       default:
           break;
    }
    return (RES_OK);
}

//-----------------------------------------------------------------------------
//����:
//����: ChipName --
//      Clean -- ������ʽ��;"1"--��;"0"--��
//����: "0" -- �ɹ�;
//      "-1" -- �����������;
//      "-2" -- �ڴ治��;
//      "-3" -- ����ʧ��;
//��ע: todo������һ��sd����ṹ��
//-----------------------------------------------------------------------------
struct FatDrvFuns SDDrv =
{
	.DrvStatus     = MMC_disk_status,
	.DrvInitialize = MMC_disk_initialize,
	.DrvRead       = MMC_disk_read,
	.DrvWrite      = MMC_disk_write,
	.DrvIoctl      = MMC_ioctl
};
extern struct CardDescription Card;
s32  ModuleInstall_SD(const char *ChipName, s32 Clean)
{
	s32 Ret;
	char *Name;
	(void)Clean;

	if(NULL == ChipName)
		return (-1);

	Name = (char*)malloc(strlen(ChipName)+1);
	if(NULL == Name)
		return (-2);

	strcpy(Name, ChipName);

	Ret = S3c2416_HsmmcInit(1);
	if(0 == Ret)
	{
		if(Driver_DeviceCreate(NULL, Name, NULL, NULL, NULL,NULL, NULL, NULL, ((ptu32_t)&SDDrv)))
		{
			if((0 == Clean) || ((Clean) &&
			  (0 == S3c2416_BlkErase(0, ((Card.CapacityInBytes >> Card.BlkLenSettings)-1)))))
			  	  return (0);// �ɹ�
		}

	}

	free(Name);
	return (-3);// ����ʧ��
}
