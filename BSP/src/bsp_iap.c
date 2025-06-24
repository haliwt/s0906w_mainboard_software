#include "bsp.h"


#define FLASH_USER_START_ADDR  0x08002000  // 应用程序起始地址(8KB后)
#define FLASH_PAGE_SIZE        0x800       // 2KB页大小

/*
*********************************************************************************************************
*	函 数 名: JumpToBootloader
*	功能说明: 跳转到系统BootLoader
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void JumpToBootloader(void)
{
	uint32_t i=0;
	void (*SysMemBootJump)(void);        /* 声明一个函数指针 */
	__IO uint32_t BootAddr = 0x1FFF0000; /* STM32F4的系统BootLoader地址 */

	/* 关闭全局中断 */
	DISABLE_INT(); 

	/* 关闭滴答定时器，复位到默认值 */
	SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

	/* 设置所有时钟到默认状态，使用HSI时钟 */
	HAL_RCC_DeInit();

	/* 关闭所有中断，清除所有中断挂起标志 */
	for (i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}	

	/* 使能全局中断 */
	ENABLE_INT();

	/* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
	SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

	/* 设置主堆栈指针 */
	__set_MSP(*(uint32_t *)BootAddr);
	
	/* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
	__set_CONTROL(0);

	/* 跳转到系统BootLoader */
	SysMemBootJump(); 

	/* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
	{

	}
}

