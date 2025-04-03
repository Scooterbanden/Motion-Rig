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

uint8_t oledStatus = 0;

void oledInit(void) {
	ssd1306_Init();
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopF_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,CoopR_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	//ssd1306_Line(0,0,128,64,White);
	//ssd1306_DrawBitmap(0,0,stars_128x64,128,64,Black);
	HAL_Delay(2000);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,pip_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,mig_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(White);
	ssd1306_DrawBitmap(0,0,MAF_128x64,128,64,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
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
