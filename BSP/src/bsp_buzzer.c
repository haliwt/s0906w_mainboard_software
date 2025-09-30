/*
 * bsp_buzzer.c
 *
 *  Created on: 2025å¹?3æœ?4æ—?
 *      Author: Administrator
 */
#include "bsp.h"


static void tim3_buzzer_sound_config(void)
{
    LL_TIM_DisableCounter(TIM3);
   LL_TIM_GenerateEvent_UPDATE(TIM3);
   LL_TIM_SetAutoReload(TIM3, 249); //1/(249+1) =0.004MHZ = 4KHZ buzzer sound 
   LL_TIM_OC_SetCompareCH4(TIM3, 125);
   LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
   LL_TIM_EnableCounter(TIM3);
}

static void tim3_stop_buzzer_sound_config(void)
{
   LL_TIM_DisableCounter(TIM3);
   LL_TIM_CC_DisableChannel(TIM3,LL_TIM_CHANNEL_CH4);
   LL_TIM_OC_SetCompareCH4(TIM3,0);


}


void buzzer_sound(void)
{
	tim3_buzzer_sound_config();//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
    vTaskDelay(pdMS_TO_TICKS(20));//osDelay(20);
   tim3_stop_buzzer_sound_config();// HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);


}



/*********************************************************************************************************
*	Function Name: MODH_Poll
*	Function: ???????. 1ms ?????
*	Input Ref: ?
*	Return Ref: 0 ????? 1????????
*********************************************************************************************************/

void Buzzer_Fan_Error_Sound(void)
{
   
 
    buzzer_sound();
	osDelay(100);//HAL_Delay(100);
	  buzzer_sound();
	osDelay(100);//HAL_Delay(100);
	  buzzer_sound();
	osDelay(100);//HAL_Delay(100);
	  buzzer_sound();
	osDelay(100);//HAL_Delay(100);

}

void Buzzer_Ptc_Error_Sound(void)
{

	  buzzer_sound();
	 vTaskDelay(pdMS_TO_TICKS(50));//LL_mDelay() ;//HAL_Delay(50);
	  buzzer_sound();
	 vTaskDelay(pdMS_TO_TICKS(50));//HAL_Delay(50);
	  buzzer_sound();
	 vTaskDelay(pdMS_TO_TICKS(50));//HAL_Delay(50);
	  buzzer_sound();
	 vTaskDelay(pdMS_TO_TICKS(50));//HAL_Delay(50);
	  buzzer_sound();
	 vTaskDelay(pdMS_TO_TICKS(50));//HAL_Delay(50);


}


