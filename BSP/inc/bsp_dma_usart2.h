#ifndef __BSP_DMA_USART2_H
#define __BSP_DMA_USART2_H
#include "main.h"

void USART2_DMA_Send(uint8_t *txBuf,uint16_t txlen);

void usart2_rx_callback_invoke(uint8_t data);

void  callback_register_usart2_rx(void);


void usart2_isr_callback_fun(uint8_t data);



#endif 

