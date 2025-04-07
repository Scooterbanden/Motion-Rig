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
#include "debug.h"

#define DELAY 200			// Button delay for debouncing ([ms])
//#define BASE 2147483648		// Encoder initial value (middle of starts in middle and above/below middle is positive/negative speed

uint8_t oledStatus = 0;
uint32_t btnATimer;
uint32_t btnBTimer;
uint32_t btnEncTimer;

GPIO_t LEDs[5] = {{GPIOA, LED1_Pin},{GPIOA, LED2_Pin},{GPIOC, LED3_Pin},{GPIOC, LED4_Pin},{GPIOB, LED5_Pin}};

void debugInit(void) {
	uint32_t initTime = HAL_GetTick();
	btnATimer = initTime;
	btnBTimer = initTime;
	btnEncTimer = initTime;
	//__HAL_TIM_SET_COUNTER(&htim5, BASE);				// Encoder starts in middle and above/below middle is positive/negative speed
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);		// User encoder
}

void btnCallbackA(void) {
	if (HAL_GetTick() - btnATimer > DELAY) {
		/*
		uint32_t ARR = 1000;
		__HAL_TIM_SET_AUTORELOAD(servo[0].pulseTimerGP, ARR);
		__HAL_TIM_SET_COMPARE(servo[0].pulseTimerGP, servo[0].TIM_CH_GP,ARR/2);
		HAL_TIM_OC_Start(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);
		*/
		//HAL_GPIO_WritePin(GPIOB, LogicShifter_OE_Pin, GPIO_PIN_SET);
		//oledPrintf("btnA pressed %d", var);

		HAL_GPIO_WritePin(servo[0].enablePin.port, servo[0].enablePin.pin, GPIO_PIN_RESET);
		setMotorSpeed(180, servo[0]);
		debugFlag = 1;

		/*
		int32_t encoder_count = __HAL_TIM_GET_COUNTER(&htim5);
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2, 0);
		oledPrintf("Writing to");
		ssd1306_SetCursor(2, 20);
		oledPrintf("EEPROM:");
		ssd1306_SetCursor(2, 40);
		oledPrintf("%d", encoder_count);
		ssd1306_UpdateScreen();
		EEPROM_Write_NUM(0, 0, encoder_count);
		//EEPROM_Write(0, 0, (uint8_t*)&encoder_count, sizeof(encoder_count));
		*/
		btnATimer = HAL_GetTick();
	}
}

void btnCallbackB(void) {
	if (HAL_GetTick() - btnBTimer > DELAY) {
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

		btnBTimer = HAL_GetTick();
	}
}

void btnCallbackEnc(void) {
	if (HAL_GetTick() - btnEncTimer > DELAY) {
		btnEncTimer = HAL_GetTick();
	}
}
void oledInit(void) {
	if (!oledCheck()) {
		return;
	}
	ssd1306_Init();

	//ssd1306_Line(0,0,128,64,White);
	//ssd1306_DrawBitmap(0,0,stars_128x64,128,64,Black);

	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,mig_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1500);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,MAF_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1500);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopF_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(500);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopR_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(500);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopC_128x64,128,64,Black);
	ssd1306_UpdateScreen();


}

uint8_t oledCheck(void) {		// To check if an oled is present
	if (HAL_I2C_IsDeviceReady(&hi2c4, SSD1306_I2C_ADDR, 1, 10) == HAL_OK) {
		oledStatus = 1;
	} else {
		oledStatus = 0;
	}
	return oledStatus;
}

// Public-facing function
void oledPrintLinef(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    oledPrintLinev(fmt, args);
    va_end(args);
}

// Clears display, writes formatted line at top and updates
void oledPrintf(const char *fmt, ...) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);

    va_list args;
    va_start(args, fmt);
    oledPrintLinev(fmt, args); // ✅ Now you can call it properly
    va_end(args);

    ssd1306_UpdateScreen();
}

// New helper that takes a va_list
void oledPrintLinev(const char *fmt, va_list args) {
    // Determine the length needed for the formatted string
    va_list args_copy;
    va_copy(args_copy, args); // Copy because vsnprintf consumes it
    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len < 0) return;

    char *buffer = malloc(len + 1);
    if (!buffer) return;

    vsnprintf(buffer, len + 1, fmt, args);
    ssd1306_WriteString(buffer, Font_11x18, White);
    free(buffer);
}

