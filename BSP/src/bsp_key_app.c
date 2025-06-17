#include"bsp.h"


uint8_t power_on_key_counter;
uint8_t mode_key_counter;

void key_handler(void)
{
	if(g_key.key_power_flag == KEY_POWER_ID){

		if(KEY_POWER_VALUE() ==KEY_DOWN && g_pro.gpower_on == power_on && power_on_key_counter  < 100 ){
			power_on_key_counter ++ ;
			if(power_on_key_counter >59){
			power_on_key_counter=200;
			g_key.key_long_power_flag =  KEY_LONG_POWER; //wifi led blink fast .
			g_wifi.gTimer_wifi_led_fast_blink = 0; //time start 120s ->look for wifi information 120s,timer.
			g_wifi.gwifi_link_net_state_flag=0 ; //clear wifi link net flag .repeat be detected wifi state.
			g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .
			g_wifi.link_net_step = 0; //WT.EDIT 2025.05.12
			g_pro.first_connect_wifi_flag=0;
			buzzer_sound();
			
			 wifi_led_fast_blink_handler(); //WT.EDIT 2025.05.20
			 if(g_disp.g_second_disp_flag ==1){
                wifi_led_fast_blink_handler(); //WT.EDIT 2025.05.20
				SendData_Set_Command(CMD_CONNECT_WIFI,0x01);
			    wifi_led_fast_blink_handler(); //WT.EDIT 2025.05.20
				osDelay(5);
				wifi_led_fast_blink_handler(); //WT.EDIT 2025.05.20
			 }

			}
		}

		if(KEY_POWER_VALUE() ==KEY_UP && power_on_key_counter < 60){
			
			g_key.key_power_flag=0;
			power_on_key_counter=0;
			buzzer_sound();
			if(g_pro.gpower_on == power_off){
				g_pro.gpower_on = power_on;
				g_wifi.gwifi_normal_power_on_flag =0;
			    SendData_Set_Command(CMD_POWER,open);
			    osDelay(5);

			}
			else{

				g_pro.gpower_on = power_off;
				g_wifi.gwifi_normal_power_on_flag =0;
				SendData_Set_Command(CMD_POWER,close);
			    osDelay(5);
			}
		}
		else if(KEY_POWER_VALUE() ==KEY_DOWN && g_pro.gpower_on == power_on && power_on_key_counter==200){

		      g_key.key_power_flag ++;
		      power_on_key_counter=0;
		      wifi_led_fast_blink_handler();


		}
		
	}
	else if(g_key.key_mode_flag == KEY_MODEL_ID ){ //&& MODEL_KEY_VALUE()==KEY_UP){

		
		if(KEY_MODE_VALUE() == KEY_DOWN && mode_key_counter < 100){

		    mode_key_counter++ ;
            if(mode_key_counter > 59){
			mode_key_counter=200;
		    if(g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
				buzzer_sound();
				g_pro.key_gtime_timer_define_flag = input_set_timer_mode;
			    g_key.key_mode_long_flag =1;
				g_pro.gTimer_switch_set_timer_times = 0;
		        HUMIDITY_ICON_OFF(); //WT.EDIT 2025.04.23
				TEMP_ICON_OFF();//WT.EDIT 2025.04.23
				TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);//WT.EDIT 2025.04.23
		    }
           }
		}

		if(KEY_MODE_VALUE() == KEY_UP && mode_key_counter < 60){
			g_key.key_mode_flag = KEY_NULL;
			mode_key_counter=0;
			g_pro.gTimer_switch_set_timer_times = 0;
			if(g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
			buzzer_sound();
	        mode_key_fun();
		    }
		}
		else if(KEY_MODE_VALUE() == KEY_UP && mode_key_counter ==200){
			
            g_key.key_mode_flag++;
			mode_key_counter=0;
		}
	}
	else if(g_key.key_down_flag ==KEY_DOWN_ID && KEY_DOWN_VALUE() == KEY_UP ){// && DEC_KEY_VALUE()==KEY_UP){
		g_key.key_down_flag = KEY_NULL;
		g_key.mode_key_switch_time_mode= works_time_mode; //WT.EDIT 2025.04.30
		if(g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		   buzzer_sound();

		    key_dwon_fun();
		}
	}
	else if(g_key.key_up_flag ==KEY_UP_ID && KEY_UP_VALUE() == KEY_UP ){ // && ADD_KEY_VALUE()==KEY_UP){
		g_key.key_up_flag =KEY_NULL;
		g_key.mode_key_switch_time_mode= works_time_mode; //WT.EDIT 2025.04.30
		
		if(g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		   buzzer_sound();
           key_up_fun();

		}
	}



}



