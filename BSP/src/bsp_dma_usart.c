#include "bsp.h"

void usart1_dma_send(uint8_t *txbuf,uint16_t txlen)
{

    LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_1);

    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_1,
                            (uint32_t)txbuf,
                            (uint32_t)&USART1->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_1,txlen);

    LL_DMA_ClearFlag_TC1(DMA1);//TC1 -> clear transfer complete flag
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_1);

    LL_USART_EnableDMAReq_TX(USART1);

}


void usart2_isr_callback_handler(uint8_t data)
{
    #if 0
     wifi_rx_inputBuf[0] =data;
     if(net_t.linking_tencent_cloud_doing ==1){

			gpro_t.wifi_rx_data_array[gpro_t.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			gpro_t.wifi_rx_data_counter++;

			if(*wifi_rx_inputBuf==0x0A) // 0x0A = "\n"
			{
				
				Wifi_Rx_InputInfo_Handler();
				gpro_t.wifi_rx_data_counter=0;
			}

	 } 
     else{

		    if(wifi_t.get_rx_beijing_time_enable==1){
					gpro_t.wifi_rx_data_array[gpro_t.wifi_rx_data_counter] = wifi_rx_inputBuf[0];
					gpro_t.wifi_rx_data_counter++;
					
			}
			else
			    Subscribe_Rx_Interrupt_Handler();
	 }
  #else 
    wifi_rx_inputBuf[0] =data;

  if(g_wifi.linking_tencent_cloud_doing ==1){
	
			   g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			   g_wifi.wifi_rx_data_counter++;
	
               if(g_wifi.wifi_rx_data_counter >1){
			   if(g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter-2]==0x0D \
			   	&& g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter-1]==0x0A){
			   
				   
				   Wifi_Rx_InputInfo_Handler();
				   g_wifi.wifi_rx_data_counter=0;
			   }
               }
	
		       
		}
		else{
	
			   if(g_wifi.get_rx_beijing_time_enable==1){
					  g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] = wifi_rx_inputBuf[0];
					  g_wifi.wifi_rx_data_counter++;
					   
			   }
			   else
				   Subscribe_Rx_Interrupt_Handler();
		}

  #endif 

}

void USART2_DMA_Send(uint8_t *txBuf,uint16_t txlen)
{
    if(txBuf ==NULL || txlen ==0) return ;
	
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_5);

    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_5,
                            (uint32_t)txBuf,
                            (uint32_t)&USART2->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_5,txlen);

   
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_5);

	 LL_DMA_ClearFlag_TC5(DMA1);//TC5 -> clear transfer complete flag

    LL_USART_EnableDMAReq_TX(USART2);


}




