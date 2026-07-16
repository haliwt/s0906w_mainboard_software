#include "bsp.h"

#include "bsp.h"


#define Zero_Degree           5828    
#define ADC_Sample_Times      10

#define COMPENSATION_VALUE    0

#define NTC_RES_F3425         1

#define DISP_VALUE            10






uint16_t *pArray[23];
uint8_t find_out_temperature_value;
uint8_t temp_uint16_t_vlue;
uint8_t length_simple;

uint16_t temp_variable_value_1;

uint8_t  temp_degree;
uint8_t temp_decimal_point;
uint16_t ntc_voltage_value;
uint8_t array_subscript;

uint8_t disp_temp_degree;

uint8_t read_input_times;

uint8_t disp_ntc_value[DISP_VALUE];




//static uint16_t Read_NTC_Temperature_Voltage(void);

static int8_t  Binary_Search(const uint8_t *array ,uint8_t key,uint8_t length);


static uint8_t Calculate_Display_Temperature_Value(const uint16_t *pt,uint8_t key,uint16_t ntc_res_value,uint8_t length);


static uint8_t ntc_res_linear_value(uint8_t ntc_value);

static void Calculate_zero_Temperature_Value(uint8_t idata);



uint8_t search_key;

typedef enum{

   degree_zero = 0,
   degree_one = 1,
   degree_two = 2,
   degree_three = 3,
   degree_four = 4,
   degree_five =5,
   degree_six = 6,
   degree_seven = 7,
   degree_eight = 8,
   degree_nine = 9,
   degree_ten = 10,
   degree_eleven = 11,
   degree_twelve = 12,
   degree_thirteen =13,
   degree_fourteen = 14,
   degree_fiveteen =15,
   degree_sixteen =16,
   degree_seventeen =17,
   degree_eighteen = 18,
   degree_nineteen = 19,
   degree_twenty = 20,
   degree_twenty_one = 21,
   degree_twenty_two =22,
   degree_twenty_three =23,
   degree_twenty_four =24
   
   


}degree_state;

typedef struct _ntc_t{

   uint8_t temperature_value;
   uint8_t ntc_voltage_value;
   uint8_t temperature_rectify_value;
   uint16_t ntc_res_read_adc_value;
   uint8_t  read_ntc_tem_value;
   
}ntc_res_t;


ntc_res_t ntc_t;



int8_t left_point =0;
int8_t right_point ;
////int8_t length = sizeof(R10K_Init_20_96_simple)/(sizeof(R10K_Init_20_96_simple[0]));
int8_t mid_value;
//拆分�???23个数组，mid =11
//static const uint16_t R10K_0_0[2]={2558,2528};
//static const uint16_t R10K_1_4[4]={2497,2466,2434,2402};


//static const uint16_t R10K_5_7[3]={2382,2348,2314};
//static const uint16_t R10K_8_10[3]={2280,2245,2210,};
//static const uint16_t R10K_11_13[3]={2174,2138,2102};

//static const uint16_t R10K_14_15[3]={2065,2028};
//static const uint16_t R10K_16_18[3]={1991,1954,1917};

//NTC RES F3950  new calculation 

static const uint16_t R10K_0_1[2]={2432,2404}; //array[0] = error range = 28
static const uint16_t R10K_2_4[3]={2375,2346,2317}; //array[0] = error range = 32
static const uint16_t R10K_5_7[3]={2287,2257,2226};//array[1] error range = 32
static const uint16_t R10K_8_11[4]={2196,2165,2134,2102}; //array[2] error range = 32
static const uint16_t R10K_12_14[3]={2071,2039,2007}; //array[2] error range = 32
static const uint16_t R10K_15_17[3]={1976,1944,1912}; //array[2] error range = 32


//
static const uint16_t R10K_18_20[3]={1880,1848,1816}; //array[0] = error range = 32
static const uint16_t R10K_21_23[3]={1784,1752,1721};//array[1] error range = 32
static const uint16_t R10K_24_26[3]={1689,1658,1627}; //array[2] error range = 32

static const uint16_t R10K_27_30[4]={1597,1567,1537,1507}; //array[3] =error range = 30
static const uint16_t R10K_31_33[3]={1477,1448,1419};       //array[4] =error range = 29
static const uint16_t R10K_34_37[4]={1391,1362,1334,1307}; //array[5]=error range= 27
static const uint16_t R10K_38_41[4]={1279,1253,1226,1200};  //array[6]=error range = 27
static const uint16_t R10K_42_44[3]={1175,1149,1124};  //array[7]=error range = 25

static const uint16_t R10K_45_49[5]={1099,1075,1051,1028,1005}; //array[8]=error range = 24

static const uint16_t R10K_50_53[4]={982,960,938,917};  //array[9]�??? error range = 21
static const uint16_t R10K_54_58[5]={896,875,855,835,816}; //array[10]：error range = 19

static const uint16_t R10K_59_64[6]={797,779,760,743,725,708};  //array[11]：error range = 16
static const uint16_t R10K_65_71[7]={692,675,660,644,629,614,600};  //array[12]�??? error range = 14 

static const uint16_t R10K_72_78[7]={585,572,558,545,532,520,507}; //array[13]:error range = 12

static const uint16_t R10K_79_88[10]={495,484,472,461,450,440,430,419,410,400}; ////array[14]:error range = 10

static const uint16_t R10K_89_100[12] ={391,382,373,364,356,348,340,332,324,317,309,302}; //array[15]:error range = 8

static const uint16_t R10K_101_118[18] ={295,289,282,276,270,264,258,252,246,241,235,230,225,220,215,211,206,201}; //array[16]:error range = 4

static const uint16_t R10K_119_151[33]={197,193,188,184,180,176,172,169,165,162,158,154,152,
	                  148,145,142,139,137,134,131,128,126,123,121,118,116,114,111,109,107,105,103,101}; //array[17] error range = 4


static const uint16_t R10K_152_180[29]={99,97,95,93,91,90,88,86,85,83,81,80,79,77,76,74,73,72,70,69,68,67,65,64,63,62,
	                   61,60,59};




///


static const uint8_t R10K_Init_0_120_simple[25]={

    24,23,22,21,20,
	19,18,17,16,15,
	14,13,12,11,10,
	9,8,7,6,5,
	4,3,2,1,0
};



static uint16_t Get_Adc_Average(uint32_t ch,uint8_t times);

static void Calculate_Speicial_Temperature_Value(uint8_t temp);

uint16_t ptc_res_temp_voltage;

uint8_t disp_value;
uint16_t adcx,temp_value;

/*****************************************************************
	*
	*Function Name: static uint16_t Get_Adc(uint32_t ch)  
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?
	*Return Ref: No
	*
*****************************************************************/
void Get_Ntc_Resistance_Temperature_Handler(uint16_t voltage)
{
   // static uint8_t read_adc_value;


    ntc_t.ntc_res_read_adc_value = voltage ;

    temp_value= voltage /100;
	
	length_simple =  sizeof(R10K_Init_0_120_simple)/sizeof(R10K_Init_0_120_simple[0]);

	if(temp_value > 0){
        disp_temp_degree = Binary_Search(R10K_Init_0_120_simple,temp_value,length_simple);
	}
	
	if(temp_value==0){
	    Calculate_zero_Temperature_Value(0);
    }
    else Calculate_Speicial_Temperature_Value(disp_temp_degree);
    
    //g_pro.read_ntc_temperature_value = ntc_res_linear_value(ntc_t.read_ntc_tem_value);
     g_pro.read_ntc_temperature_value = ntc_t.read_ntc_tem_value;
   }
/*****************************************************************
	*
	*Function Name: void getNtc_temperatureValue_init(uint16_t voltage)
	*Function :
	*Input Ref: NO
	*Return Ref: No
	*
*****************************************************************/
void getNtc_temperatureValue_init(uint16_t voltage)
{
   // static uint8_t read_adc_value;


    ntc_t.ntc_res_read_adc_value = voltage ;

    temp_value= voltage /100;
	
	length_simple =  sizeof(R10K_Init_0_120_simple)/sizeof(R10K_Init_0_120_simple[0]);
    
   	 disp_temp_degree = Binary_Search(R10K_Init_0_120_simple,temp_value,length_simple);
    
	Calculate_Speicial_Temperature_Value(disp_temp_degree);
    
    g_pro.read_ntc_temperature_value = ntc_t.read_ntc_tem_value;
     
    // g_pro.read_ntc_temperature_value = ntc_res_linear_value(ntc_t.read_ntc_tem_value);

 }

/*************************************************************************
	*
	*Functin Name: static int8_t  Binary_Search(uint8_t *R10K_NTC_81 ,uint8_t key)
	*Funtion: binary search arithmetic
	*
	*
	*
*************************************************************************/
static int8_t  Binary_Search(const uint8_t *array ,uint8_t key,uint8_t length)
{
    left_point =0 ;
	right_point = length -1;
   while(left_point <= right_point){

        mid_value = (left_point + right_point)/2;

        if(key == array[mid_value]){

		    return   mid_value;

		}
        else if(array[mid_value] >   key ){ //move right  -> right small numbers

             left_point = mid_value +1;
			

		}
		else if(array[mid_value] < key){ //move left -> left big numbers

          right_point = mid_value -1;
		   

       }
	} 
   
	return -1;
} 

/******************************************************************************
	*
	*Function Name: static void Calculate_Speicial_Temperature_Value(uint8_t temp)
	*Function :calculate special value
	*Input Ref: speical transmit display temperature value 
	*Return Ref:NO
	*
******************************************************************************/
static void Calculate_Speicial_Temperature_Value(uint8_t temp)
{
  // static uint8_t array_subscript;

   switch(temp){


   case degree_zero: //zero degree 0~1 degree

           // zero_d =2;
          array_subscript =  Calculate_Display_Temperature_Value(R10K_0_1,0,ntc_t.ntc_res_read_adc_value,2);
		 if(array_subscript > 1)array_subscript=1;

         switch(array_subscript){

         case 0:
        		 ntc_t.read_ntc_tem_value = 0 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 1:
  		 	ntc_t.read_ntc_tem_value = 1 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

  		}

     break;


      case degree_one: //zero degree 2~4 degree

		// zero_d =2;
		array_subscript =  Calculate_Display_Temperature_Value(R10K_2_4,1,ntc_t.ntc_res_read_adc_value,3);
		if(array_subscript > 2)array_subscript=2;

		switch(array_subscript){

			case 0:
			 ntc_t.read_ntc_tem_value = 2 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 1:
			ntc_t.read_ntc_tem_value = 3 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 2:
			ntc_t.read_ntc_tem_value = 4 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

		}

     break;

	  case degree_two: //zero degree 5~7 degree

			// zero_d =2;
			array_subscript =  Calculate_Display_Temperature_Value(R10K_5_7,2,ntc_t.ntc_res_read_adc_value,3);
			if(array_subscript > 2)array_subscript=2;

			switch(array_subscript){

				case 0:
				 ntc_t.read_ntc_tem_value = 5 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

				case 1:
				ntc_t.read_ntc_tem_value = 6 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

				case 2:
				ntc_t.read_ntc_tem_value = 7 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

			}

		 break;

		case degree_three: //zero degree 8~11 degree

		// zero_d =2;
		array_subscript =  Calculate_Display_Temperature_Value(R10K_8_11,3,ntc_t.ntc_res_read_adc_value,4);
		if(array_subscript > 3)array_subscript=3;

		switch(array_subscript){

			case 0:
			 ntc_t.read_ntc_tem_value = 8 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 1:
			ntc_t.read_ntc_tem_value = 9 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 2:
			ntc_t.read_ntc_tem_value = 10 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 3:
			 ntc_t.read_ntc_tem_value = 11 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

		}

		break;

		case degree_four: //zero degree 12~14 degree

			// zero_d =2;
			array_subscript =  Calculate_Display_Temperature_Value(R10K_12_14,4,ntc_t.ntc_res_read_adc_value,3);
			if(array_subscript > 2)array_subscript=2;
			switch(array_subscript){

				case 0:
				 ntc_t.read_ntc_tem_value = 12 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

				case 1:
				ntc_t.read_ntc_tem_value = 13 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

				case 2:
				ntc_t.read_ntc_tem_value = 14 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

				break;

			}

		break;


		case degree_five: //zero degree 15~17 degree

		// zero_d =2;
		array_subscript =  Calculate_Display_Temperature_Value(R10K_15_17,5,ntc_t.ntc_res_read_adc_value,3);
		if(array_subscript > 2)array_subscript=2;

		switch(array_subscript){

			case 0:
			ntc_t.read_ntc_tem_value = 15 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 1:
			ntc_t.read_ntc_tem_value = 16 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

			case 2:
			ntc_t.read_ntc_tem_value = 17 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

			break;

		}

		break;


    // older data
    case degree_six: //zero degree 18 `` 20 degree

         // zero_d =2;
        array_subscript =  Calculate_Display_Temperature_Value(R10K_18_20,6,ntc_t.ntc_res_read_adc_value,3);
        if(array_subscript > 2)array_subscript=2;
       switch(array_subscript){

       case 0:
      		 ntc_t.read_ntc_tem_value = 18 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

       break;

       case 1:
		 	ntc_t.read_ntc_tem_value = 19 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

		 case 2:
		 	ntc_t.read_ntc_tem_value = 20 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

       	   }

   break;


   case degree_seven: //zero degree  21~23 degree

         // zero_d =2;
        array_subscript =  Calculate_Display_Temperature_Value(R10K_21_23,7,ntc_t.ntc_res_read_adc_value,3);
       if(array_subscript > 2)array_subscript=2;
       switch(array_subscript){

		 case 0:
		 	ntc_t.read_ntc_tem_value = 21 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

		 case 1:
		 	ntc_t.read_ntc_tem_value = 22 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

		 case 2:
		 	ntc_t.read_ntc_tem_value = 23+ ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;


        }

   break;

   case degree_eight: //24~26 degree
   	   array_subscript =  Calculate_Display_Temperature_Value(R10K_24_26,8,ntc_t.ntc_res_read_adc_value,3);
	   if(array_subscript > 2)array_subscript=2;
	   switch(array_subscript){

		 case 0:
		 	ntc_t.read_ntc_tem_value = 24 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;

		 break;

		 case 1:
		 	ntc_t.read_ntc_tem_value = 25 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

		 case 2:
		 	ntc_t.read_ntc_tem_value = 26 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		 break;

		

        }

   break;

   
   case degree_nine : //27~~30 degree

         array_subscript =  Calculate_Display_Temperature_Value(R10K_27_30,9,ntc_t.ntc_res_read_adc_value,4);
		 // HAL_Delay(5);
  		  if(array_subscript > 3)array_subscript=3;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 27 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 28 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 2:
			  ntc_t.read_ntc_tem_value = 29 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 3:
			  ntc_t.read_ntc_tem_value = 30 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  
   
		  }

   break;
   
   case degree_ten: //31~~33 degree

         array_subscript =  Calculate_Display_Temperature_Value(R10K_31_33,10,ntc_t.ntc_res_read_adc_value,3);
		 //HAL_Delay(5);
		 if(array_subscript > 2)array_subscript=2;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 31 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 32 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 33 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }


   break;

   case degree_eleven : //34 À~~37 degree
		 array_subscript =  Calculate_Display_Temperature_Value(R10K_34_37,11,ntc_t.ntc_res_read_adc_value,4);
		 if(array_subscript > 3)array_subscript=3;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 34 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 35 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 36 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 37 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }
   break;
   
   case degree_twelve: //38 ```41 degree
   	     array_subscript =  Calculate_Display_Temperature_Value(R10K_38_41,12,ntc_t.ntc_res_read_adc_value,4);
		 // HAL_Delay(5);
		  if(array_subscript > 3)array_subscript=3;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 38 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 39 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 40 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 41 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;
   
   case degree_thirteen ://42 ````44 degree
   	     array_subscript =  Calculate_Display_Temperature_Value(R10K_42_44,13,ntc_t.ntc_res_read_adc_value,3);
		  
		 if(array_subscript > 2)array_subscript=2;

		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 42 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 43 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 2:
			  ntc_t.read_ntc_tem_value = 44 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
   
		  }

   break;

  
   case degree_fourteen : //45````49 degree
   	     array_subscript =  Calculate_Display_Temperature_Value(R10K_45_49,14,ntc_t.ntc_res_read_adc_value,5);
		  //HAL_Delay(5);
		 if(array_subscript > 4)array_subscript=4;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 45 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 46+ ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 47 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 48 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 49 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;

   
   case degree_fiveteen :  //50````53 degree
   	    array_subscript =  Calculate_Display_Temperature_Value(R10K_50_53,15,ntc_t.ntc_res_read_adc_value,4);
		  //HAL_Delay(5);
		 if(array_subscript > 3)array_subscript=3;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 50 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 51 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 52 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 3:
			  ntc_t.read_ntc_tem_value = 53 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;
   
   case degree_sixteen : //54````58 degree
   	 array_subscript =  Calculate_Display_Temperature_Value(R10K_54_58,16,ntc_t.ntc_res_read_adc_value,5);
	     // HAL_Delay(5);
	      if(array_subscript > 4)array_subscript=4;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 54 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 55 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 56 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 57 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 58 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;
   
   case  degree_seventeen ://59````64 degree
   		 array_subscript =  Calculate_Display_Temperature_Value(R10K_59_64,17,ntc_t.ntc_res_read_adc_value,6);
		 // HAL_Delay(5);
		  if(array_subscript > 5)array_subscript=5;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 59 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 60 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 61 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 3:
			  ntc_t.read_ntc_tem_value = 62 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 63 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 64 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;
   
   case degree_eighteen : //65````71 degree
   	   	 array_subscript =  Calculate_Display_Temperature_Value(R10K_65_71,18,ntc_t.ntc_res_read_adc_value,7);
	      if(array_subscript > 6)array_subscript=6;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 65 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 66 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 67 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 3:
			  ntc_t.read_ntc_tem_value = 68 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 69 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 70 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 6:
			  ntc_t.read_ntc_tem_value = 71 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   
   
		  }

   break;

   case degree_nineteen : //72````78 degree
   		 array_subscript =  Calculate_Display_Temperature_Value(R10K_72_78,19,ntc_t.ntc_res_read_adc_value,7);
		//  HAL_Delay(5);
		 if(array_subscript > 6)array_subscript=6;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 72 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 73 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 74 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 3:
			  ntc_t.read_ntc_tem_value = 75 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 76 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 77 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 6:
			  ntc_t.read_ntc_tem_value = 78 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }

   break;
   
   case degree_twenty : //79~85 degree
       array_subscript =  Calculate_Display_Temperature_Value(R10K_79_88,20,ntc_t.ntc_res_read_adc_value,10);
	     //  HAL_Delay(5);
	      if(array_subscript > 9)array_subscript=9;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 79 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 80 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 81 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 82 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 83 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 84 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 6:
			  ntc_t.read_ntc_tem_value = 85 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 7:
			  ntc_t.read_ntc_tem_value = 86 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 8:
			  ntc_t.read_ntc_tem_value = 87 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 9:
			  ntc_t.read_ntc_tem_value = 88 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }


   break;

   case degree_twenty_one : //89~100 degree
       array_subscript =  Calculate_Display_Temperature_Value(R10K_89_100,21,ntc_t.ntc_res_read_adc_value,12);
	     //  HAL_Delay(5);
	      if(array_subscript > 11)array_subscript=11;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 89 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 90 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 91 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 92 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 93 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 94 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 6:
			  ntc_t.read_ntc_tem_value = 95 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 7:
			  ntc_t.read_ntc_tem_value = 96 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 8:
			  ntc_t.read_ntc_tem_value = 97 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 9:
			  ntc_t.read_ntc_tem_value = 98 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 10:
			  ntc_t.read_ntc_tem_value = 99 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		   break;

		    case 11:
			  ntc_t.read_ntc_tem_value = 100 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		  }


   break;

   case degree_twenty_two : //101~118 degree
       array_subscript =  Calculate_Display_Temperature_Value(R10K_101_118,22,ntc_t.ntc_res_read_adc_value,18);
	     //  HAL_Delay(5);
	      if(array_subscript > 17)array_subscript=17;
		 switch(array_subscript){
   
		   case 0:
			  ntc_t.read_ntc_tem_value = 101+ ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 1:
			  ntc_t.read_ntc_tem_value = 102 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 2:
			  ntc_t.read_ntc_tem_value = 103 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 3:
			  ntc_t.read_ntc_tem_value = 104 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 4:
			  ntc_t.read_ntc_tem_value = 105 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 5:
			  ntc_t.read_ntc_tem_value = 106 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 6:
			  ntc_t.read_ntc_tem_value = 107 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 7:
			  ntc_t.read_ntc_tem_value = 108 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 8:
			  ntc_t.read_ntc_tem_value = 109 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 9:
			  ntc_t.read_ntc_tem_value = 110 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 10:
			  ntc_t.read_ntc_tem_value = 111 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 11:
			  ntc_t.read_ntc_tem_value = 112 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
   
		   break;
   
		   case 12:
			  ntc_t.read_ntc_tem_value = 113 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;
   
		   case 13:
			  ntc_t.read_ntc_tem_value = 114 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 14:
			  ntc_t.read_ntc_tem_value = 115 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 15:
			  ntc_t.read_ntc_tem_value = 116 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		    case 16:
			  ntc_t.read_ntc_tem_value = 117 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		   case 17:
			  ntc_t.read_ntc_tem_value = 118 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
   
		   break;

		 
   
		  }


   break;

    case degree_twenty_three: //119````120 degree
   	   	
		 array_subscript =	Calculate_Display_Temperature_Value(R10K_119_151,23,ntc_t.ntc_res_read_adc_value,33);
				 if(array_subscript > 33)array_subscript=33;
				 switch(array_subscript){
		   
				   case 0:
					  ntc_t.read_ntc_tem_value = 119 + ntc_t.temperature_rectify_value + COMPENSATION_VALUE;
		   
				   break;
		   
				   case 1:
					  ntc_t.read_ntc_tem_value = 120 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 2:
					  ntc_t.read_ntc_tem_value = 121 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 3:
					  ntc_t.read_ntc_tem_value = 122 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 4:
					  ntc_t.read_ntc_tem_value = 123 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 5:
					  ntc_t.read_ntc_tem_value = 124 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 6:
					  ntc_t.read_ntc_tem_value = 125 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					   
					case 7:
					ntc_t.read_ntc_tem_value = 126 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
		 
					case 8:
					ntc_t.read_ntc_tem_value = 127 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 9:
					ntc_t.read_ntc_tem_value = 128 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 10:
					ntc_t.read_ntc_tem_value = 129 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
		 
					case 11:
					ntc_t.read_ntc_tem_value = 130 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 12:
					ntc_t.read_ntc_tem_value = 131 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 13:
					  ntc_t.read_ntc_tem_value = 132 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 14:
					  ntc_t.read_ntc_tem_value = 133 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 15:
					  ntc_t.read_ntc_tem_value = 134 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 16:
					  ntc_t.read_ntc_tem_value = 135 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 17:
					  ntc_t.read_ntc_tem_value = 136 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 18:
					  ntc_t.read_ntc_tem_value = 137 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					   
					case 19:
					ntc_t.read_ntc_tem_value = 138 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
		 
					case 20:
					ntc_t.read_ntc_tem_value = 139 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 21:
					ntc_t.read_ntc_tem_value = 140 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
					
					case 22:
					ntc_t.read_ntc_tem_value = 141 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 23:
					  ntc_t.read_ntc_tem_value = 142 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 24:
					  ntc_t.read_ntc_tem_value = 143 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 25:
					  ntc_t.read_ntc_tem_value = 144 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 26:
					  ntc_t.read_ntc_tem_value = 145 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
				   
				   case 27:
					  ntc_t.read_ntc_tem_value = 146 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					case 28:
					  ntc_t.read_ntc_tem_value = 147 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		   
				   break;
		 
					   
					case 29:
					ntc_t.read_ntc_tem_value = 148 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
		 
					case 30:
					ntc_t.read_ntc_tem_value = 149 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 31:
					ntc_t.read_ntc_tem_value = 150 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					case 32:
					ntc_t.read_ntc_tem_value = 151 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;
		 
					break;
		 
					

		 }

   }
}

static void Calculate_zero_Temperature_Value(uint8_t idata)
{
  
           // zero_d =2;
          array_subscript =  Calculate_Display_Temperature_Value(R10K_152_180,24,ntc_t.ntc_res_read_adc_value,29);
		 if(array_subscript > 29)array_subscript=29;

         switch(array_subscript){

         case 0:
        		 ntc_t.read_ntc_tem_value = 152 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 1:
  		 	ntc_t.read_ntc_tem_value = 153 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 2:
        		 ntc_t.read_ntc_tem_value = 154 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 3:
  		 	ntc_t.read_ntc_tem_value = 155 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 4:
        		 ntc_t.read_ntc_tem_value = 156 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 5:
  		 	ntc_t.read_ntc_tem_value = 157 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 6:
        		 ntc_t.read_ntc_tem_value = 158 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 7:
  		 	ntc_t.read_ntc_tem_value = 159 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		   case 8:
  		 	ntc_t.read_ntc_tem_value = 160 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		 
		case 9:
		     ntc_t.read_ntc_tem_value = 161 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		case 10:
		   ntc_t.read_ntc_tem_value = 162 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		 case 11:
				ntc_t.read_ntc_tem_value = 163 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		case 12:
		   ntc_t.read_ntc_tem_value = 164 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		 case 13:
				ntc_t.read_ntc_tem_value = 165 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		case 14:
		   ntc_t.read_ntc_tem_value = 166 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		 case 15:
				ntc_t.read_ntc_tem_value = 167 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		case 16:
		   ntc_t.read_ntc_tem_value = 168 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

		case 17:
		   ntc_t.read_ntc_tem_value = 169 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;


		 case 18:
        		 ntc_t.read_ntc_tem_value = 152 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 19:
  		 	ntc_t.read_ntc_tem_value = 153 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 20:
        		 ntc_t.read_ntc_tem_value = 154 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 21:
  		 	ntc_t.read_ntc_tem_value = 155 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 22:
        		 ntc_t.read_ntc_tem_value = 156 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 23:
  		 	ntc_t.read_ntc_tem_value = 157 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		  case 24:
        		 ntc_t.read_ntc_tem_value = 158 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

         break;

         case 25:
  		 	ntc_t.read_ntc_tem_value = 159 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		   case 26:
  		 	ntc_t.read_ntc_tem_value = 160 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

  		 break;

		 
		case 27:
		     ntc_t.read_ntc_tem_value = 161 + ntc_t.temperature_rectify_value +COMPENSATION_VALUE;

		break;

  		}

    

}

/******************************************************************************
	*
	*Function Name: static uint8_t Calculate_Display_Temperature_Value(const uin16_t *pt,uint8_t length)
	*Function :calculate special value
	*Input Ref: speical transmit display temperature value 
	*Return Ref:NO
	*
******************************************************************************/
static uint8_t Calculate_Display_Temperature_Value(const uint16_t *pt,uint8_t key,uint16_t ntc_res_value,uint8_t length)
{
      uint8_t i;
	  

      for(i=0;i<length;i++){

      if(*(pt+i) ==  ntc_res_value ){

             return i;


      }
     else if(*(pt+i) >  ntc_res_value && (*(pt+i+1) <ntc_res_value)){//high temperature degree is number is smaller

              
               if(i> length) i = length; 
               return i;
				 
          
          }

      }
      return i;

}

/***********************************************************************************************
	*
	*Function Name:uint8_t ntc_res_linear_value(uint8_t ntc_value)
	*Function : display of works time value 
	*Input: NO
	*Return: NO 
	*
*************************************************************************************************/
static uint8_t ntc_res_linear_value(uint8_t ntc_value)
{

 //  static uint8_t *p ;
   static uint8_t disp_init;

     if(disp_init == 0){
	  	  disp_init++ ;
         disp_ntc_value[0] =  ntc_value;

      }
	  else 
          disp_ntc_value[1] = ntc_value;


			  

    if(disp_ntc_value[1] -  disp_ntc_value[0]  >0){

        if(disp_ntc_value[1] -  disp_ntc_value[0] ==1){

        	// read_input_times =1;

        	//display_ntc_temp_value(disp_ntc_value[1]);
  
        	 return disp_ntc_value[1];

        }
        else if(disp_ntc_value[1] - disp_ntc_value[0] > 1){

           
        	  //*p = ntc_value + 1 ;
        	  disp_ntc_value[0]= disp_ntc_value[0] + 1 ;

       

        	  return  disp_ntc_value[0]  ;


        }
    }
    else{

        if(disp_ntc_value[0] - disp_ntc_value[1] ==1){

     

           return disp_ntc_value[1];

        }
        else if(disp_ntc_value[0]- disp_ntc_value[1] > 1){

        			 
     
        	 
        	 disp_ntc_value[0]= disp_ntc_value[0] - 1 ;


        	return disp_ntc_value[0] ;


        }

    }
   return disp_ntc_value[0] ;
    
}

/***********************************************************************************************
	*
	*Function Name:static void display_ntc_temp_value(uint8_t disp)
	*Function : display of works time value 
	*Input: NO
	*Return: NO 
	*
*************************************************************************************************/




