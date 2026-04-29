#include "bsp.h"
#include "bsp_streamlight.h"



/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#define STACK_SIZE_ONE  256//1792//3072//2048//1024//896//768
#define STACK_SIZE_UI  512//384//256
#define STACK_SIZE_KEY  256

/*在 ThreadX 里，优先级数字越小，优先级越高：*/

static TX_THREAD thread_ui;
static TX_THREAD thread_start;
static TX_THREAD thread_decoder;
/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;
/*队列*/
//static TX_QUEUE uart1_rx_queue;
//static uint8_t uart1_rx_queue_buffer[UART1_RX_BUF_SIZE * sizeof(uint8_t)];


static UCHAR stack_decoder_pro[STACK_SIZE_ONE];
static UCHAR stack_ui_pro[STACK_SIZE_UI];

static UCHAR stack_start_pro[STACK_SIZE_KEY];




static void vTaskUiPro(ULONG thread_input);
static void vTaskStart(ULONG thread_input);

static void vTaskDecoderPro(ULONG thread_input);


static void threadx_handler(void);
/* 创建任务通信机制 */
static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr);

static void debug_stack_check(void);

ULONG unused ;


/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

void tx_application_define(void *first_unused_memory)
{
  
    /* 1. 消除参数未引用警告 */
    (void)first_unused_memory;

    #if DEBUG_ENABLE
    /* 2. 只有当 stack_msg_pro 是全局定义的静态数组时，这样写才有效 */
    memset(stack_ui_pro, 0xEF, sizeof(stack_ui_pro));
    // memset(stack_start_pro, 0xEF, sizeof(stack_start_pro));
    #endif 

    /* 3. 注册堆栈错误回调（推荐保持） */
 
    tx_thread_stack_error_notify(tx_thread_stack_error_handler);

    // 创建线程、信号量、事件组、队列
     threadx_handler();
}



	
/**********************************************************************************************************
*
*	Function Name: AppTaskCreate
*	功能说明: 创建应用任务
*	Input Ref:
*	Return Ref:
*
**********************************************************************************************************/

static void threadx_handler(void)
{

      /* 创建信号量 */
   tx_semaphore_create(&decoder_semaphore, "DecoderSemaphore", 0);
   
	tx_thread_create(&thread_decoder,
					"DecoderPro",
					vTaskDecoderPro,   // 每个消息大小，这里用 1 字节
					0,
					stack_decoder_pro,
					STACK_SIZE_ONE,
					0,
					0,
					TX_NO_TIME_SLICE,
					TX_AUTO_START);
				
	tx_thread_create(&thread_ui,                  /* 任务控制块地址 */ 
 	                 "UiPro",                     /* 任务名 */
                     vTaskUiPro,                  /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_ui_pro,                /* 堆栈基地址 */
                     STACK_SIZE_UI,               /* 堆栈空间大小 */ 
                     2,							   /* 任务优先级*/
                     2,							   /* 任务抢占阀值 , 允许它不被优先级 1-0 之间的任务抢占，除非是中断 */
                     TX_NO_TIME_SLICE,             /* 不开启时间片 */
                     TX_AUTO_START);               /* 创建后立即启动 */
 #if 1

    tx_thread_create(&thread_start,                /* 任务控制块地址 */    
    				 "Start",                      /* 任务名 */
                     vTaskStart,                   /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_start_pro,              /* 堆栈基地址 */
                     STACK_SIZE_KEY,			   /* 堆栈空间大小 */  
                     1, 						   /* 任务优先级*/
                     1, 						   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			   /* 不开启时间片 */
                     TX_AUTO_START);               /* 创建后立即启动 */
  #endif 


 
}

/**
  * @brief	:  static void vTaskStart(void *pvParameters
  * @note	 
  * @param	 None
  * @retval  None
  */
 static void vTaskStart(ULONG thread_input)
 {
   (void)thread_input;  /* 消除未使用的参数警告 */
  
  
   while(1){
       if(KEY_MODE_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){

	       key_mode_long_fun();
		   if(g_key.mode_key_long_counter == COUNTER_LOCK) g_key.key_mode_flag = 11;
		   else{
		        g_key.key_mode_flag = KEY_MODEL_ID;
			    g_key.key_down_flag=0;
			    g_key.key_up_flag=0;

		   }
	 }
	 else if(KEY_UP_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){

	      g_key.key_up_flag = KEY_UP_ID;
		  g_key.key_power_flag=0;
	       g_key.key_mode_flag=0;
     }
	 else if(KEY_DOWN_VALUE() == KEY_DOWN  && g_pro.gpower_on == power_on){

            key_down_long_fun();
			
			if(g_key.down_key_long_counter == COUNTER_LOCK)g_key.key_down_flag = 13;
			else{
               g_key.key_down_flag =0x01;
			   g_key.key_power_flag=0;
	           g_key.key_mode_flag=0;
		   }

     }
     else if(KEY_POWER_VALUE()  ==KEY_DOWN){

         key_power_longk_fun();
	     if(g_key.power_on_key_counter == COUNTER_LOCK)g_key.key_power_flag = 9;
         else
		     g_key.key_power_flag = KEY_POWER_ID;

     }

     #if DEBUG_ENABLE

   //  debug_stack_check();

     
   #endif 
	 //key_handler();
	 //LL_IWDG_ReloadCounter(IWDG);
	 tx_thread_sleep(20);

   }
	
 } 

/**********************************************************************************************************
*	Function Name: static void vTaskDecoderPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)

**********************************************************************************************************/
#if 1
static void vTaskDecoderPro(ULONG thread_input)
{
   (void)thread_input;  /* 消除未使用的参数警告 */

    while(1)
    {

	 // 阻塞等待 ISR 投递
      if(tx_semaphore_get(&decoder_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
      {
			/* 接收到消息，�?测那个位被按�? */
            usart1_rx_decoder(); 
	  }
	
   }
}
#endif 
/**********************************************************************************************************
*	Function Name: static void vTaskRunPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)
**********************************************************************************************************/
static void vTaskUiPro(ULONG thread_input)
{
  (void)thread_input;  /* 消除未使用的参数警告 */
  static uint16_t counter_f;
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
	
   LL_IWDG_ReloadCounter(IWDG);

   #if DEBUG_ENABLE
    debug_stack_check();
   #endif 

	tx_thread_sleep(10);

	  
    }
	  
}
/****************************************************************
*
*	Function Name: vTaskStart
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传�?�的形参
*	Return Ref:
*	priority: 3  (数�?�越小优先级越低，这个跟uCOS相反)
*
******************************************************************/
void tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{
    /* 栈溢出处理：这里你可以做任何你想做的动作 */

    /* 1. 打印线程名（如果有 UART） */
    // printf("Stack overflow in thread: %s\n", thread_ptr->tx_thread_name);

    /* 2. 拉高故障指示灯 */
    // Fault_LED_On();

    /* 3. 记录日志（如果有 Flash/EEPROM） */
    // Log_Fault(FAULT_STACK_OVERFLOW, thread_ptr->tx_thread_name);

    /* 4. 触发系统复位（汽车级） */
    // NVIC_SystemReset();

    while(1);  // 调试阶段可以卡住
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
    tx_semaphore_put(&decoder_semaphore);
}


#if DEBUG_ENABLE
static void debug_stack_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计

   #if 1
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_UI; i++)
    {
        if (stack_ui_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
   #else 
    /* 从高地址往低地址扫描 */
    for (i = STACK_SIZE_UI - 1; i >= 0; i--)
    {
        if (stack_ui_pro[i] == 0xEF)
            temp_unused++;
        else
            break;
    }



   #endif 
 
	
	unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

#endif 

