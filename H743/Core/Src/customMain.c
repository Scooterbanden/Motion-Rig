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
uint8_t debugFlag = 0;
uint8_t ledIdx = 0;

void userInit(void) {
	oledInit();
	debugInit();
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
		uint32_t elapsedTime = HAL_GetTick()-startTime;
		if (debugFlag & (elapsedTime > 1000)) {
			HAL_TIM_OC_Stop(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);
			//uint16_t count = __HAL_TIM_GET_COUNTER(&htim15);
			//uint32_t final = (pc<<16) + (uint32_t)count;
			uint32_t estPulses = elapsedTime*1500/1000;
			ssd1306_Fill(Black);
			ssd1306_SetCursor(2, 0);
			oledPrintLinef("Counted: ");
			ssd1306_SetCursor(2, 20);
			oledPrintLinef("%d", pc);
			ssd1306_SetCursor(2, 40);
			oledPrintLinef("Est: %d", estPulses);

			ssd1306_UpdateScreen();
			pc = 0;
			debugFlag = 0;
			/*
			timer = HAL_GetTick();
			HAL_GPIO_TogglePin(LEDs[ledIdx].port, LEDs[ledIdx].pin);
			ledIdx++;
			if (ledIdx >= 4) {
				ledIdx = 0;
			}
			*/
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
