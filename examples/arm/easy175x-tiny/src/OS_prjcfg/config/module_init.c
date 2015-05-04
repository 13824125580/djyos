#include "os_inc.h"
#include "cpu_peri.h"

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}

//----�����ʼ��2-----------------------------------------------------------
//���ܣ���ѡ�����ʼ����������3��:
//      1��sys_module_init����sys_init.c�У��μ��ú���ע��
//      2��prj_module_init���������ʼ����ģ�飬��Ҫ���ø����幤����صĴ��룬��
//         �����ģ�飬���˵���module_init_keyboard�����⣬��Ҫ���ø�Ӳ����ص�
//         module_init_keyboard_hard������
//      ǰ������ʼ��ʱ�����������䷽ʽ��û�г�ʼ��������������õ���̬���䣬ʹ��
//      ����׼��̬���䣬����׼��̬�����˵�����μ�mems.c�ļ���
//      3��run_module_init���μ��ú���ע�͡�
//��������
//���أ���
//-----------------------------------------------------------------------------
void prj_module_init(void)
{
    uint16_t evtt_main;

#if CN_CFG_KEYBOARD == 1
    Keyboard_ModuleInit(0);
#endif

#if CN_CFG_DJYFS == 1
    Djyfs_ModuleInit(0);
#if CN_CFG_DJYFS_FLASH == 1
    DFFSD_ModuleInit();
#endif
#endif

#if(CN_CFG_UART0_USED == 1)
    module_init_uart0(0);
#endif
#if(CN_CFG_UART1_USED == 1)
    module_init_uart1(0);
#endif
#if(CN_CFG_UART2_USED == 1)
    module_init_uart2(0);
#endif
#if(CN_CFG_UART3_USED == 1)
    module_init_uart3(0);
#endif
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                __djy_main,CN_MAIN_STACK,"main function");
    //�¼���������������Ϊ0�������shell�������������ɼ�shell�����в���
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,0);
    return ;
}

//----�����ʼ��3-----------------------------------------------------------
//���ܣ���ѡ�����ʼ����������3��:
//      1��sys_module_init����sys_init.c�У��μ��ú���ע��
//      2��prj_module_init���μ��ú���ע�͡�
//      3��run_module_init����������djy_main������ʼ�����ã���Ҫ����ϵͳ���¼�
//          ���ȿ�ʼ�󣬻�����Ҫ�������ڴ�����ʼ������ܳ�ʼ����ģ�顣
//��������
//���أ���
//-----------------------------------------------------------------------------
void run_module_init(void)
{

}

