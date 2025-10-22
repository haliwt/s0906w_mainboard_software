#include "bsp.h"
#include "bsp_streamlight.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#define POWER_BIT_0	        (1 << 0)
#define MODE_BIT_1	        (1 << 1)
#define DOWN_BIT_2           (1 << 2)
#define UP_BIT_3           (1 << 3)

#define KEY_LONG_POWER_BIT_4      (1<<4)
#define KEY_LONG_MODE_BIT_5			(1<<5)


#define MODE_LONG_KEY_8         (1 << 8)
#define DECODER_BIT_9          (1<< 9)

// 定义EXTI线，根据实际GPIO引脚对应的EXTI线号修改
#define KEY_POWER_EXTI_LINE    LL_EXTI_LINE_0
#define KEY_MODE_EXTI_LINE     LL_EXTI_LINE_1  
#define KEY_DOWN_EXTI_LINE     LL_EXTI_LINE_2
#define KEY_UP_EXTI_LINE       LL_EXTI_LINE_3



uint8_t inputBuf[1];


/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
static void vTaskRunPro(void *pvParameters);
static void vTaskDecoderPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);



/* 创建任务通信机制 */
//static void AppObjCreate(void);


/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
static TaskHandle_t xHandleTaskRunPro = NULL;
static TaskHandle_t xHandleTaskDecoderPro= NULL;
static TaskHandle_t xHandleTaskStart = NULL;



uint8_t ucKeyCode;
uint8_t uckey_number;
uint8_t key_power_flag,decoder_flag ;




void freeRTOS_Handler(void)
{
    /* 创建任务 */
	  AppTaskCreate();

	  /* 创建任务通信机制 */
	//  AppObjCreate();

	  /* 启动调度，开始执行任�?1�?7 */
	   vTaskStartScheduler();
}

/**********************************************************************************************************
*	Function Name: static void vTaskDecoderPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)

**********************************************************************************************************/
static void vTaskDecoderPro(void *pvParameters)
{
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000); /* 设置�?大等待时间为30ms */
	uint32_t ulValue;


    while(1)
    {

	xResult = xTaskNotifyWait(0x00000000,
						0xFFFFFFFF,     /* Reset the notification value to 0 on */
						&ulValue,        /* 保存ulNotifiedValue到变量ulValue�? */
						xMaxBlockTime);//portMAX_DELAY);  /* 阻塞时间30ms，释放CUP控制�?,给其它任务执行的权限*/

	if( xResult == pdPASS )
	{
			/* 接收到消息，�?测那个位被按�? */

		if((ulValue & DECODER_BIT_9) != 0){
  
			usart1_rx_decoder(); 
				
		 }
	 }
   }
}

/**********************************************************************************************************
*	Function Name: static void vTaskRunPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)
**********************************************************************************************************/
static void vTaskRunPro(void *pvParameters)
{

// uint8_t tx_buffer[20];
// uint8_t tx_len;

  while(1){
    
	key_handler();

    power_onoff_handler(g_pro.gpower_on);

	if(g_wifi.wifi_led_fast_blink_flag==0 ){
		wifi_communication_tnecent_handler();//
		getBeijingTime_cofirmLinkNetState_handler();
		wifi_auto_detected_link_state();
		#if 0
		if(gl_tMsg.ucMessageID ==1){
			gl_tMsg.ucMessageID = 0; //display command head
			strcpy((char*)tx_buffer, "has IAP Update \r\n");//tx_buffer[]="has NOT IAP Update \r\n";
			tx_len = strlen((char*)tx_buffer);//tx_len = tx_buffer[]/tx_buffer[0];
			//HAL_UART_Transmit(&huart1,tx_buffer,tx_len, 0xffff);
			JumpToBootloader();

		}
		#endif 
	}
	else{
         wifi_led_fast_blink_handler();
	}

	vTaskDelay(pdMS_TO_TICKS(20));

	  
    }
	  
}
/**********************************************************************************************************
*
*	Function Name: vTaskStart
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传�?�的形参
*	Return Ref:
*	priority: 3  (数�?�越小优先级越低，这个跟uCOS相反)
*
**********************************************************************************************************/
static void vTaskStart(void *pvParameters)
{
	//BaseType_t xResult;
    //const TickType_t xMaxBlockTime = pdMS_TO_TICKS(2000); /* 设置�?大等待时间为30ms */
//uint32_t ulValue;
   

    while(1)
    {

	 if(KEY_MODE_VALUE() == KEY_DOWN  &&g_pro.gpower_on == power_on){

	       g_key.key_mode_flag = KEY_MODEL_ID;
		   g_key.key_down_flag=0;
	       g_key.key_up_flag=0;

	 }
	 else if(KEY_UP_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){

	      g_key.key_up_flag = KEY_UP_ID;
		  g_key.key_power_flag=0;
	       g_key.key_mode_flag=0;
     }
	 else if(KEY_DOWN_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){

            g_key.key_down_flag =KEY_DOWN_ID;
			g_key.key_power_flag=0;
	        g_key.key_mode_flag=0;

     }
	 else if(KEY_POWER_VALUE()  ==KEY_DOWN){

		 g_key.key_power_flag = KEY_POWER_ID;

     }
	 else  if(g_pro.key_long_power_pressed ==1 && g_pro.gTimer_key_long_counter > 0){
				  g_key.key_power_flag = 0;
                  g_key.power_on_key_counter=0;
			      g_pro.key_long_power_pressed =0;


	  }
	 
	 
	 vTaskDelay(pdMS_TO_TICKS(10));

   }
}
	
/**********************************************************************************************************
*	Function Name: AppTaskCreate
*	功能说明: 创建应用任务
*	Input Ref:
*	Return Ref:
**********************************************************************************************************/
void AppTaskCreate (void)
{

  xTaskCreate( vTaskDecoderPro,    		/* 任务函数  */
                 "vTaskDecoderPro",  		/* 任务�?1�?7    */
                 128,         		/* stack大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 3,           		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskDecoderPro); /* 任务句柄  */

  xTaskCreate( vTaskRunPro,    		/* 任务函数  */
                 "vTaskRunPro",  		/* 任务�?1�?7    */
                 128,         		/* stack大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 1,           		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskRunPro); /* 任务句柄  */

	xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
}
/********************************************************************************
	**
	*Function Name:void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
#if 0

void HAL_UART_RxCpltCallback(void)
{
     static uint8_t state,rx_end_flag ;
     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    

    if(huart->Instance == USART1) // mainBoard receive data from display board send data USART1
	{

       DISABLE_INT();
       switch(state)
		{
		case 0:  //#0
			if(inputBuf[0] == 0xA5){  // 0xA5 -- second display board ID
               gl_tMsg.rx_data_counter=0;
			   gl_tMsg.ulid=0;
               gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];
				state=1; //=1

             }
			else if(inputBuf[0]== 0xF0){//IAP boodloader flag
        
	           state = 0x0A;//gl_tMsg.ucMessageID = 0xF0;
			}
            else
                state=0;
		break;


		case 1: //#1

            if(gl_tMsg.disp_rx_cmd_done_flag ==0){
              /* 初始化结构体指针 */
               gl_tMsg.rx_data_counter++;

	          gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];


              if(rx_end_flag == 1){

                state = 0;

                gl_tMsg.ulid = gl_tMsg.rx_data_counter;
                rx_end_flag=0;

                gl_tMsg.rx_data_counter =0;

                gl_tMsg.disp_rx_cmd_done_flag = 1 ;

                gl_tMsg.bcc_check_code=inputBuf[0];

                #if 1
                xTaskNotifyFromISR(xHandleTaskDecoderPro,  /* 目标任务 */
                                    DECODER_BIT_9,     /* 设置目标任务事件标志位bit0  */
                                    eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
                                    &xHigherPriorityTaskWoken);

                /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				#endif 

              }

              }

              if(gl_tMsg.usData[gl_tMsg.rx_data_counter] ==0xFE && rx_end_flag == 0 &&   gl_tMsg.rx_data_counter > 4){

                     rx_end_flag = 1 ;

              }

        break;


		case 0x0A:

		if(inputBuf[0]== 0xF0){//IAP boodloader flag
		   state = 0;
	       gl_tMsg.ucMessageID = 1;
		 }
		else{
			state = 0;
		}
        break;
		}
         ENABLE_INT();//WT.EDIT 2025.05.18
   // __HAL_UART_CLEAR_OREFLAG(&huart1);
	//HAL_UART_Receive_IT(&huart1,inputBuf,1);//UART receive data interrupt 1 byte

   }
   else if(huart->Instance==USART2) //WIFI USART2
	   {
	  //  DISABLE_INT();
		if(g_wifi.linking_tencent_cloud_doing ==1){
	
			   g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			   g_wifi.wifi_rx_data_counter++;
	
               if(g_wifi.wifi_rx_data_counter >1){
			   if(g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter-2]==0x0D \
			   	&& g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter-1]==0x0A){
			   
				   
				   Wifi_Rx_InputInfo_Handler();
				   g_wifi.wifi_rx_data_counter=0;
			   }
               }
	
		       
		}
		else{
	
			   if(g_wifi.get_rx_beijing_time_enable==1){
					  g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] = wifi_rx_inputBuf[0];
					  g_wifi.wifi_rx_data_counter++;
					   
			   }
			   else
				   Subscribe_Rx_Interrupt_Handler();
		}
		//	ENABLE_INT();
		// __HAL_UART_CLEAR_OREFLAG(&huart2);
		// HAL_UART_Receive_IT(&huart2,wifi_rx_inputBuf,1);
	}
}
#endif 
/**********************************************************
**********************************************************/
void ll_gpio_falling_callback(uint16_t gpio_pin)
{
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    

   switch(gpio_pin){

   case KEY_POWER_Pin:
       // DISABLE_INT(); //WT.EDIT 2024.08.15 modify.
        if(KEY_POWER_VALUE()  ==KEY_DOWN){

        xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
        POWER_BIT_0,      /* 设置目标任务事件标志位bit0  */
        eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
        &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


        }

     //  ENABLE_INT();
   break;

   case KEY_MODE_Pin:

//       if(KEY_MODE_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on){
//        xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
//               MODE_BIT_1,     /* 设置目标任务事件标志位bit0  */
//               eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
//               &xHigherPriorityTaskWoken);

//        /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


//       }

     g_key.key_mode_flag = KEY_MODEL_ID; 

   break;


   case KEY_DOWN_Pin:
      // DISABLE_INT();
       if(KEY_DOWN_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on){
        
         xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
                DOWN_BIT_2,     /* 设置目标任务事件标志位bit0  */
                eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
                &xHigherPriorityTaskWoken);

         /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
        }
     ///  ENABLE_INT();
   break;

   case KEY_UP_Pin:
      ///   DISABLE_INT();
        if(KEY_UP_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){
        xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
                UP_BIT_3,     /* 设置目标任务事件标志位bit0  */
                eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
                &xHigherPriorityTaskWoken);

         /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
         }
        
    ///    ENABLE_INT();
   break;



    }

}


/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/

void vtask_isq_handler(void)
{
	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	 xTaskNotifyFromISR(xHandleTaskDecoderPro,  /* 目标任务 */
                                    DECODER_BIT_9,     /* 设置目标任务事件标志位bit0  */
                                    eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
                                    &xHigherPriorityTaskWoken);

                /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


}
