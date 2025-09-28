#include "bsp.h"




/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void usart1_isr_callback_handler(uint8_t data)
{
    static volatile state ;
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

              }

              }

              if(gl_tMsg.usData[gl_tMsg.rx_data_counter] ==0xFE && rx_end_flag == 0 &&   gl_tMsg.rx_data_counter > 4){

                     rx_end_flag = 1 ;

              }

        break;


}


