/*
 * bsp_fan.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"




#define FAN_PWM_100     40

#define FAN_PWM_90      36

#define FAN_PWM_80      32
static void tim16_stop_fan_pmw_config(void);


/********************************************************
*
*Function Name:void SetLevel_Fan_PWMA(uint8_t levelval)
*Function: 
*
*
********************************************************/
void SetLevel_Fan_PWMA(uint8_t levelval)
{
	//  MX_TIM16_Init();
 //  HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    LL_TIM_DisableCounter(TIM16);
    LL_TIM_OC_SetMode(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_SetAutoReload(TIM16,39);//PWM = 1/(39+1)MHZ = 0.025MHZ = 25KHZ.
	LL_TIM_OC_SetCompareCH1(TIM16,levelval); //pwm duty = 20/40 =50%
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1);
	
	LL_TIM_EnableARRPreload(TIM16);   // 建议加上，避免更新不同步
	  // 4. ✅【关键添加】使能高级定时器的主输出
    LL_TIM_EnableAllOutputs(TIM16); // 此函数会设置BDTR寄存器的MOE位
	LL_TIM_EnableCounter(TIM16);
	LL_TIM_CC_EnableChannel(TIM16,LL_TIM_CHANNEL_CH1);
	
	
}

static void tim16_stop_fan_pmw_config(void)
{
  LL_TIM_DisableCounter(TIM16);
  LL_TIM_CC_DisableChannel(TIM16,LL_TIM_CHANNEL_CH1);


}



void FAN_Stop(void)
{
  
   FAN_RUN_SetLow();//SetLevel_Fan_PWMA(0);//SetLevel_Fan_PWMA(16);
  // SetLevel_Fan_PWMA(0);
  tim16_stop_fan_pmw_config();
}



void Fan_One_Speed(void)
{
   static uint8_t one_speed=0xff;
	
     FAN_RUN_SetHigh();
     if(one_speed != g_pro.g_fan_switch_gears_flag){
        g_pro.g_fan_switch_gears_flag++;
        one_speed = g_pro.g_fan_switch_gears_flag ;  //one_speed =2,5,8
	    SetLevel_Fan_PWMA(FAN_PWM_80);

     }


}

void Fan_Two_Speed(void)
{
     static uint8_t two_speed=0xff;
     
     FAN_RUN_SetHigh();
      if(two_speed != g_pro.g_fan_switch_gears_flag){
         g_pro.g_fan_switch_gears_flag++;
         two_speed = g_pro.g_fan_switch_gears_flag;  //two_speed = 3;6,9
	     SetLevel_Fan_PWMA(FAN_PWM_90);
       }

}

 void Fan_Full_Speed(void)
{
   static uint8_t full_speed = 0xff;
     FAN_RUN_SetHigh();
    if(full_speed != g_pro.g_fan_switch_gears_flag){
        g_pro.g_fan_switch_gears_flag++;
         full_speed = g_pro.g_fan_switch_gears_flag;  //full_speed =1,4,7,10
         SetLevel_Fan_PWMA(FAN_PWM_100);
    }

}



void Fan_RunSpeed_Fun(void)
{

    if(g_wifi.set_wind_speed_value < 34 ){
        Fan_One_Speed();
    }
    else if(g_wifi.set_wind_speed_value > 33  && g_wifi.set_wind_speed_value < 67 ){

        Fan_Two_Speed();

    }
    else if(g_wifi.set_wind_speed_value > 66){

         Fan_Full_Speed();

    }

      

}

/**
*@brief
*@note
*@param
*@retval
*/





