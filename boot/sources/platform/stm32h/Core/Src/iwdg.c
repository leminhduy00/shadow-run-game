/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   This file provides code for the configuration
  *          of the IWDG instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "iwdg.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* IWDG1 init function */
void MX_IWDG1_Init(void)
{

  /* USER CODE BEGIN IWDG1_Init 0 */

  /* USER CODE END IWDG1_Init 0 */

  /* USER CODE BEGIN IWDG1_Init 1 */

  /* USER CODE END IWDG1_Init 1 */
  LL_IWDG_Enable(IWDG1);
  LL_IWDG_EnableWriteAccess(IWDG1);
  LL_IWDG_SetPrescaler(IWDG1, LL_IWDG_PRESCALER_256);
  LL_IWDG_SetReloadCounter(IWDG1, 3749);
  while (LL_IWDG_IsReady(IWDG1) != 1)
  {
  }

  LL_IWDG_ReloadCounter(IWDG1);
  /* USER CODE BEGIN IWDG1_Init 2 */

  /* USER CODE END IWDG1_Init 2 */

}

/* USER CODE BEGIN 1 */
void IWDG_ReloadCounter(void)
{
    LL_IWDG_ReloadCounter(IWDG1);
}
/* USER CODE END 1 */

