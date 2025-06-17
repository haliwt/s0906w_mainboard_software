/*
 * bsp_message.c
 *
 *  Created on: 2025�??3�??4�??
 *      Author: Administrator
 */
#include "bsp.h"

static void copy_receive_data(uint8_t cmd,uint8_t type,uint8_t mycmd);


uint8_t power_off_test_counter;
uint8_t temperature_value;
uint8_t power_on_counter;

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
  
   if(pdata[1] == 0x02){ //the second display board DEVICE_ID 

    switch(pdata[2]){

     case 0:


     break;

     case 0x01: //表示�??机指�??

        if(pdata[3] == 0x00){ // comand 判断是数据还是命�??

		
          if(pdata[4] == 0x01){ 
		  g_disp.g_second_disp_flag = 1;
		  g_pro.gpower_on = power_on;
		  power_on_counter++;
          buzzer_sound();
		 
		 
          SendWifiData_Answer_Cmd(CMD_POWER,0x01); //WT.EDIT 2025.01.07 
          osDelay(5);
        }
        else{ //close 
         
          buzzer_sound();
		  g_disp.g_second_disp_flag = 1;
		  g_pro.gpower_on = power_off;
          power_off_test_counter++;
		
		 SendWifiData_Answer_Cmd(CMD_POWER,0x0); //WT.EDIT 2025.01.07
		 osDelay(5);

        }

    	}

     break;


	 case 0x11: //has the second display board exist .this notice 
		   if(pdata[3] == 0x00){ // comand 判断是数据还是命�??
		   
				  
		if(pdata[4] == 0x01){ 

	       g_disp.g_second_disp_flag = 1;

		}
	}
	 break;

     case 0x02: //PTC打开关闭指令

     if(pdata[3] == 0x00){ //判断是否是数据，或�?�指令�?�知�?? 00- 命令和指令，下一个字节是指令 �??0x0F- 数据，下�??个字节是数据个数
	 	if(pdata[4]==0x01){
	 	if(g_pro.gpower_on == power_on){
		 
          buzzer_sound();
          g_pro.gDry = 1;
		   LED_DRY_ON();
		  //manual close flag :
		   SendWifiData_Answer_Cmd(CMD_PTC,0x01); //WT.EDIT 2025.01.07
		  g_pro.g_manual_shutoff_dry_flag = 0;
		  if(g_pro.works_two_hours_interval_flag==0){
		      DRY_OPEN();
		  }
		  g_pro.gTimer_set_temp_counter=1;
		 //compare_temperature_value_hanlder();
    
         if(g_wifi.gwifi_link_net_state_flag==wifi_link_success){
              MqttData_Publish_SetPtc(0x01);
	  	      osDelay(50);//HAL_Delay(350);
          }
       
       }
       }
       else if(pdata[4] == 0x0){
	   	 if(g_pro.gpower_on == power_on){
		  g_pro.g_manual_shutoff_dry_flag = 1;
          buzzer_sound();
          g_pro.gDry =0;
		  LED_DRY_OFF();
          DRY_CLOSE();
		  if(g_disp.g_second_disp_flag ==1){
		  SendWifiData_Answer_Cmd(CMD_PTC,0x0); //WT.EDIT 2025.01.07
		   osDelay(5);
		  }
            
         if(g_wifi.gwifi_link_net_state_flag==wifi_link_success){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(50);//HAL_Delay(350);
          }
	   	 }
       }
     	}
     break;

	 


	  case 0x22: //notice cmd ,PTC打开关闭指令,buzzer don't sound,温度对比后的指令

	  if(pdata[3]==0){ //表示是指�??

      if(pdata[4] == 0x01){
        
        if(g_pro.gpower_on == power_on){

        g_pro.gDry = 1;
		LED_DRY_ON();
     	if(g_pro.works_two_hours_interval_flag==0){
		  	DRY_OPEN();
     	 }

	 	 
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x01);
	  	      osDelay(50);//HAL_Delay(350);
          }
       
       
				}
	  }
      else if(pdata[4] == 0x0){
        if(g_pro.gpower_on == power_on){

         
            g_pro.gDry =0;
		    LED_DRY_OFF();
          	DRY_CLOSE();
		  
		
            
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(50);//HAL_Delay(350);
          }
	   	 
       
      }
		}
	  	}
     break;

     case 0x03: //PLASMA 打开关闭指令

       if(pdata[3] == 0x00){
	    if(pdata[4]==0x01){
       if(g_pro.gpower_on == power_on){
	   	  
            buzzer_sound();
			g_pro.gPlasma = 1;
		    LED_PLASMA_ON();
		    if(g_pro.works_two_hours_interval_flag==0){
                PLASMA_OPEN();
		   }
		if(g_wifi.gwifi_link_net_state_flag==1){
           MqttData_Publish_SetPlasma(1);
		       osDelay(50);//HAL_Delay(350);
          }
           
        }
        }
        else if(pdata[4] == 0x0){
        if(g_pro.gpower_on == power_on){ 
            buzzer_sound();
			g_pro.gPlasma = 0;
		    LED_PLASMA_OFF();
		     PLASMA_CLOSE();
		   
		    if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPlasma(0);
		       osDelay(50);//HAL_Delay(350);
            }
           
        }
        }
       	}
      break;


      case 0x04: //ultrasonic  打开关闭指令

       if(pdata[3] == 0x00){ // 00-》表示是指令或�?��?�知，不是数据，下一个数据就是命令或者�?�知
	 	if(pdata[4]==0x01){
          if(g_pro.gpower_on == power_on){ 
            buzzer_sound();
			g_pro.gMouse = 1;
		    LED_MOUSE_ON();
		    if(g_pro.works_two_hours_interval_flag==0){
                mouse_open();
		   }
		if(g_wifi.gwifi_link_net_state_flag==1){
           MqttData_Publish_SetUltrasonic(1);
		       osDelay(50);//HAL_Delay(350);
          }
           
          }
       }
        else if(pdata[4] == 0x0){ //close 
		 if(g_pro.gpower_on == power_on){ 
					buzzer_sound();
					g_pro.gMouse = 0;
					LED_MOUSE_OFF();
					mouse_close();
          if(g_wifi.gwifi_link_net_state_flag==1){
				MqttData_Publish_SetUltrasonic(0);
			    osDelay(50);//HAL_Delay(350);
			}

        }
       }
    
       	}
     break;

      case 0x05: // link wifi command

       if(pdata[3] == 0x00){
	 	if(pdata[4]==0x01){
        if(g_pro.gpower_on == power_on){ 
		  SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
		  osDelay(5);
          buzzer_sound();
		  
        
	      g_wifi.gTimer_wifi_led_fast_blink = 0; //look for wifi information 120s,timer.
	      g_wifi.gwifi_link_net_state_flag=0 ; //clear wifi link net flag .repeat be detected wifi state.
		  g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .
          g_wifi.link_net_step = 0; //WT.EIDT 2025.05.1;
          g_pro.first_connect_wifi_flag=0;
         }
        
       	}
       	}

     break;

     case 0x06: //buzzer sound command 

        if(pdata[3] == 0x00){  //buzzer sound 
          if(pdata[4]==0x01){
           buzzer_sound();
        }
        else if(pdata[4] == 0x0){ // don't buzzer sound .
        
        }
    	}

     break;


	 case 0x10: //has two display board.

	   if(pdata[3] == 0x00){ //notice message.
	   	   if(pdata[4]==0x01){
           g_disp.g_second_disp_flag = 1; 

	       }
		   else{
		   g_disp.g_second_disp_flag = 0; 
		   	}
	   	}
		
     break;

     case 0x16 : //buzzer sound command with answer .

      
       if(pdata[3] == 0x00){  //buzzer sound 

	      if(pdata[4]==0x01){

          if(g_pro.gpower_on == power_on){  
          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.01.07
          osDelay(5);
          buzzer_sound();

          }
	      }
       	}


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

	  
	 case 0x2A: //display board set up tempeature value send data to mainboard
	 
			if(pdata[3] == 0x0F){ //数据

			   if(pdata[4]==0x01){ // has dat only one value ,next receive byte is value
	         
			   if(g_pro.gpower_on == power_on){ 

			    g_pro.g_dispboard_set_temp_flag = 1;
				g_pro.key_set_temperature_flag=1;
                g_pro.g_manual_shutoff_dry_flag =0;
               
				g_pro.set_temperature_value_success = 1;
				g_pro.gTimer_input_set_temp_timer=0;
				
			    if(pdata[5] < 41 && pdata[5]> 19){
					g_pro.gset_temperture_value = pdata[5];
					
					g_wifi.wifi_set_temperature_value = pdata[5];
			    }
				g_pro.gTimer_set_temp_counter=10;
		        compare_temperature_value_hanlder();

      
				if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
				   TM1639_Display_Temperature(g_pro.gset_temperture_value);
				  

				}
				
             }

			 }
				
	 
			}
	 break;

	  

    

     case 0x27: //smart phone set AI mode

      if(pdata[3] == 0x02){ //don't AI
       
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
      
       }


     break;

	case 0x2B: //set up timer timing value 
		if(pdata[3] == 0x0F){ //数据

			if(pdata[4]==0x01){ // has dat only one value ,next receive byte is value

			if(g_pro.gpower_on == power_on){ 
                g_pro.gdisp_timer_hours_value = pdata[5];
				g_key.mode_key_switch_time_mode = timer_time_mode;
				g_pro.gTimer_switch_set_timer_times = 0;
			
				g_pro.timer_powerOff_oneHour_flag=0;//only one hour don't 00H,that  "01H",flag

				g_pro.gTimer_timer_time_second=0;
				 g_pro.disp_timer_minutes_value=0;
				g_pro.key_set_timer_flag=TIMER_TIME;
			  
			    g_pro.g_disp_smg_timer_or_temp_hours_item = timer_time_mode;//input_set_timer_mode;//WT.EDIT 2025.04.23//input_temp_time_mode  ;
                if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
				TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
                }
			 }

			}
			else{

			g_pro.gdisp_timer_hours_value = 0;
			g_key.mode_key_switch_time_mode = works_time_mode;
			g_pro.gTimer_switch_set_timer_times = 0;
		
			g_pro.timer_powerOff_oneHour_flag=0;//only one hour don't 00H,that	"01H",flag

			g_pro.gTimer_timer_time_second=0;
			 g_pro.disp_timer_minutes_value=0;
			g_pro.key_set_timer_flag=WORKS_TIME;
		  
			g_pro.g_disp_smg_timer_or_temp_hours_item = works_time_mode;//input_set_timer_mode;//WT.EDIT 2025.04.23//input_temp_time_mode  ;
			if(g_pro.fan_warning ==0 && g_pro.ptc_warning==0){
			TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
			}
       }
	 	
     break;

	 case 0x5C: // display has been set up timer timing value 
		if(pdata[3] == 0x0F){ //数据

			if(pdata[4]==0x03){ // has dat only one value ,next receive byte is value

				if(g_pro.gpower_on == power_on){

                if(g_pro.gdisp_timer_hours_value > 1){
				   g_pro.gdisp_timer_hours_value = pdata[5];

                }
				g_pro.disp_timer_minutes_value=pdata[6];
				g_pro.gTimer_timer_time_second=pdata[7];
				}
			}
		}
	 	
     break;

     case 0xFF: //copy send cmd acknowlege

	 copy_receive_data(pdata[3],pdata[4],pdata[5]);

	       
     break;

	 default:
	 break;
        
     
     }

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
static void copy_receive_data(uint8_t cmd,uint8_t type,uint8_t mycmd)
{
     switch(cmd){

       case CMD_POWER:

   
	        if(type == 0x0){
				if(mycmd ==1){
                 g_disp.g_second_disp_flag =1;
			    }
                else{
             
			    g_disp.g_second_disp_flag =0;
			   }
            }
	 

	   break;
	  

	   case CMD_PTC :
	   	if(mycmd == 1){

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
	   	if(mycmd == 1){
			buzzer_sound();
            g_key.key_long_power_flag =  KEY_LONG_POWER; //wifi led blink fast .
			g_wifi.gTimer_wifi_led_fast_blink = 0; //time start 120s ->look for wifi information 120s,timer.
			g_wifi.gwifi_link_net_state_flag=0 ; //clear wifi link net flag .repeat be detected wifi state.
			g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .

		}
		else{



		}

	   break;


	 }


}


