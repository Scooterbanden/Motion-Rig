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

uint32_t millisecondsPerFrame = 400;
uint32_t timer;

uint8_t debugFlag = 0;
uint8_t ledIdx = 0;

//GPIO_t configs[5] = {{GPIOE, Config_0_Pin},{GPIOE, Config_1_Pin},{GPIOE, Config_2_Pin},{GPIOE, Config_3_Pin},{GPIOE, Config_4_Pin}};
//GPIO_t OE_pin = {GPIOB,LogicShifter_OE_Pin};

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
	SSD1306_COLOR optionColor[5];
	uint8_t colorToggle = 1;
	const SSD1306_Font_t *fontSelect;
	uint8_t fontHeight;
	//setGPIO(OE_pin,GPIO_PIN_SET);
	while (1) {
		if ((HAL_GetTick()-timer) > millisecondsPerFrame) {
			timer = HAL_GetTick();
			ssd1306_Fill(Black);
			int16_t newEncoderValue = __HAL_TIM_GET_COUNTER(&htim4);
			int16_t difference = newEncoderValue - encoderValue;
			encoderValue = newEncoderValue;
			switch (menuState.current_menu->type) {
			case Node:
				// Changes selected option if encoder value changed
				if (difference != 0) { colorToggle = 1; }
				if (difference > 0) {
					if (menuState.selected_index < (menuState.current_menu->num_items - 1)) {
						menuState.selected_index += 1;
					}
				}
				else if (difference < 0) {
					if (menuState.selected_index > 0) {
						menuState.selected_index -= 1;
					}
				}

				// Set font size based on menu options
				if (menuState.current_menu->num_items < 4) {
					fontSelect = &Font_11x18;
					fontHeight = 20;
				} else if (menuState.current_menu->num_items < 6) {
					fontSelect = &Font_7x10;
					fontHeight = 12;
				} else {
					fontSelect = &Font_6x8;
					fontHeight = 10;
				}

				// Reset colors and toggle color of selected option
				for (int i = 0; i < 5; i++) {
					optionColor[i] = White;
				}
				if (colorToggle) {
					optionColor[menuState.selected_index] = Black;
					colorToggle = 0;
				} else {colorToggle = 1;}

				// Prints options
				for (int i=0; i < menuState.current_menu->num_items; i++) {
					ssd1306_SetCursor(2, i*fontHeight);
					oledPrintLinef(*fontSelect, optionColor[i], menuState.current_menu->items[i].label);
				}
				break;

			case Action:
				/*
				 * Action menu (like speed control) Prints label of (only) item as well as the encodervalue*(Enc->Rpm gain)
				 * Currently there is control types:
				 * Step, where the item has an action (sets rpm)
				 * Speed, pulses are set each frame for enabled servos (no action on item)
				*/
				ssd1306_SetCursor(2, 0);
				oledPrintLinef(Font_11x18, White, menuState.current_menu->items[0].label);
				ssd1306_SetCursor(2, 20);
				oledPrintLinef(Font_11x18, White, "%d", encoderValue*ENCRPMGAIN);

				if (menuState.current_menu->items[0].action == NULL) {
					for (int i = 0; i < 4; i++) {
						if (servo[i].enableFlag) {
							setMotorSpeed(encoderValue*ENCRPMGAIN, servo[0]);
						}
					}
				}

			}
			if (menuState.current_menu->type == Action) {

			} else if (menuState.current_menu->type == Node) {


			}
			if (menuState.current_menu->parent != NULL) {
				ssd1306_DrawBitmap(110,2,Return_16x8,16,8,White);
			}
			ssd1306_DrawBitmap(116,54,Help_16x8,16,8,White);
			ssd1306_UpdateScreen();

			/*
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
				//ssd1306_DrawBitmap(110,2,Return_16x8,16,8,White);
				ssd1306_DrawBitmap(116,54,Help_16x8,16,8,White);
				ssd1306_UpdateScreen();

				break;
			case INITIAL_HELP:

				break;
			case CONTROL_MODE:

				break;
			case CONTROL_HELP:

				break;
			case DEBUG_MODE:

				break;


			case Ė̷͓̝R̶͙̱̈́̇̾͜R̸̤̝̎Ō̶̳̱R̴̖̩͙̎̎͛͜_̴̳̼̰̔͘U̶̻̅N̴̠͉̱͆͂͝Ȁ̷͔̅̇U̸̢̹̓͝T̷̪̮͑H̶͇̯͐͊̚O̶̳̠͝R̵̙̋Ĭ̵̞̘͊̏Z̶͉̼͇͛E̶͖̥͕̍͋D̸̰̄_̸̙̓A̵̦̹̎C̶͇͐̓͂C̴͍͐E̷̗̱̋̚͝S̷̜̯̋̎̎S:

				break;
			}

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
	uint8_t config = 0;//getGPIO(configs[0]) + (getGPIO(configs[1]) << 1) + (getGPIO(configs[2]) << 2) + (getGPIO(configs[3]) << 3) + (getGPIO(configs[4]) << 4);
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
