/*
 * bsp_key.c
 *
 *  Created on: 2025�????3�????4�????
 *      Author: Administrator
 */
#include "bsp.h"


#define MAX_TEMPERATURE 	40
#define MIN_TEMPERATURE 	20
#define MAX_TIMER_HOURS 	24
#define MIN_TIMER_HOURS 	0
#define TIMER_SECONDS_PER_MINUTE 60

#define CHECK_TIME_THRESHOLD_4S  3  // 4�????
#define CHECK_TIME_THRESHOLD_3S  150  // 3�????
#define TEMPERATURE_HIGH_THRESHOLD  39  // 高温阈�??
#define TEMPERATURE_LOW_THRESHOLD   38  // 低温阈�??
#define TEMPERATURE_DIFF_THRESHOLD  3   // 温度差阈�????



KEY_PROCESS_TYPEDEF  g_key;

int8_t  gl_timer_minutes_value;
uint8_t define_timer_mode;

uint8_t ptc_on_flag =0xff,ptc_off_flag=0xff;



static void adjust_temperature(int8_t delta) ;
static void adjust_timer(int8_t delta) ;
static void handleSetTemperatureControl(void) ;
static void handleDefaultTemperatureControl(void);
static void setDryState(uint8_t state);
static void publishMqttData(DryState state, uint8_t temperature);

static void set_timer_mode(void);
//static void set_normal_mode(void);


uint8_t readTemperature(void);



uint8_t timer_power_off_flag;

void key_referen_init(void)
{
  g_pro.set_temperature_success_flag=0;
  gl_timer_minutes_value =0;
  define_timer_mode=0;
  g_pro.key_set_temperature_flag=0;
  g_pro.key_add_dec_be_pressed_flag=0;
  g_pro.set_timing_or_timer_time_flag=WORKS_TIME; //WT.EDIT 2025.10.18
 
  
}

/**
 * @brief       设置GPIO某个引脚的输出状�????
 * @param       p_gpiox: GPIOA~GPIOG, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, �????0位代表Px0, �????1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @param       status: 0/1, 引脚状�??(仅最低位有效), 设置如下:
 *   @arg       0, 输出低电�????
 *   @arg       1, 输出高电�????
 * @retval      �????
 */
void sys_write_gpio_pin_value(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status)
{
    if (status & 0X01)
    {
        p_gpiox->BSRR |= pinx;  /* 设置GPIOx的pinx�????1 */
    }
    else
    {
        p_gpiox->BSRR |= (uint32_t)pinx << 16;  /* 设置GPIOx的pinx�????0 */
    }
}

/**
 * @brief       读取GPIO某个引脚的状�????
 * @param       p_gpiox: GPIOA~GPIOG, GPIO指针
 * @param       0X0000~0XFFFF, 引脚位置, 每个位代表一个IO, �????0位代表Px0, �????1位代表Px1, 依次类推. 比如0X0101, 代表同时设置Px0和Px8.
 *   @arg       SYS_GPIO_PIN0~SYS_GPIO_PIN15, 1<<0 ~ 1<<15
 * @retval      返回引脚状�??, 0, 低电�????; 1, 高电�????
 */
uint8_t sys_read_gpio_pin_value(GPIO_TypeDef *p_gpiox, uint16_t pinx)
{
   #if 0
	if (p_gpiox->IDR & pinx)
    {
        return 1;   /* pinx的状态为1 */
    }
    else
    {
        return 0;   /* pinx的状态为0 */
    }
   #endif 
	 return (p_gpiox->IDR & pinx) ? 1 : 0;
}


/**
 * @brief  void adjust_temperature(int8_t delta)
 * @param  NO: 
 * @retval None
 */
static void adjust_temperature(int8_t delta) 
{

   //static uint8_t temperature_init_value ;
	if (g_pro.temperature_init_value == 0) {
        g_pro.temperature_init_value++;
        g_pro.gset_temperture_value = (delta > 0) ? 40 : 20;
    } else {
        g_pro.gset_temperture_value += delta;
        if (g_pro.gset_temperture_value > MAX_TEMPERATURE) g_pro.gset_temperture_value = MAX_TEMPERATURE;
        if (g_pro.gset_temperture_value < MIN_TEMPERATURE) g_pro.gset_temperture_value = MIN_TEMPERATURE;
    }
	g_pro.gTimer_input_set_temp_timer=0;
    g_pro.g_manual_shutoff_dry_flag = 0;
 
	g_pro.key_set_temperature_flag=1;

	
    g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
		
    TM1639_Display_Temperature(g_pro.gset_temperture_value);
    g_pro.gTimer_input_set_temp_times = 0;
    g_pro.gTimer_switch_temp_hum = 0;
	g_pro.gTimer_input_set_temp_timer=0;
	
	
}

/**
 * @brief  void adjust_timer(int8_t delta)
 * @param  NO: 
 * @retval None
 */
static void adjust_timer(int8_t delta) 
{
    g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
	g_pro.gTimer_switch_set_timer_times = 0;
    g_pro.key_add_dec_be_pressed_flag = 1;
	g_pro.disp_59minutes_flag =0;  //WT.EDIT 2025.10.06
    g_pro.gdisp_timer_hours_value += delta;
    if (g_pro.gdisp_timer_hours_value > MAX_TIMER_HOURS) g_pro.gdisp_timer_hours_value = MAX_TIMER_HOURS;
    if (g_pro.gdisp_timer_hours_value < MIN_TIMER_HOURS) g_pro.gdisp_timer_hours_value = MIN_TIMER_HOURS;
    TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
	
}



/**
 * @brief  key_dwon_fun(void)
 * @param  NO: 
 * @retval None
 */
void key_dwon_fun(void)
{
    switch (g_pro.switch_disp_time_or_temp_item) {
        case temperature_mode:
            adjust_temperature(-1);
	        g_pro.first_set_ptc_on  = 0;
            break;
        case timer_time_mode: //WT.EDIT 2025.04.23//timer_time_mode:
            if(g_key.key_mode_long_flag ==1){
               adjust_timer(-1);
            }
            break;
        default:
            break;
    }
}
/**
 * @brief : void key_up_fun(void)
 * @param  NO: 
 * @retval None
 */
 void key_up_fun(void)
{
	 switch (g_pro.switch_disp_time_or_temp_item) {
        case temperature_mode:
            adjust_temperature(1);
	        g_pro.first_set_ptc_on  = 0;
            break;
        case timer_time_mode: //WT.EDIT 2025.04.23//:
            if(g_key.key_mode_long_flag ==1){
                adjust_timer(1);
            }
            break;
        default:
            break;
    }
  

}

/******************************************************************************
	*
	*Function Name:void bsp_init(void)
	*Funcion: 
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void set_temperature_value_handler(void)
{

   if((g_pro.key_set_temperature_flag==1 || g_wifi.g_wifi_set_temp_flag==1) && g_pro.gTimer_input_set_temp_timer >2)
   {
        g_pro.key_set_temperature_flag++;
        g_wifi.g_wifi_set_temp_flag=0;
		g_pro.set_temperature_success_flag=1;
	
		g_pro.g_manual_shutoff_dry_flag =0;
		g_pro.first_set_ptc_on=0;
		
		if (g_pro.current_temperature >= g_pro.gset_temperture_value){

			g_pro.gDry= DRY_STATE_OFF;
		    setDryState(g_pro.gDry);
		
			
            if(g_disp.g_second_disp_flag == 1) {
				 SendWifiData_One_Data(0x2A,g_pro.gset_temperture_value);
	             osDelay(100);
				 //sendDisplayCommand(0x02,g_pro.gDry); // 关闭干燥功能
	             //osDelay(5);
				    
			}

			if(g_wifi.gwifi_link_net_success==wifi_link_success){
               publishMqttData(DRY_STATE_OFF, g_pro.gset_temperture_value);
			   osDelay(200);
			}
        } 
		else if (g_pro.current_temperature < g_pro.gset_temperture_value){
			if(g_pro.works_two_hours_interval_flag ==0){

			   g_pro.gDry = DRY_STATE_ON;
			   setDryState(g_pro.gDry);

		   }
			
			
            if (g_disp.g_second_disp_flag == 1 ) {

			    SendWifiData_One_Data(0x2A,g_pro.gset_temperture_value);
	            osDelay(100);
				
                //sendDisplayCommand(0x02,0x01); // 打开干燥功能
                //osDelay(5);
				
            }


			if(g_wifi.gwifi_link_net_success==wifi_link_success){
            publishMqttData(DRY_STATE_ON, g_pro.gset_temperture_value);
			osDelay(200);
			}
        }

	   
	
		
    }
    else if(g_pro.key_set_temperature_flag!=1){

        if(g_pro.set_temperature_success_flag==1  && read_wifi_temperature_value()==0){
		
		       handleSetTemperatureControl();

			
         }
		else if(g_pro.set_temperature_success_flag==0){ //don't set temperature value 
				handleDefaultTemperatureControl();
		 
		}
    }
                
  }
/*
* @brief:
* @note:
* @param:
* @retrval:
*
*/
void compare_temperature_value_hanlder(void)
{
	if(g_pro.set_temperature_success_flag==1 ){

			  handleSetTemperatureControl();


	   }
	   else{ //don't set temperature value
			handleDefaultTemperatureControl();

		}
}



// 读取温度�????
uint8_t readTemperature(void) 
{
    return read_dht11_temperature_value();
}

/******************************************************************************
	*
	*Function Name:static void handleSetTemperatureControl(void) 
	*Funcion: // 处理温度控制逻辑
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
static void handleSetTemperatureControl(void) 
{

    
    if( g_pro.gTimer_set_temp_counter >= CHECK_TIME_THRESHOLD_4S) { // 4�????
          g_pro.gTimer_set_temp_counter =0;



        if ( g_pro.gset_temperture_value <= g_pro.current_temperature){// set_36 << current_40
            g_pro.gDry = 0;
		    DRY_CLOSE();
            LED_DRY_OFF();
		   

			  if(g_pro.first_set_ptc_on==0)g_pro.first_set_ptc_on=1;  //the first open ptc heating //WT.DEDIT 2028.08.27 modify this flow codes
			   else if(g_pro.first_set_ptc_on==2)g_pro.first_set_ptc_on=3;
			   else if(g_pro.first_set_ptc_on==4)g_pro.first_set_ptc_on=5;
			
	        #if 0 //by display board send data to mainboard.
		    if(g_disp.g_second_disp_flag == 1 && ptc_off_flag != g_pro.gDry){
				ptc_off_flag = g_pro.gDry;
				sendDisplayCommand(0x02,0);
				osDelay(100);
		    }
			#endif 
			
		   if (g_wifi.gwifi_link_net_success == wifi_link_success &&  ptc_off_flag != g_pro.gDry){
		      	ptc_off_flag = g_pro.gDry;
		        MqttData_Publish_SetPtc(0x0);
			    osDelay(200);
		   	}
			
        }
        else{

           if((g_pro.first_set_ptc_on==1 || g_pro.first_set_ptc_on==0 )&& g_pro.works_two_hours_interval_flag == 0 && g_pro.g_manual_shutoff_dry_flag ==0 ){

                if(g_pro.first_set_ptc_on==1)g_pro.first_set_ptc_on=2;
				else if(g_pro.first_set_ptc_on==0)g_pro.first_set_ptc_on=4;

				 g_pro.gDry = DRY_STATE_ON;
				 LED_DRY_ON();
				 DRY_OPEN();
                #if 0
				 if(g_disp.g_second_disp_flag == 1){
				   sendDisplayCommand(0x02,0x01); // 打开干燥功能
				   osDelay(100);
				 }
				 #endif 
				 
				 if(g_wifi.gwifi_link_net_success == wifi_link_success && ptc_on_flag != g_pro.gDry) {
				 	ptc_on_flag = g_pro.gDry;
					MqttData_Publish_SetPtc(0x01);
					 osDelay(200);
				 }

           	}
			else if ((g_pro.first_set_ptc_on==3 || g_pro.first_set_ptc_on==5) &&(g_pro.gset_temperture_value - TEMPERATURE_DIFF_THRESHOLD) >= g_pro.current_temperature ){
				
            	
					
	               if(g_pro.works_two_hours_interval_flag == 0 && g_pro.g_manual_shutoff_dry_flag ==0){
				   	   g_pro.gDry = 0x01;
					   LED_DRY_ON();
						DRY_OPEN();
					}
				 
            	

				#if 0
				if(g_disp.g_second_disp_flag == 1 && g_pro.g_manual_shutoff_dry_flag ==0){
				   sendDisplayCommand(0x02,0x01); // 打开干燥功能
				   osDelay(100);
				}
				#endif 
				
				if (g_wifi.gwifi_link_net_success == wifi_link_success && ptc_on_flag != g_pro.gDry ) {
				 ptc_on_flag = g_pro.gDry;
					MqttData_Publish_SetPtc(0x01);
					 osDelay(200);
				 }
           
            }
			
        }

		if(g_pro.g_manual_shutoff_dry_flag ==1){
				   	  // g_pro.gDry = 0x01;
					LED_DRY_OFF();
					DRY_CLOSE();
		}
    }

	
	 
}
/******************************************************************************
	*
	*Function Name:static void handleDefaultTemperatureControl(void)
	*Funcion: 处理默认温度控制逻辑
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
static void handleDefaultTemperatureControl(void) 
{
    
  
    if ( g_pro.gTimer_disp_temp_humidity_vlaue > 3) { // 3�????
         g_pro.gTimer_disp_temp_humidity_vlaue = 0;
      

        if(g_pro.current_temperature > 39) {
		
			g_pro.gDry =  DRY_STATE_OFF;
            LED_DRY_OFF();
			DRY_CLOSE();
			g_pro.first_rcoder_ptc_on_flag  = 1;

			#if 0 //by display board send data to main board .
		    if(g_disp.g_second_disp_flag ==1){
		     sendDisplayCommand(0x02,0x0); // send data to the second displayboard .关闭干燥功能
			 osDelay(5);
			}
			#endif 
			
			 if (g_wifi.gwifi_link_net_success == wifi_link_success && ptc_off_flag != g_pro.gDry) {
				ptc_off_flag = g_pro.gDry;
             	MqttData_Publish_SetPtc(DRY_STATE_OFF) ;
			    osDelay(200);
			 }
        } 
		else if(g_pro.first_rcoder_ptc_on_flag  == 1 && g_pro.current_temperature <38 && g_pro.g_manual_shutoff_dry_flag==0){

			        g_pro.gDry= DRY_STATE_ON;
					LED_DRY_ON();

			        if(g_pro.works_two_hours_interval_flag ==0){
						
					 
					   DRY_OPEN();//setDryState(g_pro.gDry);
					}
			  	
                #if 0
				if(g_disp.g_second_disp_flag ==1 && g_pro.g_manual_shutoff_dry_flag ==0){
					sendDisplayCommand(0x02,0x01); // 打开干燥功能
					osDelay(100);
				}
				#endif 
				if (g_wifi.gwifi_link_net_success == 1 && ptc_on_flag != g_pro.gDry) {
					ptc_on_flag = g_pro.gDry;
					MqttData_Publish_SetPtc(DRY_STATE_ON);//publishMqttData(DRY_STATE_ON, current_temperature) ;
					osDelay(200);
				}

		}
        else if ( g_pro.first_rcoder_ptc_on_flag == 0 && g_pro.current_temperature < 40 && g_pro.g_manual_shutoff_dry_flag ==0){
                g_pro.gDry= DRY_STATE_ON;

			     LED_DRY_ON();
				
				if(g_pro.works_two_hours_interval_flag ==0){
					
					DRY_OPEN();//setDryState(g_pro.gDry);
				 }
			     #if 0
				 if(g_disp.g_second_disp_flag ==1 && g_pro.g_manual_shutoff_dry_flag ==0){
				  sendDisplayCommand(0x02,0x01); // 第二个显示板，打�????干燥功能
				  osDelay(100);
				 }
				 #endif 
				  if (g_wifi.gwifi_link_net_success == wifi_link_success && ptc_on_flag != g_pro.gDry) {
				  	  ptc_on_flag = g_pro.gDry;
				   
				     MqttData_Publish_SetPtc(DRY_STATE_ON);//publishMqttData(DRY_STATE_ON, current_temperature) ;
	                 osDelay(200);
				  }
	            }
		
            if(g_pro.g_manual_shutoff_dry_flag ==1){
				   	  // g_pro.gDry = 0x01;
					LED_DRY_OFF();
					DRY_CLOSE();
		     }
	     }
}
    


/******************************************************************************
	*
	*Function Name:static void setDryState(uint8_t state)
	*Funcion: set temperature value 
	*Input Ref: state: 0-off,1-on
	*Return Ref:NO
	*
******************************************************************************/
static void setDryState(uint8_t state) 
{
    //g_pro.gDry = state;
	
    if (state == DRY_STATE_ON){
        DRY_OPEN();
        LED_DRY_ON();
    } else {
        DRY_CLOSE();
        LED_DRY_OFF();
    }
}

// 发布MQTT数据
void publishMqttData(DryState state, uint8_t temperature) 
{
    if (g_wifi.gwifi_link_net_success == 1) {
        MqttData_Publis_SetTemp(temperature);
        osDelay(200);
        MqttData_Publish_SetPtc(state);
        osDelay(200);
    }
}


/******************************************************************************
	*
	*Function Name:void sendDisplayCommand(uint8_t command,uint8_t data) 
	*Funcion: // 发�?�显示命�????
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void sendDisplayCommand(uint8_t command,uint8_t data) 
{
    if(g_disp.g_second_disp_flag ==1){
		SendData_Set_Command(command, data);
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
   
  
   if(g_key.key_mode_long_flag ==1 && g_pro.gTimer_switch_set_timer_times > 3 ){ // 2

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
			
//			if(g_pro.gdisp_timer_hours_value > 1)
//			   g_pro.gdisp_timer_minutes_value=60;//60 minutes
//			else
//			   g_pro.gdisp_timer_minutes_value=0;//60 minutes
			
	
			g_pro.disp_59minutes_flag = 0;
            SendWifiData_One_Data(0x2B,g_pro.gdisp_timer_hours_value);
	        osDelay(100);

               
			}
			else{
			
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
			  
              if(g_pro.gdisp_timer_hours_value==1){
                  g_pro.gdisp_timer_hours_value=0;
				  g_pro.disp_59minutes_flag = 1;
              }
			  else {
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
      

	if(g_pro.gdisp_timer_minutes_value ==59 && g_pro.disp_59minutes_flag == 0 && g_disp.g_second_disp_flag ==1){
			  
            if(g_pro.gdisp_timer_hours_value==1){
                  g_pro.gdisp_timer_hours_value=0;
				  g_pro.disp_59minutes_flag = 1;
          }

      }
  }
}




void mode_short_key_fun(void)
{
	g_pro.key_set_temperature_flag=0;//WT.EDIT 2025.10.17
	g_pro.switch_disp_time_or_temp_item = timer_time_mode; //WT.EDIT 2025.10.17
	set_timer_mode();
}



static void set_timer_mode(void)
{
    g_pro.gAI = 0;
    LED_AI_OFF();
    HUMIDITY_ICON_OFF();
    TEMP_ICON_OFF();
	
	if(g_pro.disp_59minutes_flag ==0)
       TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
	else 
	   TM1639_Display_3_Digit(g_pro.gdisp_timer_minutes_value);

	
   
  
 }

