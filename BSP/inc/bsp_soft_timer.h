#ifndef __BSP_SOFT_TIMER_H
#define __BSP_SOFT_TIMER_H
#include "main.h"
#include "app_threadx.h"

typedef struct {
    ULONG last;
    ULONG interval;
} soft_timer_t;


// 仅仅是声明，告诉编译器这些变量在别处
extern soft_timer_t t_mqtt_0;
extern soft_timer_t t_mqtt_1;
extern soft_timer_t t_display;
extern soft_timer_t t_xdp;
extern soft_timer_t t_wifi_0;
extern soft_timer_t t_dht11;




void soft_timer_init(void);

int timer_expired(soft_timer_t *t);


#endif 

