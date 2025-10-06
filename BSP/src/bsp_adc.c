#include "bsp_adc.h"
#include "bsp.h"



static void Judge_PTC_Temperature_Value(uint16_t adc_ptc);

static void Judge_Fan_State(void);

static void ADC_GetValues(void);

static uint16_t compute_voltage(uint16_t raw_value) ;

static uint8_t ADC_StartConversion(void);
// ADC相关变量定义
#define SAMPLE_COUNT 6

#define ADC_BUFFER_SIZE 2
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE]; // DMA传输缓冲区

uint16_t mean_fan_buf[SAMPLE_COUNT];


volatile uint8_t adc_conversion_complete = 0;

uint16_t ptc_temp_voltage;
uint16_t fan_detect_voltage = 1000;

static uint8_t counter_error;


/**********************************************************************
	*
	*Functin Name: void adc_detected_hundler(void)
	*Function :
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void adc_detected_hundler(void)
{
    
 
        Fan_Full_Speed();
	   //switch_flag = switch_flag ^ 0x01;
	  if(ADC_StartConversion()){
	   	  ADC_GetValues();
	   		
	   }

	   if(fan_detect_voltage < 400){
             counter_error ++ ;
			  if(counter_error > 5){
			      g_pro.fan_warning=1;
				  g_pro.ptc_on_off_flag = 1;
			      g_pro.gDry =0;
				  DRY_CLOSE();

			  }
		}
	    else{

		  counter_error=0;

		}

	 
    

   if(g_pro.fan_warning==1 && fan_detect_voltage < 400){
   	
      Judge_Fan_State();
	  g_pro.gDry =0;
	  DRY_CLOSE();
   	}
	

}

//void Get_PTC_Temperature_Voltage(uint32_t channel,uint8_t times)
//{
//    static uint8_t times_i;
//	uint16_t adcx;
//	
//	//adcx = Get_Adc_Average(channel,times);

//    ptc_temp_voltage  =(uint16_t)((adcx * 3300)/4096); //amplification 100 ,3.11V -> 311

//    if(times_i < 2){
//	    times_i++;
//	    ptc_temp_voltage=2000;
//	
//	}
//	#ifdef DEBUG
//      printf("ptc= %d",gctl_t.ptc_temp_voltage);
//	#endif 

//	 Judge_PTC_Temperature_Value(ptc_temp_voltage);

//     
//}


/*****************************************************************
	*
	*Function Name: void Judge_PTC_Temperature_Value(void)
	*Function: PTC adc read voltage
	*Input Ref: NO
	*Return Ref: No
	*
	*
*****************************************************************/
static void Judge_PTC_Temperature_Value(uint16_t adc_ptc)
{
  
 #if HAINAN
  if(adc_ptc < 215 || adc_ptc == 215){  //115 degree 
         g_pro.ptc_warning =1;

		 g_pro.gDry=0 ;

         DRY_CLOSE();//Ptc_Off();
		 LED_DRY_OFF();//LED_PTC_ICON_OFF();
		 vTaskDelay(pdMS_TO_TICKS(50));//HAL_Delay(50);
		
      

		Publish_Data_Warning(ptc_temp_warning,1);
		HAL_Delay(200);  
        
		MqttData_Publish_SetPtc(0);
		HAL_Delay(100);  
		
		Buzzer_Ptc_Error_Sound();
		
			  
				
	   	}
  #endif 
}

/*****************************************************************
	*
	*Function Name: static void Judge_Fan_State(void)
	*Function:
	*Input Ref:  
	*Return Ref: No
	*
	*
*****************************************************************/
static void Judge_Fan_State(void)
{

		  Publish_Data_Warning(fan_warning,g_pro.fan_warning);
	      //Delay(200);//HAL_Delay(200);

		   MqttData_Publis_SetFan(0);
	       //Delay(100);//HAL_Delay(100);

		  Buzzer_Fan_Error_Sound();

		  SendWifiData_To_Cmd(0x09,0x01);//Fan fault warning .
		  osDelay(5);

}
/*****************************************************************
*
	*Function Name: static uint8_t ADC_StartConversion(void)
	*Function :LL DAM adc switch result 
	*Input Ref: NO
	*Return Ref: No
	*
	*
*****************************************************************/
static uint8_t ADC_StartConversion(void)
{
   if(LL_ADC_IsEnabled(ADC1)==0){
      return 0;
   }

   if(LL_DMA_IsEnabledChannel(DMA1,LL_DMA_CHANNEL_5)){
		 LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);


   }
   //配置DMA传输
   LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_5,
   						LL_ADC_DMA_GetRegAddr(ADC1,LL_ADC_DMA_REG_REGULAR_DATA),
   						(uint32_t)adc_buffer,
   						LL_DMA_DIRECTION_PERIPH_TO_MEMORY);


	LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_5,ADC_BUFFER_SIZE);

	
	// 使能DMA通道
	   LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
	   
	   // 启动ADC转换
	   LL_ADC_REG_StartConversion(ADC1);
	   
	   return 1;

						


}

// 获取ADC转换结果
static void ADC_GetValues(void)
{
     static uint8_t fan_counter,ptc_counter;
	 uint8_t i;
	
	uint32_t sum =0;
        
   
	   
	   mean_fan_buf[fan_counter] = compute_voltage(adc_buffer[0]);//(adc_buffer[0] * 3300 )/4095;//compute_voltage(adc_buffer[0]) ;
	   vTaskDelay(pdMS_TO_TICKS(10));
	    fan_counter++;
	    if(fan_counter >=6){

            for (i = 1; i < SAMPLE_COUNT; i++) {
                sum += mean_fan_buf[i];
            }
		   
		  fan_detect_voltage = sum/5;
		  fan_counter =0;
	


		}
		
        //fan_detect_voltage = (adc_buffer[0] * 3300 )/4095; // PA0 - FAN
      // ptc_detect_voltage =  compute_voltage(adc_buffer[1]) ;
       // ptc_detect_voltage = (adc_buffer[1] * 3300)/4095; // PA1 - PTC
      //  adc_conversion_complete = 0;
       // return 1;
    //}
    //return 0;
}

/*****************************************************************
	*
	*Function Name: 
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
static uint16_t compute_voltage(uint16_t raw_value) 
{
    uint64_t temp ;
	const uint32_t multiplier = 3462835200U;
   temp = (uint64_t)raw_value * multiplier;
    return (uint16_t)(temp >> 32);  // ��λ��mV
}


