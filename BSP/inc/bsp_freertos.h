/*
 * bsp_freertos.h
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */

#ifndef BSP_INC_BSP_FREERTOS_H_
#define BSP_INC_BSP_FREERTOS_H_
#include "main.h"

extern uint8_t inputBuf[1];


void freeRTOS_Handler(void);


void ll_gpio_falling_callback(uint16_t gpio_pin);


void vtask_isq_handler(void);

//void usart1_isr_callback_handler(uint8_t data);

#endif /* BSP_INC_BSP_FREERTOS_H_ */
