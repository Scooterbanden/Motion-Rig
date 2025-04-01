/*
 * Custom.h
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk
 */

#ifndef INC_CUSTOM_H_
#define INC_CUSTOM_H_

#include "main.h"

void userInit(void);
void userLoop(void);

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} LED_t;

#endif /* INC_CUSTOM_H_ */
