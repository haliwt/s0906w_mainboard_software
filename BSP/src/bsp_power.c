/*
 * bsp_power.c
 *
 *  Created on: 2025�??3�??4�??
 *      Author: Administrator
 */
#include "bsp.h"


typedef struct{

  uint8_t process_on_step;
  uint8_t process_off_step;


}POWER_RUN_STATE;

POWER_RUN_STATE gl_run;



//DisplayMode disp_temp_hum = DISPLAY_TEMP;  // 默认显示温度

uint8_t send_wifi_power_on_state;


/**********************************************************************
	*
	*Function Name: 
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_onoff_handler(uint8_t data)
{
  static uint8_t power_on_flag;
   switch(data){	

	   case power_on :

          power_on_run_handler();

	    if(gl_run.process_on_step > 30)gl_run.process_on_step=1;
		if(g_pro.fan_warning > 1 || g_pro.ptc_warning >1){
           if(g_pro.fan_warning > 1) g_pro.fan_warning =0;
		   if(g_pro.ptc_warning >1)  g_pro.ptc_warning =0;
		}
         if(g_pro.disp_3_numbers_f ==1){
		 	g_pro.disp_3_numbers_f=0;
            display_digital_3_numbers();
         }

	   

        break;

	  case power_off:
        
         power_off_run_handler();

	   break;
  
      }
	}
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
	       g_pro.gAI =1;
		   g_pro.gDry =1;
		   g_pro.gPlasma =1;
		   g_pro.gMouse = 1;
		   g_pro.gTemp_value = 40;
		 
		   g_pro.gdisp_hours_value =0;
		   g_pro.gdisp_timer_hours_value =0; //设置定时时间�??

		   g_pro.switch_disp_time_or_temp_item=temperature_mode; //
		   
		
		 
		   // function led is turn on 
            power_on_led();
		   //display smg led turn on
		    Fan_Full_Speed();
		   
		    DRY_OPEN();
			PLASMA_OPEN();
			mouse_open();
		   
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
void power_on_run_handler(void)
{

    static uint8_t temp_second_displboard,switch_dht11,send_net_state,ptc_counter;
	volatile uint16_t ptc_teperature_value;
	switch(gl_run.process_on_step){


     case 0:  //initial reference 
       gl_run.process_off_step =0 ; //clear power off process step .
       LED_WIFI_ON();
       LED_TEMP_ICON_ON();
       LED_HUM_ICON_OFF();
       Update_Dht11_toDisplayBoard_Value();
	   disp_all_sumg_led();
	   power_on_led();

	   if(g_wifi.app_timer_power_on_flag ==1){
	      
	   	  g_wifi.gwifi_link_net_success=wifi_link_success;
          MqttData_Publish_SetOpen(1);  
		  //vTaskDelay(100);
		}
        else{
		   
		   	 if(g_wifi.gwifi_link_net_success == wifi_link_success && g_wifi.gwifi_normal_power_on_flag == 0){
		       MqttData_Publish_SetOpen(1);  
		       //vTaskDelay(100);
		    }
        }
		if(g_disp.g_second_disp_flag ==1){
		     updateDht11_toDisplayBoard_value();
		}
			 
		   
		   
      if(g_wifi.gwifi_link_net_success == wifi_no_link){//逻辑不严�??//if(g_wifi.gwifi_link_net_success == wifi_no_link || g_wifi.app_timer_power_on_flag == 0)
	      
		   power_on_init_ref();

       }
	   else if(g_wifi.gwifi_link_net_success == wifi_link_success &&  g_wifi.app_timer_power_on_flag == 0){ //has wifi net initial
		  
		   power_on_init_ref();
		
		  
		  updateDht11_toDisplayBoard_value();
	     // vTaskDelay(100);
		   	
	        send_wifi_power_on_state = 1;
		
	   }
	   else{

		    power_on_smart_app_led();

	   }
	   key_referen_init();
	   gl_run.process_on_step =1;

	   break;


	   case 1:
	  LED_WIFI_ON();
	  LED_TEMP_ICON_ON();
      LED_HUM_ICON_OFF();
	   
	   g_pro.gTimer_send_dht11_disp=5;
       
	   
	   g_pro.g_fan_switch_gears_flag++;
	   gl_run.process_off_step=0;
	   //reset wifi 
	   g_wifi.wifi_led_fast_blink_flag=0;

	   //reset temperature value and ptc 
	   g_pro.set_temperature_success_flag=0;
	   g_pro.gset_temperture_value =40; 	//WT.EDIT 2026-05-12
	  
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
	   g_pro.gAI =1;//g_pro.set_timing_or_timer_time_flag=WORKS_TIME; //WT.EDIT 2025.10.18

	 
	   
	   
	   temp_second_displboard=0;

	   gl_run.process_on_step =2;
	 break;

	 case 2:
	 	LED_TEMP_ICON_ON();
        LED_HUM_ICON_OFF();
	    LED_WIFI_ON();
        disp_all_sumg_led();
		power_on_led();
		
	    gl_run.process_on_step =3;

	 break;

	 case 3:

      if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
	
		  if(g_disp.g_second_disp_flag == 1 || temp_second_displboard < 5){

		     if(temp_second_displboard < 8){
                   temp_second_displboard ++;
			 }
			 
		    if(g_pro.gTimer_send_dht11_disp > 2){ //3s
		       g_pro.gTimer_send_dht11_disp=0;
	           updateDht11_toDisplayBoard_value();

		   }
		  }

		  if(send_wifi_power_on_state ==1){
		      send_wifi_power_on_state++;
		      g_pro.gset_temperture_value = 40;
			   MqttData_Publish_Update_Data();
			  // vTaskDelay(200);


		  }
	
		  gl_run.process_on_step =4; 
      }
	  else{
	  
	     fault_handler();
         gl_run.process_on_step =3; 
	  }


	case 4: //DISPAY 3 digital numbers . process .
    
	      ptc_counter ++ ;
		 if(ptc_counter > 10){//100ms *10 = 1000ms =1s
		     ptc_counter =0;
		 	 ptc_teperature_value = ADC_PTC_GetValues();
	         Get_Ntc_Resistance_Temperature_Handler(ptc_teperature_value);
		 }
	    
	    gl_run.process_on_step =5; 
	

	 break;

	 case 5: //WIFI link process
	  
         if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		 
		if(g_wifi.gTimer_update_dht11_data > 20 && g_wifi.gwifi_link_net_success ==wifi_link_success){
		   g_wifi.gTimer_update_dht11_data=0;

		   if(g_wifi.gwifi_link_net_success ==1){

		       switch_dht11 = switch_dht11 ^0x01;
			   if(switch_dht11==1){
	         	   Subscriber_Data_FromCloud_Handler();
			
                   
			   	}
			    else{
				Update_Dht11_Totencent_Value()	;
				


				}
			   
		   	}

          }
		    

         }
		    
	     gl_run.process_on_step =6;

	 break;

	 case 6: // wifi function
	  
         if(g_pro.gTimer_display_adc_value > 5 && g_pro.works_two_hours_interval_flag==0){
		 	g_pro.gTimer_display_adc_value=0;
			send_net_state++;
              adc_detected_hundler();
		 
               if(send_net_state > 2){
			   	 send_net_state=0;
				 if(g_wifi.gwifi_link_net_success==1) {
				     SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
			         osDelay(100);
				 }
				 else{
				     SendWifiData_To_Cmd(0x1F,0); //link wifi order 1 --link wifi net is success.
					  osDelay(100);

				 }
				 
              	}
			 
		  }

      gl_run.process_on_step =7;

	 break;

	 case 7:
     
	    if(g_pro.gTimer_to_disp_counter > 3){    
			 g_pro.gTimer_to_disp_counter=0;
			  Update_Dht11_toDisplayBoard_Value();
		
		}
		
	     gl_run.process_on_step =8;

	 break;

	 case 8:

        works_run_two_hours_state();
	     gl_run.process_on_step =9;

	 break;

	 case 9:
	 	
	    set_temperature_value_handler(); //logic is confuse "set temp ? or timer timing " only displya one.
		 gl_run.process_on_step =10;		
	 break;

	 case 10:
	      set_timer_timing_value_handler();
          gl_run.process_on_step =11;
	 break;

	 case 11:
	 	 smart_phone_timer_power_on_handler();
		 gl_run.process_on_step =12;
	        
	break;

	 case 12:
	 	 led_wifi_connected_net();
	     link_wifi_to_tencent_handler(g_wifi.wifi_led_fast_blink_flag); //detected ADC of value 
         gl_run.process_on_step =3;
	 break;


	
	 default :

	  break;

	}
   
 }

/**********************************************************************
	*
	*Functin Name: void power_off_run_handler(void)
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_off_run_handler(void)
{

   static uint8_t fan_flag,wifi_first_connect,fan_run_one_minute,switch_f;
   static uint8_t dc_on_f;
   switch(gl_run.process_off_step){

   case 0:
   	  gl_run.process_on_step =0;
	
	  g_pro.g_real_hours_counter=0;

   	  power_off_led();
      TM1639_Display_ON_OFF(0);
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
	   g_disp.g_second_disp_flag =0;
	 
	   g_pro.works_two_hours_interval_flag=0; //WT.EDIT 2025.05.07

        gl_run.process_off_step = 1;

   break;

   case 1:
  
         if(dc_on_f==0){
             dc_on_f ++;
			 LL_GPIO_ResetOutputPin(LED_POWER_GPIO_Port, LED_POWER_Pin);
			 buzzer_sound();
		 }
      gl_run.process_off_step = 2;

   break;

   case 2:

     power_off_led();
      TM1639_Display_ON_OFF(0);
	  if(g_wifi.gwifi_link_net_success == wifi_link_success){
            MqttData_Publish_SetOpen(0);  
			//vTaskDelay(100);//osDelay(50);
	        MqttData_Publish_PowerOff_Ref() ;//
	        //vTaskDelay(100);//osDelay(100);
           
	  }
	 
   power_off_led();
		TM1639_Display_ON_OFF(0);

   gl_run.process_off_step = 3;


   break;

   case 3:

     power_off_led();
      TM1639_Display_ON_OFF(0);
   if(g_wifi.gwifi_link_net_success == wifi_link_success){

		 MqttData_Publish_PowerOff_Ref() ;//
		 //vTaskDelay(100);//osDelay(100);
		
   }
    power_off_led();
      TM1639_Display_ON_OFF(0);
   gl_run.process_off_step = 4;

   break;

   case 4:
	 power_off_led();
  

     if(fan_flag == 0){
	 	fan_flag++;
	    fan_run_one_minute =2;
     }
	
	 if(fan_run_one_minute ==1){
	 
		   if(g_pro.gTimer_fan_run_one_minute  < 61){
	 
				Fan_Full_Speed();
		   }
		   else{
	 
			  fan_run_one_minute++;
			  FAN_Stop();
	 
		   }
	 
	   }
	

     mainboard_close_all_fun();
	

	 wifi_first_connect++;

	 if(g_wifi.gwifi_link_net_success == wifi_link_success && wifi_first_connect > 250){
	 	    wifi_first_connect=0;
			switch_f = switch_f ^ 0x01;
	        if(switch_f==1)
              MqttData_Publish_SetOpen(0);  
		    else
	          MqttData_Publish_PowerOff_Ref() ;//
	      
           
	 }

   

   break;

   
   	}

}



