#include "bsp.h"

// 数码管段码表�?0-9的显示码
static const uint8_t TM1639_Number_Table[] = {
    0xF3, // 0: 0011 1111   （f,e,d,c,b,a�?--0x3F
    0x60, // 1: 0000 0110 --0x06--写数据式冲低位开始，向高位开始写
    0xB5, // 2: 0101 1011 --0x5B
    0xF4, // 3: 0100 1111 --0x4F
    0x66, // 4: 0110 0110
    0xD6, // 5: 0110 1101 --0x6D
    0xD7, // 6: 0111 1101  --0x7D 
    0x70, // 7: 0000 0111
    0xF7, // 8: 0111 1111
    0xF6  // 9: 0110 1111
};

// 字母和特殊字符显示码
static const uint8_t TM1639_Char_Table[] = {
    0x67, // H: 0111 0110 (b,c,e,f,g)
    0x36, // °: 0110 0011 (b,c,g)
    0x93, // C: 0011 1001 (a,d,e,f)
    0x05  // RH的H部分: 0101 0000 (e,g)
};

#define TM1639_CHAR_H TM1639_Char_Table[0]
#define TM1639_CHAR_DEGREE TM1639_Char_Table[1]
#define TM1639_CHAR_C TM1639_Char_Table[2]
#define TM1639_CHAR_RH TM1639_Char_Table[3]

#define TM1639_DOT 0x08 // 小数点段�?,from low position start

/**
 * @brief  TM1639写入�?个字�?
 * @param  byte: 要写入的字节
 * @retval None
 */
static void TM1639_Write_Byte(uint8_t byte)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        TM1639_CLK_SetLow();
        //delay_us(2); //is big error .DATA.2025.06.13
        
        if(byte & 0x01)
            TM1639_DIO_SetHigh(); //写入数据 �?1�?
        else
            TM1639_DIO_SetLow(); //写入数据 �?0�?
            
       // delay_us(2);//is big error .DATA.2025.06.13
        TM1639_CLK_SetHigh();
       // delay_us(2);//is big error .DATA.2025.06.13
        byte >>= 1;
    }
}

/**
 * @brief  TM1639�?始信�?
 * @param  None
 * @retval None
 */
static void TM1639_Start(void)
{
    TM1639_STB_SetHigh();
    delay_us(2);
    TM1639_CLK_SetHigh();
    delay_us(2);
    TM1639_STB_SetLow();
    delay_us(2);
}

/**
 * @brief  TM1639停止信号
 * @param  None
 * @retval None
 */
static void TM1639_Stop(void)
{
    TM1639_CLK_SetLow();
    delay_us(2);
    TM1639_DIO_SetLow();
    delay_us(2);
    TM1639_STB_SetHigh();
    delay_us(2);
}

/**
 * @brief  初始化TM1639
 * @param  None
 * @retval None
 */
void TM1639_Init(void)
{
    // 设置数据命令：自动地�?增加
    TM1639_Start();
    TM1639_Write_Byte(TM1639_CMD_DATA);
    TM1639_Stop();
    
    // 设置显示控制：显示开，最大亮�?
    TM1639_Display_ON_OFF(1);
    TM1639_Set_Brightness(TM1639_BRIGHTNESS_MAX);
}

/**
 * @brief  设置显示亮度
 * @param  bright: 亮度级别(0-7)
 * @retval None
 */
void TM1639_Set_Brightness(uint8_t bright)
{
    if(bright > TM1639_BRIGHTNESS_MAX)
        bright = TM1639_BRIGHTNESS_MAX;
        
    TM1639_Start();
    TM1639_Write_Byte(TM1639_CMD_DISPLAY | TM1639_DISPLAY_ON | bright);
    TM1639_Stop();
}

/**
 * @brief  显示�?关控�?
 * @param  status: 1-�?显示�?0-关显�?
 * @retval None
 */
void TM1639_Display_ON_OFF(uint8_t status)
{
    TM1639_Start();
    if(status)
        TM1639_Write_Byte(TM1639_CMD_DISPLAY | TM1639_BRIGHTNESS_MAX);
    else
        TM1639_Write_Byte(TM1639_DONOT_DISPLAY);
    TM1639_Stop();
}

/**
 * @brief  写入完整的一位数码管（包括高4位和�?4位）
 * @param  addr_h: �?4位地�?
 * @param  addr_l: �?4位地�?
 * @param  data: 要显示的段码数据
 * @retval None
 */
void TM1639_Write_Digit_Full(uint8_t addr_h, uint8_t addr_l, uint8_t data)
{
    // 先写入低4�?
    TM1639_Start();
    TM1639_Write_Byte(addr_l);
    TM1639_Write_Byte(data & 0x0F);  // �?4位数�?
    TM1639_Stop();
    
    // 再写入高4�?
    TM1639_Start();
    TM1639_Write_Byte(addr_h);
    TM1639_Write_Byte(data >> 4);  // �?4位数�?
    TM1639_Stop();
}

/**
 * @brief  写入半个�?位数码管（包括高4位或�?4位）
 * @param  addr_h: �?4位地�?
 * @param  addr_l: �?4位地�?
 * @param  data: 要显示的段码数据
 * @retval None
 */
void TM1639_Write_Half_Digit(uint8_t addr, uint8_t data)
{

    
    // 先写�?4�?--高字节或者低字节
    TM1639_Start();
    TM1639_Write_Byte(addr);
    TM1639_Write_Byte(data);  // �?4位数�?
    TM1639_Stop();
    
  
}


/**
 * @brief  显示3位数�?
 * @param  num: 要显示的数字(0-999)
 * @retval None
 */
void TM1639_Display_3_Digit(uint8_t num)
{
    uint8_t ten, one;
    
   // 提取各位数字
   
    ten = num  / 10;
    one = num % 10;
    
    // 写入十位（最左边�?
  
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L, TM1639_Number_Table[ten]);
        
    // 写入十位（中间）
 
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L, TM1639_Number_Table[one]);
        
    // 写入个位（最右边�?'H'
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L,TM1639_CHAR_H);
}

/**
 * @brief  显示带小数点的数�?
 * @param  num: 要显示的数字(0-999)
 * @param  dot_pos: 小数点位�?(0-2)�?0表示第一位数字后的小数点
 * @retval None
 */
void TM1639_Display_Decimal(uint16_t num, uint8_t dot_pos)
{
    uint8_t hundred, ten, one;
    
    if(num > 999) num = 999;
    if(dot_pos > 2) dot_pos = 2;
    
    // 提取各位数字
    hundred = num / 100;
    ten = (num % 100) / 10;
    one = num % 10;
    
    // 写入百位（可能带小数点）
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L,
        TM1639_Number_Table[hundred] | (dot_pos == 0 ? TM1639_DOT : 0));
    
    // 写入十位（可能带小数点）
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L,
        TM1639_Number_Table[ten] | (dot_pos == 1 ? TM1639_DOT : 0));
    
    // 写入个位（可能带小数点）
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L,
        TM1639_Number_Table[one] | (dot_pos == 2 ? TM1639_DOT : 0));
}

/**
 * @brief  显示温度�?
 * @param  temp: 温度值（-9�?99℃）
 * @retval None
 */
void TM1639_Display_Temperature(int8_t temp)
{

        TEMP_ICON_ON();
		HUMIDITY_ICON_OFF();
        // 显示十位
       if(temp >= 10){
	   	     
            TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L,TM1639_Number_Table[temp / 10]);
       	}
       else{
	   	  
            TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L,TM1639_Number_Table[0]);
       	}
        
        // 显示个位
       TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L,TM1639_Number_Table[temp % 10] );
        
        // 显示度数符号
       //TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L, TM1639_CHAR_DEGREE);
        //显示小数点�?��?��?? 显示数字�?0�?
      TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L,TM1639_Number_Table[0] | TM1639_DOT);
}
/**
 * @brief  显示湿度�?
 * @param  humi: 湿度值（0-99%RH�?
 * @retval None
 */
void TM1639_Display_Humidity(uint8_t humi)
{
    if(humi > 99) humi = 99;

	HUMIDITY_ICON_ON();
    TEMP_ICON_OFF() ;
    // 显示十位
    if(humi >= 10){
        TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L, 
            TM1639_Number_Table[humi / 10]);
    }
    else
        TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L, TM1639_Number_Table[0]);
    
    // 显示个位带小数点
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L,TM1639_Number_Table[humi % 10]);
    
    // 显示RH符号
    //TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L, TM1639_CHAR_RH);
    //显示小数点�?��?��?? + 数字 �?0�?
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L, TM1639_Number_Table[0] | TM1639_DOT );
}

/**
 * @brief  清空显示
 * @param  None
 * @retval None
 */
void TM1639_Clear(void)
{
    // 清空�?有显示位
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L, 0x00);
}

/**
 * @brief  在指定位置显示字母H
 * @param  position: 显示位置(0-2)�?0为最左边
 * @retval None
 */
void TM1639_Display_H(uint8_t position)
{
 
    
    if(position > 2) position = 2;
    
    TM1639_Start();
    TM1639_Write_Byte(position); // 设置显示位置
    TM1639_Write_Byte(TM1639_CHAR_H);              // 写入字母H的段�?
    TM1639_Stop();
}

/**
 * @brief  关闭�?有显示（包括数码管和LED�?
 * @param  None
 * @retval None
 */
void TM1639_All_Off(void)
{
    // 关闭数码管显示（GRID1-GRID3�?
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG1_H, TM1639_ADDR_DIG1_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG2_H, TM1639_ADDR_DIG2_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_DIG3_H, TM1639_ADDR_DIG3_L, 0x00);
    
    // 关闭LED显示（GRID4-GRID8�?
    TM1639_Write_Digit_Full(TM1639_ADDR_GRID4_H, TM1639_ADDR_GRID4_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_GRID5_H, TM1639_ADDR_GRID5_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_GRID6_H, TM1639_ADDR_GRID6_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_GRID7_H, TM1639_ADDR_GRID7_L, 0x00);
    TM1639_Write_Digit_Full(TM1639_ADDR_GRID8_H, TM1639_ADDR_GRID8_L, 0x00);
    
    // 关闭显示
    TM1639_Display_ON_OFF(TM1639_DISPLAY_OFF);
}

/**
 * @brief  显示小数�?
 * @param  num: 
 * @retval None
 */

void disp_decimal_point(uint8_t idata)
{
    if(idata==1){ //DIG3 ->SEG_H
       


	}
	else{ //disp two deicmal point 


	}


}
