#ifndef __BSP_DMA_USART_H
#define __BSP_DMA_USART_H
#include "main.h"


void usart1_dma_send(uint8_t *txbuf,uint16_t txlen);



void USART2_DMA_Send(uint8_t *txBuf,uint16_t txlen);

void usart2_rx_callback_invoke(uint8_t data);

void callbck_register_usat2_rx(void);

#endif 

