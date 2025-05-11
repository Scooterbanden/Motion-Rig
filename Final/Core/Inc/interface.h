/*
 * debug.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"

extern int32_t encoderValue;
extern GPIO_t LEDs[9];


void interfaceInit(void);
void btnCallbackA(void);
void btnCallbackB(void);
void btnCallbackEnc(void);

void oledInit(void);
uint8_t oledCheck(void);

void toggleRed(void);
void toggleGreen(void);
void toggleBlue(void);

void oledPrintLinef(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, ...);
void oledPrintf(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, ...);
void oledPrintLinev(SSD1306_Font_t font, SSD1306_COLOR color, const char *fmt, va_list args);


#endif /* INC_INTERFACE_H_ */
