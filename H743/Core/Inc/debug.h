/*
 * debug.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"

extern GPIO_t LEDs[5];

void debugInit(void);
void btnCallbackA(void);
void btnCallbackB(void);
void btnCallbackEnc(void);

void oledInit(void);
uint8_t oledCheck(void);

void oledPrintLinef(const char *fmt, ...);
void oledPrintf(const char *fmt, ...);
void oledPrintLinev(const char *fmt, va_list args);


#endif /* INC_DEBUG_H_ */
