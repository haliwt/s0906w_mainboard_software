/*
 * bsp_freertos.h
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */

#ifndef __BSP_THREADX_H_
#define __BSP_THREADX_H_
#include "main.h"



void vtask_isq_handler(void);
void vtask_key_power(void);

void open_beep_sound(void);
void tx_set_once_timer(void);


void tx_close_beep(void);


#endif /* BSP_INC_BSP_FREERTOS_H_ */
