/*
 * Custom.c
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk
 */

//#include "stm32h743xx.h"
//#include "stm32h7xx_hal_tim.h"
//#include "main.h"
#include <customMain.h>


uint32_t timer;
GPIO_t LEDs[5] = {{GPIOA, LED1_Pin},{GPIOA, LED2_Pin},{GPIOC, LED3_Pin},{GPIOC, LED4_Pin},{GPIOB, LED5_Pin}};

uint8_t ledIdx = 0;

void userInit(void) {
	oledInit();
	timer = HAL_GetTick();
	//HAL_GPIO_WritePin(EN_PINS[3].port, EN_PINS[3].pin, GPIO_PIN_RESET);

	//HAL_UART_Receive_IT(&huart2, tempData, 1);
	//__HAL_TIM_SET_COUNTER(&htim4, BASE);				// Encoder starts in middle and above/below middle is positive/negative speed
	//HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);		// User encoder
	//HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);		// Motor encoder
	//HAL_TIM_PWM_Init(&htim3);							// Pulse output
	//__HAL_RCC_TIM3_CLK_ENABLE();
	//HAL_TIM_Base_Start(&htim3);
	//oledCheck();
	//timer = HAL_GetTick();
	//btnTimer = HAL_GetTick();
}
void userLoop(void) {
	while (1) {
		if (HAL_GetTick()-timer > 500) {
			timer = HAL_GetTick();
			HAL_GPIO_TogglePin(LEDs[ledIdx].port, LEDs[ledIdx].pin);
			ledIdx++;
			if (ledIdx >= 5) {
				ledIdx = 0;
			}
		}
	}
}

/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint8_t pinNumber = __builtin_ctz(GPIO_Pin); // Get bit position of the pin
    if (pinNumber < sizeof(extiHandlers) / sizeof(ExtiHandler) && extiHandlers[pinNumber]) {
        extiHandlers[pinNumber](); // Call the corresponding handler
    }
}*/
