#ifndef __BSP_KEY_APP_H
#define __BSP_KEY_APP_H
#include "main.h"

#define SHORT_PRESS_THRESHOLD   50
#define LONG_PRESS_THRESHOLD    60
#define COUNTER_LOCK            200



void key_handler(void);


void key_power_longk_fun(void);

void key_mode_long_fun(void);

void key_down_long_fun(void);

void handle_mode_key(void);

void handle_power_key(void);
void handle_mode_key(void);
void handle_down_key(void);
void handle_up_key(void);


#endif 

