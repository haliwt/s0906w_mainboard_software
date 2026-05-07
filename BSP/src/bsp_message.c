/*
 * bsp_message.c
 *
 *  Created on: 2025�??3�??4�??
 *      Author: Administrator
 */
#include "bsp.h"

static void copy_receive_data(uint8_t *pdata);

//uint8_t ptc_on_default=0xff, ptc_off_default=0xff;
/**********************************************************************
    *
    *Function Name:uint8_t bcc_check(const unsigned char *data, int len) 
    *Function: BCC校验函数
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
uint8_t bcc_check(const unsigned char *data, int len) 
{
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}
/**********************************************************************
    *
    *Function Name:void receive_data_from_displayboard(uint8_t *pdata,uint8_t len)
    *Function: receive usart touchkey of command 
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
void receive_data_from_displayboard(uint8_t *pdata)
{

   static uint8_t ptc_on_default=0xff, ptc_off_default=0xff;

    switch(pdata[2]){

     case 0:


     break;

     case 0x01: //表示�??机指�??

       if(pdata[3] == 0x01){ 
		  g_pro.disp_second_f = 1;

	      if(g_disp.soft_version ==0){//
             g_pro.gpower_on = power_on;
		  }
	
	
          buzzer_sound();
		 
		   SendWifiData_Answer_Cmd(CMD_POWER,0x01); //WT.EDIT 2025.01.07 
           tx_thread_sleep(10);
          	
        }
        else{ //close 
         
          buzzer_sound();
		  g_pro.disp_second_f = 1;
		  g_pro.gpower_on = power_off;
        
		   SendWifiData_Answer_Cmd(CMD_POWER,0x0); //WT.EDIT 2025.01.07
		   tx_thread_sleep(10);
		 	

        }

    	

     break;

	 

	 case 0x10: //显示板已经开机或者关机,再次,发送给主机开机和关机 
          if(pdata[3] == 0x01){ //open

		   
		   g_pro.gpower_on = power_on;
	       
		   SendWifiData_Answer_Cmd(0x10,0x01);
	       tx_thread_sleep(10);
	       
	         
	    }
        else if(pdata[3] == 0x0){ //close 

			  SendWifiData_Answer_Cmd(0x10,0x0); //power off .

               tx_thread_sleep(10); 
              
      
             
            g_pro.gpower_on = power_off;
			 
		     
        }

	 break;
	/*****************************power on off end**************************************/	

     //function transmit information 
     case 0x02: //PTC打开关闭指令

    // if(pdata[3] == 0x00){ //判断是否是数据，或�?�指令�?�知�?? 00- 命令和指令，下一个字节是指令 �??0x0F- 数据，下�??个字节是数据个数
	 	if(pdata[3]==0x01){
			 g_pro.disp_second_f=1;
	 	if(g_pro.gpower_on == power_on){
		 
          buzzer_sound();
          g_pro.gDry = 1;
		   LED_DRY_ON();
		  //manual close flag :
		 
		  g_pro.g_manual_shutoff_dry_flag = 0;
		  if(g_pro.works_two_hours_interval_flag==0){
		      DRY_OPEN();
		  }
		
		   if(g_pro.disp_second_f ==1){
		   	  SendWifiData_Answer_Cmd(CMD_PTC,0x01); //WT.EDIT 2025.01.07
		      tx_thread_sleep(10);
		   	}
    
         if(g_wifi.gwifi_link_net_success==wifi_link_success && ptc_on_default != g_pro.gDry){
		 	  ptc_on_default = g_pro.gDry;
              MqttData_Publish_SetPtc(0x01);
	  	      tx_thread_sleep(20);//HAL_Delay(350);
          }
       
       }
       }
       else if(pdata[3] == 0x0){
	   	g_pro.disp_second_f=1;
	   	 if(g_pro.gpower_on == power_on){
		  g_pro.g_manual_shutoff_dry_flag = 1;
          buzzer_sound();
          g_pro.gDry =0;
		  LED_DRY_OFF();
          DRY_CLOSE();
		  if(g_pro.disp_second_f ==1){
		  SendWifiData_Answer_Cmd(CMD_PTC,0x0); //WT.EDIT 2025.01.07
		   tx_thread_sleep(10);
		  }
            
         if(g_wifi.gwifi_link_net_success==wifi_link_success && ptc_off_default != g_pro.gDry){
		 	  ptc_off_default = g_pro.gDry;
              MqttData_Publish_SetPtc(0x0);
	  	      tx_thread_sleep(20);//HAL_Delay(350);
          }
	   	 }
       }
     
     break;

	 case 0x03: //PLASMA 打开关闭指令

      
	    if(pdata[3]==0x01){
			g_pro.disp_second_f=1;
       if(g_pro.gpower_on == power_on){
	   	  
            buzzer_sound();
			g_pro.gPlasma = 1;
		    LED_PLASMA_ON();
		    if(g_pro.works_two_hours_interval_flag==0){
                PLASMA_OPEN();
		   }
		if(g_wifi.gwifi_link_net_success==1){
           MqttData_Publish_SetPlasma(1);
		       tx_thread_sleep(20);//HAL_Delay(350);
          }
           
        }
        }
        else if(pdata[3] == 0x0){
			g_pro.disp_second_f=1;
        if(g_pro.gpower_on == power_on){ 
            buzzer_sound();
			g_pro.gPlasma = 0;
		    LED_PLASMA_OFF();
		     PLASMA_CLOSE();
		   
		    if(g_wifi.gwifi_link_net_success==1){
              MqttData_Publish_SetPlasma(0);
		       tx_thread_sleep(20);//HAL_Delay(350);
            }
           
        }
        }
       	
      break;


      case 0x04: //ultrasonic  打开关闭指令

        // 00-》表示是指令或�?��?�知，不是数据，下一个数据就是命令或者�?�知
	 	if(pdata[3]==0x01){
          if(g_pro.gpower_on == power_on){ 
            buzzer_sound();
			g_pro.gMouse = 1;
		    LED_MOUSE_ON();
		    if(g_pro.works_two_hours_interval_flag==0){
                mouse_open();
		   }
		if(g_wifi.gwifi_link_net_success==1){
           MqttData_Publish_SetUltrasonic(1);
		       tx_thread_sleep(20);//HAL_Delay(350);
          }
           
          }
       }
        else if(pdata[3] == 0x0){ //close 
		 if(g_pro.gpower_on == power_on){ 
					buzzer_sound();
					g_pro.gMouse = 0;
					LED_MOUSE_OFF();
					mouse_close();
          if(g_wifi.gwifi_link_net_success==1){
				MqttData_Publish_SetUltrasonic(0);
			    tx_thread_sleep(20);//HAL_Delay(350);
			}

        }
       }
    
       	
     break;

      case 0x05: // link wifi command

 
	 	if(pdata[3]==0x01){
		   g_pro.disp_second_f=1;
        if(g_pro.gpower_on == power_on){ 
		  if(g_pro.disp_second_f ==1){
		  	SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
		  tx_thread_sleep(10);
		  	}
          buzzer_sound();
		  
        
	      g_wifi.gTimer_wifi_led_fast_blink = 0; //look for wifi information 120s,timer.
	      g_wifi.gwifi_link_net_success=0 ; //clear wifi link net flag .repeat be detected wifi state.
		  g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .
          g_wifi.link_net_step = 0; //WT.EIDT 2025.05.1;
          g_pro.first_connect_wifi_flag=0;
         }
        
       	}
       	

     break;

     case 0x06: //buzzer sound command 

        
        if(pdata[3]==0x01){
		   g_pro.disp_second_f=1;
           buzzer_sound();
        }
       

     break;

	 

     case 0x11: //notice has outside the second connector display board 

        if(pdata[3]==0x01){
		   g_pro.disp_second_f=1;
          
        }
       

     break;


     case 0x16 : //buzzer sound command with answer .

      
         if(pdata[3]==0x01){
          g_pro.disp_second_f=1;
          buzzer_sound();
          if(g_pro.disp_second_f ==1){SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.01.07
          tx_thread_sleep(10);
          	}
          

          }
	      
      break;

	  case 0x19 : //works two hours flag bit is notice.
       #if 0
	    if(pdata[3]==0x01){
		

		    g_pro.delay_run_adc_counter=0;
			g_pro.gTimer_two_hours_counter= 0;
		    g_pro.g_fan_switch_gears_flag++;
		    g_pro.works_two_hours_interval_flag=1;

			PLASMA_CLOSE(); //
			DRY_CLOSE();
			mouse_close();
		    g_pro.gTimer_fan_run_one_minute=0;
		   

			g_pro.one_minutes_fan_flag=1;
           
           

	    }
		else{
		  g_pro.gTimer_two_hours_counter =0; 
		 
		 g_pro.delay_run_adc_counter=0;
         g_pro.works_two_hours_interval_flag=0;
         mainboard_special_fun();

        }
      #endif 


	  break;


      case 0x1A: //read 温度数据

       if(pdata[3] == 0x0F){ //数据
       
	     

          

	   }
      break;

      case 0x1B: //湿度数据

        if(pdata[3] == 0x0F){ //数据
            

        }
      break;

      case 0x1C: //表示时间：小时，分，�??

        if(pdata[3] == 0x0F){ //数据



        }
      break;

        case 0x1D: //表示日期�?? 年，月，�??

        if(pdata[3] == 0x0F){ //数据

             
            

        }
      break;

	 case 0x27: //smart phone set AI mode

      if(pdata[3] == 0x02){ //don't AI
       
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
      
       }


     break;


	  case 0x22: //notice  don't command,温度对比后的指令

       if(pdata[3] == 0x01){
        
        if(g_pro.gpower_on == power_on && g_pro.g_manual_shutoff_dry_flag ==0){
         g_pro.disp_second_f=1;
        g_pro.gDry = 1;
		LED_DRY_ON();
     	if(g_pro.works_two_hours_interval_flag==0 ){
		  	DRY_OPEN();
     	 }
         if(g_wifi.gwifi_link_net_success==1 && ptc_on_default != g_pro.gDry){
		 	  ptc_on_default = g_pro.gDry;
              MqttData_Publish_SetPtc(0x01);
	  	      tx_thread_sleep(20);//HAL_Delay(350);
         }
       
       
		}
	  }
      else if(pdata[3] == 0x0){
        if(g_pro.gpower_on == power_on){

            g_pro.disp_second_f=1;
            g_pro.gDry =0;
		    LED_DRY_OFF();
          	DRY_CLOSE();
		  
		
            
         if(g_wifi.gwifi_link_net_success==1 && ptc_off_default != g_pro.gDry){
		 	  ptc_off_default = g_pro.gDry;
              MqttData_Publish_SetPtc(0x0);
	  	      tx_thread_sleep(20);//HAL_Delay(350);
          }
	   	 
       
      }
		}
	  	
     break;

	  
	 case 0x2A: //display board set up tempeature value send data to mainboard
	 
			if(pdata[4] == 0x01){ //数据
  				if(pdata[5] < 41 && pdata[5]> 19 && g_pro.gpower_on == power_on){
				   
			
	                g_pro.g_manual_shutoff_dry_flag =0;
					g_pro.first_set_ptc_on=0;//recoder over set up temperature value .
	               
					g_pro.key_set_temperature_flag = 1;
					g_pro.gTimer_input_set_temp_timer=0;
					g_pro.gset_temperture_value = pdata[5];
					g_wifi.wifi_set_temperature_value = pdata[5];

					if(g_pro.gset_temperture_value > g_pro.g_temperature_value){
                   
				           g_pro.gDry = 1;
		                   LED_DRY_ON();
					       if(g_pro.works_two_hours_interval_flag==0){
					        DRY_OPEN();
					       }

										       
				         if(g_wifi.gwifi_link_net_success==1 && ptc_on_default != g_pro.gDry){
						 	  ptc_on_default = g_pro.gDry;
				              MqttData_Publish_SetPtc(0x01);
					  	      tx_thread_sleep(20);//HAL_Delay(350);
				          }
									      

					}
					else{

					    g_pro.gDry = 0;
						LED_DRY_OFF();
					    DRY_CLOSE();

										    
				         if(g_wifi.gwifi_link_net_success==1 && ptc_off_default != g_pro.gDry){
						 	  ptc_off_default = g_pro.gDry;
				              MqttData_Publish_SetPtc(0x0);
					  	      tx_thread_sleep(20);//HAL_Delay(350);
				          }

					 }
					if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
				      TM1639_Display_Temperature(g_pro.gset_temperture_value);
				  
                    }
				
             }

		}
				
	 
			
	 break;

	  case 0x2B: //set up timer timing value 
		
         if(pdata[4]==0x01 && g_pro.gpower_on == power_on){ // has dat only one value ,next receive byte is value

		        g_pro.disp_second_f=1;

			    if(pdata[5] > 0){
				g_pro.gAI = 0;
				LED_AI_OFF();
                g_pro.gdisp_timer_hours_value = pdata[5];
			
		
				g_pro.gTimer_switch_set_timer_times = 0;
			 
				

				g_pro.switch_disp_time_or_temp_item = temperature_mode; //define UP and down key is set temperature value 
			    g_pro.set_timing_or_timer_time_flag=TIMER_TIME;
			    g_pro.gTimer_timer_time_second=0;
				g_pro.gdisp_timer_minutes_value=0;
				  
			  
			    //g_pro.g_disp_smg_timer_or_temp_hours_item = timer_time_mode;//input_set_timer_mode;//WT.EDIT 2025.04.23//input_temp_time_mode  ;
                if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
				//TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
				TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
				  
                }
			 }
			else{
	           

			    g_pro.gAI = 1;
				LED_AI_ON();

				g_pro.key_add_dec_be_pressed_flag=0xf0;
				
				g_pro.switch_disp_time_or_temp_item = temperature_mode; //WT.EDIT 2025.10.17
			
				g_pro.gTimer_switch_set_timer_times = 0;
			
				g_pro.gdisp_timer_hours_value = 0;
				g_pro.gdisp_timer_minutes_value=0;

				g_pro.gTimer_timer_time_second=0;
				
				
				g_pro.set_timing_or_timer_time_flag = WORKS_TIME;
				

				if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
					//TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
					TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
                    tx_thread_sleep(50);
				}
	           	
		   		
		   }
		}
     break;

	 case 0x6C: // display  Synchronization Time don't beijing timing.
		if(pdata[4] == 0x03){ //数据

			if(pdata[5] < 25 && pdata[6] < 61 && pdata[7] < 61){
              
	              g_pro.disp_second_f=1;
                  g_pro.gdisp_hours_value = pdata[5];
                  g_pro.gdisp_minutes_value=pdata[6];
				  g_pro.gTimer_disp_time_second=pdata[7];

             
			}
		} 
		
	 	
     break;


	 case 0x6B: // display  Synchronization Timer  Time
		if(pdata[4] == 0x03){ //数据
            g_pro.disp_second_f=1;
		    g_pro.gdisp_timer_hours_value = pdata[5];
			g_pro.gdisp_timer_minutes_value=pdata[6];
			g_pro.gTimer_timer_time_second=pdata[7];
			
			}
		 
		
	 	
     break;

	 
	 	
	 case 0xF0: //software version difference older and new sotfware 
		  
		   g_disp.soft_version = pdata[3];
		 
		  // printf("gpro_t.soft_version = %d\r\n",gpro_t.soft_version);
	 
		  
	break;



     case 0xFF: //copy send cmd acknowlege

	 copy_receive_data(pdata);

	       
     break;

	 default:
	 break;
        
     
     }

 }
 


/********************************************************************
	*
	*Function Name: static void copy_receive_data(uint8_t cmd,uint8_t data)
	*Function: receive display board command .     		   
	*Input Ref:cmd-display board command . data- 1:open or 0:close
	*Return Ref:NO
	*
*********************************************************************/
static void copy_receive_data(uint8_t *pdata)
{
     switch(pdata[3]){

       case CMD_POWER:

   
	   break;
	  

	   case CMD_PTC :
	   	if(pdata[4] == 1){

		   buzzer_sound();
		   g_pro.gDry=1;
		   LED_DRY_ON();
		   if(g_pro.works_two_hours_interval_flag ==0){

               DRY_OPEN();
              }

		  }
		  else{
			  g_pro.gDry=0;

              DRY_CLOSE();
              LED_DRY_OFF();

		 }

	   break;

	   case CMD_CONNECT_WIFI:
	   	if(pdata[4] == 1){
			buzzer_sound();
           // g_key.key_long_power_flag =  KEY_LONG_POWER; //wifi led blink fast .
			g_wifi.gTimer_wifi_led_fast_blink = 0; //time start 120s ->look for wifi information 120s,timer.
			g_wifi.gwifi_link_net_success=0 ; //clear wifi link net flag .repeat be detected wifi state.
			g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .

		}
		else{



		}

	   break;

	   case 0x11:

	     if(pdata[4] == 1){
           g_pro.disp_second_f = 1;
	     }
	  
	   break;


	 }


}


