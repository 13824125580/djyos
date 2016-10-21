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
// �ļ���     ��dm9000c.c
// ģ������: DM9000cep������������DJYOS����̫��Э��ʵ��Ҫ��ʵ�ֵ������ײ�����
// ģ��汾: V1.00
// ������Ա: HM
// ����ʱ��: 18/09.2014
// =============================================================================
#include <sys/socket.h>

#include "dm9000a.h"

#ifndef DM9000_DBG
#define DM9000_DBG       printk
#endif

#define CN_PKG_MAX_LEN   1500
#define CN_DM9000NAME_LEN  32
typedef struct
{
	char   devname[CN_DM9000NAME_LEN];
	u8     devmac[CN_MACADDR_LEN];
    struct SemaphoreLCB rcvsync;
    struct MutexLCB     devsync;
    ptu32_t handle;
    u32 irqno;
    bool_t (*clearextint)(u32 irqno);
    u16 *cmdaddr;
    u16 *dataddr;
}tagDm9000Dev;

u8  regread(tagDm9000Dev *dm9000,u16 reg)
{
	u16  data;
	*dm9000->cmdaddr = reg;
	data = *dm9000->dataddr;
	return (u8)data;
}
void  regwrite(tagDm9000Dev *dm9000,u16 reg,u16 data)
{
	*dm9000->cmdaddr = reg;
	*dm9000->dataddr = data;
	return;
}

static u8  regTab[]={0,1,2,3,4,5,6,7,8,9,10,254,255};
#define CN_REGLEN  sizeof(regTab)

void __showDm9000Reg(tagDm9000Dev *dm9000)
{
	int i;
	u8  value;
	for(i =0;i < CN_REGLEN;i++ )
	{
		value = regread(dm9000,regTab[i]);
		printk("reg:0x%02x:value:0x%02x\n\r",regTab[i],value);
	}

	return;
}

static bool_t __dm9000Probe(tagDm9000Dev *dm9000)
{
    u32 id_val;

    id_val  = regread(dm9000,DM9KS_VID_L);
    id_val |= regread(dm9000,DM9KS_VID_H) << 8;
    id_val |= regread(dm9000,DM9KS_PID_L) << 16;
    id_val |= regread(dm9000,DM9KS_PID_H) << 24;
    if (id_val == DM9KS_ID)
    {
        DM9000_DBG("dm9000 i/o:  id: 0x%x \r\n",id_val);
        return true;
    }
    else
    {
        DM9000_DBG("dm9000 not found !\r\n");
        return false;
    }
}
// =============================================================================
// ��  ��: дdm9000 PHY �Ĵ���
// ��  ��: phy_reg �� �Ĵ�����ַ
//       writedata �� д�������
// ��  ��: ��
// =============================================================================
static void __dm9000PhyWrite(tagDm9000Dev *dm9000,u8 phy_reg, u16 writedata)
{
    /* ����д�� PHY �Ĵ�����λ�� */
    regwrite(dm9000,DM9000_REG_EPAR, phy_reg | DM9000_PHY);

    /* ����д�� PHY �Ĵ�����ֵ */
    regwrite(dm9000,DM9000_REG_EPDRH, ( writedata >> 8 ) & 0xff);
    regwrite(dm9000,DM9000_REG_EPDRL, writedata & 0xff);

    regwrite(dm9000,DM9000_REG_EPCR, 0x0a);                       /* ������д�� PHY �Ĵ��� */
    while(regread(dm9000,DM9000_REG_EPCR) & 0x01); //todo ���˳�����  /* ��Ѱ�Ƿ�ִ�н��� */
    regwrite(dm9000,DM9000_REG_EPCR, 0x08);                       /* ���д������ */
}
// =============================================================================
// ��  ��: ��dm9000E���������λ
// ��������
// ���أ���
// =============================================================================
static  void __dm9000Reset(tagDm9000Dev *dm9000)
{

	if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
	{
	    regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* �� dm9000 ����������� */
	    Djy_DelayUs(10);                                /* delay 10us */
	    regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* �� dm9000 ����������� */
	    Djy_DelayUs(10);                                /* delay 10us */

	    /* �����Ǵ���������� */
	    regwrite(dm9000,DM9000_REG_IMR, DM9000_IMR_OFF);          /* �����ڴ��Ի�ģʽ */
	    regwrite(dm9000,DM9000_REG_TCR2, DM9000_TCR2_SET);        /* ���� LED ��ʾģʽ1:ȫ˫��������˫���� */

	    /* ���������Ѷ */
	    regwrite(dm9000,DM9000_REG_NSR, 0x2c);
	    regwrite(dm9000,DM9000_REG_TCR, 0x00);
	    regwrite(dm9000,DM9000_REG_ISR, 0x3f);

	#ifdef dm9000_FLOW_CONTROL
	    regwrite(dm9000,DM9000_REG_BPTR, DM9000_BPTR_SET);            /* ��˫���������� */
	    regwrite(dm9000,DM9000_REG_FCTR, DM9000_FCTR_SET);            /* ȫ˫���������� */
	    regwrite(dm9000,DM9000_REG_FCR, DM9000_FCR_SET);          /* ������������ */
	#endif

	#ifdef dm9000_UPTO_100M
	    /* dm9000�޴˼Ĵ��� */
	    regwrite(dm9000,DM9000_REG_OTCR, DM9000_OTCR_SET);            /* ����Ƶ�ʵ� 100Mhz ���� */
	#endif

	    regwrite(dm9000,DM9000_REG_IMR, DM9000_IMR_SET);          /* ���� �ж�ģʽ */

	    regwrite(dm9000,DM9000_REG_RCR, 0x39/*DM9000_RCR_SET*/);

    	Lock_MutexPost(&dm9000->devsync);
	}
         /* ���� ���չ��� */
}


// =============================================================================
// ���ܣ����� dm9000 MAC �� �㲥 �� �ಥ �Ĵ���
// ��������
// ���أ���
// =============================================================================
static void dm9000_hash_table(tagDm9000Dev *dm9000)
{
    u8 i;
    u8 mac[6],*p;

    if(1)
    {
        p   =mac;
        for(i = 0; i < 6; i++)
        {
            p[i] =regread(dm9000,DM9000_REG_PAR + i);
        }

        DM9000_DBG("DM_MAC:  %02X,%02X,%02X,%02X,%02X,%02X.\r\n",
                    p[0],p[1],p[2],p[3],p[4],p[5]);
    }

    /* ���� ���� MAC λ�ã������ MyHardware */
    for(i = 0; i < 6; i++)
    {
        regwrite(dm9000,DM9000_REG_PAR + i, dm9000->devmac[i]);
    }

    for(i = 0; i < 8; i++)                              /* ��� �����ಥ���� */
    {
        regwrite(dm9000,DM9000_REG_MAR + i, 0x00);
    }

    regwrite(dm9000,DM9000_REG_MAR + 7, 0x80);                    /* ������ �㲥�� ���� */
}

// =============================================================================
//��  ��: ��ʼ��dm9000E������������ϣ����صļĴ������õ�
// ��������
// ���أ���
// =============================================================================
static void __dm9000HardInit(tagDm9000Dev *dm9000)
{
	u32 timeout = 1000;
	regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* �� dm9000 ����������� */
    Djy_DelayUs(10);                                /* delay 10us */

    dm9000_hash_table(dm9000);                               /* ���� dm9000 MAC �� �ಥ*/

    __dm9000Reset(dm9000);                                    /* ���� dm9000 ������� */

    regwrite(dm9000,DM9000_REG_GPR, DM9000_PHY_OFF);          /* �ر� PHY ������ PHY ����*/
    __dm9000PhyWrite(dm9000,0x00, 0x8000);                    /* ���� PHY �ļĴ��� */
#ifdef dm9000_FLOW_CONTROL
    __dm9000PhyWrite(dm9000,0x04, 0x01e1 | 0x0400);           /* ���� ����Ӧģʽ���ݱ� */
#else
    __dm9000PhyWrite(dm9000,0x04, 0x01e1);                    /* ���� ����Ӧģʽ���ݱ� */
#endif
    //__dm9000PhyWrite(dm9000,0x00, 0x1000);                  /* ���� ��������ģʽ */
    /* ����ģʽ����
      0x0000 : �̶�10M��˫��
      0x0100 : �̶�10Mȫ˫��
      0x2000 : �̶�100M��˫��
      0x2100 : �̶�100Mȫ˫��
      0x1000 : ����Ӧģʽ
    */
    __dm9000PhyWrite(dm9000,0x00, 0x1000);                   /* ���� ��������ģʽ */

    regwrite(dm9000,DM9000_REG_GPR, DM9000_PHY_ON);  /* ���� PHY ����, ���� PHY */


    while(!(regread(dm9000,DM9000_NSR) & NSR_LINKST))
    {
    	if(timeout == 0)
    	{
    		DM9000_DBG("DM9000 Link failed!\r\n");
    		break;
    	}
    	timeout--;
    	Djy_EventDelay(1000);
    }

    if(timeout)
    	DM9000_DBG("DM9000 Linked!\r\n");
}

// =============================================================================
// �������ܣ�NetHard_Send
//          �����������ݰ�
// ���������netdev,ʹ�õ�����
//        pkg,�����͵����ݰ�
//        netdevtask,������Ҫ��ɵĹ���
// ���������
// ����ֵ  ��true���ͳɹ�  false����ʧ�ܡ�
// ˵��    �����ÿ����ķ�ʽ���ͣ���������ʹ��������
// =============================================================================
static bool_t __dm9000Snd(ptu32_t handle,tagNetPkg *pkg,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    u16 *mysrc;
    u16 sndlen;
    u16 i;
    tagDm9000Dev  *dm9000;

    result = false;
    if((0 != handle)&&(NULL != pkg))
    {
    	dm9000 = (tagDm9000Dev  *)NetDevPrivate(handle);
        sndlen = 0;
        tmp = pkg;
        //cout the len
        tmp = pkg;
        sndlen = 0;
        while(NULL != tmp)
        {
            sndlen +=tmp->datalen;
            if(PKG_ISLISTEND(tmp))
            {
                tmp = NULL;
            }
            else
            {
                tmp = tmp->partnext;
            }
        }
        if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
        {
            /* ��� dm9000 �Ƿ��ڴ����У����ǵȴ�ֱ�����ͽ��� */
            while(regread(dm9000,DM9000_REG_TCR) & DM9000_TCR_SET)
            {
                Djy_DelayUs (5);
            }

            //snd all the pkg
            tmp = pkg;
            //init the dm9000
            regwrite(dm9000,DM9000_TXPLH, (sndlen>>8) & 0x0ff);//���÷������ݳ���
            regwrite(dm9000,DM9000_TXPLL, sndlen & 0x0ff);

            *dm9000->cmdaddr = DM9000_REG_MWCMD;                    //�������ݻ��渳�����ݶ˿�

            while(NULL!= tmp)
            {
                sndlen = tmp->datalen;
                mysrc = (u16 *)(tmp->buf + tmp->offset);
                //��������
                for(i=0;i<sndlen;i+=2)
                {
                    *dm9000->dataddr = *mysrc++;  //8λ����ת��Ϊ16λ�������
                }
                if(PKG_ISLISTEND(tmp))
                {
                    tmp = NULL;
                }
                else
                {
                    tmp = tmp->partnext;
                }
            }
            //ok now start transfer;
            regwrite(dm9000,DM9000_REG_TCR, DM9000_TCR_SET);  /* ���д��� */

    //        while((dm9000_reg_read(DM9000_NSR) & 0x0c) == 0)
    //        ;                                               //�ȴ����ݷ������
            regwrite(dm9000,DM9000_NSR, 0x2c);                 //���TX״̬

            result = true;

        	Lock_MutexPost(&dm9000->devsync);
        }

    }
    return result;
}
// =============================================================================
// �������ܣ�NetHard_Rcv
//          ���������ݺ���
// �����������
// ���������
// ����ֵ  ��
// ˵��    ������������
// =============================================================================
static tagNetPkg *__dm9000RcvPkg(tagDm9000Dev *dm9000)
{
    s32   rx_length,i;
    u8    rx_ready;
    u16 rx_status;
    tagNetPkg *pkg=NULL;
    u16 *dst;

    if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
    {
        rx_ready = regread(dm9000,DM9000_MRCMDX);      //�ȶ�ȡһ����Ч������,һ�����0
        rx_ready = regread(dm9000,DM9000_MRCMDX);     //������ȡ�������ݰ����ֽ�

        if(rx_ready == 1)                           //�ж����ֽ��Ƿ�Ϊ1��0
        {
        	*dm9000->cmdaddr = DM9000_MRCMD;            //������ȡ���ݰ�����
            rx_status = *dm9000->dataddr;               //״̬�ֽ�
            rx_length = *dm9000->dataddr;               //���ݳ���
            rx_status &= 0xff;

            if((rx_length > 0) && (rx_length <= CN_PKG_MAX_LEN))
            {
                if(rx_length%2)
                    pkg =PkgMalloc(rx_length+1,0);
                else
                    pkg =PkgMalloc(rx_length,0);
                pkg->partnext = NULL;
            }
            if(pkg != NULL)
            {
                dst = (u16 *)(pkg->buf + pkg->offset);
                if(!(rx_status & 0xbf00) && (rx_length < 10000))//�ж������Ƿ����Ҫ��
                {
                    for(i=0; i<rx_length; i+=2)          //16λ����ת��Ϊ8λ���ݴ洢
                    {
                        *dst++ = *dm9000->dataddr;
                    }
                }
                pkg->datalen = rx_length;
                pkg->partnext= NULL;
            }
        }

    	Lock_MutexPost(&dm9000->devsync);
    }


    return pkg;
}


#define CN_DM9000RCV_TIMEOUT   (1*mS)    //5 seconds timeout
static ptu32_t dm9000Rcv(void)
{
    tagNetPkg *pkg;
	tagDm9000Dev *dm9000;

    Djy_GetEventPara((ptu32_t *)&dm9000,NULL);
    while(1)
    {
        Lock_SempPend(&dm9000->rcvsync,CN_DM9000RCV_TIMEOUT);
        while((pkg = __dm9000RcvPkg(dm9000))!= NULL)
        {
            LinkPost(dm9000->handle,pkg);
            PkgTryFreePart(pkg);
        }
    	//rcv overflow will reset the dm9000a
        u8  rcvstat;
    	u32 times = 0;

        rcvstat = regread(dm9000,DM9000_REG_NSR);
        if(rcvstat &NSR_RXOV)
        {
        	times++;
        	rcvstat = regread(dm9000,DM9000_REG_RSR);
        	printk("RSR  :0X%02x\n\r",rcvstat);
        	rcvstat = regread(dm9000,DM9000_REG_ROCR);
        	printk("ROCR :0X%02x\n\r",rcvstat);


    		__dm9000Reset(dm9000);
    		printk("%s:dm9000a reset for rcv overflow--%08d times\n\r",\
    				__FUNCTION__,times++);
        }
    }
    return 0;
}
u32  __dm9000Isr(ptu32_t para)
{
	tagDm9000Dev *dm9000;
    u8 IntStatus;
    dm9000 = (tagDm9000Dev *)para;

    regwrite(dm9000,DM9000_REG_IMR , DM9000_IMR_OFF); //�ر� DM9000A �ж�

    IntStatus = regread(dm9000,DM9000_REG_ISR);   //ȡ���жϲ���ֵ
    regwrite(dm9000,DM9000_REG_ISR,IntStatus);    //���ж�
    if(IntStatus & ISR_PRS)                       //�����ж�
    {
        Lock_SempPost(&dm9000->rcvsync);
    }
    dm9000->clearextint(dm9000->irqno);               //���CPU�жϱ�־

    regwrite(dm9000,DM9000_REG_IMR , DM9000_IMR_SET); // ���� DM9000A �ж�

    return 0;
}


static bool_t __dm9000CreateDev(tagDm9000Dev *dm9000)
{
	bool_t res = false;
    u16 evttID;
    u16 eventID;
    tagNetDevPara  devpara;

    Lock_MutexCreate_s(&dm9000->devsync,NULL);
    Lock_SempCreate_s(&dm9000->rcvsync,1,0,CN_SEMP_BLOCK_FIFO,NULL);
    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
    						(ptu32_t (*)(void))dm9000Rcv,NULL, 0x1000, dm9000->devname);
    if(evttID == CN_EVTT_ID_INVALID)
    {
    	goto EVTT_FAILED;
    }

    eventID=Djy_EventPop(evttID, NULL, 0, (ptu32_t)dm9000, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
    	goto EVENT_FAILED;
    }
    devpara.ifsend = __dm9000Snd;
    devpara.iftype = EN_LINK_ETHERNET;
    devpara.devfunc = 0x00;//COULD DO NO CRC
    memcpy(devpara.mac, dm9000->devmac,CN_MACADDR_LEN);
    devpara.name = dm9000->devname;
    devpara.private = 0;
    devpara.mtu = 14;
    devpara.private = (ptu32_t)dm9000;
    dm9000->handle = NetDevInstall(&devpara);

    if(0 == dm9000->handle)
    {
    	goto DEV_FAILED;
    }

    //OK,NOW INSTALL THE INTERRUPT
    Int_Register(dm9000->irqno);
    Int_IsrConnect(dm9000->irqno,__dm9000Isr);
    Int_SetClearType(dm9000->irqno,CN_INT_CLEAR_AUTO);
    Int_SettoAsynSignal(dm9000->irqno);
    Int_ClearLine(dm9000->irqno);
    Int_SetIsrPara(dm9000->irqno,(ptu32_t)dm9000);
    Int_RestoreAsynLine(dm9000->irqno);

    res = true;
    return res;

DEV_FAILED:
    //do the unpop
EVENT_FAILED:
	Djy_EvttUnregist(evttID);
EVTT_FAILED:
	Lock_MutexDelete_s(&dm9000->devsync);
	Lock_SempDelete_s(&dm9000->rcvsync);
    return res;
}


static tagDm9000Dev *pDm9000;
//THIS FUNCTION USED BY USER TO INSTALL AN DM9000 DEV WITH THE SPECIFIED NAME AND MAC
bool_t Dm9000Install(tagDm9000Para *para)
{
	bool_t res = false;
    tagDm9000Dev *dm9000;

    dm9000 = malloc(sizeof(tagDm9000Dev));
    if(NULL == dm9000)
    {
    	DM9000_DBG("%s:mem failed\n\r",__FUNCTION__);
    	goto MEM_FAILED;
    }

    memset((void *)dm9000,0,sizeof(tagDm9000Dev));
    //cpy from the para
    memcpy((void *)dm9000->devname,(void *)para->devname,CN_DM9000NAME_LEN);
    memcpy((void *)dm9000->devmac,(void *)para->mac,CN_MACADDR_LEN);
    dm9000->cmdaddr = para->cmdaddr;
    dm9000->dataddr = para->dataddr;
    dm9000->irqno = para->irqno;
    dm9000->clearextint = para->clearextint;

    //OK, NOW PROBE THE DM9000A DEVICE
    res = __dm9000Probe(dm9000);
    if(false == res)
    {
    	DM9000_DBG("%s:probe failed\n\r",__FUNCTION__);
    	goto PROBE_FAILED;
    }

    //OK,NOW HARD INITIALIZE
    __dm9000HardInit(dm9000);

    //NOW REGISTER IT FOR THE NETSTACK
    res = __dm9000CreateDev(dm9000);
    if(false == res)
    {
    	DM9000_DBG("%s:Creaet Dm9000 Dev failed\n\r",__FUNCTION__);
    	goto DEV_FAILED;
    }
	DM9000_DBG("%s:ISNTALL DM9000 DEV SUCCESS\n\r",__FUNCTION__);
	pDm9000 = dm9000;
	__showDm9000Reg(dm9000);
    return res;


DEV_FAILED:
PROBE_FAILED:
	free((void *)dm9000);
	dm9000 = NULL;

MEM_FAILED:
	return res;
}

bool_t shellDm9000Reg(char *param)
{
	__showDm9000Reg(pDm9000);
	return true;
}



