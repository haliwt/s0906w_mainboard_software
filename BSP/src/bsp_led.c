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
    LED_TAPE_CTL_ON();
    LED_AI_ON();
    LED_WIFI_ON();
    LED_TEMP_ICON_ON();
    LED_DRY_ON();
    LED_PLASMA_ON();
    LED_HUM_ICON_ON();
    LED_MOUSE_ON();
    LED_FUN_ON();
	//TEMP_ICON_ON() ;
	//HUMIDITY_ICON_ON();
}

void power_on_smart_app_led(void)
{
    LED_POWER_ON();
    LED_TAPE_CTL_ON();
    LED_AI_ON();
    LED_WIFI_ON();
    LED_TEMP_ICON_ON();
	LED_FUN_ON();
	DHT11_Display_Data(0); //display temperature value 
   
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
    
    if(g_pro.gTimer_send_dht11_disp >= 1){
       g_pro.gTimer_send_dht11_disp =0;
            LED_POWER_TOGGLE();

    }
      
}
/************************************************************************
 * Function Name: LED_Power_Breathing(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/
void LED_FUN_ON(void)
{
  //smg 

   TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L,0xFF); // 
   TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L,0xFF); // 
   TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L,0xFF); //

   //streamlinght led
   TM1639_Write_Digit_Full(TM1639_ADDR_GRID4_H, TM1639_ADDR_GRID4_L,0xFF); //
   TM1639_Write_Digit_Full(TM1639_ADDR_GRID5_H, TM1639_ADDR_GRID5_L,0xFF); //
   TM1639_Write_Digit_Full(TM1639_ADDR_GRID6_H, TM1639_ADDR_GRID6_L,0xFF); //
   TM1639_Write_Digit_Full(TM1639_ADDR_GRID7_H, TM1639_ADDR_GRID7_L,0xFF); //

   TM1639_Write_Digit_Full(TM1639_ADDR_GRID8_H, TM1639_ADDR_GRID8_L,0xFF); //

   //key led
   
  // TM1639_Write_Digit_Full(TM1639_ADDR_GRID8_H, TM1639_ADDR_GRID8_L,0xCF); // 0xCF :

   TM1639_Display_ON_OFF(1);

}
/************************************************************************
 * Function Name: void wifi_led_fast_blink_handler(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/
 void wifi_led_fast_blink_handler(void)
{
  
   volatile static uint8_t slowly_counter=0;
   if(g_pro.gpower_on==power_on && g_wifi.wifi_led_fast_blink_flag==1 && g_wifi.gwifi_link_net_success==0){
	  if(++slowly_counter > 15){
					slowly_counter =0;
           LED_WIFI_TOGGLE();
	  	}
   }
   else if(g_pro.gpower_on==power_on){

      if(g_wifi.gwifi_link_net_success==1){
         
			  LED_WIFI_ON();
            
	  }
	  else if(g_wifi.gwifi_link_net_success ==0 ){
             if(++slowly_counter > 100){
			 	slowly_counter =0;
			    LED_WIFI_TOGGLE();  
             }
	   }
    }
	   
}

  
	


#if 0
void wifi_led_slowly_blink(void)
{

    if(g_wifi.gwifi_link_net_success==1){

       		LED_WIFI_ON();
	}
	else if(g_wifi.gwifi_link_net_success ==0){

             wifi_led_slowly_blink();
     }
   	}

	if(g_pro.led_wiif_on_f == 0){
		g_pro.led_wiif_on_f++;
        LED_WIFI_ON();
		g_pro.gTimer_wifi_slowly_blink=0;

	}
	else if(g_pro.led_wiif_on_f==1 && g_pro.gTimer_wifi_slowly_blink > 1){
		 g_pro.gTimer_wifi_slowly_blink =0;
	      g_pro.led_wiif_on_f++;
	      LED_WIFI_ON();
	}
    else if(g_pro.gTimer_wifi_slowly_blink >= 1 && g_pro.led_wiif_on_f==2)  // 1.5s // blink 周期1秒 =50 =3s
    {
        g_pro.gTimer_wifi_slowly_blink = 0;
       
        LED_WIFI_TOGGLE();
        
    }
}





#endif 























