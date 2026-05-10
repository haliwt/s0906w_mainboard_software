#include "bsp.h"

#define Bit_RESET 0
#define Bit_SET   1

uint8_t read_flag;

static void DHT11_Mode_InPut(void);
static void DHT11_Mode_OutPut(void);
static uint8_t DHT11_ReadByte_Fun(void);
static void TIM17_Init_1MHz(void);


DHT11_Data_TypeDef DHT11;
void static Dht11_Read_TempHumidity_Handler(DHT11_Data_TypeDef * pdth11);

uint8_t DHT11_ReadData(uint8_t *humi, uint8_t *temp);



uint8_t dht11_read_flag;

//??us??
//void delay_us(unsigned long i)
//{
//	unsigned long j;
//	for(;i>0;i--)
//	{
//			for(j=6;j>0;j--);
//	}
//}
void DHT11_Init(void)
{
    /* GPIO 默认配置为输出高电平 */
   

    DHT11_Mode_OutPut();//DHT11_GPIO_Output();
    DHT11_Dout_HIGH();//DHT11_WritePin(1);

    /* 初始化 TIM14 为 1MHz 计数 */
   // TIM17_Init_1MHz();
}

/*---------------- TIM14 1MHz 延时 ----------------*/

static void TIM17_Init_1MHz(void)
{
    /* 1. 开启 TIM16 时钟 */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);

    /* 2. 配置计数器 */
    LL_TIM_SetPrescaler(TIM17, 64 - 1);     // 64MHz / 64 = 1MHz
    LL_TIM_SetAutoReload(TIM17, 0xFFFF);    // 最大周期
    LL_TIM_SetCounterMode(TIM17, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM17, LL_TIM_CLOCKDIVISION_DIV1);

    /* 3. 使能计数器 */
    LL_TIM_EnableCounter(TIM17);

    /* 4. 可选：允许更新事件 */
    LL_TIM_GenerateEvent_UPDATE(TIM17);
}


static void delay_us_dht11(uint32_t us)
{
   uint16_t start = TIM17->CNT;

    while ((uint16_t)(TIM17->CNT - start) < us)
    {
        /* busy wait */
    }
}


/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_InPut(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
 	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

     /**/
	  GPIO_InitStruct.Pin = TEMP_SENSOR_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; //DOWN;
	  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_OutPut(void)
{
	

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
/**/
	GPIO_InitStruct.Pin = TEMP_SENSOR_Pin ;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;//LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
  * ????: ?DHT11??????,MSB??
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static uint8_t DHT11_ReadByte ( void )
{

      uint8_t i,dat=0;
	 for(i=0;i<8;i++) 
		  {
			   	while(DHT11_Data_IN()==Bit_RESET);
			
				delay_us_dht11(40);
				
				  if(DHT11_Data_IN()==Bit_SET)
					{
						while(DHT11_Data_IN()==Bit_SET);
					 
						
						dat|=(uint8_t)(0x01 << (7 - i)); // ����1
					}
					else
					{
						dat&=(uint8_t)~(0x01 << (7 - i)); // ����0
					}
			}
		  return dat;
 
}

/**
 * @brief  读取 DHT11 温湿度
 * @param  humi: 湿度输出指针
 * @param  temp: 温度输出指针
 * @retval 0: 成功，其他: 失败
 */
uint8_t DHT11_ReadData(uint8_t *humi, uint8_t *temp)
{
    uint8_t data[5] = {0};
    uint32_t timeout;
    UINT old_post;

    //if (humi == 0 || temp ==0)
        ///return 1;

    /* 1. 禁止 ThreadX 调度 + 全局中断（关键时序区） */
	
    old_post = tx_interrupt_control(TX_INT_DISABLE);

   // __disable_irq();

    /* 2. 主机拉低 18ms */
    DHT11_Mode_OutPut();//DHT11_GPIO_Output();
    DHT11_Dout_LOW();//DHT11_WritePin(0);
    delay_us(18000);//delay_us_dht11(18000);

    /* 3. 拉高 20~40us */
    DHT11_Dout_HIGH();//DHT11_WritePin(1);
    delay_us(30);//delay_us_dht11(30);

    /* 4. 切换输入，等待 DHT11 响应 */
    DHT11_Mode_InPut();//DHT11_GPIO_Input();
    delay_us(5);//delay_us_dht11(5);

    /* 等待 DHT11 拉低（80us） */
    timeout = 0;
    while (DHT11_Data_IN())
    {
        if (++timeout > 3000) goto error_1;
        delay_us(1);//delay_us_dht11(1);
    }

    /* 等待 DHT11 拉高（80us） */
    timeout = 0;
    while (!DHT11_Data_IN()	 )
    {
        if (++timeout > 9000) goto error_2;
        delay_us(1);//delay_us_dht11(1);
    }

    /* 等待 DHT11 再次拉低，开始传输数据 */
    timeout = 0;
    while (DHT11_Data_IN()	 )
    {
        if (++timeout > 3000) goto error_3;
        delay_us(1);//delay_us_dht11(1);
    }

    /* 5. 读取 5 字节（40bit） */
    for (uint8_t i = 0; i < 5; i++)
    {
        data[i] = DHT11_ReadByte();
        if (data[i] == 0xFF) goto error_4;
    }

    /* 6. 恢复中断 & 调度 */
    ///__enable_irq();
    tx_interrupt_control(old_post);


    /* 7. 校验 */
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return 2;

	

    *humi = data[0];
    *temp = data[2];

	


    return 0;

error_1:
   // __enable_irq();
    tx_interrupt_control(old_post);
    return 1;

	error_2:
		//__enable_irq();
		tx_interrupt_control(old_post);
		return 2;

		error_3:
    //__enable_irq();
    tx_interrupt_control(old_post);
    return 3;
    
    	error_4:
    //__enable_irq();
    tx_interrupt_control(old_post);
    return 3;

}


/**
*@breif :
*@note:
*@param:
*@return:
*
*/
 
/**
*@breif :
*@note:
*@param:
*@return:
*
*/
uint8_t dht11_f ;


/**
 * @brief  在TM1639上显示DHT11的温湿度数据
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */

/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void read_sensorData(void)
{
	
	dht11_f =  DHT11_ReadData(&g_pro.g_humidity_value,&g_pro.g_temperature_value);// Dht11_Read_TempHumidity_Handler(&DHT11);
	    if(g_pro.disp_second_f == 1){
			if(timer_expired(&t_display)){
			 sendData_Real_TimeHum(g_pro.g_humidity_value, g_pro.g_temperature_value);
		     tx_thread_sleep(10);
			}
         }
	
}

/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void Update_Dht11_Totencent_Value(void)
{

  
	DHT11_ReadData(&g_pro.g_humidity_value,&g_pro.g_temperature_value);//Dht11_Read_TempHumidity_Handler(&DHT11);
	if(timer_expired(&t_mqtt_0)){
	MqttData_Publis_ReadTempHum(g_pro.g_humidity_value, g_pro.g_temperature_value);
   // tx_thread_sleep(20);//HAL_Delay(100);
	}

}







