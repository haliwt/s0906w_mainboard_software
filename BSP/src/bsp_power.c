/*
 * bsp_power.c
 *
 *  Created on: 2025�??3�??4�??
 *      Author: Administrator
 */
#include "bsp.h"


// --- 1. 定义任务的时间周期（单位：毫秒，假设基础Tick为1ms） ---
#define PERIOD_DISP_NUMBERS     3    // 10ms*3
#define PERIOD_SMART_PHONE     50    // 10ms* = 2000ms = 2s
#define PERIOD_WORKS_HOURS     400    //  10ms*150 = 1500ms = 1.5s
#define PERIOD_LINK_WIFI       10    //  10ms*250 = 2500ms = 2.5s
#define PERIOD_SET_TEMP        130    //   10ms * 500 = 50000ms = 5s 
#define PERIOD_READ_DHT11      200    //   10ms * 100 = 1000ms = 1s
#define PERIOD_SET_TIMER       4    //   10ms * 130 = 1300ms = 1.3s
#define PERIOD_PERIPHERAL      100     //   10ms* 50 = 500ms
#define PERIOD_TX_WIFI_REF     200
#define PERIOD_READ_PTC       300

// --- 2. 定义分时任务控制结构体 ---
typedef struct {
    uint32_t last_tick;        // 记录上一次真正运行时的系统绝对时间戳
    //uint32_t counter;       // 时间计数器
    uint32_t period;        // 任务运行周期
    void (*task_handler)(void); // 任务函数指针
} TimeSharingTask_t;

static void handler_disp_threee_numbers(void);

static void handler_smart_phone(void);

static void handler_link_wifi(void)	;	

static void handler_set_temperature(void);

static void handler_set_timer(void);
static void handler_read_dht11(void);
static void handler_tx_wifi_ref(void);

static void handler_wifi_led(void);
static void handler_works_hours(void);;
static void handler_read_ptc(void);
static void handler_main_module(void);


// --- 4. 初始化分时任务表 ---
TimeSharingTask_t g_tasks[] = {
    {0, PERIOD_DISP_NUMBERS,       	handler_disp_threee_numbers},
    {0, PERIOD_SMART_PHONE,      	handler_smart_phone},
    {0, PERIOD_WORKS_HOURS,      	handler_works_hours},
    {0, PERIOD_LINK_WIFI,          	handler_link_wifi},
    {0, PERIOD_SET_TEMP,        	handler_set_temperature},
    {0, PERIOD_READ_DHT11,       	handler_read_dht11},
    {0, PERIOD_SET_TIMER,        	handler_set_timer},
    {0, PERIOD_TX_WIFI_REF,        	handler_tx_wifi_ref},
    {0, PERIOD_LINK_WIFI,         	handler_wifi_led},
    {0, PERIOD_READ_PTC,       		handler_read_ptc},
    {0, PERIOD_PERIPHERAL,       	handler_main_module}
   
    
	
};

#define TASK_NUM (sizeof(g_tasks) / sizeof(TimeSharingTask_t))

static void handler_fault(void);
static void power_on_cycle_handler(void);


typedef struct{

  uint8_t process_on_step;
 // uint8_t process_off_step;


}POWER_RUN_STATE;







POWER_RUN_STATE gl_run;

static void power_off_init_handler(void);

static void power_off_cycle_handler(void);


uint8_t dc_power_fan_flag ;
uint8_t fan_run_one_minute;
uint8_t off_time_slot;





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
			//TM1639_Display_Temperature(g_pro.real_temperature_value);  //DHT11_Display_Data(0); //display temperature value 
		    
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
	uint32_t boot_tick=0;
	uint8_t i;
 
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
	    g_pro.two_hours_interval_f=0; //WT.EDIT 2025.05.07
		g_pro.gTimer_two_hours_counter = 0;
		g_pro.gTimer_two_minutes=0;
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

	  #if 1	
        boot_tick = tx_time_get();
		for(i=0;i < TASK_NUM;i ++){

		     g_tasks[i].last_tick = boot_tick;
		}
#endif 
	  gl_run.process_on_step =0xfe; 

	 break;

	 default:
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
void power_off_handler(void)
{
    
	if(g_pro.process_off_step <20){
	power_off_init_handler();
	}
    else{

	
	  power_off_cycle_handler();
    }

  off_time_slot ++;
  if(off_time_slot > 4) off_time_slot = 0;
}

static void power_off_init_handler(void)
{


   static uint8_t power_on_flag=0;
 
   
   switch(g_pro.process_off_step){

   case 0:
    
       if(power_on_flag==0){
             power_on_flag ++;
			
		 buzzer_power_sound();
	       
	   }
      g_pro.process_off_step =1;


   case 1:
   	  gl_run.process_on_step =0;
      g_pro.gpower_on_key_f = 0;
    
	  g_pro.g_real_hours_counter=0;
	  TM1639_Display_ON_OFF(0);
      power_off_led();
      DRY_CLOSE();

	   
	   if(dc_power_fan_flag ==0){
	   	dc_power_fan_flag =1;
		
	    }
	   else{
	   	fan_run_one_minute = 1;
	   	dc_power_fan_flag =2;
	 
	   	}
	   
      g_pro.process_off_step = 2;
   break;


   case 2:
	 // g_key.key_long_power_flag  = 0;
	  g_key.key_long_mode_flag = 0;
	
	  g_pro.switch_disp_time_or_temp_item = temperature_mode;
	
	  g_pro.led_bar =0;

	
	  g_pro.gTimer_fan_run_one_minute =0;

	 
	 
	   g_pro.g_fan_switch_gears_flag++;
      
	 
	   g_wifi.app_timer_power_on_flag =0;
	   g_pro.fan_warning =0 ;
	   g_pro.ptc_warning =0;
	   g_pro.disp_second_f =0;
	 
	   g_pro.two_hours_interval_f=0; //WT.EDIT 2025.05.07
	   g_pro.process_off_step = 3;

   break;
   
   case 3:
        mainboard_close_all_fun();
        g_pro.process_off_step = 4;

   break;

   case 4:
    
   	
	  if(g_wifi.gwifi_link_net_success == wifi_link_success){
            MqttData_Publish_SetOpen(0);  
			tx_thread_sleep(20);
           
	  }
     g_pro.process_off_step = 5;

  break;

  case 5:

   if(g_wifi.gwifi_link_net_success == wifi_link_success){
          
	        MqttData_Publish_PowerOff_Ref() ;//
	        tx_thread_sleep(20);
        }


      g_pro.process_off_step = 0xfe;
  break;

   	}
}



static void power_off_cycle_handler(void)
{
  
  static uint16_t counter_send=0;
  static uint16_t fan_flag,wifi_first_connect,switch_f;

  switch(off_time_slot)
  {
  case 0:
    
     	
  break;
	 
  case 1:
  	
	 if(fan_run_one_minute ==1){
	 
		   if(g_pro.gTimer_fan_run_one_minute  < 61){
	 
				Fan_Full_Speed();
		   }
		   else{
	 
			  fan_run_one_minute++;
			  FAN_Stop();
	 
		   }
	 
	   }
	
   break;

   case 2:
	 
    // LED_Power_Breathing();

   case 3:

      if(g_pro.gTimer_to_disp_counter > 9){//10ms*200 =2000ms =2s
			g_pro.gTimer_to_disp_counter=0;
         read_sensorData();
      }
     

   break;

   case 4:
	 wifi_first_connect++;

	 if(g_wifi.gwifi_link_net_success == wifi_link_success && wifi_first_connect > 350){//10ms * 
	 	    wifi_first_connect=0;
			switch_f = switch_f ^ 0x01;
	        if(switch_f ==1){
             MqttData_Publish_SetOpen(0);  
			 tx_thread_sleep(1);
	        }
		    else{
	         MqttData_Publish_PowerOff_Ref() ;//
	          tx_thread_sleep(1);
		    }
	       
           
	 }



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
void power_on_handler(void)
{
   if(gl_run.process_on_step < 20){
        power_on_initial();

   }
   else{
     power_on_cycle_handler();

   }

}
/**********************************************************************
	*
	*Functin Name: 
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
static void power_on_cycle_handler(void)
{

     // 获取当前系统的绝对时间戳
      uint32_t current_tick = tx_time_get();
// ✨【新增：紧急事件拦截响应】✨
			// 如果按键任务设置完温度，将 g_pro.g_immediate_heat_f 置为 1
			if (g_pro.g_immediate_heat_f == 1)
			{
				g_pro.g_immediate_heat_f = 0; // 立即清除触发标志，防止重复执行
				
				// 强制、立刻执行一次加热控制函数
				// 确保底层硬件（如继电器、PWM、PTC）在 20ms 内得到响应
				immediatley_temp_comare_value();//set_temperature_value_handler(); 
			}

     // 通过时间片轮询核心算法，分时调用各个功能模块
    for (uint8_t i = 0; i < TASK_NUM; i++) 
    {
        if ((current_tick - g_tasks[i].last_tick) >= g_tasks[i].period) 
        {
            // 【工业级进化：防轰炸饱和截断】
            // 如果卡顿/被高优先级抢占的时间超过了 2 个周期，直接对齐当前时间，放弃追赶
            if ((current_tick - g_tasks[i].last_tick) > (g_tasks[i].period * 2)) 
            {
                g_tasks[i].last_tick = current_tick;
            }
            else 
            {
                // 如果只是正常范围内的轻微抖动，滚动累加周期，死锁锁相，消除长期长跑漂移
                g_tasks[i].last_tick += g_tasks[i].period;
            }
            
            // 触发对应周期的执行函数（确保不为 NULL，防止空指针崩溃）
            if (g_tasks[i].task_handler != NULL)
            {
                g_tasks[i].task_handler(); 
            }
        }
    }



}

/**********************************************************************
	*
	*Functin Name: 
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
static void handler_disp_threee_numbers(void)
{
	display_digital_3_numbers();
}
	
static void handler_smart_phone(void)
{
    smart_phone_timer_power_on_handler();

}

static void handler_link_wifi(void)		
{
      link_wifi_to_tencent_handler(g_wifi.wifi_led_fast_blink_flag);
}		


static void handler_set_temperature(void)
{
    set_temperature_value_handler();

}

static void handler_set_timer(void)
{
      set_timer_timing_value_handler();

}

static void handler_read_dht11(void)
{
  if(g_pro.gTimer_to_disp_counter > 4){//10ms*200 =2000ms =2s
	g_pro.gTimer_to_disp_counter=0;
	read_sensorData();//Update_Dht11_toDisplayBoard_Value();

 }
}
		

static void handler_tx_wifi_ref(void)
{
	static uint8_t switch_dht11 =0;	
        if( g_pro.fan_warning ==0 && g_pro.ptc_warning ==0){
		 
		if(g_wifi.gTimer_update_dht11_data > 6 && g_wifi.gwifi_link_net_success ==wifi_link_success){
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

}


static void handler_fault(void)
{
           
         fault_handler();
}

static void handler_wifi_led(void)
{

		 wifi_led_slowly_blink_handler();

}

static void handler_works_hours(void)
{
	works_run_two_hours_state();
		

}	
        
static void handler_read_ptc(void)
{
  uint16_t ptc_teperature_value ;
  uint8_t  err_counter =0;
  	
	ptc_teperature_value = ADC_PTC_GetValues();
	Get_Ntc_Resistance_Temperature_Handler(ptc_teperature_value);

	if(g_pro.read_ntc_temperature_value > 111 ){
	   err_counter++;
	  if(err_counter > 1){
	  	 err_counter =0;
	    g_pro.ptc_warning = 1;

	  }

	}
	else{
	 err_counter =0;

	}
}

static void handler_main_module(void)
{
     mainboard_fun_handler();
    send_wifi_set_temperature();

}	

	

      

		
		  
       
	



