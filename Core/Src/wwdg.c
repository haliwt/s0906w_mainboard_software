/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wwdg.c
  * @brief   This file provides code for the configuration
  *          of the WWDG instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "wwdg.h"

#if 0

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* WWDG init function */
void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */

  /* USER CODE END WWDG_Init 0 */

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);

  /* USER CODE BEGIN WWDG_Init 1 */

  /* USER CODE END WWDG_Init 1 */
  LL_WWDG_SetCounter(WWDG, 0x7F);//
  LL_WWDG_Enable(WWDG);
  LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_1);
  LL_WWDG_SetWindow(WWDG, 0x7F);//
  /* USER CODE BEGIN WWDG_Init 2 */
 
  /* USER CODE END WWDG_Init 2 */

}
#endif 
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */



/* IWDG init function */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
 
  LL_IWDG_EnableWriteAccess(IWDG);
  //LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);
  //LL_IWDG_SetReloadCounter(IWDG, 4095);

  // 设置分频与重载（约 5s）
  //T = ((Reload+1)*Prescaler)/Flsi; //Flsi = 32KHZ .
   LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_128); //Prescaler = 128
   LL_IWDG_SetReloadCounter(IWDG, 1249);// T =((Reload+1)*Prescaler)/32000=5s
	
  while (LL_IWDG_IsReady(IWDG) != 1)
  {
  }
  /* 立即喂狗一次，加载新参数 */
  LL_IWDG_ReloadCounter(IWDG);
  /* USER CODE BEGIN IWDG_Init 2 */
   // 启动 IWDG
    LL_IWDG_Enable(IWDG);

    /* 再喂一次，确保参数生效 */
    LL_IWDG_ReloadCounter(IWDG);

  /* USER CODE END IWDG_Init 2 */

}


