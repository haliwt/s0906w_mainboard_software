#include "bsp.h"







typedef void (*Usart1RxCallback)(uint8_t data);

static Usart1RxCallback  usart1_rx_cb = NULL;

// 注册回调函数
void usart1_register_rx_callback(Usart1RxCallback cb)
{
   usart1_rx_cb = cb;

}
/**
*@brief // 提供一个接口给中断调用
*@note
*@param
*@interval
**/
void usart1_rx_displayboard_callbck_invoke(uint8_t data)
{
    if (usart1_rx_cb != NULL)
    {
        usart1_rx_cb(data);
    }
}

/**
*@brief  //注册一个实现函数
*@note
*@param
*@interval
**/
void callback_register_usart1_rx(void)
{

    usart1_register_rx_callback(usart1_isr_callback_handler);

}

/**
*@brief  
*@note
*@param
*@interval
**/
typedef struct Msg
{

	uint8_t  usData[12];
    uint8_t  ucMessageID;
    uint8_t  rx_data_counter;
    uint8_t  disp_rx_cmd_done_flag;
    uint8_t  bcc_check_code;
    volatile uint8_t ulid;

}MSG_T;

MSG_T   gl_tMsg; /* 定义丢�个结构体用于消息队列 */

uint8_t check_code;
uint8_t rx_end_flag;


/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  receive the second display board data outside
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void usart1_isr_callback_handler(uint8_t data)
{
    static volatile uint8_t state ;
    switch(state)
		{
		case 0:  //#0
			if(inputBuf[0] == 0xA5){  // 0xA5 -- second display board ID
               gl_tMsg.rx_data_counter=0;
			   gl_tMsg.ulid=0;
               gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];
				state=1; //=1

             }
			else if(inputBuf[0]== 0xF0){//IAP boodloader flag
        
	           state = 0x0A;//gl_tMsg.ucMessageID = 0xF0;
			}
            else
                state=0;
		break;


		case 1: //#1

            if(gl_tMsg.disp_rx_cmd_done_flag ==0){
              /* 初始化结构体指针 */
               gl_tMsg.rx_data_counter++;

	          gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];


              if(rx_end_flag == 1){

                state = 0;

                gl_tMsg.ulid = gl_tMsg.rx_data_counter;
                rx_end_flag=0;

                gl_tMsg.rx_data_counter =0;

                gl_tMsg.disp_rx_cmd_done_flag = 1 ;

                gl_tMsg.bcc_check_code=inputBuf[0];

                 vtask_isq_handler();
                 //usart1_rx_decoder();

              }

              }

              if(gl_tMsg.usData[gl_tMsg.rx_data_counter] ==0xFE && rx_end_flag == 0 &&   gl_tMsg.rx_data_counter > 4){

                     rx_end_flag = 1 ;

              }

        break;

		  }
}


void usart1_rx_decoder(void)
{

	gl_tMsg.disp_rx_cmd_done_flag = 0;
	check_code =	bcc_check(gl_tMsg.usData,gl_tMsg.ulid);

	if(check_code == gl_tMsg.bcc_check_code ){

	receive_data_from_displayboard(gl_tMsg.usData);
	// memset(gl_tMsg.usData, 0, 4); // 初始化为 0

	}



}



