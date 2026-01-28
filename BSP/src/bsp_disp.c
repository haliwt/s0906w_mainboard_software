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
    
	 static uint8_t read_error_flag;//,switch_adc;

	// If any warning is active, do nothing
    if (g_pro.fan_warning || g_pro.ptc_warning){
        return;
    }
	
	 if(g_pro.key_set_temperature_flag == 1){
	 
			TM1639_Display_Temperature(g_pro.gset_temperture_value);
		    return ;
	 
	}

	

     switch(g_pro.switch_disp_time_or_temp_item){

       case timer_time_mode:
	  

    	 if(g_pro.gTimer_switch_set_timer_times < 2){
    	           g_pro.gAI=0;
    	 		   LED_AI_OFF();
		           HUMIDITY_ICON_OFF();
		           TEMP_ICON_OFF();//WT.EDIT 2025.04.28
		           if((g_pro.set_timing_or_timer_time_flag == TIMER_TIME || g_key.key_mode_long_flag ==1) && g_pro.key_add_dec_be_pressed_flag == 1){
						 TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);

				   }
		           else if((g_pro.set_timing_or_timer_time_flag == TIMER_TIME || g_key.key_mode_long_flag ==1) && (g_pro.key_add_dec_be_pressed_flag == 0 || g_pro.key_add_dec_be_pressed_flag == 2)){
				   	   if(g_pro.gdisp_timer_hours_value >0)
			               TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
					   else
					   	   TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value);
		           }
				   else if(g_key.key_mode_long_flag !=1 && g_pro.set_timing_or_timer_time_flag == WORKS_TIME){
					   g_pro.gdisp_timer_hours_value=0;

				       TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);

				   }
		          
    	 }
		 else{

			 g_pro.switch_disp_time_or_temp_item = temperature_mode;// g_pro.g_disp_smg_timer_or_temp_hours_item = temperature_mode; //WT.EDIT 2025.010.06
             //at once display "temperature_mode" //WT.EDIT 2025.10.17
             g_pro.gTimer_switch_temp_hum=5;
		     if(g_pro.set_timing_or_timer_time_flag !=TIMER_TIME && g_key.key_mode_long_flag != 1){
                  g_pro.gAI=1;
    	 		  LED_AI_ON(); 
			   #if DEBUG_ENABLE
			      printf("gAI = 1 \r\n");
			   #endif 
			 }
			 else if(g_pro.set_timing_or_timer_time_flag ==TIMER_TIME){
			    g_pro.gAI=0;
				LED_AI_OFF(); 
			 #if DEBUG_ENABLE
			    printf("gAI = 0 \r\n");
			 #endif 

            }
		
        }
       break;

	   case temperature_mode :

	     if(g_key.key_mode_long_flag == 1) return ;

         if(g_pro.set_timing_or_timer_time_flag ==TIMER_TIME){
			         
		     LED_AI_OFF(); 
		   }
		  else{
		     LED_AI_ON(); 

		   }

          
                  
          if (g_pro.gTimer_switch_temp_hum > SWITCH_THRESHOLD && g_key.key_mode_long_flag !=1){
			  g_pro.gTimer_switch_temp_hum = 0; // 重置计时�??

			  disp_temp_hum = disp_temp_hum ^ 0x01;

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

/******************************************************************************
	*
	*Function Name:void set_timer_timing_value_handler(void)
	*Function:
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void set_timer_timing_value_handler(void)
{
   
  
   if(g_key.key_mode_long_flag ==1 && g_pro.gTimer_switch_set_timer_times > 2 ){ // 2

   	      g_pro.gTimer_switch_set_timer_times=0;
		  g_key.key_mode_long_flag++;

		  	
          if(g_pro.key_add_dec_be_pressed_flag==1){
		  	
            g_pro.key_add_dec_be_pressed_flag ++ ;
			if(g_pro.gdisp_timer_hours_value>0){
			g_pro.gAI = 0;
			LED_AI_OFF();
			g_pro.switch_disp_time_or_temp_item = temperature_mode; //define UP and down key is set temperature value 
			g_pro.set_timing_or_timer_time_flag=TIMER_TIME;
			g_pro.gTimer_timer_time_second=0;
			g_pro.gdisp_timer_minutes_value =0;
			

			SendWifiData_One_Data(0x2B,g_pro.gdisp_timer_hours_value);
	        osDelay(100);

               
			}
			else if(g_pro.gdisp_timer_hours_value ==0){
			
				g_pro.gAI = 1;
				LED_AI_ON();
				g_pro.gdisp_timer_hours_value=0;
				g_pro.gdisp_timer_minutes_value =0;

				g_pro.set_timing_or_timer_time_flag = WORKS_TIME; //WT.EDIT 2025.10.18

				g_pro.switch_disp_time_or_temp_item = temperature_mode;
				
				SendWifiData_One_Data(0x2B,g_pro.gdisp_timer_hours_value);
	            osDelay(100);
			}
		}
		else{ //times is done ,exit this process
		   g_pro.switch_disp_time_or_temp_item = temperature_mode; //WT.EDIT 2025.10.17
		   if(g_pro.set_timing_or_timer_time_flag==TIMER_TIME){ //WT.EDIT 2025.11.07 
               LED_AI_OFF();
		   }
		   else{
               LED_AI_ON();
           }
		  g_pro.gTimer_mainboard_fun_counter=10; //WT.EDIT 2025.11.07
		 
        }
   	}
    else if(g_pro.set_timing_or_timer_time_flag==TIMER_TIME && g_key.key_mode_long_flag !=1){ //has been set up timer timing value .

       if(g_pro.gTimer_timer_time_second > 59){
	       g_pro.gTimer_timer_time_second=0;
		   #if TEST_UNIT
		   	 g_pro.gdisp_timer_minutes_value =g_pro.gdisp_timer_minutes_value - 40;
		   #else
		    g_pro.gdisp_timer_minutes_value--;
		   #endif 

		   if(g_pro.gdisp_timer_minutes_value< 0){

		       g_pro.gdisp_timer_minutes_value =59;
			  
           
                g_pro.gdisp_timer_hours_value--;
				

             }
			 
			
			 if(g_pro.gdisp_timer_hours_value < 0){
            	
                   buzzer_sound();
			       g_pro.gpower_on = power_off;
			       SendData_Set_Command(CMD_POWER,close);
	               osDelay(100);

			  }
			 
          }
      }
}






