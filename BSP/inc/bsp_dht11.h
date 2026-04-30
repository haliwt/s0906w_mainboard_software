/*
 * bsp_dht11.h
 *
 *  Created on: 2025年3月5日
 *      Author: Administrator
 */

#ifndef __BSP_DHT11_H_
#define __BSP_DHT11_H_
#include "main.h"



#define TEMP_ICON_ON()         	do{LED_TEMP_GPIO_Port->BSRR |=(uint32_t)LED_TEMP_Pin << 16 ;}while(0) //low level
#define TEMP_ICON_OFF()         do{LED_TEMP_GPIO_Port->BSRR |=LED_TEMP_Pin;}while(0) //high level

#define HUMIDITY_ICON_ON()      do{LED_HUM_SYMBOL_GPIO_Port->BSRR |=(uint32_t)LED_HUM_SYMBOL_Pin << 16;}while(0)
#define HUMIDITY_ICON_OFF()     do{LED_HUM_SYMBOL_GPIO_Port->BSRR |=LED_HUM_SYMBOL_Pin;}while(0)




//IO????
#define DHT11_DATA_IO_IN()      {GPIOB->MODER&=0XFFFFFFFC;GPIOB->MODER|=0<<0;}  //0x00 input mode
#define DHT11_DATA_IO_OUT()     {GPIOB->MODER&=0XFFFFFFFC;GPIOB->MODER|=1<<0;}   //0x01 output  mode 

#define DHT11_DATA      GPIO_PIN_4
#define DHT11_GPIO      GPIOA

#define DHT11_DATA_SetHigh()            do{DHT11_GPIO -> BSRR = DHT11_DATA;}while(0)//HAL_GPIO_WritePin(DHT11_GPIO,DHT11_DATA,GPIO_PIN_SET)    // output high level
#define DHT11_DATA_SetLow()             do{DHT11_GPIO -> BSRR = (uint32_t)DHT11_DATA<<16;}while(0)//HAL_GPIO_WritePin(DHT11_GPIO,DHT11_DATA,GPIO_PIN_RESET)    // output low level

//#define DHT11_ReadData()	           LL_GPIO_IsInputPinSet(DHT11_GPIO,DHT11_DATA) //HAL_GPIO_ReadPin(DHT11_GPIO,DHT11_DATA)

/* ??? -------------------------------------------------------------------*/
/***********************   DHT11 ??????  **************************/
#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()//__HAL_RCC_GPIOA_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOA
#define DHT11_Dout_PIN                            LL_GPIO_PIN_4

/***********************   DHT11 ?????  ****************************/
#define DHT11_Dout_LOW()                         do{DHT11_Dout_PORT->BSRR |=(uint32_t)DHT11_Dout_PIN<<16;}while(0)// HAL_GPIO_WritePin(DHT11_Dout_PORT, DHT11_Dout_PIN, GPIO_PIN_RESET)
#define DHT11_Dout_HIGH()                        do{DHT11_Dout_PORT->BSRR =DHT11_Dout_PIN;}while(0)//HAL_GPIO_WritePin(DHT11_Dout_PORT, DHT11_Dout_PIN, GPIO_PIN_SET)
#define DHT11_Data_IN()	                         LL_GPIO_IsInputPinSet(DHT11_Dout_PORT,DHT11_Dout_PIN)//HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)



/* ???? ------------------------------------------------------------------*/
/************************ DHT11 ??????******************************/

// DHT11错误代码定义
typedef enum {
    DHT11_OK = 0,           // 操作成功
    DHT11_NO_RESPONSE,      // 传感器无响应
    DHT11_CHECKSUM_ERROR,   // 校验和错误
    DHT11_TIMEOUT_ERROR     // 通信超时错误
} DHT11_Status;


typedef struct
{
	uint8_t  humi_high8bit;		//????:???8?
	uint8_t  humi_low8bit;	 	//????:???8?
	uint8_t  temp_high8bit;	 	//????:???8?
	uint8_t  temp_low8bit;	 	//????:???8?
	uint8_t  check_sum;	 	    //???
	  float    humidity;            //????
  float    temperature;        //????  
} DHT11_Data_TypeDef;

extern DHT11_Data_TypeDef DHT11;

void DHT11_Init(void);

void updateDht11_sensorData_toDisp(void);

void Update_Dht11_Totencent_Value(void);    
                            
void read_sensorData(void);








#endif /* BSP_INC_BSP_DHT11_H_ */
