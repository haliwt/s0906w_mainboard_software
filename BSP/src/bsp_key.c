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

#define TEMP_ON_THRESHOLD   38   // 温度低于 38℃ 打开 PTC
#define TEMP_OFF_THRESHOLD  40   // 温度高于等于 40℃ 关闭 PTC
//#define TEMP_SAFE_LIMIT     60   // 安全保护温度
#define DEFAULT_TEMP        40   // 默认温度（未设置时）


KEY_PROCESS_TYPEDEF  g_key;

typedef enum {
    PTC_STATE_OFF = 0,
    PTC_STATE_ON  = 1
} PTC_State;

static PTC_State ptc_state = PTC_STATE_OFF;


uint8_t ptc_on_flag =0xff,ptc_off_flag=0xff;



static void adjust_temperature(int8_t delta) ;
static void adjust_timer(int8_t delta) ;
//static void handleSetTemperatureControl(void) ;
//static void handleDefaultTemperatureControl(void);
static void setDryState(uint8_t state);
static void publishMqttData(DryState state, uint8_t temperature);

static void disp_set_timer_mode(void);
static void CompareSetAndActualTemperature(void);



uint8_t readTemperature(void);



uint8_t timer_power_off_flag;

void key_referen_init(void)
{
  g_pro.set_temperature_success_flag=0;

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
	g_pro.set_temp_counter ++ ;
	g_pro.first_set_ptc_on  = 0;
	
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

    g_pro.gdisp_timer_hours_value += delta;
    if (g_pro.gdisp_timer_hours_value > MAX_TIMER_HOURS) g_pro.gdisp_timer_hours_value = MAX_TIMER_HOURS;
    if (g_pro.gdisp_timer_hours_value < MIN_TIMER_HOURS) g_pro.gdisp_timer_hours_value = MIN_TIMER_HOURS;
    //TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
    TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
	
}



/**
 * @brief  key_dwon_fun(void)
 * @param  NO: 
 * @retval None
 */
void key_dwon_fun(void)
{

   if(g_key.key_mode_long_flag !=1)g_pro.switch_disp_time_or_temp_item = temperature_mode;
	switch (g_pro.switch_disp_time_or_temp_item) {
        case temperature_mode:
		
            adjust_temperature(-1);
	       

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

     if(g_key.key_mode_long_flag !=1)g_pro.switch_disp_time_or_temp_item = temperature_mode;

	 switch (g_pro.switch_disp_time_or_temp_item) {

	   
        case temperature_mode:
			
             adjust_temperature(1);
		   
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
	    g_pro.gTimer_set_temp_counter = 10;
		g_pro.g_manual_shutoff_dry_flag =0;
		g_pro.first_set_ptc_on=0;
		
		if (g_pro.current_temperature >= g_pro.gset_temperture_value){

			g_pro.gDry= DRY_STATE_OFF;
		    setDryState(g_pro.gDry);
		
			
                
				 if(g_pro.disp_second_f ==1){
				 	if(timer_expired(&t_xdp)){
				 	SendWifiData_One_Data(0x2A,g_pro.gset_temperture_value);
	                //tx_thread_sleep(10);
				 	}
				 }
				 
				if(g_pro.disp_second_f ==1){
					if(timer_expired(&t_xdp)){
				  	  sendDisplayCommand(0x02,g_pro.gDry); // 关闭干燥功能
	                 //tx_thread_sleep(10);
						}
				 }
				    
			

			if(g_wifi.gwifi_link_net_success==wifi_link_success){
				
                 publishMqttData(DRY_STATE_OFF, g_pro.gset_temperture_value);
				
			   
			}

			
        } 
		else if (g_pro.current_temperature < g_pro.gset_temperture_value){
			if(g_pro.works_two_hours_interval_flag ==0){

			   g_pro.gDry = DRY_STATE_ON;
			   setDryState(g_pro.gDry);
             }
			
			  if(g_pro.disp_second_f ==1){ 
			  	if(timer_expired(&t_xdp)){
			   	   SendWifiData_One_Data(0x2A,g_pro.gset_temperture_value);
	            //tx_thread_sleep(10);
			  	}
			   	}
				
                if(g_pro.disp_second_f ==1){
					if(timer_expired(&t_xdp)){
					  sendDisplayCommand(0x02,0x01); // 打开干燥功能
                    //tx_thread_sleep(10);
					}
                }
				
           if(g_wifi.gwifi_link_net_success==wifi_link_success){
                publishMqttData(DRY_STATE_ON, g_pro.gset_temperture_value);
			    
			}
        }

	   
	
		
    }
    else {

      compare_temperature_value_hanlder();
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
	 if( g_pro.gTimer_set_temp_counter >= CHECK_TIME_THRESHOLD_4S) { // 4�????
          g_pro.gTimer_set_temp_counter =0;
          CompareSetAndActualTemperature();
	 }
	   
}



// 读取温度�????
uint8_t readTemperature(void) 
{
    return g_pro.g_temperature_value;
}


 
/**
*@brief:
*@notice
*@param
*
*
**/
static void CompareSetAndActualTemperature(void)
{

  static uint8_t dry_on_counter = 0xff,dry_off_counter =0xff;
	if(g_pro.works_two_hours_interval_flag ==1 || g_pro.g_manual_shutoff_dry_flag==1 || g_pro.key_set_temperature_flag ==1)return ;
	// 控制 PTC 加热器开关（带滞后控制）
	uint8_t real_temp = g_pro.g_temperature_value;
	int8_t target_temp;
	
		// 确定目标温度：有设置用设置值，否则默认 40℃
		//  target_temp = (gpro_t.set_temp_value_success == 1) ? gctl_t.set_temperature_value : DEFAULT_TEMP;
		
		if (g_pro.set_temperature_success_flag==1) {
			target_temp = g_pro.gset_temperture_value;
		} 
		else {
			target_temp = DEFAULT_TEMP;
		}
	
		// 安全保护：超过 40℃ 强制关闭
		if (real_temp >= target_temp) {
			
                g_pro.gDry =  0;
                LED_DRY_OFF();
			    DRY_CLOSE();
				ptc_state = PTC_STATE_OFF;
				g_pro.first_rcoder_ptc_on_flag =1;
				if(g_pro.disp_second_f ==1){
					if(timer_expired(&t_xdp)){
					SendData_Set_Command(0x22, 0x00); // close PTC
					//tx_thread_sleep(10);
					}
				}

				if(g_wifi.gwifi_link_net_success==wifi_link_success && (dry_off_counter != g_pro.set_temp_counter)){
					dry_off_counter = g_pro.set_temp_counter;
                    publishMqttData(DRY_STATE_OFF, g_pro.gset_temperture_value);
			   
			      }
			
			    
			return;
		}
	
        if(ptc_state == PTC_STATE_OFF){

			 // 第一次打开：不需要滞后
			if(g_pro.first_rcoder_ptc_on_flag ==0 ||g_pro.first_rcoder_ptc_on_flag ==1){
		        if(real_temp < target_temp ){

		         
				   ptc_state = PTC_STATE_ON;
				   if(g_pro.first_rcoder_ptc_on_flag==1) g_pro.first_rcoder_ptc_on_flag =2;

				    g_pro.gDry = 0x01;
					LED_DRY_ON();
					DRY_OPEN();


				   
				   if(g_pro.disp_second_f ==1){
				   	if(timer_expired(&t_xdp)){
				   	 SendData_Set_Command(0x22, 0x01); // open PTC

				      //tx_thread_sleep(10);
				   		}
				   	}
				   if(g_wifi.gwifi_link_net_success==wifi_link_success && (dry_on_counter != g_pro.set_temp_counter)){
				   	  dry_on_counter = g_pro.set_temp_counter;
                     publishMqttData(DRY_STATE_ON, g_pro.gset_temperture_value);
			   
			       }
				  


				}
			}
	        else{
	            
				if (real_temp < (target_temp - 2)) { // 第二次及之后：需要滞后 (target - 2)
					// 当前关闭状态 → 低于 (目标温度 - 2℃) 才打开
				
				   
					ptc_state = PTC_STATE_ON;

					g_pro.gDry = 0x01;
					LED_DRY_ON();
					DRY_OPEN();
					if(g_pro.disp_second_f ==1){
						if(timer_expired(&t_xdp)){
						SendData_Set_Command(0x22, 0x01); // open PTC

					    //tx_thread_sleep(10);
							}
						}

					if(g_wifi.gwifi_link_net_success==wifi_link_success && (dry_on_counter != g_pro.set_temp_counter)){
						dry_on_counter = g_pro.set_temp_counter;
                       publishMqttData(DRY_STATE_OFF, g_pro.gset_temperture_value);
			   
			          }
				}
			} 
        }
		else {
			// 当前开启状态 → 高于等于目标温度才关闭
			if (real_temp >= target_temp) {
				g_pro.gDry =  0;
                LED_DRY_OFF();
			    DRY_CLOSE();
                ptc_state = PTC_STATE_OFF;
				if(g_pro.disp_second_f ==1){
					if(timer_expired(&t_xdp)){
					SendData_Set_Command(0x22, 0x00); // close PTC
				    // tx_thread_sleep(10);
						}
					}

				if(g_wifi.gwifi_link_net_success==wifi_link_success && (dry_off_counter != g_pro.set_temp_counter)){
					dry_off_counter = g_pro.set_temp_counter;
                  publishMqttData(DRY_STATE_OFF, g_pro.gset_temperture_value);
			   
			      }
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
		if(timer_expired(&t_mqtt_0)){
           MqttData_Publis_SetTemp(temperature);
          //tx_thread_sleep(30);
		}
		if(timer_expired(&t_mqtt_1)){
           MqttData_Publish_SetPtc(state);
        //tx_thread_sleep(30);
		}
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
    if(g_pro.disp_second_f ==1){
		SendData_Set_Command(command, data);
    }
}
/**
*@brief :
*@notice:
*@param:
*
**/
void mode_short_key_fun(void)
{
	g_pro.key_set_temperature_flag=0;//WT.EDIT 2025.10.17
	g_pro.switch_disp_time_or_temp_item = timer_time_mode; //WT.EDIT 2025.10.17
	
	disp_set_timer_mode();
}

/**
*@brief :
*@notice:
*@param:
*
**/
static void disp_set_timer_mode(void)
{
    g_pro.gAI = 0;
    LED_AI_OFF();
    HUMIDITY_ICON_OFF();
    TEMP_ICON_OFF();

   switch(g_pro.set_timing_or_timer_time_flag){

     case TIMER_TIME:
	
//	    if(g_pro.gdisp_timer_hours_value >0)
//	         TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);
//		else 
//		   TM1639_Display_setTimerMinutes_3_Digit(g_pro.gdisp_timer_minutes_value); //display "0 0 n"

		
	break;

	case WORKS_TIME:
	  // g_pro.gdisp_timer_hours_value=0;
       //TM1639_Display_3_Digit(g_pro.gdisp_timer_hours_value);
     //  TM1639_Display_setTimerHours_3_Digit(g_pro.gdisp_timer_hours_value);

	break;
   	}
	
}

