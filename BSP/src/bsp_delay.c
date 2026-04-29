/*
 * bsp_delay.c
 *
 *  Created on: 2025年3月5日
 *
 *    Author: Administrator
 */
#include "bsp.h"



/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率, 即CPU频率(HCLK), 72Mhz
 * @retval    无
 */
void delay_init(uint16_t sysclk)
{

}


/**
 * @brief     延时nus
 * @note      无论是否使用OS, 都是用时钟摘取法来做us延时
 * @param     nus: 要延时的us数
 * @note      nus取值范围: 0 ~ (2^32 / fac_us) (fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_us(uint32_t nus)
{
    #if 0
	uint32_t cycles = (SystemCoreClock / 1000000) * nus;
    while (cycles--)
    {
        __NOP();
    }

	#else 
	uint32_t start = SysTick->VAL;
	uint32_t ticks = nus * (SystemCoreClock / 1000000);	 // 64 ticks = 1us
	uint32_t reload = SysTick->LOAD;
	uint32_t now;
	uint32_t elapsed = 0;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while (elapsed < ticks)
	{
		now = SysTick->VAL;

		if (now <= start)
			elapsed += start - now; 		 // 正常递减
		else
			elapsed += start + (reload - now); // SysTick 重装载

		start = now;
	}


	#endif 

}

/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数 (0< nms <= (2^32 / fac_us / 1000))(fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_ms(uint16_t nms)
{
     delay_us((uint32_t)(nms * 1000));      

}


