/*
 * bsp_fan.h
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */

#ifndef BSP_INC_BSP_FAN_H_
#define BSP_INC_BSP_FAN_H_
#include "main.h"



#define FAN_PWM_100     2560//40

#define FAN_PWM_90      2304//36

#define FAN_PWM_80      0//256//1280//34


//#define FAN_COM_SetHigh()         do{FAN_CCW_GPIO_Port->BSRR =FAN_CCW_Pin;}while(0)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_SET)    // output high level
//#define FAN_COM_SetLow()          do{FAN_CCW_GPIO_Port->BSRR =(uint32_t)FAN_CCW_Pin<<16;}while(0)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_RESET)    // output low level

#define FAN_RUN_SetHigh()         do{FAN_RUN_GPIO_Port->BSRR =FAN_RUN_Pin;}while(0)//HAL_GPIO_WritePin(FAN_GPIO,FAN_RUN_GPIO_Port-)    // output high level
#define FAN_RUN_SetLow()          do{FAN_RUN_GPIO_Port->BSRR =(uint32_t)FAN_RUN_Pin<<16;}while(0)    // output low level

//#define FAN_CCW_SetHigh()         do{FAN_CCW_GPIO_Port->BSRR |=FAN_CCW_Pin;}while(0)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_SET)    // output high level
//#define FAN_CCW_SetLow()          do{FAN_CCW_GPIO_Port->BSRR |=(uint32_t)FAN_CCW_Pin<<16;}while(0)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_RESET)    // output low level


void FAN_Stop(void);

void Fan_Full_Speed(void);


void Fan_RunSpeed_Fun(void);

void SetLevel_Fan_PWMA(uint8_t levelval);








#endif /* BSP_INC_BSP_FAN_H_ */
