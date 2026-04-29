#include"bsp.h"


/* 实际分配内存的地方 */
soft_timer_t t_mqtt_0;
soft_timer_t t_mqtt_1;
soft_timer_t t_display;
soft_timer_t t_xdp;
soft_timer_t t_wifi_0;
soft_timer_t t_dht11;


static inline void timer_set(soft_timer_t *t, ULONG ms)
{
    t->interval = ms;
    t->last = tx_time_get();
}

int timer_expired(soft_timer_t *t)
{
    ULONG now = tx_time_get();
    if (now - t->last >= t->interval) {
        t->last = now;
        return 1;
    }
    return 0;
}




void soft_timer_init(void)
{
	timer_set(&t_mqtt_0, 20);       //10ms*20 =200ms
	timer_set(&t_mqtt_1, 20);       //10ms*20 =200ms
	timer_set(&t_display, 10);    //10ms*10 =100ms
    timer_set(&t_xdp, 10);    //10ms*10 =100ms
	timer_set(&t_wifi_0, 5);        //10ms*5 =200ms
	timer_set(&t_dht11, 20);      //10ms*20 =200ms
	//timer_set(&t_adc, 10);        //10ms*10 =200ms

}
