/*
 * bsp_disp.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"

#define SWITCH_THRESHOLD 2

uint8_t disp_temp_hum;

void display_digital_3_numbers(void)
{
    
	 static uint8_t read_error_flag,switch_adc;

	// If any warning is active, do nothing
    if (g_pro.fan_warning || g_pro.ptc_warning){
        return;
    }
	

	  	
     if(g_pro.key_set_temperature_flag == 1){//if(g_pro.key_set_temperature_flag==1 && g_key.mode_key_switch_time_mode != timer_time_mode){

          TM1639_Display_Temperature(g_pro.gset_temperture_value);
          return ;

     }

     switch(g_pro.key_gtime_timer_define_state){


	 case timer_time_mode:
	  

    	 if(g_pro.gTimer_switch_set_timer_times < 2){
    	           g_pro.gAI=0;
    	 		   LED_AI_OFF();
		           HUMIDITY_ICON_OFF();
		           TEMP_ICON_OFF();//WT.EDIT 2025.04.28
		           if(g_pro.disp_59minutes_flag ==0)
    	 		    TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value); // 显示时间//WT.EDIT 2025.04.23
    	 		   else 
				   	 TM1639_Display_3_Digit(g_pro.disp_timer_minutes_value);
    	 		  
    	  }
		  else{

			 g_pro.key_gtime_timer_define_state = temperature_mode;// g_pro.g_disp_smg_timer_or_temp_hours_item = temperature_mode; //WT.EDIT 2025.010.06
             g_pro.gTimer_switch_temp_hum=5; //at once display "temperature_mode" //WT.EDIT 2025.10.17
//			 if(g_pro.disp_59minutes_flag ==0)
//    	 		   TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value); // 显示时间//WT.EDIT 2025.04.23
//    	 	 else 
//				   TM1639_Display_3_Digit(g_pro.disp_timer_minutes_value);
			  }
       break;

	   case temperature_mode :
                   //else if(g_pro.key_gtime_timer_define_state == temperature_mode && (g_pro.key_set_temperature_flag!=1)){////else  if(g_pro.g_disp_smg_timer_or_temp_hours_item == temperature_mode && (g_pro.key_set_temperature_flag!=1)){		
          if (g_pro.gTimer_switch_temp_hum > SWITCH_THRESHOLD) {
			  g_pro.gTimer_switch_temp_hum = 0; // 重置计时�??
	         
//			   disp_temp_hum++;
//			if (disp_temp_hum > 2) {
//				disp_temp_hum = 1; // 循环显示状�??
//			}
			    disp_temp_hum = (disp_temp_hum % 2) + 1;

				if(disp_temp_hum==1){
				
					LED_TEMP_ICON_ON();
					LED_HUM_ICON_OFF();

					read_error_flag =DHT11_Display_Data(DISPLAY_TEMP); // 显示温度
					if(read_error_flag == 0)DHT11_Display_Data(DISPLAY_TEMP); // 显示温度
				}
				else {
                    LED_TEMP_ICON_OFF();
					LED_HUM_ICON_ON();
				    read_error_flag =DHT11_Display_Data(DISPLAY_HUM);  // 显示湿度
					if(read_error_flag == 0)DHT11_Display_Data(DISPLAY_HUM);  // 显示湿度
					
                }
       

          }
         break;
     	}
}

	 #if 0
     else if((g_pro.g_disp_smg_timer_or_temp_hours_item == timer_time_mode && (g_pro.key_set_temperature_flag!=1)) && read_wifi_temperature_value()==0){
             
	    // 如果计时器超过阈值，切换显示模式

		  if (g_pro.gTimer_switch_temp_hum > SWITCH_THRESHOLD) {
			g_pro.gTimer_switch_temp_hum = 0; // 重置计时�??
	
			disp_temp_hum++;
			if (disp_temp_hum > 3) {
				disp_temp_hum = 1; // 循环显示状�??
			}
	
			// 根据状�?�调用显示函�??
			switch (disp_temp_hum) {
				case 1:
					LED_TEMP_ICON_ON();
					LED_HUM_ICON_OFF();

					read_error_flag =DHT11_Display_Data(DISPLAY_TEMP); // 显示温度
					if(read_error_flag == 0)DHT11_Display_Data(DISPLAY_TEMP); // 显示温度
					break;
				case 2:
                    LED_TEMP_ICON_OFF();
					LED_HUM_ICON_ON();
				    read_error_flag =DHT11_Display_Data(DISPLAY_HUM);  // 显示湿度
					if(read_error_flag == 0)DHT11_Display_Data(DISPLAY_HUM);  // 显示湿度
					break;
				case 3:
					LED_AI_OFF();
					LED_TEMP_ICON_OFF();
					LED_HUM_ICON_OFF();
					TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value); // 显示时间
					break;
			}
		}
	}
  
	 }


}

#endif 


