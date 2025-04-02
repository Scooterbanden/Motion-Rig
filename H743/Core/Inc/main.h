/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_HRTIM_MspPostInit(HRTIM_HandleTypeDef *hhrtim);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Config_1_Pin GPIO_PIN_2
#define Config_1_GPIO_Port GPIOE
#define Config_2_Pin GPIO_PIN_3
#define Config_2_GPIO_Port GPIOE
#define Config_3_Pin GPIO_PIN_4
#define Config_3_GPIO_Port GPIOE
#define Config_4_Pin GPIO_PIN_5
#define Config_4_GPIO_Port GPIOE
#define Config_5_Pin GPIO_PIN_6
#define Config_5_GPIO_Port GPIOE
#define UI_EncA_Pin GPIO_PIN_0
#define UI_EncA_GPIO_Port GPIOA
#define UI_EncB_Pin GPIO_PIN_1
#define UI_EncB_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_4
#define LED2_GPIO_Port GPIOA
#define UI_Sw_Pin GPIO_PIN_5
#define UI_Sw_GPIO_Port GPIOA
#define UI_Sw_EXTI_IRQn EXTI9_5_IRQn
#define S4_PulseGP_Pin GPIO_PIN_6
#define S4_PulseGP_GPIO_Port GPIOA
#define S3_PulseGP_Pin GPIO_PIN_7
#define S3_PulseGP_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOC
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOC
#define LED5_Pin GPIO_PIN_0
#define LED5_GPIO_Port GPIOB
#define BTN_A_Pin GPIO_PIN_1
#define BTN_A_GPIO_Port GPIOB
#define BTN_A_EXTI_IRQn EXTI1_IRQn
#define BTN_B_Pin GPIO_PIN_2
#define BTN_B_GPIO_Port GPIOB
#define BTN_B_EXTI_IRQn EXTI2_IRQn
#define S4_Ready_Pin GPIO_PIN_7
#define S4_Ready_GPIO_Port GPIOE
#define S4_Ready_EXTI_IRQn EXTI9_5_IRQn
#define S4_EncA_Pin GPIO_PIN_9
#define S4_EncA_GPIO_Port GPIOE
#define S4_EncB_Pin GPIO_PIN_11
#define S4_EncB_GPIO_Port GPIOE
#define S4_Enable_Pin GPIO_PIN_14
#define S4_Enable_GPIO_Port GPIOE
#define S4_Direction_Pin GPIO_PIN_15
#define S4_Direction_GPIO_Port GPIOE
#define Alt_EncA_Pin GPIO_PIN_10
#define Alt_EncA_GPIO_Port GPIOB
#define S4_EncZ_Pin GPIO_PIN_11
#define S4_EncZ_GPIO_Port GPIOB
#define S4_EncZ_EXTI_IRQn EXTI15_10_IRQn
#define LogicShifter_OE_Pin GPIO_PIN_12
#define LogicShifter_OE_GPIO_Port GPIOB
#define S4_Treach_Pin GPIO_PIN_13
#define S4_Treach_GPIO_Port GPIOB
#define S4_Treach_EXTI_IRQn EXTI15_10_IRQn
#define S2_PulseGP_Pin GPIO_PIN_14
#define S2_PulseGP_GPIO_Port GPIOB
#define S3_Ready_Pin GPIO_PIN_15
#define S3_Ready_GPIO_Port GPIOB
#define S3_Ready_EXTI_IRQn EXTI15_10_IRQn
#define S3_Enable_Pin GPIO_PIN_8
#define S3_Enable_GPIO_Port GPIOD
#define S3_Direction_Pin GPIO_PIN_9
#define S3_Direction_GPIO_Port GPIOD
#define S3_Treach_Pin GPIO_PIN_10
#define S3_Treach_GPIO_Port GPIOD
#define S3_Treach_EXTI_IRQn EXTI15_10_IRQn
#define Alt_EncB_Pin GPIO_PIN_11
#define Alt_EncB_GPIO_Port GPIOD
#define S3_EncA_Pin GPIO_PIN_12
#define S3_EncA_GPIO_Port GPIOD
#define S3_EncB_Pin GPIO_PIN_13
#define S3_EncB_GPIO_Port GPIOD
#define S3_EncZ_Pin GPIO_PIN_14
#define S3_EncZ_GPIO_Port GPIOD
#define S3_EncZ_EXTI_IRQn EXTI15_10_IRQn
#define S2_Enable_Pin GPIO_PIN_15
#define S2_Enable_GPIO_Port GPIOD
#define S3_PulseHR_Pin GPIO_PIN_6
#define S3_PulseHR_GPIO_Port GPIOC
#define S2_Direction_Pin GPIO_PIN_7
#define S2_Direction_GPIO_Port GPIOC
#define S2_PulseHR_Pin GPIO_PIN_8
#define S2_PulseHR_GPIO_Port GPIOC
#define S1_PulseGP_Pin GPIO_PIN_9
#define S1_PulseGP_GPIO_Port GPIOC
#define S2_Ready_Pin GPIO_PIN_8
#define S2_Ready_GPIO_Port GPIOA
#define S2_Ready_EXTI_IRQn EXTI9_5_IRQn
#define S2_Treach_Pin GPIO_PIN_9
#define S2_Treach_GPIO_Port GPIOA
#define S2_Treach_EXTI_IRQn EXTI9_5_IRQn
#define S1_PulseHR_Pin GPIO_PIN_10
#define S1_PulseHR_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define S2_EncA_Pin GPIO_PIN_15
#define S2_EncA_GPIO_Port GPIOA
#define S2_EncZ_Pin GPIO_PIN_12
#define S2_EncZ_GPIO_Port GPIOC
#define S2_EncZ_EXTI_IRQn EXTI15_10_IRQn
#define S1_EncZ_Pin GPIO_PIN_0
#define S1_EncZ_GPIO_Port GPIOD
#define S1_EncZ_EXTI_IRQn EXTI0_IRQn
#define S1_Direction_Pin GPIO_PIN_1
#define S1_Direction_GPIO_Port GPIOD
#define S1_Enable_Pin GPIO_PIN_2
#define S1_Enable_GPIO_Port GPIOD
#define S1_Treach_Pin GPIO_PIN_3
#define S1_Treach_GPIO_Port GPIOD
#define S1_Treach_EXTI_IRQn EXTI3_IRQn
#define S1_Ready_Pin GPIO_PIN_4
#define S1_Ready_GPIO_Port GPIOD
#define S1_Ready_EXTI_IRQn EXTI4_IRQn
#define S2_EncB_Pin GPIO_PIN_3
#define S2_EncB_GPIO_Port GPIOB
#define S1_EncA_Pin GPIO_PIN_4
#define S1_EncA_GPIO_Port GPIOB
#define S1_EncB_Pin GPIO_PIN_5
#define S1_EncB_GPIO_Port GPIOB
#define EEPROM_WP_Pin GPIO_PIN_0
#define EEPROM_WP_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
