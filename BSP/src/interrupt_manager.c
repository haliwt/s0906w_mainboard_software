/*
 * interrupt_manager.c
 *
 *  Created on: 2025�?3�?4�?
 *      Author: Administrator
 */
#include "bsp.h"

#define MAX_TIMERS   20   // 根据需要调整,定时器


// 通用定时器回调函数类型
typedef void (*TimerCallback)(void);


static TimerCallback tim_callbacks[MAX_TIMERS] = {0};

static void tim17_isr_callback_handler(void);


//static TimerCallback tim_callbacks[MAX_TIMERS] = {0};


// 注册接口
//void tim_register_callback(uint8_t tim_id, TimerCallback cb);

// 注册回调
void tim_register_callback(uint8_t tim_id, TimerCallback cb) 
{
    if (tim_id < MAX_TIMERS) {
        tim_callbacks[tim_id] = cb;
    }
}

// ISR 调用时触发
void tim_invoke_callback(uint8_t tim_id) 
{
    if (tim_id < MAX_TIMERS && tim_callbacks[tim_id]) {
        tim_callbacks[tim_id]();
    }
}


// 注册回调函数
void callback_register_fun(void)
{
	 // 注册回调函数
    //tim_register_callback(16, tim16_handler);
    tim_register_callback(17, tim17_isr_callback_handler);


}


/********************************************************************************
	**
	*Function Name:
	*Function : 回调函数实现的逻辑
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
static void tim17_isr_callback_handler(void)

{
   static  uint16_t tm0;
       tm0++;
	  
	   g_pro.gTimer_led_wifi_bilnk_counter++;
	   if(tm0> 999){ //1s
	      tm0=0;
		   g_pro.gTimer_disp_time_second ++;
		   g_pro.gTimer_timer_time_second ++ ;
		   g_pro.gTimer_switch_temp_hum ++;
	
		   g_pro.gTimer_switch_set_timer_times++;
		   g_pro.gTimer_input_set_temp_times++;
		   g_pro.gTimer_fan_run_one_minute++;
		   g_pro.gTimer_input_set_temp_timer++;
		   g_pro.gTimer_two_hours_counter++;
		   g_pro.gTimer_disp_temp_humidity_vlaue++;
		   g_pro.gTimer_display_adc_value++;
		   g_pro.gTimer_wifi_slowly_blink ++;
		   g_pro.gTimer_send_dht11_disp++;
		  g_pro.gTimer_set_temp_counter++;
		  g_pro.gTimer_led_wifi_bilnk_counter++;
		  g_pro.gTimer_key_long_counter++;
		  g_pro.gTimer_to_disp_counter++;
		  g_pro.gTimer_mainboard_fun_counter++;
		   
		  

		   //wifi timer 
		   
	       g_wifi.gTimer_wifi_led_fast_blink ++ ;
		   g_wifi.gTimer_get_data_from_tencent_data++; 
		   g_wifi.gTimer_auto_link_net_time++;
		   g_wifi.gTimer_auto_detected_net_state_times++;
		   g_wifi.gTimer_link_net_timer_time++;
		   g_wifi.gTimer_get_beijing_time++;
		   g_wifi.gTimer_update_dht11_data++;
		

   

	 
	   

   }
}

