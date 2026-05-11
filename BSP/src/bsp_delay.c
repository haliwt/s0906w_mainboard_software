/*
 * bsp_delay.c
 *
 *  Created on: 2025年3月5日
 *
 *    Author: Administrator
 */
#include "bsp.h"
#include "core_cm0plus.h"


void TIM17_Init_1MHz(void)
{
    RCC->APBENR2 |= RCC_APBENR2_TIM17EN;

    TIM16->PSC = (SystemCoreClock / 1000000) - 1;   // 64MHz → 1MHz
    TIM16->ARR = 0xFFFF;
    TIM16->CR1 |= TIM_CR1_CEN;
}


/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率, 即CPU频率(HCLK), 72Mhz
 * @retval    无
 */
void delay_init(void)
{
//	RCC->APBENR2 |= RCC_APBENR2_TIM17EN;
	
//	  TIM16->PSC = (SystemCoreClock / 1000000) - 1;   // 64MHz → 1MHz
//	  TIM16->ARR = 0xFFFF;
//	  TIM16->CR1 |= TIM_CR1_CEN;

}
//static inline void delay_us_dht11(uint32_t us)
//{
//    while (us--) {
//        for (volatile uint32_t i = 0; i < 25; i++) {
//            __NOP();
			
//        }
//    }
//}


/**
 * @brief     延时nus
 * @note      无论是否使用OS, 都是用时钟摘取法来做us延时
 * @param     nus: 要延时的us数
 * @note      nus取值范围: 0 ~ (2^32 / fac_us) (fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_us(uint32_t nus)
{
        // 1. 获取当前频率下的 1us tick 数
		// G030 频率 64MHz 时，ticksPerUs = 64
		uint32_t ticksPerUs = SystemCoreClock / 1000000;
		uint32_t ticks = nus * ticksPerUs;
		
		uint32_t reload = SysTick->LOAD;
		uint32_t start = SysTick->VAL;
		uint32_t elapsed = 0;
		uint32_t now;
	
		// 2. 阻塞等待，直到累积的 ticks 达到目标值
		while (elapsed < ticks)
		{
			now = SysTick->VAL;
			
			if (now <= start)
			{
				// 正常向下递减计数
				elapsed += (start - now);
			}
			else
			{
				// 此时发生了 SysTick 重装载（可能是 ThreadX 的心跳中断触发了）
				// 跨越零点流逝掉的 ticks = 当前值到0的距离 + 重装载值到新值的距离
				// 简化公式：elapsed += start + (reload - now);
				//elapsed += (start + (reload - now));
				
			    elapsed += (start + (reload + 1 - now)); 
			}
			start = now;
	
			// 3. 安全兜底：如果 nus 输入过大导致逻辑错误，防止死循环
			// 如果延时超过了 100ms，建议检查代码逻辑是否应改用 tx_thread_sleep
			//if (elapsed > (SystemCoreClock / 10)) break; 
			 
		}
      
		
	
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


