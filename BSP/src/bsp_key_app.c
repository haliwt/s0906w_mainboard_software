#include"bsp.h"




//uint8_t complex_counter;



static void handle_power_key(void);
static void handle_mode_key(void);
static void handle_down_key(void);
static void handle_up_key(void);

/*
	*@brief: all key parase state
	*@notice:all key handler function
    #@param:
*/
void key_handler(void)
{
    if(g_key.key_power_flag == 9 && KEY_POWER_VALUE() ==KEY_UP){

	    g_key.key_power_flag ++;
		g_key.power_on_key_counter=0;
	    g_key.down_key_long_counter = 0;
		g_key.mode_key_long_counter = 0;

	}
    else if (g_key.key_power_flag == KEY_POWER_ID && KEY_POWER_VALUE() ==KEY_UP){
		g_key.key_power_flag=0;
		g_key.power_on_key_counter=0;
	    g_key.mode_key_long_counter = 0;
		g_key.down_key_long_counter = 0;
        handle_power_key();
    }
	else if(g_key.key_mode_flag == 11 && KEY_MODE_VALUE() == KEY_UP){
      g_key.key_down_flag ++;
	  g_key.mode_key_long_counter = 0;
      g_key.power_on_key_counter=0;
      g_key.down_key_long_counter = 0;
           
	}
    else if (g_key.key_mode_flag == KEY_MODEL_ID && KEY_MODE_VALUE() == KEY_UP && g_pro.ptc_warning ==0 &&g_pro.fan_warning ==0)
    {
        g_key.key_mode_flag  ++;
	    g_key.mode_key_long_counter = 0;
		g_key.power_on_key_counter=0;
		g_key.down_key_long_counter = 0;
		
		handle_mode_key();
    }
	else if(g_key.key_down_flag == 13 &&  KEY_DOWN_VALUE() == KEY_UP){

	      g_key.key_down_flag++;
          g_key.down_key_long_counter = 0;
	     g_key.mode_key_long_counter = 0;
	   	 g_key.power_on_key_counter=0;
	}
    else if (g_key.key_down_flag == 0x01 && KEY_DOWN_VALUE() == KEY_UP && !g_pro.fan_warning && !g_pro.ptc_warning)
    {
         g_key.key_down_flag++;
		 g_key.down_key_long_counter = 0;
		 g_key.mode_key_long_counter = 0;

		 g_key.power_on_key_counter=0;
		 handle_down_key();
    }
    else if (g_key.key_up_flag == KEY_UP_ID && KEY_UP_VALUE() == KEY_UP && !g_pro.fan_warning && !g_pro.ptc_warning)
    {
        g_key.key_up_flag= KEY_NULL;
		g_key.mode_key_long_counter = 0;
		g_key.power_on_key_counter=0;
		g_key.down_key_long_counter = 0;
		handle_up_key();
    }
}

/*
	*@brief:Power Key 
	*@notice:
    *@param:
    *@retrval
    
*/
static void handle_power_key(void)
{
    
    if (KEY_POWER_VALUE() == KEY_UP)
    {
        g_key.key_power_flag = 0;
        g_key.power_on_key_counter = 0;
        buzzer_sound();

      
        if(g_pro.gpower_on==power_off){
		   g_pro.gpower_on  = power_on;
           g_pro.power_on_ref_f =1;
		   DRY_OPEN();
		   power_on_init_ref();

		}
		else {
			g_pro.gpower_on=power_off;// DRY_CLOSE();
			g_pro.power_on_ref_f =4;
		  
            LED_TAPE_CTL_OFF();
		    power_off_led();
            TM1639_Display_ON_OFF(0);
			vTaskDelay(100);

		}
		
        g_wifi.gwifi_normal_power_on_flag = 0;
        if(g_disp.g_second_disp_flag==1){
			SendData_Set_Command(CMD_POWER,(g_pro.gpower_on == power_on) ? open : close);
            osDelay(100);
        }
    }
}
/*
	*@brief: key power be pressed long time.
	*@notice:
    *@param:
    *@retrval
    
*/
void key_power_longk_fun(void)
{
	if(g_pro.gpower_on == power_on && g_key.power_on_key_counter < 100)
    {
        g_key.power_on_key_counter++;
		
        if (g_key.power_on_key_counter > LONG_PRESS_THRESHOLD - 1)
        {
            g_key.power_on_key_counter = COUNTER_LOCK;
           // g_key.key_long_power_flag = KEY_LONG_POWER;

            g_wifi.gTimer_wifi_led_fast_blink = 0; //reset wifi led blink timer
            g_wifi.gwifi_link_net_success = 0; //reset wifi link status
            g_wifi.wifi_led_fast_blink_flag = 1;//
            g_wifi.link_net_step = 0;//reset wifi link step
            g_pro.first_connect_wifi_flag = 0;//reset first connect wifi flag

            buzzer_sound();
            wifi_led_fast_blink_handler();
            SendData_Set_Command(CMD_CONNECT_WIFI, 0x01);
            osDelay(100);
            wifi_led_fast_blink_handler();
            
        }
    }

}

/*
	*@brief:mode Key 
	*@notice:
    *@param:
    *@retrval
    
*/
static void handle_mode_key(void)
{
    g_pro.gTimer_switch_set_timer_times = 0;
	g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
    buzzer_sound();
    mode_short_key_fun();
    
     
}

/*
	*@brief:mode Key 
	*@notice:
    *@param:
    *@retrval
    
*/
void key_mode_long_fun(void)
{
  if (!g_pro.fan_warning && !g_pro.ptc_warning && g_key.mode_key_long_counter < 100) {
        g_key.mode_key_long_counter++;

       if(g_key.mode_key_long_counter > LONG_PRESS_THRESHOLD - 1){
           g_key.mode_key_long_counter= COUNTER_LOCK;
        buzzer_sound();
        g_pro.switch_disp_time_or_temp_item = timer_time_mode;
        g_key.key_mode_long_flag = 1;
		g_pro.g_real_hours_counter =0;
        g_pro.gTimer_switch_set_timer_times = 0;
		g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
        g_pro.gAI = 0;
		LED_AI_OFF();
        HUMIDITY_ICON_OFF();
        TEMP_ICON_OFF();
		if(g_pro.set_timing_or_timer_time_flag == TIMER_TIME){
		    if(g_pro.gdisp_timer_hours_value >0)
	          TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
		    else 
		      TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value);
		}
		else{
			 g_pro.gdisp_timer_minutes_value=0;

		     TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value);//TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);

		}

		//TM1639_Display_3_Digit(g_pro.disp_59minutes_flag ? g_pro.gdisp_timer_minutes_value :g_pro.gdisp_timer_hours_value);
      }
  	}
}
/*
	*@brief:mode Key 
	*@notice:
    *@param:
    *@retrval
    
*/
static void handle_down_key(void)
{
 
  
       g_key.down_key_long_counter =0;
       buzzer_sound();
       key_dwon_fun();
            
       
   
}
/*
	*@brief: key down long  time
	*@notice:
    *@param:
    *@retrval
    
*/
void key_down_long_fun(void)
{

	if(g_key.down_key_long_counter < 100){
        g_key.down_key_long_counter++;
        if(g_key.down_key_long_counter > SHORT_PRESS_THRESHOLD - 1) {
            g_key.down_key_long_counter = COUNTER_LOCK;
            buzzer_sound();
            g_pro.led_bar ^= 0x01;
            if (g_pro.led_bar)
                LED_TAPE_CTL_OFF();
            else
                LED_TAPE_CTL_ON();
        }
    }
  
}

/*
	*@brief:mode Key 
	*@notice:
    *@param:
    *@retrval
    
*/

/* ---------------- Up Key ---------------- */
static void handle_up_key(void)
{
   
      g_key.key_up_flag = KEY_NULL;
      buzzer_sound();
       key_up_fun();
        
    
}









