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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PP__1_MCU_Pin GPIO_PIN_5
#define PP__1_MCU_GPIO_Port GPIOE
#define SIGOUT1_1_MCU_Pin GPIO_PIN_1
#define SIGOUT1_1_MCU_GPIO_Port GPIOC
#define SIGOUT2_1_MCU_Pin GPIO_PIN_2
#define SIGOUT2_1_MCU_GPIO_Port GPIOC
#define PZ_1_MCU_Pin GPIO_PIN_3
#define PZ_1_MCU_GPIO_Port GPIOC
#define PA_1_MCU_Pin GPIO_PIN_0
#define PA_1_MCU_GPIO_Port GPIOA
#define PB_1_MCU_Pin GPIO_PIN_1
#define PB_1_MCU_GPIO_Port GPIOA
#define PD__1_MCU_Pin GPIO_PIN_2
#define PD__1_MCU_GPIO_Port GPIOA
#define SIGIN1_1_MCU_Pin GPIO_PIN_3
#define SIGIN1_1_MCU_GPIO_Port GPIOA
#define PD__2_MCU_Pin GPIO_PIN_4
#define PD__2_MCU_GPIO_Port GPIOA
#define SIGIN1_2_MCU_Pin GPIO_PIN_5
#define SIGIN1_2_MCU_GPIO_Port GPIOA
#define PA_2_MCU_Pin GPIO_PIN_6
#define PA_2_MCU_GPIO_Port GPIOA
#define PB_2_MCU_Pin GPIO_PIN_7
#define PB_2_MCU_GPIO_Port GPIOA
#define PZ_2_MCU_Pin GPIO_PIN_4
#define PZ_2_MCU_GPIO_Port GPIOC
#define SIGOUT2_2_MCU_Pin GPIO_PIN_5
#define SIGOUT2_2_MCU_GPIO_Port GPIOC
#define SIGOUT1_2_MCU_Pin GPIO_PIN_0
#define SIGOUT1_2_MCU_GPIO_Port GPIOB
#define PZ_3_MCU_Pin GPIO_PIN_7
#define PZ_3_MCU_GPIO_Port GPIOE
#define PA_3_MCU_Pin GPIO_PIN_9
#define PA_3_MCU_GPIO_Port GPIOE
#define PB_3_MCU_Pin GPIO_PIN_11
#define PB_3_MCU_GPIO_Port GPIOE
#define PD__3_MCU_Pin GPIO_PIN_12
#define PD__3_MCU_GPIO_Port GPIOE
#define SIGIN1_3_MCU_Pin GPIO_PIN_13
#define SIGIN1_3_MCU_GPIO_Port GPIOE
#define SIGOUT2_3_MCU_Pin GPIO_PIN_14
#define SIGOUT2_3_MCU_GPIO_Port GPIOE
#define SIGOUT1_3_MCU_Pin GPIO_PIN_11
#define SIGOUT1_3_MCU_GPIO_Port GPIOB
#define PP__4_MCU_Pin GPIO_PIN_14
#define PP__4_MCU_GPIO_Port GPIOB
#define SIGOUT1_4_MCU_Pin GPIO_PIN_15
#define SIGOUT1_4_MCU_GPIO_Port GPIOB
#define SIGOUT2_4_MCU_Pin GPIO_PIN_8
#define SIGOUT2_4_MCU_GPIO_Port GPIOD
#define PZ_4_MCU_Pin GPIO_PIN_9
#define PZ_4_MCU_GPIO_Port GPIOD
#define PD__4_MCU_Pin GPIO_PIN_10
#define PD__4_MCU_GPIO_Port GPIOD
#define SIGIN1_4_MCU_Pin GPIO_PIN_11
#define SIGIN1_4_MCU_GPIO_Port GPIOD
#define PA_4_MCU_Pin GPIO_PIN_12
#define PA_4_MCU_GPIO_Port GPIOD
#define PB_4_MCU_Pin GPIO_PIN_13
#define PB_4_MCU_GPIO_Port GPIOD
#define USB_D__MCU_Pin GPIO_PIN_11
#define USB_D__MCU_GPIO_Port GPIOA
#define USB_D__MCUA12_Pin GPIO_PIN_12
#define USB_D__MCUA12_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define USART2_TX_MCU_Pin GPIO_PIN_5
#define USART2_TX_MCU_GPIO_Port GPIOD
#define USART2_RX_MCU_Pin GPIO_PIN_6
#define USART2_RX_MCU_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define PP__3_MCU_Pin GPIO_PIN_8
#define PP__3_MCU_GPIO_Port GPIOB
#define PP__2_MCU_Pin GPIO_PIN_9
#define PP__2_MCU_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
