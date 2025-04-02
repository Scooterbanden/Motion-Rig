/*
 * Custom.c
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk
 */

//#include "stm32h743xx.h"
//#include "stm32h7xx_hal_tim.h"
//#include "main.h"
#include <custom.h>

uint32_t timer;
GPIO_t LEDs[5] = {{GPIOA, LED1_Pin},{GPIOA, LED2_Pin},{GPIOC, LED3_Pin},{GPIOC, LED4_Pin},{GPIOB, LED5_Pin}};

uint8_t ledIdx = 0;

void userInit(void) {
	timer = HAL_GetTick();
	//HAL_GPIO_WritePin(EN_PINS[3].port, EN_PINS[3].pin, GPIO_PIN_RESET);
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
