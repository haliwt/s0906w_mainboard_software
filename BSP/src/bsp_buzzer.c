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
   #if 0
	tim3_buzzer_sound_config();//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
    tx_thread_sleep(2);//tx_thread_sleep(20);
    //open_beep_sound();//tim3_stop_buzzer_sound_config();// HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
	tim3_stop_buzzer_sound_config();
   #else 
   tim3_stop_buzzer_sound_config();
   
	tim3_buzzer_sound_config();//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	//tx_close_beep();
    tx_set_once_timer();
		
	open_beep_sound();//tim3_stop_buzzer_sound_config();// HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
		

   #endif 

}

void buzzer_sound_close(void)
{
	tim3_stop_buzzer_sound_config();
	


}

void buzzer_power_sound(void)
{
	tim3_buzzer_sound_config();//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
    tx_thread_sleep(2);//tx_thread_sleep(20);
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
	static uint8_t beep_step = 0;
	static ULONG g_beep_err_timeout = 0;
	static uint8_t beep_count = 0;
	

    buzzer_sound();
	tx_thread_sleep(1);//HAL_Delay(100);
	  buzzer_sound();
	tx_thread_sleep(1);//HAL_Delay(100);
	  buzzer_sound();
	tx_thread_sleep(1);//HAL_Delay(100);
	  buzzer_sound();
	tx_thread_sleep(1);//HAL_Delay(100);

}

void Buzzer_Ptc_Error_Sound(void)
{

	  buzzer_sound();
	 tx_thread_sleep((5));//LL_mDelay() ;//HAL_Delay(50);
	  buzzer_sound();
	 tx_thread_sleep((5));//HAL_Delay(50);
	  buzzer_sound();
	 tx_thread_sleep((5));//HAL_Delay(50);
	  buzzer_sound();
	 tx_thread_sleep((5));//HAL_Delay(50);
	  buzzer_sound();
	 tx_thread_sleep((5));//HAL_Delay(50);


}


