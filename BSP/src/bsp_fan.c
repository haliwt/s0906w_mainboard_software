/*
 * bsp_fan.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"




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

  #if 0
	//  MX_TIM16_Init();
 //  HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    LL_TIM_DisableCounter(TIM16);
    LL_TIM_OC_SetMode(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_SetAutoReload(TIM16,2559);//PWM = 1/(39+1)MHZ = 0.025MHZ = 25KHZ.
	LL_TIM_OC_SetCompareCH1(TIM16,levelval); //pwm duty = 20/40 =50%
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1);
	
	LL_TIM_EnableARRPreload(TIM16);   // 建议加上，避免更新不同步
	  // 4. ✅【关键添加】使能高级定时器的主输出
    LL_TIM_EnableAllOutputs(TIM16); // 此函数会设置BDTR寄存器的MOE位
	LL_TIM_EnableCounter(TIM16);
	LL_TIM_CC_EnableChannel(TIM16,LL_TIM_CHANNEL_CH1);
	#else 
	   
	
		// 1. 写入初始占空比
	LL_TIM_OC_SetCompareCH1(TIM16, 2048);

	// 2. 开启通道预装载与 ARR 预装载
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1); 
	LL_TIM_EnableARRPreload(TIM16); 

	// 3. 使能通道（使能输出控制电路）
	LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);

	// 4. 开启高级定时器的主输出（MOE 必须在产生更新前开启）
	LL_TIM_EnableAllOutputs(TIM16); 

	// 5. ⭐ 核心：强行产生一次更新，把上面步骤 1 写入的值，从 Preload 寄存器立刻强行刷入到工作寄存器中
	LL_TIM_GenerateEvent_UPDATE(TIM16); 

	// 6. ⭐ 核心：立刻清除更新事件产生的标志位，防止卡死在中断中
	LL_TIM_ClearFlag_UPDATE(TIM16); 

	// 7. 启动计数器，开始输出 PWM
	LL_TIM_EnableCounter(TIM16);
	 
	

	#endif 
	
	
}

void fan_adjust_speed_middle(void)
{
  	// 1. 写入初始占空比
	LL_TIM_OC_SetCompareCH1(TIM16,1792);

	// 2. 开启通道预装载与 ARR 预装载
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1); 
	LL_TIM_EnableARRPreload(TIM16); 

	// 3. 使能通道（使能输出控制电路）
	LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);

	// 4. 开启高级定时器的主输出（MOE 必须在产生更新前开启）
	LL_TIM_EnableAllOutputs(TIM16); 

	// 5. ⭐ 核心：强行产生一次更新，把上面步骤 1 写入的值，从 Preload 寄存器立刻强行刷入到工作寄存器中
	LL_TIM_GenerateEvent_UPDATE(TIM16); 

	// 6. ⭐ 核心：立刻清除更新事件产生的标志位，防止卡死在中断中
	LL_TIM_ClearFlag_UPDATE(TIM16); 

	// 7. 启动计数器，开始输出 PWM
	LL_TIM_EnableCounter(TIM16);
	 
}

void fan_adjust_speed_high(void)
{
  	// 1. 写入初始占空比
	LL_TIM_OC_SetCompareCH1(TIM16, 2560);

	// 2. 开启通道预装载与 ARR 预装载
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1); 
	LL_TIM_EnableARRPreload(TIM16); 

	// 3. 使能通道（使能输出控制电路）
	LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);

	// 4. 开启高级定时器的主输出（MOE 必须在产生更新前开启）
	LL_TIM_EnableAllOutputs(TIM16); 

	// 5. ⭐ 核心：强行产生一次更新，把上面步骤 1 写入的值，从 Preload 寄存器立刻强行刷入到工作寄存器中
	LL_TIM_GenerateEvent_UPDATE(TIM16); 

	// 6. ⭐ 核心：立刻清除更新事件产生的标志位，防止卡死在中断中
	LL_TIM_ClearFlag_UPDATE(TIM16); 

	// 7. 启动计数器，开始输出 PWM
	LL_TIM_EnableCounter(TIM16);
	 
}



void fan_adjust_speed_low(void)
{
  	// 1. 写入初始占空比
	LL_TIM_OC_SetCompareCH1(TIM16, 768);

	// 2. 开启通道预装载与 ARR 预装载
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1); 
	LL_TIM_EnableARRPreload(TIM16); 

	// 3. 使能通道（使能输出控制电路）
	LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);

	// 4. 开启高级定时器的主输出（MOE 必须在产生更新前开启）
	LL_TIM_EnableAllOutputs(TIM16); 

	// 5. ⭐ 核心：强行产生一次更新，把上面步骤 1 写入的值，从 Preload 寄存器立刻强行刷入到工作寄存器中
	LL_TIM_GenerateEvent_UPDATE(TIM16); 

	// 6. ⭐ 核心：立刻清除更新事件产生的标志位，防止卡死在中断中
	LL_TIM_ClearFlag_UPDATE(TIM16); 

	// 7. 启动计数器，开始输出 PWM
	LL_TIM_EnableCounter(TIM16);
	 
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
	
    // if(one_speed != g_pro.g_fan_switch_gears_flag){
        g_pro.g_fan_switch_gears_flag++;
       /// one_speed = g_pro.g_fan_switch_gears_flag ;  //one_speed =2,5,8
	    fan_adjust_speed_low();//SetLevel_Fan_PWMA(FAN_PWM_80);

    /// }


}

void Fan_Two_Speed(void)
{
     static uint8_t two_speed=0xff;
     
     FAN_RUN_SetHigh();
      if(two_speed != g_pro.g_fan_switch_gears_flag){
         g_pro.g_fan_switch_gears_flag++;
         two_speed = g_pro.g_fan_switch_gears_flag;  //two_speed = 3;6,9
	     fan_adjust_speed_middle();//SetLevel_Fan_PWMA(FAN_PWM_90);
       }

}

 void Fan_Full_Speed(void)
{
   static uint8_t full_speed = 0xff;
     FAN_RUN_SetHigh();
    if(full_speed != g_pro.g_fan_switch_gears_flag){
        g_pro.g_fan_switch_gears_flag++;
         full_speed = g_pro.g_fan_switch_gears_flag;  //full_speed =1,4,7,10
        fan_adjust_speed_high() ;//SetLevel_Fan_PWMA(FAN_PWM_100);
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





