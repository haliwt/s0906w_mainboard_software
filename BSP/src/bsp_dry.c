/*
 * bsp_dry.c
 *
 *  Created on: 2025年3月7日
 *      Author: Administrator
 */
#include "bsp.h"

void active_error_handler(void)
{

	if(g_pro.gDry > 1 || g_pro.gPlasma > 1 || g_pro.gMouse >1){
			if(g_pro.gDry > 1){
				g_pro.gDry =1;
				DRY_OPEN();
				LED_DRY_ON();

			}
			if(g_pro.gPlasma > 1){
              g_pro.gPlasma =1;
			  PLASMA_OPEN();
			  LED_PLASMA_ON();

			}
			if(g_pro.gMouse >1){
               g_pro.gMouse =1;
			   mouse_open();
			   LED_MOUSE_ON();
           

			}
	}

}


