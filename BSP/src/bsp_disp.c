/*
 * bsp_disp.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"

#define SWITCH_THRESHOLD 2

uint8_t disp_temp_hum,dsip_timer_value,disp_timer_time_f=0;
uint8_t timerbuf[1];

void display_digital_3_numbers(void)
{
    
	 static uint8_t read_error_flag;//,switch_adc;

	// If any warning is active, do nothing
    if (g_pro.fan_warning || g_pro.ptc_warning) return;
    
	
	 if(g_pro.key_set_temperature_flag == 1){

	    if(g_pro.gAI ==1)LED_AI_ON();
		else LED_AI_OFF();
	 
	    TM1639_Display_Temperature(g_pro.gset_temperture_value);
		 return ;
	 
	}

	

     switch(g_pro.switch_disp_time_or_temp_item){


	   case timer_time_disp_mode:
	     disp_timer_time_f = 1;

       case timer_time_mode:
	  

    	 if(g_pro.gTimer_switch_set_timer_times < 2){
    	          // g_pro.gAI=0;  WT.EDIT 2026-05-12
    	 		   LED_AI_OFF();
		           HUMIDITY_ICON_OFF();
		           TEMP_ICON_OFF();//WT.EDIT 2025.04.28
		           if((g_key.key_mode_long_flag ==1 || disp_timer_time_f ==1) && g_pro.key_add_dec_be_pressed_flag == 1){
						 TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);

				   }
		           else if((g_key.key_mode_long_flag ==1 || disp_timer_time_f ==1) && (g_pro.key_add_dec_be_pressed_flag == 0 || g_pro.key_add_dec_be_pressed_flag == 2)){
				   	   if(g_pro.gdisp_timer_hours_value >0)
			               TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
					   else
					   	   TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value);
		           }
				   else if(g_pro.gdisp_timer_hours_value== 0 && g_pro.gdisp_timer_minutes_value ==0){//g_key.key_mode_long_flag !=1 &&
					  // g_pro.gdisp_timer_hours_value=0;
					   ///g_pro.gdisp_timer_minutes_value=0;

				       TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value);//TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);

				   }
		          
    	 }
		 else{
			 disp_timer_time_f =0;
             g_pro.switch_disp_time_or_temp_item = temperature_mode;// g_pro.g_disp_smg_timer_or_temp_hours_item = temperature_mode; //WT.EDIT 2025.010.06
             //at once display "temperature_mode" //WT.EDIT 2025.10.17
             g_pro.gTimer_switch_temp_hum=5;
		     if(g_pro.gAI ==1){ // && g_key.key_mode_long_flag != 1){
                  
    	 		  LED_AI_ON(); 
			   #if DEBUG_ENABLE
			      printf("gAI = 1 \r\n");
			   #endif 
			 }
			 else 
			 {
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

         if(g_pro.gAI ==0){
			         
		     LED_AI_OFF(); 
		   }
		  else{
		     LED_AI_ON(); 

		   }

         if (g_pro.gTimer_switch_temp_hum > SWITCH_THRESHOLD ){
			  g_pro.gTimer_switch_temp_hum = 0; // 針置计时�??

			  disp_temp_hum = disp_temp_hum ^ 0x01;

			    if(disp_temp_hum==1){
				
					LED_TEMP_ICON_ON();
					LED_HUM_ICON_OFF();

					DHT11_Display_Data(DISPLAY_TEMP); // 显示温度
					vTaskDelay(100);
				}
				else {
                    LED_TEMP_ICON_OFF();
					LED_HUM_ICON_ON();
					
				    DHT11_Display_Data(DISPLAY_HUM);  // 显示湿度
					
					vTaskDelay(100);
                }
       

          }
         break;
     	}


	   	
  
}

/******************************************************************************
	*
	*Function Name:void set_timer_timing_value_handler(void)
	*Function:逻辑嵌套过深、全局变量满天飞,业务辑与硬件操作（LED, Buzzer, Wifi）高度耦合
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
#if 0
void set_timer_timing_value_handler(void)
{
   
   if(g_key.key_mode_long_flag ==1 && g_pro.gTimer_switch_set_timer_times > 2 ){ // 2

   	      g_pro.gTimer_switch_set_timer_times=0;
		  g_key.key_mode_long_flag++;

		  	
          if(g_pro.key_add_dec_be_pressed_flag==1){
		  	
            g_pro.key_add_dec_be_pressed_flag ++ ;
			if(g_pro.gdisp_timer_hours_value>0){
			timerbuf[0] = g_pro.gdisp_timer_hours_value;
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
		   #if 0 //TEST_UNIT
		   	 g_pro.gdisp_timer_minutes_value =g_pro.gdisp_timer_minutes_value - 40;
		   #else
		     g_pro.gdisp_timer_minutes_value--;

		   #endif 

		   if(g_pro.gdisp_timer_minutes_value< 0){

		       g_pro.gdisp_timer_minutes_value =59;

               g_pro.g_real_hours_counter++;
		        // 坪有当尝时数 > 1 时扝凝尝时2026.03.09 .WT.EDIT .
		        if (g_pro.gdisp_timer_hours_value > 1 &&  g_pro.gdisp_timer_hours_value !=1 ){

				    dsip_timer_value = timerbuf[0] - g_pro.g_real_hours_counter +1;
		            g_pro.gdisp_timer_hours_value = dsip_timer_value;
		        }
				
				if(g_pro.gdisp_timer_hours_value ==1 || g_pro.gdisp_timer_hours_value==0){

				    g_pro.gdisp_timer_hours_value--;
                }
		        
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


#else 


static void handle_timer_setting_logic(void ); // 抽离设置逻辑

static void execute_system_shutdown(void);
//static void update_countdown(int16_t *h, int16_t *m, int16_t *s) ;

static void timer_timing_handler(void);


void set_timer_timing_value_handler(void)
{
    // 1. 处理设置模式 (长按逻辑)
    if (g_key.key_mode_long_flag == 1 && g_pro.gTimer_switch_set_timer_times > 2) {
        handle_timer_setting_logic(); // 抽离设置逻辑
        return;
    }

    // 2. 处理倒计时逻辑 (只有在 TIMER_TIME 模式下才运行)
    if ( g_pro.gAI == 1 || g_key.key_mode_long_flag == 1) {
        return; 
    }

	//3. 定时器设置成功,到计数开始
	timer_timing_handler();

}

    // 每一秒执行一次
static void timer_timing_handler(void)
{
    if (g_pro.gTimer_timer_time_second > 59) {
        g_pro.gTimer_timer_time_second = 0;

		// 分钟减1
	    g_pro.gdisp_timer_minutes_value--;
	    
	    // 如果分钟小于0，向小时借位
	    if (g_pro.gdisp_timer_minutes_value < 0) {
			
	        g_pro.gdisp_timer_minutes_value = 59;
	       
            g_pro.g_real_hours_counter++;
	        // 坪有当尝时数 > 1 时扝凝尝时2026.03.09 .WT.EDIT .
	        if (g_pro.gdisp_timer_hours_value > 1 &&  g_pro.gdisp_timer_hours_value !=1 ){

			    dsip_timer_value = timerbuf[0] - g_pro.g_real_hours_counter +1;
	            g_pro.gdisp_timer_hours_value = dsip_timer_value;
	        }
			
			if(g_pro.gdisp_timer_hours_value ==1 || g_pro.gdisp_timer_hours_value==0){

			    g_pro.gdisp_timer_hours_value--;
            }
		}
       

        // 3. 状态触发动作 (避免在逻辑中硬编码 HW 操作)
        if (g_pro.gdisp_timer_hours_value < 0) {
            execute_system_shutdown(); // 抽离关机动作
        }
    }
}

// 封装动作：以后换了蜂鸣器或关机指令，只改这里
static void execute_system_shutdown(void) 
{
    buzzer_sound();
    g_pro.gpower_on = power_off;
	if(g_disp.g_second_disp_flag ==1){
    SendData_Set_Command(CMD_POWER, close);
    vTaskDelay(20);
    // 可扩展：可以在这里加日志、存Flash等
    }
}


// 纯逻辑：输入当前时分秒，返回减去1秒后的结果
#if 0
static void update_countdown(int16_t *h, int16_t *m, int16_t *s) 
{
    if (--(*s) < 0) {
        *s = 59;
        if (--(*m) < 0) {
            *m = 59;
            --(*h);
        }
    }
}

static bool update_countdown(int16_t *h, int16_t *m, int16_t *s) 
{
    if (--(*s) < 0) {
        *s = 59;
        if (--(*m) < 0) {
            *m = 59;
            if (--(*h) < 0) {
                // 已经倒计时到负数，防止显示负数
                *h = 0;
                *m = 0;
                *s = 0;
                return true;  // 倒计时结束
            }
        }
    }
    
    // 检查是否正好归零
   // return (*h == 0 && *m == 0 && *s == 0);
}


#endif 
static void handle_timer_setting_logic(void ) // 抽离设置逻辑
{
	 if(g_key.key_mode_long_flag ==1 && g_pro.gTimer_switch_set_timer_times > 2 ){ // 2

   	      g_pro.gTimer_switch_set_timer_times=0;
		  g_key.key_mode_long_flag++;

		  	
          if(g_pro.key_add_dec_be_pressed_flag==1){
		  	
            g_pro.key_add_dec_be_pressed_flag ++ ;
			if(g_pro.gdisp_timer_hours_value>0){
			timerbuf[0] = g_pro.gdisp_timer_hours_value;
			g_pro.gAI = 0;
			LED_AI_OFF();
			g_pro.switch_disp_time_or_temp_item = temperature_mode; //define UP and down key is set temperature value 
			//g_pro.set_timing_or_timer_time_flag=TIMER_TIME;
			g_pro.gTimer_timer_time_second=0;
			g_pro.gdisp_timer_minutes_value =0;
			

			if(g_disp.soft_version ==1){
				SendWifiData_One_Data(0x2B,g_pro.gdisp_timer_hours_value);
	            osDelay(100);
				}

               
			}
			else if(g_pro.gdisp_timer_hours_value ==0){
			
				g_pro.gAI = 1;	//g_pro.set_timing_or_timer_time_flag = WORKS_TIME; //WT.EDIT 2025.10.18
				LED_AI_ON();
				g_pro.gdisp_timer_hours_value=0;
				g_pro.gdisp_timer_minutes_value =0;

			     g_pro.switch_disp_time_or_temp_item = temperature_mode;
				
				if(g_disp.g_second_disp_flag ==1){
					SendWifiData_One_Data(0x2B,g_pro.gdisp_timer_hours_value);
	                osDelay(100);
					}
			}
		}
		else{ //times is done ,exit this process
		   g_pro.switch_disp_time_or_temp_item = temperature_mode; //WT.EDIT 2025.10.17
		   if(g_pro.gAI ==0){ //WT.EDIT 2025.11.07 
               LED_AI_OFF();
		   }
		   else{
               LED_AI_ON();
           }
		  g_pro.gTimer_mainboard_fun_counter=10; //WT.EDIT 2025.11.07
		 
        }
   	}

}





#endif 



