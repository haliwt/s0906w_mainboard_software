#ifndef __BSP_USART_H
#define __BSP_USART_h
#include "main.h"



void usart1_rx_decoder(void);



void usart1_rx_callback_invoke(uint8_t data);


void callback_register_usart1_rx(void);



#endif 


