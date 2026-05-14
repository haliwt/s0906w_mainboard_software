/*
 * bsp.h
 *
 *  Created on: 2025å¹?3æœ?4æ—?
 *      Author: Administrator
 */

#ifndef BSP_INC_BSP_H_
#define BSP_INC_BSP_H_
#include "main.h"
#include "stdio.h"
#include "string.h"



#include "bsp_streamlight.h"
#include "bsp_key.h"
#include "bsp_key_app.h"
#include "bsp_fan.h"
#include "bsp_buzzer.h"
#include "bsp_tm1639.h"
#include "bsp_smg.h"
#include "bsp_power.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_dht11.h"
#include "bsp_dry.h"
#include "bsp_mouse.h"
#include "bsp_plasma.h"
#include "bsp_disp.h"
#include "bsp_usart.h"
#include "bsp_dma_usart.h"
#include "bsp_message.h"
#include "bsp_cmd_link.h"
#include "bsp_led_tape.h"
#include "bsp_adc.h"
#include "bsp_iap.h"
#include "bsp_wwdg.h"
#include "bsp_threadx.h"

#include "interrupt_manager.h"

//wifi function
#include "bsp_wifi.h"
#include "bsp_esp8266.h"
#include "bsp_publish.h"
#include "bsp_subscribe.h"
#include "bsp_mqtt.h"
#include "bsp_wifi_decoder.h"
#include "bsp_dma_usart2.h"

#include "bsp_connect_wifi.h"
#include "bsp_soft_timer.h"


//second display board
#include "bsp_second_display.h"


//mainboard

#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "wwdg.h"

//Threadx
#include "app_threadx.h"








#define USE_THREADX       1  // ??? ThreadX

#define Enable_EventRecorder          0
  
#define TEST_UNIT         1

#define DEBUG_ENABLE       0 //Enable_EventRecorder 


#if USE_THREADX == 1
    #include "tx_api.h"
    
    /* ThreadX ?????????????????? */
    //extern UINT old_post; 
    #define DISABLE_INT()   UINT __old_post = tx_interrupt_control(TX_INT_DISABLE)
    #define ENABLE_INT()     tx_interrupt_control(__old_post)
#else
    /* ??? RTOS ?????????? */
    #define ENABLE_INT()    __set_PRIMASK(0)    /* ?????? */
    #define DISABLE_INT()   __set_PRIMASK(1)    /* ?????? */
#endif

#if Enable_EventRecorder == 1
	#include "EventRecorder.h"
#endif



typedef enum{
   power_off,
   power_on,
}process_state;

typedef enum{
	wifi_no_link,
	wifi_link_success,
}wifi_state;

typedef enum{
	temperature_mode,
	timer_time_mode=1,
	beijing_time_mode=2,
	disp_timer_mode=3,
	disp_temp_hum_mode=4,
	input_set_temperature_mode=5, //WT.EDIT 2025.04.23
	input_set_null=6,
	input_set_timer_mode_null=7,
	input_set_normal_time_mode=8,
	input_normal_null=9,
	disp_works_mode
	
}time_state;

typedef struct _process{

   //main board reference
   uint16_t fan_detect_voltage;
   uint16_t gTimer_two_hours_counter;
   uint8_t gpower_on;
   uint8_t gDry;
   uint8_t gPlasma;
   uint8_t gAI;
   uint8_t gMouse;
   uint8_t works_two_hours_interval_flag;
   uint8_t g_manual_shutoff_dry_flag;
   uint8_t key_add_dec_be_pressed_flag;
   uint8_t set_timing_or_timer_time_flag;

   uint8_t first_set_ptc_on;
   uint8_t first_rcoder_ptc_on_flag ;
   uint8_t one_minutes_fan_flag;
   uint8_t process_off_step;
   
   
   uint8_t g_humidity_value;
   uint8_t g_temperature_value;
   uint8_t disp_second_f ;

 

   //uint8_t key_long_power_pressed;

   uint8_t g_fan_switch_gears_flag;
   uint8_t set_temperature_success_flag;
   uint8_t key_set_temperature_flag;
   

   
   uint8_t gset_temperture_value;
   uint8_t temperature_init_value;

   uint8_t switch_disp_time_or_temp_item;
   uint8_t gdisp_hours_value;
   uint8_t gdisp_minutes_value;
   

   uint8_t g_copy_power_onoff_flag;
   uint8_t delay_run_adc_counter;


   uint8_t first_connect_wifi_flag;
   uint8_t DMA_txComplete;
   uint8_t timer_powerOff_oneHour_flag;
   uint8_t gpower_on_key_f;
   uint8_t g_immediate_heat_f;

   


   int8_t gdisp_timer_hours_value;
   int8_t  gdisp_timer_minutes_value;
   
   uint8_t ptc_warning;
   uint8_t fan_warning ;
   uint8_t led_bar ;
   uint8_t g_real_hours_counter;
   uint8_t set_temp_counter;

   //timer
   
   uint8_t  gTimer_disp_time_second;
   uint8_t  gTimer_timer_time_second;

   uint8_t  gTimer_switch_temp_hum;
   uint8_t  gTimer_switch_set_timer_times;
   uint8_t  gTimer_input_set_temp_times;
   uint8_t   gTimer_fan_run_one_minute;
   uint8_t   gTimer_input_set_temp_timer;
   uint8_t gTimer_disp_temp_humidity_vlaue;
   uint8_t gTimer_display_adc_value;

   uint8_t gTimer_send_dht11_disp;
   uint8_t gTimer_set_temp_counter;
   uint8_t time_50ms_f;


   uint8_t  gTimer_to_disp_counter;
   uint8_t gTimer_mainboard_fun_counter;
   uint8_t gTimer_read_dth11_sensor;
   uint8_t time_20ms_f;
   
}process_t;

extern process_t g_pro;



void bsp_init(void);

void mainboard_fun_handler(void);

void mainboard_close_all_fun(void);

void works_run_two_hours_state(void);

void copy_cmd_hanlder(void);

void  smart_phone_timer_power_on_handler(void);

void fault_handler(void);

#endif /* BSP_INC_BSP_H_ */
