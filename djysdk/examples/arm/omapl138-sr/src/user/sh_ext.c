/*============================================================================*
 * �ļ�ģ��˵��:                                                              *
 *   Ϊdjyos��shell��������������vxworks�Ĺ��ܣ���Ҫʹ��djyos����չ����ӿڡ� *
 * �ļ��汾: v1.00 (˵�����ļ��ĵ�ǰ�汾��Ϣ)                                 *
 * ������Ա: lst                                                           *
 * ����ʱ��:                                                                  *
 * Copyright(c) 2011-2011  ��������������޹�˾                               *
 *============================================================================*
 * �����޸ļ�¼(���µķ�����ǰ��):								              *
 *  <�汾��> <�޸�����>, <�޸���Ա>: <�޸Ĺ��ܸ���>				              *
 *===========================================================================*/
#include "os_inc.h"
#include "shell.h"
#include <string.h>
typedef struct 
{
    char name[256];
    u32 *address;
    u32 value;
}tagSymbolParse;
u32 g_u32SymOffset = 0;
char *g_cSymbolTable = (char *)0xc0380000;
extern     djy_handle_t pg_shell_lhdl;
bool_t exsh_command(char *cmd);

/************************************************************
* �������ƣ�module_init_sh_ext
* �������ܣ�djyos��׼shell�У���������vxworks�������Ҫע����չ��shell�����
*           ��ʵ�֡�
* ���������para��djyosģ���ʼ��������Ҫ��Ĳ�������������ʹ��
* �����������
* ����ֵ  ������1
************************************************************/
ptu32_t module_init_sh_ext(ptu32_t para)
{
    sh_install_sh_ext(exsh_command);
	return 1;
}
//�������У�������ֻ�пո����
u32 move_newline(u32 Offset)
{
    for(; g_cSymbolTable[Offset] != '\0'; Offset++)
    {
        if((g_cSymbolTable[Offset] == '\n') || (g_cSymbolTable[Offset] == '\r'))
            break;
    }
    for(; g_cSymbolTable[Offset] != '\0'; Offset++)
    {
        if((g_cSymbolTable[Offset] != '\n') && (g_cSymbolTable[Offset] != '\r'))
            break;
    }
    return Offset;
}

void format_name(char *name)
{
    u32 i;
    for(i=0; name[i] != '\0'; i++);
    for(; i < 32; i++)
        name[i] = ' ';
    name[i] = '\0';
}
bool_t ExshReadLine(tagSymbolParse *result)
{
    u32 i,temp;
    char hex[11];
    if(g_cSymbolTable[g_u32SymOffset] == '\0')
        return false;
    //�������׿ո�
    for(i = g_u32SymOffset; g_cSymbolTable[i] == ' '; i++);

    for(temp = 0; temp < 256; temp++)
    {
        result->name[temp] = g_cSymbolTable[i];
        i++;
        if((g_cSymbolTable[i] == '\n') || (g_cSymbolTable[i] == '\r'))
        {
            result->name[temp+1] = '\0';
            return false;       //δ��������ļ�¼��Ϣ�С�
        }
        if(g_cSymbolTable[i] == ' ')
        {
            result->name[temp+1] = '\0';
            break;
        }
    }       //��ɷ���������
    if(temp == 256)
        result->name[temp] = '\0';
    
    for(;g_cSymbolTable[i] == ' '; i++);       //�ÿ��ո�
    for(temp = 0; temp < 10; temp++)
    {
        hex[temp] = g_cSymbolTable[i];
        i++;
    }
    hex[10] = '\0';     //��ɶ�ȡ���ŵ�ַ
    result->address = djy_atol(hex);
    result->value = *(result->address);
    g_u32SymOffset = move_newline(i);

    return true;
}

//�����ʽ:
//symbol [����]��
//������ʽ:�޲�����ʾ�ӵ�ǰλ�ÿ�ʼ��ʾ10�����š�
//����Ϊ�ַ�����ʾ��ʾ��Ӧ���ţ��Ҳ����÷�������ʾ"�޴˷���"
bool_t ExshShowSymbol(char *param)
{
    u32 loop,temp;
    char *word,*next_param;
    tagSymbolParse Symbol;

    word = Sh_GetWord(param,&next_param);
    if(word == NULL)
    {
        for(loop =0; loop < 20; loop++)
        {
            if(ExshReadLine( &Symbol) == true)
            {
                format_name(Symbol.name);
                djy_printf("\r\n����: %s",Symbol.name);
                djy_printf("  ��ַ: %8x",(u32)Symbol.address);
                djy_printf("  ֵ = %8x",Symbol.value);
            }else
            {
                break;
            }
        }
        djy_printf("\r\n");
    }else
    {
        bool_t suc = false;
        temp = g_u32SymOffset;
        g_u32SymOffset = 0;
        while(g_cSymbolTable[g_u32SymOffset] != '\0')
        {
            if(ExshReadLine( &Symbol) == true)
            {
                if(strcmp(Symbol.name,word) == 0)
                {
                	djy_printf("\r\n����: %s",Symbol.name);
                	djy_printf("  ��ַ: %8x",(u32)Symbol.address);
                	djy_printf("  ֵ = %8x",Symbol.value);
                    suc = true;
                }
            }
        }
        if(suc == false)
        {
        	djy_printf("\r\nû���ҵ�����: %s",Symbol.name);
        }
        djy_printf("\r\n");
        g_u32SymOffset = temp;
    }
    return true;
}
#if 0
/************************************************************
* �������ƣ�ExshSetMemory
* �������ܣ����4~64M�ռ��ڴ棬���ڼ�鸴λ��DDRII�����ݵ�Ӱ�죬���Ĵ�ƽ̨���
*           ģʽ��Ҫ��ס��ϰ�ť��λϵͳ��Ȼ����ϳ�������ڴ棬��ϳ����ܹ�
*           ��ȷ������ǰ���ǣ���λ���������ƻ�DDRII�ڵ����ݡ�
*           ����һ�������õĴ���
* �����������
* �����������
* ����ֵ  �� ����0
************************************************************/
bool_t ExshSetMemory(void)
{
    u32 time;
    u32 i=0,*p;
    time = DjyGetTime();
    for(p = (u32*)0xc0000000; p < (u32*)0xc4000000; p++)
    {
        *p = i;
        i++;
    }
    time = DjyGetTime()-time ;
    printf_ng("\r\ntime = 0x",time);
    return true;
}

/************************************************************
* �������ƣ�ExshCheckMemory
* �������ܣ����4~64M�ռ��ڴ�������Ƿ����ExshSetMemory�������ȥ��ֵ�����ڼ�
*           �鸴λ��DDRII�����ݵ�Ӱ�죬���Ĵ�ƽ̨���
*           ģʽ��Ҫ��ס��ϰ�ť��λϵͳ��Ȼ����ϳ�������ڴ棬��ϳ����ܹ�
*           ��ȷ������ǰ���ǣ���λ���������ƻ�DDRII�ڵ����ݡ�
*           ����һ�������õĴ���
* �����������
* �����������
* ����ֵ  �� ����0
************************************************************/
bool_t ExshCheckMemory(void)
{
    u32 time;
    u32 i=0,*p;
    time = DjyGetTime();
    for(p = (u32*)0xc0000000; p < (u32*)0xc4000000; p++)
    {
        if(*p != i)
        {
            printf_ng("\r\n error address:%8x = %8x",(u32)p,*p);
            printf_ng(",right = %8x \r\n",i);
            return true;
        }
        i++;
    }
    time = DjyGetTime() - time;
    printf_ng("\r\n test pass time = %8x",time);
    return true;
}
#endif
/*
void test_at45(void)
{
    u32 page,byte,buf[128];
    volatile u32 temp;
    for(page = 0; page<4096; page+=32)
    {
        for(byte = 0; byte <128; byte++)
            buf[byte] = (page<<17) +byte;
        
        At45_U26PageErase(page);
        At45_U26WritePage(page,0,(u8*)buf,CN_U26_PAGE_SIZE);
        At45_U26ReadPage(page,0,(u8*)buf,CN_U26_PAGE_SIZE);
        for(byte = 0; byte <128; byte++)
        {
            if(buf[byte] != (page<<17) +byte)
            {
                printf_ng("\r\n page error:%4x",page);
                break;
            }
        }
    }
    printf_ng("\r\n end of testing at45");
}
*/

#define zmodem_soh     1
#define zmodem_eot     4
#define zmodem_ack     6
#define zmodem_nak     0x15
#define zmodem_can     0x18
#define zmodem_err     0xff

#define flash_end_block 4096    //end of 2M
u8 *arm_run_address = (u8*)0xc0400000;
u8 *vx_verity_address = (u8*)0xc0800000;
bool_t exsh_download_vx(void)
{
    u8 package[132],checksum,over=0;
    u8 flash_buf[512];
    u32 reads,readed,i,pack_no=1; //���Ŵ�1��ʼ����
    Djy_PutChar(zmodem_nak);
    readed = 0;
    while(1)
    {
        if(djy_evtt_pop_sync(Djy_MyEvttId(),1,1000*mS) == CN_SYNC_TIMEOUT)
        {
            if(over++ <30)
            {
                Djy_PutChar(zmodem_nak);                //��ʱ�����·���NAK
                continue;
            }else
            {
                
                printf_ng("\r\n ��ʱδ����");
                return true;
            }
        }
        reads = Driver_DevReadLeft(pg_shell_lhdl,(ptu32_t)(package+readed),
                              132-readed,CN_TIMEOUT_FOREVER);   //���豸������
        readed +=reads;
        if(readed < 132)                                //����������
        {
            if(package[0] == zmodem_eot)                //����֡
            {
                Djy_PutChar(zmodem_ack);            //������ȷ������Ӧ��
                if(pack_no%4 != 1)
                {
                    At45_U26PageErase(flash_end_block - pack_no/4 -1);
                    At45_U26WritePage(flash_end_block - pack_no/4 -1,0,
                                        flash_buf,CN_U26_PAGE_SIZE);
                }
                At45_U26ReadPage(16,0,flash_buf,CN_U26_PAGE_SIZE);
                At45_U26PageErase(16);
                ((u32*)flash_buf)[1] = (pack_no-1)*128; //vxworks�ߴ�
                At45_U26WritePage(16,0,flash_buf,CN_U26_PAGE_SIZE);
                printf_ng("\r\n �������");
                return true;
            }else
                continue;
        }
        switch(package[0])
        {
            case zmodem_soh:                            //���������
            {
                if((package[1] + package[2]) != 0xff)   //������������У�����
                {
                    Djy_PutChar(zmodem_nak);            //Ӧ��NAK�������ط�
                    break;
                }
                checksum = 0;
                for(i = 3; i<131; i++)
                    checksum += package[i];
                if(checksum != package[131])            //У��ʹ�
                {
                    Djy_PutChar(zmodem_nak);            //Ӧ��NAK�������ط�
                    break;
                }
                Djy_PutChar(zmodem_ack);            //������ȷ������Ӧ��
                if(package[1] != (pack_no & 0xff))    //�ظ�������Ŵ��󣬶���
                    break;
                memcpy(&flash_buf[(pack_no-1)%4*128],package+3,128);
//                memcpy(&arm_run_address[(pack_no-1)*128],package+3,128);
                readed = 0;
                if(pack_no%4 == 0)                      //������һ��flash page
                {
                    At45_U26PageErase(flash_end_block - pack_no/4);
                    At45_U26WritePage(flash_end_block - pack_no/4,0,
                                        flash_buf,CN_U26_PAGE_SIZE);
//					At45U26ReadPage(flash_end_block - pack_no/4,0,verity,CN_U26_PAGE_SIZE);
//					for(i=0;i<512;i++)
//					{
//						if(flash_buf[i] != verity[i])
//							i = i;
//					}
                }
                pack_no++;
            }break;
            default:break;
        }
    }
}
bool_t exsh_run_arm(void)
{
    u32 code_size,page,code_start_page;
    code_start_page = 0x200000/512-1;
    At45_U26ReadPage(16,4,(u8*)&code_size,4);
    for(page = 0; page*512 < code_size; page++)
        At45_U26ReadPage(code_start_page-page,0,
                        (u8*)(arm_run_address+page*512),CN_U26_PAGE_SIZE);
    ((void(*)(void))arm_run_address)();
    return true;
}

bool_t exsh_command(char *cmd)
{
    bool_t result;
    char *param;
    if(Sh_CmdParser(cmd,"symbol",&param)==true)
        result = ExshShowSymbol(param);
    else if(Sh_CmdParser(cmd,"load",&param)==true)
        result = exsh_download_vx();
    else if(Sh_CmdParser(cmd,"verity",&param)==true)
        result = exsh_verity_vx();
    else if(Sh_CmdParser(cmd,"run",&param)==true)
        result = exsh_run_vx();
    else
    {
        return false;
    } 
    return result;
}

