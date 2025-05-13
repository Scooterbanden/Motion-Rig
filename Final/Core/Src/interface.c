/*
 * debug.c
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 *
 *      Control for on-board user interface
 *      ssd1306 Oled, encoder and buttons
 */

#include <customMain.h>
#include <interface.h>

#define DELAY 200			// Button delay for debouncing ([ms])
//#define BASE 2147483648		// Encoder initial value (middle of starts in middle and above/below middle is positive/negative speed

uint32_t btnATimer;
uint32_t btnBTimer;
uint32_t btnEncTimer;
int32_t encoderValue;

GPIO_t LEDs[9] = {{GPIOA, LED1_Pin},{GPIOA, LED2_Pin},{GPIOC, LED3_Pin},{GPIOC, LED4_Pin},{GPIOB, LED5_Pin},{GPIOE, LED6_Pin},{GPIOE, LED7_Pin},{GPIOE, LED8_Pin},{GPIOE, LED9_Pin}};



// #define DEBUGGING				// Uncomment to override normal button functions (grayed out code is not used)

// Main use is help on oled menu
void btnCallbackA(void) {
	if (HAL_GetTick() - btnATimer > DELAY) {
		#ifdef DEBUGGING					//
			/*
			uint32_t ARR = 1000;
			__HAL_TIM_SET_AUTORELOAD(servo[0].pulseTimerGP, ARR);
			__HAL_TIM_SET_COMPARE(servo[0].pulseTimerGP, servo[0].TIM_CH_GP,ARR/2);
			HAL_TIM_OC_Start(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);

			//HAL_GPIO_WritePin(GPIOB, LogicShifter_OE_Pin, GPIO_PIN_SET);
			//oledPrintf("btnA pressed %d", var);

			HAL_GPIO_WritePin(servo[0].enablePin.port, servo[0].enablePin.pin, GPIO_PIN_RESET);
			setMotorSpeed(180, servo[0]);
			debugFlag = 1;


			int32_t encoder_count = __HAL_TIM_GET_COUNTER(&htim5);

			EEPROM_Write_NUM(0, 0, encoder_count);
			//EEPROM_Write(0, 0, (uint8_t*)&encoder_count, sizeof(encoder_count));
			*/
		#else

		#endif
		btnATimer = HAL_GetTick();
	}
}

// Main use is return on oled menu
void btnCallbackB(void) {
	if (HAL_GetTick() - btnBTimer > DELAY) {
		#ifdef DEBUGGING
			HAL_TIM_OC_Stop(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);
			/*
			int32_t valueRead = (int32_t)EEPROM_Read_NUM(0,0);
			ssd1306_Fill(Black);
			ssd1306_SetCursor(2, 0);
			oledPrintf("Value read");
			ssd1306_SetCursor(2, 20);
			oledPrintf("from EEPROM");
			ssd1306_SetCursor(2, 40);
			oledPrintf("%d", valueRead);
			ssd1306_UpdateScreen();
			*/
		#else
			if (menuState.current_menu->type == Idle) {
				// Slowdown before disabling servo
				for (int i = 0; i < 4; i++) {
					setMotorSpeed(0,&servo[i]);
					controlMode = IDLE;
					controlCounter = 0;
					setGPIO(servo[i].enablePin, GPIO_PIN_RESET);
					servo[i].enableFlag = 0;

				}
			}
			if (menuState.current_menu->parent != NULL) {
				menuState.current_menu = menuState.current_menu->parent;
			}
		#endif
		btnBTimer = HAL_GetTick();
	}
}

void btnCallbackEnc(void) {
	if (HAL_GetTick() - btnEncTimer > DELAY) {
		#ifdef DEBUGGING

		#else
			if (menuState.current_menu->items[menuState.selected_index].action != NULL) {
				menuState.current_menu->items[menuState.selected_index].action();
			}

		#endif
		btnEncTimer = HAL_GetTick();
	}
}

void interfaceInit(void) {
	uint32_t initTime = HAL_GetTick();					// button timers for debouncing
	btnATimer = initTime;
	btnBTimer = initTime;
	btnEncTimer = initTime;
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);		// User encoder
	HAL_LPTIM_Encoder_Start(&hlptim2, 0xFFFF);
	encoderValue = HAL_LPTIM_ReadCounter(&hlptim2);
}

void oledInit(void) {
	if (!oledCheck()) {
		return;
	}
	ssd1306_Init();

	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopF_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	toggleRed();
	HAL_Delay(400);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopR_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	toggleRed();
	toggleGreen();
	HAL_Delay(400);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopC_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	toggleGreen();
	toggleBlue();
	HAL_Delay(400);
	toggleBlue();

}

void toggleRed(void) {
	HAL_GPIO_TogglePin(LEDs[0].port,LEDs[0].pin);
	HAL_GPIO_TogglePin(LEDs[4].port,LEDs[4].pin);
	HAL_GPIO_TogglePin(LEDs[8].port,LEDs[8].pin);
}

void toggleGreen(void) {
	HAL_GPIO_TogglePin(LEDs[1].port,LEDs[1].pin);
	HAL_GPIO_TogglePin(LEDs[3].port,LEDs[3].pin);
	HAL_GPIO_TogglePin(LEDs[5].port,LEDs[5].pin);
	HAL_GPIO_TogglePin(LEDs[7].port,LEDs[7].pin);
}

void toggleBlue(void) {
	HAL_GPIO_TogglePin(LEDs[2].port,LEDs[2].pin);
	HAL_GPIO_TogglePin(LEDs[6].port,LEDs[6].pin);
}

uint8_t oledCheck(void) {		// To check if an oled is present
	if (HAL_I2C_IsDeviceReady(&hi2c4, SSD1306_I2C_ADDR, 1, 10) == HAL_OK) {
		return 1;
	} else {
		return 0;
	}
}

// Public-facing function
void oledPrintLinef(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    oledPrintLinev(font, color, fmt, args);
    va_end(args);
}

// Clears display, writes formatted line at top and updates
void oledPrintf(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, ...) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    va_list args;
    va_start(args, fmt);
    oledPrintLinev(font, color, fmt, args); // ✅ Now you can call it properly
    va_end(args);

    ssd1306_UpdateScreen();
}

// New helper that takes a va_list
void oledPrintLinev(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, va_list args) {
    // Determine the length needed for the formatted string
    va_list args_copy;
    va_copy(args_copy, args); // Copy because vsnprintf consumes it
    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len < 0) return;

    char *buffer = malloc(len + 1);
    if (!buffer) return;

    vsnprintf(buffer, len + 1, fmt, args);
    ssd1306_WriteString(buffer, font, color);
    free(buffer);
}

