/*
 * bsp_disp.h
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */

#ifndef BSP_INC_BSP_DISP_H_
#define BSP_INC_BSP_DISP_H_
#include "main.h"


typedef enum {
    DISPLAY_TEMP = 0,
    DISPLAY_HUM = 1,
	DISPLAY_TIME = 2
} DisplayMode;



void display_digital_3_numbers(void);



#endif /* BSP_INC_BSP_DISP_H_ */
