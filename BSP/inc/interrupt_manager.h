/*
 * interrupt_manager.h
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */

#ifndef __INTERRUPT_MANAGER_H_
#define __INTERRUPT_MANAGER_H_
#include "main.h"


//void tim17_isr_callback_handler(void);

// ISR 调用的触发函数
void tim_invoke_callback(uint8_t tim_id);

void callback_register_fun(void);



#endif /* BSP_INC_INTERRUPT_MANAGER_H_ */
