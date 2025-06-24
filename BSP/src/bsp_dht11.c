#include "bsp.h"


DHT11_Data_TypeDef dht11_data;
DHT11_Status status;


/**
 * @brief       复位DHT11
 * @param       data: 要写入的数据
 * @retval      �??
 */
static void dht11_reset(void)
{
    DHT11_DQ_OUT(0);    /* 拉低DQ */
    delay_ms(20);       /* 拉低至少18ms */
    DHT11_DQ_OUT(1);    /* DQ=1 */
    delay_us(30);       /* 主机拉高10~35us */
}

/**
 * @brief       等待DHT11的回�??
 * @param       �??
 * @retval      0, DHT11正常
 *              1, DHT11异常/不存�??
 */
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DHT11_DQ_IN && retry < 100)  /* DHT11会拉�??83us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DHT11_DQ_IN && retry < 100) /* DHT11拉低后会再次拉高87us */
        {
            retry++;
            delay_us(1);
        }
        if (retry >= 100) rval = 1;
    }
    
    return rval;
}

/**
 * @brief       从DHT11读取�??个位
 * @param       �??
 * @retval      读取到的位�??: 0 / 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)  /* 等待变为低电�?? */
    {
        retry++;
        delay_us(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) /* 等待变高电平 */
    {
        retry++;
        delay_us(1);
    }

    delay_us(40);       /* 等待40us */

    if (DHT11_DQ_IN)    /* 根据引脚状�?�返�?? bit */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       从DHT11读取�??个字�??
 * @param       �??
 * @retval      读到的数�??
 */
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* 循环读取8位数�?? */
    {
        data <<= 1;                 /* 高位数据先输�??, 先左移一�?? */
        data |= dht11_read_bit();   /* 读取1bit数据 */
    }

    return data;
}

/**
 * @brief       从DHT11读取�??次数�??
 * @param       temp: 温度�??(范围:-20~50°)
 * @param       humi: 湿度�??(范围:5%~95%)
 * @retval      0, 正常.
 *              1, 失败
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* 读取40位数�?? */
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief       初始化DHT11的IO�?? DQ 同时�??测DHT11的存�??
 * @param       �??
 * @retval      0, 正常
 *              1, 不存�??/不正�??
 */
uint8_t dht11_init(void)
{
    GPIO_InitTypeDef gpio_init_struct={0};

    DHT11_DQ_GPIO_CLK_ENABLE();     /* �??启DQ引脚时钟 */

    gpio_init_struct.Pin = DHT11_DQ_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* �??漏输�?? */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高�?? */
    HAL_GPIO_Init(DHT11_DQ_GPIO_PORT, &gpio_init_struct);   /* 初始化DHT11_DQ引脚 */
    /* DHT11_DQ引脚模式设置,�??漏输�??,上拉, 这样就不用再设置IO方向�??, �??漏输出的时�??(=1), 也可以读取外部信号的高低电平 */

    dht11_reset();
    return dht11_check();
}



/**
 * @brief  read_
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */
uint8_t read_sensor_dht11_data(void)
{
   uint8_t status;  
	 // 读取DHT11数据
    status = dht11_read_data(&dht11_data.temperature,&dht11_data.humidity);
    //dht11_read_data(&dht11_data.temperature,&dht11_data.humidity);
	osDelay(200);
    if(status != DHT11_OK)
    {
        // 读取失败，显示错误代�??
        LED_TEMP_ICON_ON();
        LED_HUM_ICON_OFF();
        SMG_Display_Err(0);
        return status;
    }

	return  DHT11_OK;

}


/**
 * @brief  在TM1639上显示DHT11的温湿度数据
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */
DHT11_Status DHT11_Display_Data(uint8_t mode)
{

    static uint8_t  copy_temp_value, copy_humidity_value;
    uint8_t  status;
   
    // 读取DHT11数据
    status = dht11_read_data(&dht11_data.temperature,&dht11_data.humidity);


	if(status !=0){
	    if(mode == 0)
	    {
	    	 LED_TEMP_ICON_ON();
	         LED_HUM_ICON_OFF();
	           
	         TM1639_Display_Temperature(copy_temp_value);
	    }
	    else
	    {
	        // 显示湿度
	    	LED_TEMP_ICON_OFF();
	    	LED_HUM_ICON_ON();
	        TM1639_Display_Humidity(copy_humidity_value);
		}
    }
	else if(status==0){
   
    // 根据模式显示温度或湿�??
    if(mode == 0)
    {
        // 显示温度
//        if(dht11_data.is_negative){
//            TM1639_Display_Temperature(-dht11_data.temperature);
//        }
//        else{
        	LED_TEMP_ICON_ON();
        	LED_HUM_ICON_OFF();
           
            TM1639_Display_Temperature(dht11_data.temperature);
		     copy_temp_value = dht11_data.temperature;
				
        
    }
    else
    {
        // 显示湿度
    	LED_TEMP_ICON_OFF();
    	LED_HUM_ICON_ON();
        TM1639_Display_Humidity(dht11_data.humidity);
		copy_humidity_value = dht11_data.humidity;
    }
	}
    
    return DHT11_OK;
}

/**
 * @brief  在TM1639上显示DHT11的温湿度数据
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */

uint8_t read_dht11_temperature_value(void)
{
    static uint8_t copy_dht11_value;
	uint8_t error_flag;
	error_flag =  dht11_read_data(&dht11_data.temperature,&dht11_data.humidity);
	if(error_flag !=0){
		return copy_dht11_value;
	}
	else if(error_flag ==0){
		 copy_dht11_value=dht11_data.temperature;
	    return dht11_data.temperature;

	}
	
}


/**
 * @brief  void updateDht11_toDisplayBoard_value(void)
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */
void updateDht11_toDisplayBoard_value(void)
{
    
     static uint8_t error_flag,counter;
	 static uint8_t copy_dht11_temp,copy_dht11_hum;
	 error_flag = dht11_read_data(&dht11_data.temperature,&dht11_data.humidity);
	
    if(error_flag ==0){

	    
		if(dht11_data.temperature!=0 && dht11_data.humidity!=0){
		    sendData_Real_TimeHum(dht11_data.humidity,dht11_data.temperature);
			osDelay(5);
			copy_dht11_temp= dht11_data.temperature;
		    copy_dht11_hum = dht11_data.humidity;
			g_pro.current_temperature= copy_dht11_temp;
		
		}
		else if(dht11_data.temperature==0 && dht11_data.humidity!=0){
          counter++;
		  if(counter > 4){//continuce 4 times is confirm is dht11_data.temperature=0
		      counter=0;
		      sendData_Real_TimeHum(dht11_data.humidity,dht11_data.temperature);
			  osDelay(5);


		  }
		  else{
		  	  counter=0;
			  sendData_Real_TimeHum(copy_dht11_hum,copy_dht11_temp);
			  osDelay(5);
		  }

		}
		
    }
	else{
	    sendData_Real_TimeHum(copy_dht11_hum,copy_dht11_temp);
		   
		osDelay(5);


	}
	
    
}

/**
 * @brief  void Update_Dht11_Totencent_Value(void)
 * @param  mode: 0-显示温度�??1-显示湿度
 * @retval DHT11_Status 类型的操作结�??
 */
void Update_Dht11_Totencent_Value(void)
{
    static uint8_t error_flag;
    error_flag= dht11_read_data(&dht11_data.temperature, &dht11_data.humidity);

	//Dht11_Read_TempHumidity_Handler(&DHT11);
	if(error_flag == 0){
	 g_pro.g_temperature_value = dht11_data.temperature;
	 g_pro.g_humidity_value= dht11_data.humidity;

	MqttData_Publis_ReadTempHum(dht11_data.temperature,dht11_data.humidity);
    osDelay(100);//HAL_Delay(100);

    }

}









