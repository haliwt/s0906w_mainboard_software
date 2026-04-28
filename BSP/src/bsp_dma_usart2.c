#include "bsp.h"

//static void usart2_isr_callback_handler(uint8_t data);

/**
* @brief  call back example
*
*
*/

typedef void (*Usart2RxCallback)(uint8_t data);

static Usart2RxCallback usart2_rx_cb = NULL;


//注册一个回调函数
void usart2_register_rx_callback(Usart2RxCallback cb)
{
    usart2_rx_cb = cb;

}

// 提供一个接口给中断调用
void usart2_rx_callback_invoke(uint8_t data)
{
    if (usart2_rx_cb != NULL)
    {
        usart2_rx_cb(data);
    }
}


//具体实例一个函数

void callback_register_usart2_rx(void)
{

 // usart2_register_rx_callback(usart2_isr_callback_handler);


}

/**
* @brief  call back example
*
*
*/
void usart2_isr_callback_fun(uint8_t data)
{
    #if 0
     wifi_rx_inputBuf[0] =data;
     if(g_wifi.linking_tencent_cloud_doing ==1){

			g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			g_wifi.wifi_rx_data_counter++;

			if(*wifi_rx_inputBuf==0x0A) // 0x0A = "\n"
			{
				
				Wifi_Rx_InputInfo_Handler();
				g_wifi.wifi_rx_data_counter=0;
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
    
    dma_tx_done = 0;
	
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_4);
    /* 5. 配置传输参数 */
    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_4,
                            (uint32_t)txBuf,
                            (uint32_t)&USART2->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
     /* 3. 配置长度 */
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_4,txlen);

   
    /* 4. 清除所有相关标志（顺序必须在 EnableChannel 前） */
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_ClearFlag_TE4(DMA1);
    
    /* 使能 DMA 中断 */
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    
      /* 6. 启动 DMA */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

    /* 7. 使能 USART2 的 DMA TX 请求 */
    LL_USART_EnableDMAReq_TX(USART2);
    
    
//    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_4);

//	 LL_DMA_ClearFlag_TC4(DMA1);//TC5 -> clear transfer complete flag

//    LL_USART_EnableDMAReq_TX(USART2);


}

