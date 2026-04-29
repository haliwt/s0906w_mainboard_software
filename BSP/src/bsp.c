/*
 * bsp.c
 *
 *  Created on: 2025�?3�?4�?
 *      Author: Administrator
 */
#include "bsp.h"


process_t g_pro;

 uint16_t mainboard_time;

 static void mainboard_special_fun(void);
uint8_t counter_flag ;
/******************************************************************************
	*
	*Function Name:void bsp_init(void)
	*Funcion: 
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void bsp_init(void)
{
	 delay_init(64); 
	  DHT11_Init();
	 key_referen_init();
	 g_pro.gset_temperture_value=40; //WT.EDIT 2025.05.05
	 g_pro.gDry = 1;
	 callback_register_fun();
	 callback_register_usart1_rx();
	 callback_register_usart2_rx();
#if(Enable_EventRecorder == 1) 
	/* ³õÊ¼»¯EventRecorder²¢¿ªÆô */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
	 

}

/******************************************************************************
	*
	*Function Name:void mainboard_fun_handler(void)
	*Funcion: dry,plasma,mouse,fan ADC
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void mainboard_fun_handler(void)
{
  static uint8_t ptc_on_default =0xff, ptc_off_default= 0xff;
  if(g_pro.gTimer_mainboard_fun_counter > 4){// 2s  //300 ~= 6s, 50 ~=1s
       g_pro.gTimer_mainboard_fun_counter=0;


  if(g_pro.set_timing_or_timer_time_flag ==TIMER_TIME){
	  g_pro.gAI = 0;
  
      LED_AI_OFF();
      counter_flag ++;

   }
   else{
	
	   g_pro.gAI = 1;
       LED_AI_ON();

	}
   
   	

	if(g_pro.gDry == 1 && g_pro.g_manual_shutoff_dry_flag ==0 && g_pro.works_two_hours_interval_flag ==0){
		DRY_OPEN();
		LED_DRY_ON();
//	    if(g_disp.g_second_disp_flag == 1){
//	    	sendDisplayCommand(0x02,g_pro.gDry); // 关闭干燥功能
//	    	tx_thread_sleep(100);
//	    }
        if(g_wifi.gwifi_link_net_success==1 && ptc_on_default != g_pro.gDry){
			 ptc_on_default = g_pro.gDry;
			 MqttData_Publish_SetPtc(0x01);
		  	 tx_thread_sleep(200);
		 
         }
	}
	else{
		g_pro.gDry = 0;
		LED_DRY_OFF();
		DRY_CLOSE();
//	    if(g_disp.g_second_disp_flag == 1){
//		    sendDisplayCommand(0x02,g_pro.gDry); // 关闭干燥功能
//		    tx_thread_sleep(100);
//	    }

		if(g_wifi.gwifi_link_net_success==1 && ptc_off_default != g_pro.gDry){
			ptc_off_default = g_pro.gDry;
		 	MqttData_Publish_SetPtc(0x0);
	  		 tx_thread_sleep(200);
		 
         }

	}

	if(g_pro.gMouse == 1){
		LED_MOUSE_ON();
		mouse_open();
	}
	else{
		LED_MOUSE_OFF();
		mouse_close();
	}

	if(g_pro.gPlasma == 1){
		LED_PLASMA_ON();
		PLASMA_OPEN();
	}
	else{
		LED_PLASMA_OFF();
		PLASMA_CLOSE();
	}

	Fan_RunSpeed_Fun();
	
   	}
	
   
}

static void mainboard_special_fun(void)
{
   //static uint16_t mainboard_time;

   if(g_pro.gAI == 1){

   
	   LED_AI_ON();
   	
   	}
    else{
     
	   LED_AI_OFF();

	}

	if(g_pro.gDry == 1 && g_pro.g_manual_shutoff_dry_flag ==0){
		DRY_OPEN();
		LED_DRY_ON();
	}
	else{
		g_pro.gDry =0;
		DRY_CLOSE();
		LED_DRY_OFF();

	}

	if(g_pro.gMouse == 1){
		mouse_open();
		LED_MOUSE_ON();
	}
	else{
		mouse_close();
		LED_MOUSE_OFF();
	}

	if(g_pro.gPlasma == 1){
		PLASMA_OPEN();
		LED_PLASMA_ON();
	}
	else{
		PLASMA_CLOSE();
		LED_PLASMA_OFF();
	}
	
   Fan_RunSpeed_Fun();
	

}

void mainboard_close_all_fun(void)
{
	DRY_CLOSE();
	mouse_close();

	PLASMA_CLOSE();
    TM1639_Display_ON_OFF(0);
	power_off_led();

}


/**********************************************************************
    *
    *Functin Name: void works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void works_run_two_hours_state(void)
{
  

   #if TEST_UNIT
	if(g_pro.gTimer_two_hours_counter > 300 && g_pro.works_two_hours_interval_flag==0){ //five minutes 5x60=300s


   #else 
    if(g_pro.gTimer_two_hours_counter > 7200 && g_pro.works_two_hours_interval_flag==0 ){ //two hours

   #endif 
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

   switch(g_pro.works_two_hours_interval_flag){

    case 1:

   

     #if TEST_UNIT 
	 if(g_pro.gTimer_two_hours_counter  > 600){ //2minutes x 60s = 120s
           
         g_pro.gTimer_two_hours_counter =0;  
		 g_pro.delay_run_adc_counter =0;
         g_pro.works_two_hours_interval_flag=0;
           
     

         mainboard_special_fun();
            
      }
     #else 

      if(g_pro.gTimer_two_hours_counter  > 600){ // 10 minutes *60=  minutes =
         g_pro.gTimer_two_hours_counter =0; 
		 
		 g_pro.delay_run_adc_counter=0;
         g_pro.works_two_hours_interval_flag=0;
         mainboard_special_fun();
              
      }


      #endif 

	 if(g_pro.one_minutes_fan_flag ==1){

	      if(g_pro.gTimer_fan_run_one_minute < 60){
	  
	              Fan_Full_Speed();
				  PLASMA_CLOSE(); //
				  DRY_CLOSE();
	              mouse_close();
	        } 
			else if(g_pro.gTimer_fan_run_one_minute > 59){
	           
			   g_pro.gTimer_fan_run_one_minute=0;
			
			  g_pro.one_minutes_fan_flag=0;
             
			   FAN_Stop();
			   g_pro.delay_run_adc_counter=0;
	       }

	  }

   
    break;

    case 0:
	    mainboard_fun_handler();
	
        
    break;
   }

}



/**********************************************************************
    *
    *Functin Name: void works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void copy_cmd_hanlder(void)
{
    if(g_pro.g_copy_power_onoff_flag == power_on){
		
	   
	    SendWifiData_Answer_Cmd(CMD_POWER,0x01); //WT.EDIT 2025.01.07 
			tx_thread_sleep(5);
      g_pro.g_copy_power_onoff_flag =0xff;

	}
	else if( g_pro.g_copy_power_onoff_flag ==0x02){

	  g_pro.g_copy_power_onoff_flag =0xfe;
	  SendWifiData_Answer_Cmd(CMD_POWER,0x0); //WT.EDIT 2025.01.07 
        tx_thread_sleep(5);

	}

}


/**********************************************************************
    *
    *Functin Name: void works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void  smart_phone_timer_power_on_handler(void)
{
  if(g_wifi.app_timer_power_on_flag ==1){
       g_wifi.app_timer_power_on_flag++; 
       
	    smartphone_timer_power_handler();
		 

   }
   else if(g_wifi.app_timer_power_on_flag==2){
			g_wifi.app_timer_power_on_flag++; 

		    MqttData_Publish_Update_Data();//property_report_phone_timer_on_data();// MqttData_Publish_Update_Data();
	        tx_thread_sleep(100);//HAL_Delay(100);



   }
  

}


/**********************************************************************
    *
    *Functin Name: void fault_handler(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void fault_handler(void)
{

    if(g_pro.fan_warning == 1){
       HUMIDITY_ICON_OFF();
	   TEMP_ICON_OFF();//WT.EDIT 2025.04.28
	   DRY_CLOSE();
	   LED_DRY_OFF();
       SMG_Display_Err(2);
	   tx_thread_sleep(1000);


	}

	if(g_pro.ptc_warning ==1){
         HUMIDITY_ICON_OFF();
		 TEMP_ICON_OFF();//WT.EDIT 2025.04.28
		 DRY_CLOSE();
	     LED_DRY_OFF();
         
	     SMG_Display_Err(1);
	     tx_thread_sleep(1000);
		 


	}


}

