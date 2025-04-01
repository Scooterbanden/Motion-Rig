/*
 * Custom.c
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk
 */

//#include "stm32h743xx.h"
//#include "stm32h7xx_hal_tim.h"
//#include "main.h"
#include "Custom.h"

uint32_t timer;
LED_t LEDS[5] = {{GPIOA, LED1_Pin},{GPIOA, LED2_Pin},{GPIOC, LED3_Pin},{GPIOC, LED4_Pin},{GPIOB, LED5_Pin}};
LED_t EN_PINS[4] = {{GPIOD, S1_Enable_Pin},{GPIOD, S2_Enable_Pin},{GPIOD, S3_Enable_Pin},{GPIOE, S4_Enable_Pin}};
uint8_t ledIdx = 0;

void userInit(void) {
	timer = HAL_GetTick();
	HAL_GPIO_WritePin(EN_PINS[3].port, EN_PINS[3].pin, GPIO_PIN_RESET);
}
void userLoop(void) {
	while (1) {
		if (HAL_GetTick()-timer > 500) {
			timer = HAL_GetTick();
			HAL_GPIO_TogglePin(LEDS[ledIdx].port, LEDS[ledIdx].pin);
			for (uint8_t i = 0; i<3; i++) {
				HAL_GPIO_TogglePin(EN_PINS[i].port, EN_PINS[i].pin);
			}
			ledIdx++;
			if (ledIdx >= 5) {
				ledIdx = 0;
			}
		}
	}
}
