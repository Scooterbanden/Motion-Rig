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

#define millisecondsPerFrame 400

uint32_t timer;

uint8_t debugFlag = 0;
uint8_t ledIdx = 0;
system_state sysState = INITIAL_STATE;

GPIO_t configs[5] = {{GPIOE, Config_0_Pin},{GPIOE, Config_1_Pin},{GPIOE, Config_2_Pin},{GPIOE, Config_3_Pin},{GPIOE, Config_4_Pin}};

void userInit(void) {
	oledInit();
	interfaceInit();

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

void userLoop(void) {														// Lowest priority code, handles updating oled display (user inputs are interrupt based)
	// Some commonly used helper variables
	system_state prevState = INITIAL_STATE;
	int32_t encoderValue = __HAL_TIM_GET_COUNTER(&htim5);
	uint8_t toggle = 0;
	int8_t select = 0;
	SSD1306_COLOR optionColor[5];
	while (1) {
		if (sysState != prevState) {
			prevState = sysState;
			toggle = 0;
			select = 0;
		}
		//uint32_t elapsedTime = HAL_GetTick()-startTime;
		if ((HAL_GetTick()-timer) > millisecondsPerFrame) {
			timer = HAL_GetTick();
			int32_t newEncoderValue = __HAL_TIM_GET_COUNTER(&htim5);
			int32_t difference = newEncoderValue - encoderValue;
			if (difference != 0) { toggle = 1; }
			encoderValue = newEncoderValue;

			ssd1306_Fill(Black);
			ssd1306_SetCursor(2, 0);
			for (int i = 0; i < 5; i++) {
				optionColor[i] = White;
			}

			switch (sysState) {
			case INITIAL_STATE:				// Main menu 3 options: CONTROL_MODE, DEBUG_MODE,      Ė̷͓̝R̶͙̱̈́̇̾͜R̸̤̝̎Ō̶̳̱R̴̖̩͙̎̎͛͜_̴̳̼̰̔͘U̶̻̅N̴̠͉̱͆͂͝Ȁ̷͔̅̇U̸̢̹̓͝T̷̪̮͑H̶͇̯͐͊̚O̶̳̠͝R̵̙̋Ĭ̵̞̘͊̏Z̶͉̼͇͛E̶͖̥͕̍͋D̸̰̄_̸̙̓A̵̦̹̎C̶͇͐̓͂C̴͍͐E̷̗̱̋̚͝S̷̜̯̋̎̎S
				select += difference;
				if (select < 0) { select = 0; }
				else if (select > 2) { select = 2; }
				if (toggle) {
					optionColor[select] = Black;
					toggle = 0;
				} else {
					toggle = 1;
				}
				oledPrintLinef(Font_11x18, optionColor[0], "Control");
				ssd1306_SetCursor(2, 20);
				oledPrintLinef(Font_11x18, optionColor[1], "Debug");
				ssd1306_SetCursor(2, 40);
				oledPrintLinef(Font_11x18, optionColor[2], "Other");
				ssd1306_UpdateScreen();

				break;
			case CONTROL_MODE:

				break;
			case DEBUG_MODE:

				break;


			case Ė̷͓̝R̶͙̱̈́̇̾͜R̸̤̝̎Ō̶̳̱R̴̖̩͙̎̎͛͜_̴̳̼̰̔͘U̶̻̅N̴̠͉̱͆͂͝Ȁ̷͔̅̇U̸̢̹̓͝T̷̪̮͑H̶͇̯͐͊̚O̶̳̠͝R̵̙̋Ĭ̵̞̘͊̏Z̶͉̼͇͛E̶͖̥͕̍͋D̸̰̄_̸̙̓A̵̦̹̎C̶͇͐̓͂C̴͍͐E̷̗̱̋̚͝S̷̜̯̋̎̎S:

				break;
			}
			/*
			// HAL_TIM_OC_Stop(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);
			// uint16_t count = __HAL_TIM_GET_COUNTER(&htim15);
			// uint32_t final = (pc<<16) + (uint32_t)count;
			// uint32_t estPulses = elapsedTime*1500/1000;
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

uint8_t getConfig(void) {
	uint8_t config = getGPIO(configs[0]) + (getGPIO(configs[1]) << 1) + (getGPIO(configs[2]) << 2) + (getGPIO(configs[3]) << 3) + (getGPIO(configs[4]) << 4);
	return config;
}

void setGPIO(GPIO_t gpio, GPIO_PinState state) {
	HAL_GPIO_WritePin(gpio.port, gpio.pin, state);
}

GPIO_PinState getGPIO(GPIO_t gpio) {
	return HAL_GPIO_ReadPin(gpio.port, gpio.pin);
}

/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint8_t pinNumber = __builtin_ctz(GPIO_Pin); // Get bit position of the pin
    if (pinNumber < sizeof(extiHandlers) / sizeof(ExtiHandler) && extiHandlers[pinNumber]) {
        extiHandlers[pinNumber](); // Call the corresponding handler
    }
}*/
