/*
 * bsp_power.c
 *
 *  Created on: 2025�??3�??4�??
 *      Author: Administrator
 */
#include "bsp.h"


typedef struct{

  uint8_t process_on_step;
 // uint8_t process_off_step;


}POWER_RUN_STATE;

POWER_RUN_STATE gl_run;



//DisplayMode disp_temp_hum = DISPLAY_TEMP;  // 默认显示温度



/**********************************************************************
	*
	*Function Name: 
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
//void power_on_off_handler(uint8_t data)
//{
//  

//  if(g_pro.gpower_on == power_on){

//        power_on_run_handler();
//         
//        if(gl_run.process_on_step !=0  && gl_run.process_on_step !=1 && gl_run.process_on_step !=2){ //logically rigorous
//              if( g_pro.time_20ms_f == 1){
//			  	   g_pro.time_20ms_f =0;
//			       display_digital_3_numbers();
//              }
//			 
//	    }
//  	}
//    else{
//			
//      power_off_handler();

//  
//      }
//}
/**********************************************************************
	*
	*Function Name: void power_on_init_ref(void)
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_on_init_ref(void)
{

      if(g_wifi.app_timer_power_on_flag ==0){
		   g_pro.gAI =1;
		
		   g_pro.gDry =1;
		   g_pro.gPlasma =1;
		   g_pro.gMouse = 1;
      	}
		
		   //display time timing value 
		   g_pro.gdisp_hours_value =0;
		   g_pro.gdisp_timer_hours_value =0; //设置定时时间�??

		   g_pro.switch_disp_time_or_temp_item=temperature_mode; //
		  // g_pro.g_disp_smg_timer_or_temp_hours_item = temperature_mode;
		
		 
		   // function led is turn on 
            power_on_led(); 
		    disp_all_sumg_led();
		   //display smg led turn on
		    Fan_Full_Speed();
		    if(g_wifi.app_timer_power_on_flag ==0){
			    DRY_OPEN();
				PLASMA_OPEN();
				mouse_open();
				TEMP_ICON_ON() ;
	        //HUMIDITY_ICON_ON();
		    }
			//TM1639_Display_Temperature(g_pro.g_temperature_value);  //DHT11_Display_Data(0); //display temperature value 
		    
           //timer 
           g_pro.g_manual_shutoff_dry_flag = 0;
		   g_pro.gTimer_disp_time_second= 0;
	       g_pro.gTimer_timer_time_second=0;
		   g_wifi.set_wind_speed_value = 100;


}
/**********************************************************************
	*
	*Functin Name: power_on_run_handler(void)
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
uint8_t read_dht11_f;
static void power_on_initial(void)
{
	//static uint8_t temp_second_displboard,switch_dht11,send_net_state;
 
	switch(gl_run.process_on_step){


     case 0:  //initial reference 
       g_pro.process_off_step =0 ; //clear power off process step .

	   if(g_wifi.gwifi_link_net_success == wifi_no_link){//逻辑不严�??//if(g_wifi.gwifi_link_net_success == wifi_no_link || g_wifi.app_timer_power_on_flag == 0)
	       read_sensorData();//updateDht11_toDisplayBoard_value();
	       if(g_pro.gpower_on_key_f != 1){
		      power_on_init_ref();
	       	
	       }

       }
	   else if(g_wifi.gwifi_link_net_success == wifi_link_success &&  g_wifi.app_timer_power_on_flag == 0){ //has wifi net initial
		   read_sensorData();//updateDht11_toDisplayBoard_value();
           g_pro.first_connect_wifi_flag =1;
		   if(g_pro.gpower_on_key_f != 1){  
		        power_on_init_ref();
	        }
		
		
	   }
	   else if(g_wifi.gwifi_link_net_success == wifi_link_success &&  g_wifi.app_timer_power_on_flag == 1){

		    power_on_smart_app_led();
			

	   }
         gl_run.process_on_step =1;
	   break;

	   case 1:

	   if(g_wifi.app_timer_power_on_flag ==1){
	      
	   	  if(g_wifi.gwifi_link_net_success==wifi_link_success){
		  	  
                 MqttData_Publish_SetOpen(1);  
		  }
		}
        else if(g_wifi.gwifi_link_net_success == wifi_link_success && g_wifi.gwifi_normal_power_on_flag == 0){
			 MqttData_Publish_SetOpen(1);  
         }
		 gl_run.process_on_step =2;
	   break; 

	   case 2:
	   
	   g_pro.gset_temperture_value=40;
	 
       g_pro.g_fan_switch_gears_flag++;
	   g_pro.process_off_step=0;
	   //reset wifi 
	   g_wifi.wifi_led_fast_blink_flag=0;

	   //reset temperature value and ptc 
	   g_pro.set_temperature_success_flag=0;
	   g_pro.temperature_init_value=0;
	   g_pro.g_manual_shutoff_dry_flag = 0;

	   g_pro.first_set_ptc_on=0;
	   g_pro.first_rcoder_ptc_on_flag =0;

	   //reset timer timing 
	    g_pro.gTimer_disp_time_second= 0;
	    
		g_pro.gdisp_timer_hours_value =0;
		g_pro.gdisp_timer_minutes_value =0;
		g_pro.gTimer_timer_time_second=0;

		
	   //two hours works timing
	    g_pro.works_two_hours_interval_flag=0; //WT.EDIT 2025.05.07
		g_pro.gTimer_two_hours_counter = 0;
	   //reset fan wind
		g_wifi.set_wind_speed_value = 100;
	 
	   g_pro.fan_warning =0 ;
	   g_pro.ptc_warning =0;
	   g_pro.gTimer_display_adc_value=0;
	   g_pro.delay_run_adc_counter=0;
	   g_pro.g_real_hours_counter =0;
	   
	    g_pro.gAI =1 ;//g_pro.set_timing_or_timer_time_flag=WORKS_TIME; //WT.EDIT 2025.10.18
	   
	    g_pro.gTimer_to_disp_counter= 20;
		g_pro.gset_temperture_value =40;

	   gl_run.process_on_step =3;
	 break;

	
      

	case 3: //DISPAY 3 digital numbers . process .
   
	   read_sensorData();//Update_Dht11_toDisplayBoard_Value();

		
	  gl_run.process_on_step =0xff; 

	 break;

	 default:
	 break;
	}

}
	
#if 0
	 case 4: //WIFI link process
	  
         if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		 
		if(g_wifi.gTimer_update_dht11_data > 20 && g_wifi.gwifi_link_net_success ==wifi_link_success){
		   g_wifi.gTimer_update_dht11_data=0;

		   if(g_wifi.gwifi_link_net_success ==1){

		       switch_dht11 = switch_dht11 ^0x01;
			   if(switch_dht11==1){
			   	
                   if(timer_expired(&t_mqtt_1)){
				     Subscriber_Data_FromCloud_Handler();
			       
                     //tx_thread_sleep(20);
                   	}
			   	}
			    else{
					if(timer_expired(&t_mqtt_0)){
				       Update_Dht11_Totencent_Value()	;
				        //tx_thread_sleep(20);
						}


				}
			   
		   	}

          }
		    

         }
		    
	     gl_run.process_on_step =5;

	 break;

	 case 5: // wifi function
	  
       wifi_led_slowly_blink_handler();
	
      gl_run.process_on_step =6;

	 break;

	 case 6:
   
	      gl_run.process_on_step =7;

	 break;


	 case 7:
	 	  	works_run_two_hours_state();
	        gl_run.process_on_step =8;

	 break;

     case 8:
	 	 smart_phone_timer_power_on_handler();
	        
		gl_run.process_on_step =9;

	 break;

	 case 9:
	    link_wifi_to_tencent_handler(g_wifi.wifi_led_fast_blink_flag);

	 
      gl_run.process_on_step =10;

	 break;

	 case 10:
	   set_temperature_value_handler(); //logic is confuse "set temp ? or timer timing " only displya one.
				 
	 gl_run.process_on_step =11;

	 break;

	 case 11:
	 set_timer_timing_value_handler();
	 gl_run.process_on_step =12;

	 break;

	 case 12:
	 	
	 if(g_pro.gTimer_display_adc_value > 6 && g_pro.works_two_hours_interval_flag==0){
		g_pro.gTimer_display_adc_value=0;
		send_net_state++;
		adc_detected_hundler();

		if(send_net_state > 1){
			send_net_state=0;
			if(g_wifi.gwifi_link_net_success==1) {
				if(g_pro.disp_second_f ==1){
				
				SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
				tx_thread_sleep(10);
				
				}
			}
			else{
			if(g_pro.disp_second_f ==1){
				
				SendWifiData_To_Cmd(0x1F,0); //link wifi order 1 --link wifi net is success.
				tx_thread_sleep(10);
				
			}

			}

		}

		}
	     gl_run.process_on_step =13;


	 break;


	 case 13:
	 	
	 if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		 
		   if(send_wifi_power_on_state ==1){
		   send_wifi_power_on_state++;
		   g_pro.gset_temperture_value = 40;

		 
			  MqttData_Publish_Update_Data();
			  tx_thread_sleep(10);
		   


		}
		 
	     gl_run.process_on_step =3; 
	 }
	 else{
		   
	     fault_handler();
		 wifi_led_slowly_blink_handler();
		 gl_run.process_on_step =13; 
	  }
      
	 break;

#endif 	 
/**********************************************************************
	*
	*Functin Name: void power_off_run_handler(void)
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_off_handler(void)
{

   static uint8_t fan_flag,wifi_first_connect,fan_run_one_minute,switch_f;
   static uint8_t power_on_flag=0;
   static uint16_t counter_send=0;
   switch(g_pro.process_off_step){

   case 0:
    
       if(power_on_flag==0){
             power_on_flag ++;
			 buzzer_power_sound();
	   }
   	  gl_run.process_on_step =0;
      g_pro.gpower_on_key_f = 0;
      gl_run.process_on_step =0;
	  g_pro.g_real_hours_counter=0;
	  TM1639_Display_ON_OFF(0);
      power_off_led();
      DRY_CLOSE();

	   
	     
	   
      g_pro.process_off_step = 1;
   break;

   case 1:

   	  
      g_pro.process_off_step = 2;
   break;

   case 2:
	 // g_key.key_long_power_flag  = 0;
	  g_key.key_long_mode_flag = 0;
	
	  g_pro.switch_disp_time_or_temp_item = temperature_mode;
	
	  g_pro.led_bar =0;

	  fan_run_one_minute = 1;
	  g_pro.gTimer_fan_run_one_minute =0;

	 
	 
	   g_pro.g_fan_switch_gears_flag++;
      
	   g_pro.set_temperature_success_flag=0;
	   g_wifi.app_timer_power_on_flag =0;
	   g_pro.fan_warning =0 ;
	   g_pro.ptc_warning =0;
	   g_pro.disp_second_f =0;
	 
	   g_pro.works_two_hours_interval_flag=0; //WT.EDIT 2025.05.07
        mainboard_close_all_fun();
        g_pro.process_off_step = 3;

   break;

   case 3:
    
     if(fan_flag == 0){
	 	fan_flag++;
	    fan_run_one_minute =2;
     }
      g_pro.process_off_step = 4;

   break;


   case 4:
   	
	  if(g_wifi.gwifi_link_net_success == wifi_link_success){
            MqttData_Publish_SetOpen(0);  
			
           
	  }
     g_pro.process_off_step = 5;

  break;

  case 5:

   if(g_wifi.gwifi_link_net_success == wifi_link_success){
          
	        MqttData_Publish_PowerOff_Ref() ;//
	       
        }


      g_pro.process_off_step = 6;
  break;

  case 6:
     counter_send ++ ;
    
     if(counter_send > 300){//10ms * 100
	 	counter_send=0;
	   
	 	  SendWifiData_To_Cmd(0x11,0); //主板发送询问指令,是否有外接显示板?
	      tx_thread_sleep(10);

     	}
     	
    g_pro.process_off_step = 7;

  break;
	 
  case 7:
	 if(fan_run_one_minute ==1){
	 
		   if(g_pro.gTimer_fan_run_one_minute  < 61){
	 
				Fan_Full_Speed();
		   }
		   else{
	 
			  fan_run_one_minute++;
			  FAN_Stop();
	 
		   }
	 
	   }
	   g_pro.process_off_step = 8;
   break;

   case 8:
	 
      LED_Power_Breathing();

      if(g_pro.gTimer_to_disp_counter > 9){//10ms*200 =2000ms =2s
			g_pro.gTimer_to_disp_counter=0;
         read_sensorData();
      }
      g_pro.process_off_step = 9;

   break;

   case 9:
	 wifi_first_connect++;

	 if(g_wifi.gwifi_link_net_success == wifi_link_success && wifi_first_connect > 250){//10ms * 
	 	    wifi_first_connect=0;
			switch_f = switch_f ^ 0x01;
	        if(switch_f ==1){
             MqttData_Publish_SetOpen(0);  
	        }
		    else{
	         MqttData_Publish_PowerOff_Ref() ;//
		    }
	       
           
	 }

    g_pro.process_off_step = 6;

   break;

   
   	}

}
/**********************************************************************
	*
	*Functin Name: void power_on_handler(void)
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
uint8_t time_slot = 0;

void power_on_handler(void)
{
    static uint8_t  switch_dht11 =0;
  

   if(g_pro.time_20ms_f ==1){
		g_pro.time_20ms_f =0;
		power_on_initial();
		

	 // ✨【新增：紧急事件拦截响应】✨
        // 如果按键任务设置完温度，将 g_pro.g_immediate_heat_f 置为 1
        if (g_pro.g_immediate_heat_f == 1)
        {
            g_pro.g_immediate_heat_f = 0; // 立即清除触发标志，防止重复执行
            
            // 强制、立刻执行一次加热控制函数
            // 确保底层硬件（如继电器、PWM、PTC）在 20ms 内得到响应
            immediatley_temp_comare_value();//set_temperature_value_handler(); 
        }

		switch(time_slot){

		case 0://20ms

           display_digital_3_numbers();
		break;

		case 1:
		   smart_phone_timer_power_on_handler();

		break;


		case 2://20ms*1 =20

            link_wifi_to_tencent_handler(g_wifi.wifi_led_fast_blink_flag);
		break;


		case 3://20ms*2=40
            set_temperature_value_handler();

		break;

		case 4: //20ms* 3 =60ms
            set_timer_timing_value_handler();

		break;

		case 5: //20ms* 4=80ms

	        if(g_pro.gTimer_to_disp_counter > 4){//10ms*200 =2000ms =2s
				g_pro.gTimer_to_disp_counter=0;
			    read_sensorData();//Update_Dht11_toDisplayBoard_Value();

			}
		break;

		case 6: //100ms
			
        if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		 
		if(g_wifi.gTimer_update_dht11_data > 20 && g_wifi.gwifi_link_net_success ==wifi_link_success){
		   g_wifi.gTimer_update_dht11_data=0;

		   if(g_wifi.gwifi_link_net_success ==1){

		       switch_dht11 = switch_dht11 ^0x01;
			   if(switch_dht11==1){
			   	
                   if(timer_expired(&t_mqtt_1)){
				     Subscriber_Data_FromCloud_Handler();
			       
                     //tx_thread_sleep(20);
                   	}
			   	}
			    else{
					if(timer_expired(&t_mqtt_0)){
				       Update_Dht11_Totencent_Value()	;
				        //tx_thread_sleep(20);
						}


				}
			   
		   	}

          }
		    

         }

		break;


		case 7: //20ms *6 =120ms
           
         fault_handler();
		 wifi_led_slowly_blink_handler();

		break;

		case 8://180ms 
			works_run_two_hours_state();
		break;

          }

		
		  
         // ==================== 4. 时间片轮转维护 ====================
           time_slot++;
           if (time_slot >8 ) time_slot = 0; 
      }
}
	



