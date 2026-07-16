/*
 * bsp_led.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"

/**************************************************************************
 * power_on_led(void)
 * 功能:开启所有LED
 * 参数:无
 * 返回值:无
 ************************************************************************/
void power_on_led(void)
{
    LED_POWER_ON();
    LED_TAPE_CTL_ON();//WT.EDIT 2026-07-16
    LED_AI_ON();
    LED_WIFI_ON();
    LED_MOUSE_ON();
    LED_DRY_ON();
    LED_PLASMA_ON();
	LED_TEMP_ICON_ON();
   // LED_HUM_ICON_ON();
    
}

void power_on_smart_app_led(void)
{
    LED_POWER_ON();
    LED_TAPE_CTL_ON();
    LED_AI_ON();
    LED_WIFI_ON();
    LED_TEMP_ICON_ON();
    disp_all_sumg_led() ;
	//DHT11_Display_Data(0); //display temperature value 
   
}

/**************************************************************************
 * power_off_led(void)
 * 功能:关闭所有LED
 * 参数:无
 * 返回值:无
 ************************************************************************/
void power_off_led(void)
{
    //LED_POWER_OFF();
    LED_TAPE_CTL_OFF();
    LED_AI_OFF();
    LED_WIFI_OFF();
    LED_TEMP_ICON_OFF();
    LED_DRY_OFF();
    LED_PLASMA_OFF();
    LED_HUM_ICON_OFF();
    LED_MOUSE_OFF();
	TEMP_ICON_OFF() ;
	HUMIDITY_ICON_OFF();
}
/************************************************************************
 * LED_Power_Breathing(void)
 * 功能:LED呼吸灯函数
 * 参数:无
 * 返回值:无
 ************************************************************************/
void LED_Power_Breathing(void)
{
    static uint8_t breathing_time = 0;
    static uint8_t led_state = 0;
    
    breathing_time++;
    if(breathing_time > 39) //69//80 // 呼吸周期1秒
    {
        breathing_time = 0;
        led_state = !led_state;
        
        if(led_state)
        {
            LED_POWER_ON();
        }
        else
        {
            LED_POWER_OFF();
        }
    }
}
/************************************************************************
 * Function Name: LED_Power_Breathing(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/

/************************************************************************
 * Function Name: void wifi_led_fast_blink_handler(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/
void wifi_led_fast_blink_handler(void)
{
    static uint8_t c1scnt=0; //100ms
    if(g_pro.gpower_on==power_on &&g_wifi.wifi_led_fast_blink_flag==1 && g_wifi.gwifi_link_net_success==0){

           LED_WIFI_TOGGLE();
	  	
	}
	else if(g_pro.gpower_on==power_on && g_wifi.gwifi_link_net_success ==0 && g_wifi.wifi_led_fast_blink_flag==0){
             if(++c1scnt > 9){ //10ms * 100 =1s.
			 	c1scnt = 0;
                LED_WIFI_TOGGLE();
             }
     }
    else if(g_pro.gpower_on==power_off){

	           if(++c1scnt > 9){ //10ms * 100 =1s.
				  c1scnt = 0;
				  LED_POWER_TOGGLE();
			   }


	}
}

void led_wifi_connected_net(void)
{
   if(g_wifi.gwifi_link_net_success==1 && g_wifi.wifi_led_fast_blink_flag !=1){

		   LED_WIFI_ON();
 }
}

void wifi_led_slowly_blink(void)
{
 
    if(g_pro.gTimer_wifi_slowly_blink > 1)  // 1.5s // blink 周期1秒 =50 =3s
    {
        g_pro.gTimer_wifi_slowly_blink = 0;
       
        LED_WIFI_TOGGLE();
        
    }
}





























