#include "bsp.h"



void IWDG_Refresh(void)
{
     LL_IWDG_ReloadCounter(IWDG); // 至少每 < 1s 调用一次
}

