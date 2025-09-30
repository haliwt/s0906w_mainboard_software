/*
 * bsp_mouse.c
 *
 *  Created on: 2025年3月7日
 *      Author: Administrator
 */
#include "bsp.h"

static void tim1_ultr_pwm_config(void)
{
    LL_TIM_DisableCounter(TIM1);
	LL_TIM_SetAutoReload(TIM1,39);//PWM = 1/(39+1)MHZ = 0.025MHZ = 25KHZ.
	LL_TIM_OC_SetCompareCH1(TIM1,20); //pwm duty = 20/40 =50%
	LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH1);
	
	LL_TIM_EnableAllOutputs(TIM1); // 等价于 TIM1->BDTR |= TIM_BDTR_MOE;
	LL_TIM_EnableCounter(TIM1);

}

static void tim1_stop_ultr_pmw_config(void)
{
  LL_TIM_DisableCounter(TIM1);
  LL_TIM_CC_DisableChannel(TIM1,LL_TIM_CHANNEL_CH1);


}


void mouse_open(void)
{
  tim1_ultr_pwm_config(); // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic On
}
void mouse_close(void)
{
  tim1_stop_ultr_pmw_config(); // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off
}



