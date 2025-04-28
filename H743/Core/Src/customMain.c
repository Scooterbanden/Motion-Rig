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

GPIO_t configs[5] = {{GPIOE, Config_0_Pin},{GPIOE, Config_1_Pin},{GPIOE, Config_2_Pin},{GPIOE, Config_3_Pin},{GPIOE, Config_4_Pin}};
GPIO_t OE_pin = {GPIOB,LogicShifter_OE_Pin};

uint8_t colorToggle = 1;
const SSD1306_Font_t *fontSelect;
uint8_t fontHeight;

void userInit(void) {
	oledInit();
	interfaceInit();
	setGPIO(OE_pin,GPIO_PIN_SET);
	setFPS(10);
	timer = HAL_GetTick();
}

void EncUpdate(servo_t* s) {
	// Handle overflow or underflow logic
	if (__HAL_TIM_IS_TIM_COUNTING_DOWN(s->encoder)) {
		s->position -= 65536;
	} else {
		s->position += 65536;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {		// Main screen generate frame
    switch ((uintptr_t)htim->Instance) {  // Cast the timer instance to uintptr_t for comparison
        case (uintptr_t)TIM1:
        	EncUpdate(&servo[0]);
            break;
        case (uintptr_t)TIM2:
        	EncUpdate(&servo[0]);
            break;
        case (uintptr_t)TIM3:
        	EncUpdate(&servo[2]);
            break;
        case (uintptr_t)TIM4:
        	EncUpdate(&servo[3]);
            break;
        case (uintptr_t)TIM15:
        	controlLoop();
            break;
        case (uintptr_t)TIM16:
        	displayLoop();
            break;
        default:
            // Handle error or unexpected case if needed
            break;
    }
}

void userLoop(void) {														// Lowest priority code, handles updating oled display (user inputs are interrupt based)
	while (1) {
		HAL_Delay(500);
	}
}

void displayLoop(void) {
	SSD1306_COLOR optionColor[5] = {White};
	ssd1306_Fill(Black);
	int32_t newEncoderValue = __HAL_TIM_GET_COUNTER(&htim5);
	int32_t difference = newEncoderValue - encoderValue;
	encoderValue = newEncoderValue;
	switch (menuState.current_menu->type) {
	case Node:
	case Idle:
	case Motion:
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
			if (HAL_GetTick()-timer > 500) {
				timer = HAL_GetTick();
				colorToggle = 0;
			}
		} else {
			if (HAL_GetTick()-timer > 500) {
				timer = HAL_GetTick();
				colorToggle = 1;
			}
		}

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
		uint32_t servoEnc = servo[0].position + __HAL_TIM_GET_COUNTER(&htim2);
		ssd1306_SetCursor(2, 40);
		oledPrintLinef(Font_11x18, White, "%d", servoEnc);
		if (menuState.current_menu->items[0].action == NULL) {
			for (int i = 0; i < 4; i++) {
				if (servo[i].enableFlag) {
					setMotorSpeed(encoderValue*ENCRPMGAIN, servo[0]);
				}
			}
		}
	default:
		break;

	}
	if (menuState.current_menu->parent != NULL) {
		ssd1306_DrawBitmap(110,2,Return_16x8,16,8,White);
	}
	ssd1306_DrawBitmap(116,54,Help_16x8,16,8,White);
	ssd1306_UpdateScreen();
}

void setFPS(uint8_t fps) {
	HAL_TIM_Base_Stop_IT(&htim16);
	uint16_t arr = 10000/fps;
	__HAL_TIM_SET_AUTORELOAD(&htim16, arr);
	HAL_TIM_Base_Start_IT(&htim16);
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

