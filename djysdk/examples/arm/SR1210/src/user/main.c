#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "os.h"

void djy_main(void)
{
    printf("%s:��ӭʹ��SR1210���\r\n",__FUNCTION__);
    for ( ; ; )
    {
        Djy_EventDelay(1000*mS);
    }
}
