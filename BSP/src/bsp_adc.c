#include "bsp_adc.h"
#include "bsp.h"



//static void Judge_PTC_Temperature_Value(uint16_t adc_ptc);

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
    
   static uint8_t detectted_has_adc_flag ,counter_error;
   if(g_pro.fan_warning==0){
 
        Fan_Full_Speed();
	   //switch_flag = switch_flag ^ 0x01;
	  if(ADC_StartConversion()){
	   	  ADC_GetValues();
		  detectted_has_adc_flag=1 ;
	   	  	
	  }

	  if(detectted_has_adc_flag==1){//WT.EDIT 2025.11.24
	  	 detectted_has_adc_flag++;
	   if(fan_detect_voltage < 350){
             counter_error ++ ;
			 #if DEBUG_ENABLE
			   printf("error_counter = %d\r\n",counter_error);

			 #endif 
			  if(counter_error > 9){
			      g_pro.fan_warning=1;
			
			      g_pro.gDry =0;
				  DRY_CLOSE();

			  }
		}
	    else{

		  counter_error=0;
		 

		}
	  }

	 }
	 else if(g_pro.fan_warning==1){
   	  
         Judge_Fan_State();
		  g_pro.gDry =0;
		  DRY_CLOSE();
      	
   	 }
	

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
	       tx_thread_sleep(20);//HAL_Delay(200);

		   MqttData_Publis_SetFan(0);
	       tx_thread_sleep(20);//Delay(100);//HAL_Delay(100);

		  Buzzer_Fan_Error_Sound();

		  if(g_pro.disp_second_f ==1){
		  	SendWifiData_To_Cmd(0x09,0x01);//Fan fault warning .
		     tx_thread_sleep(10);
		  }

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
  //   static uint8_t ptc_counter;
	// uint8_t i;
	
//	uint32_t sum =0;
        
   
	   
	   //mean_fan_buf[fan_counter] = compute_voltage(adc_buffer[0]);//(adc_buffer[0] * 3300 )/4095;//compute_voltage(adc_buffer[0]) ;
	  // mean_fan_buf[fan_counter]
	   fan_detect_voltage= (adc_buffer[0] * 3300 )/4095;
       tx_thread_sleep(10);
	   #if DEBUG_ENABLE
          printf("fan_voltale = %d \r\n",fan_detect_voltage);
	   #endif 
//	    fan_counter++;
//	    if(fan_counter >=6){

//            for (i = 1; i < SAMPLE_COUNT; i++) {
//                sum += mean_fan_buf[i];
//            }
		   
//		  fan_detect_voltage = sum/5;
//		  fan_counter =0;
	


//		}
		
  
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


