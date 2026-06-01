#include "bsp.h"
#include "bsp_streamlight.h"



#define KEY_MODE_SHORT   (1 << 0)
#define KEY_MODE_LONG    (1 << 1)

#define KEY_UP_SHORT     (1 << 2)
#define KEY_UP_LONG      (1 << 3)

#define KEY_DOWN_SHORT   (1 << 4)
#define KEY_DOWN_LONG    (1 << 5)

#define KEY_POWER_SHORT  (1 << 6)
#define KEY_POWER_LONG   (1 << 7)


/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#define STACK_SIZE_DECODER  256//512//128//1792//3072//2048//1024//896//768
#define STACK_SIZE_UI      1536//1024//1536//1280//1024//1536//1024//896//1792//1664//1280
#define STACK_SIZE_KEY     256//512//512
#define STACK_SIZE_EVENT   512//640//768//256

__attribute__((aligned(8))) static UCHAR stack_ui_pro[STACK_SIZE_UI];
__attribute__((aligned(8))) static UCHAR stack_decoder_pro[STACK_SIZE_DECODER];
__attribute__((aligned(8))) static UCHAR stack_start_pro[STACK_SIZE_KEY];
__attribute__((aligned(8))) static UCHAR stack_key_event[STACK_SIZE_EVENT];






/*在 ThreadX 里，优先级数字越小，优先级越高：*/

static TX_THREAD thread_ui;
static TX_THREAD thread_start;
static TX_THREAD thread_decoder;
static TX_THREAD thread_key_event;

/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;
/*队列*/
//static TX_QUEUE uart1_rx_queue;
//static uint8_t uart1_rx_queue_buffer[UART1_RX_BUF_SIZE * sizeof(uint8_t)];

TX_EVENT_FLAGS_GROUP key_event;

//TX_TIMER beep_timer;


static void vTaskUiPro(ULONG thread_input);
static void vTaskStart(ULONG thread_input);
static void vTaskKeyEvent(ULONG thread_input);


static void vTaskDecoderPro(ULONG thread_input);
//static void beep_timer_callback(ULONG input);


static void threadx_handler(void);
/* 创建任务通信机制 */
static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr);

#if DEBUG_ENABLE

static void debug_stack_ui_check(void);

static void debug_stack_key_check(void);

static void debug_stack_decoder_check(void);

static void debug_stack_key_event_check(void);

ULONG unused,unused_key,unused_decoder,unused_event ;
#endif 

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
    memset(stack_start_pro, 0xEF, sizeof(stack_start_pro));
	memset(stack_decoder_pro, 0xEF, sizeof(stack_decoder_pro));
	memset(stack_key_event, 0xEF, sizeof(stack_key_event));
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

   tx_event_flags_create(&key_event, "key_event");
   
	tx_thread_create(&thread_decoder,       /* 任务控制块地址 */ 
					"DecoderPro",           /* 任务名 */
					vTaskDecoderPro,       // 每个消息大小，这里用 1 字节
					0,                       /* 传递给任务的参数 */
					stack_decoder_pro,      /* 堆栈基地址 */
					STACK_SIZE_DECODER,       /* 堆栈空间大小 */ 
					3,
					3,
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
                     0, 						   /* 任务优先级*/
                     0, 						   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			   /* 不开启时间片 */
                     TX_AUTO_START);               /* 创建后立即启动 */
  #endif 
  tx_thread_create(&thread_key_event, 			   /* 任务控制块地址 */	  
					 "KeyEvent",						/* 任务名 */
					  vTaskKeyEvent,					/* 启动任务函数地址 */
					  0,							/* 传递给任务的参数 */
					  stack_key_event,				/* 堆栈基地址 */
					  STACK_SIZE_EVENT,				/* 堆栈空间大小 */  
					  1,							/* 任务优先级*/
					  1,							/* 任务抢占阀值 */
					   TX_NO_TIME_SLICE, 			/* 不开启时间片 */
					  TX_AUTO_START);				/* 创建后立即启动 */

//   tx_timer_create(&beep_timer,     /* timer of  block */
//				   "20msTimer",
//				   beep_timer_callback, /*callback function */
//   				   0,
//				   2,                    /* 第一次延迟 20ms*/
//				   2,                    /*周期 20 ticks*/
//				   TX_AUTO_ACTIVATE);    

 
}
/**********************************************************************************************************
*	Function Name: static void vTaskDecoderPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)

**********************************************************************************************************/
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
			 #if DEBUG_ENABLE
              debug_stack_decoder_check();
            #endif 
	  }
	
   }
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

    static uint16_t mode_cnt = 0;
    static uint16_t up_cnt = 0;
    static uint16_t down_cnt = 0;
    static uint16_t power_cnt = 0;

    const uint16_t LONG_PRESS_TIME = 40;   // 300 * 10ms = 3000ms
  
  
   while(1)
   {

        if(KEY_POWER_VALUE() == KEY_DOWN)
        {
            power_cnt++;
            if(power_cnt == LONG_PRESS_TIME && g_pro.gpower_on == power_on){
                tx_event_flags_set(&key_event, KEY_POWER_LONG, TX_OR);
             }
        }
        else if(KEY_POWER_VALUE() == KEY_UP && power_cnt > 0)
        {
            if(power_cnt > 0 && power_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_POWER_SHORT, TX_OR);

            power_cnt = 0;
        }
		else if(KEY_MODE_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on)
        {
            mode_cnt++;
            if(mode_cnt == LONG_PRESS_TIME){
				tx_event_flags_set(&key_event, KEY_MODE_LONG, TX_OR);
               
            }
        }
        else if(KEY_MODE_VALUE() == KEY_UP && mode_cnt > 0)
        {
            if(mode_cnt > 0 && mode_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_MODE_SHORT, TX_OR);
            mode_cnt = 0;
        }
	    else if(KEY_UP_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on)
        {
            up_cnt++;
            if(up_cnt == LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_UP_LONG, TX_OR);
        }
        else if(KEY_UP_VALUE() == KEY_UP && up_cnt > 0)
        {
            if(up_cnt > 0 && up_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_UP_SHORT, TX_OR);

            up_cnt = 0;
        }
		else if(KEY_DOWN_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on)
        {
            down_cnt++;
            if(down_cnt == LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_DOWN_LONG, TX_OR);
        }
        else if(KEY_DOWN_VALUE() == KEY_UP && down_cnt > 0)
        {
            if(down_cnt > 0 && down_cnt < LONG_PRESS_TIME)
                tx_event_flags_set(&key_event, KEY_DOWN_SHORT, TX_OR);

            down_cnt = 0;
        }
       

    
   #if DEBUG_ENABLE
    debug_stack_key_check();
   #endif 

     

	 tx_thread_sleep(8);//10ms *2 

   }
	
 } 

uint8_t  event_error_counter;
/**********************************************************************************************************
*	Function Name: static void vTaskRunPro(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传的形参
*	Return Ref:
*   priority: 1  (数值越小优先级越低，这个跟uCOS相反)
**********************************************************************************************************/
static void vTaskKeyEvent(ULONG thread_input)
{
  (void)thread_input;  /* 消除未使用的参数警告 */
  ULONG flags;
  UINT status;
 

  while(1)
  {

     status = tx_event_flags_get(&key_event,
                           0xFFFFFFFF,
                           TX_OR_CLEAR,
                           &flags,
                           TX_WAIT_FOREVER);//50);//(200));//TX_NO_WAIT);//TX_WAIT_FOREVER);//
                           
     if(status == TX_SUCCESS){

	    if(flags & KEY_POWER_SHORT) handle_power_key();
	    else if(flags & KEY_POWER_LONG)  key_power_longk_fun();//handle_power_long_key();
        else if(flags & KEY_MODE_SHORT)  handle_mode_key();
	    else if(flags & KEY_MODE_LONG)   key_mode_long_fun();
        else if(flags & KEY_UP_SHORT)    handle_up_key();
	    else if(flags & KEY_DOWN_SHORT)  handle_down_key();
	    else if(flags & KEY_DOWN_LONG)   key_down_long_fun();//handle_down_long_key();
        tx_thread_sleep(20); //WT.EDIT 2026-05-23
         //LL_IWDG_ReloadCounter(IWDG);
        #if DEBUG_ENABLE
             // debug_stack_key_event_check();
          #endif 
	   
     }
	 else{

	   tx_thread_sleep(30); 

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
static void vTaskUiPro(ULONG thread_input)
{
  (void)thread_input;  /* 消除未使用的参数警告 */
  static uint8_t wifi_check_counter = 0;
  static uint8_t beijing_time_counter = 0;
	
  while(1)
  {
      if(g_pro.gpower_on == power_on){
          //pw_counter++;
          power_on_handler();
	  }
	  else if(g_pro.gpower_on == power_off){
          power_off_handler();

	  }
    
	  // 限制WiFi通信处理频率，避免长时间阻塞
      if(g_pro.time_50ms_f ==1 && g_wifi.wifi_led_fast_blink_flag==0){
	  	  g_pro.time_50ms_f =0;
	  	  wifi_communication_tnecent_handler();
	  }

	  // WiFi状态检查：每200ms执行一次（约每10个循环）
      wifi_check_counter++;//20ms * 50 = 1000ms =1s
      if(wifi_check_counter >= 50 && g_wifi.wifi_led_fast_blink_flag==0){
          wifi_check_counter = 0;
          wifi_auto_detected_link_state();
      }
      else{
      // 北京时获取：每9s执行一次（约每25个循环）
       getBeijingTime_cofirmLinkNetState_handler();
      }
      

	  #if DEBUG_ENABLE
		    debug_stack_ui_check();
	  #endif 
	  LL_IWDG_ReloadCounter(IWDG);
      tx_thread_sleep(1);
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
    //NVIC_SystemReset();
    tx_thread_sleep(20);
    //while(1);  // 调试阶段可以卡住
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

void vtask_key_power(void)
{
  tx_event_flags_set(&key_event, KEY_POWER_SHORT, TX_OR);
}

//static void beep_timer_callback(ULONG input)
//{
//  (void)input;
//  buzzer_sound_close();
//}

//void open_beep_sound(void)
//{
//  tx_timer_activate(&beep_timer);
//}


#if DEBUG_ENABLE
static void debug_stack_ui_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计

  
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_UI; i++)
    {
        if (stack_ui_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
  
 
	
	unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_stack_key_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计


    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_KEY; i++)
    {
        if (stack_start_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
    unused_key = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_stack_decoder_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计


    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_DECODER; i++)
    {
        if (stack_decoder_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
    unused_decoder = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_stack_key_event_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计


    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_EVENT; i++)
    {
        if (stack_key_event[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
    unused_event = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}


#endif 

