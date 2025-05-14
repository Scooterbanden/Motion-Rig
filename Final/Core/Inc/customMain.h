/*
 * Custom.h
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk & maf
 */

#ifndef INC_CUSTOMMAIN_H_
#define INC_CUSTOMMAIN_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "main.h"
#include "EEPROM.h"

#define ENCRPMGAIN 10


typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} GPIO_t;

#include "menus.h"
#include <interface.h>
#include "comms.h"
#include "servos.h"
#include "control.h"

extern uint8_t debugFlag;

typedef enum
{
  INITIAL_STATE = 0U,
  INITIAL_HELP,
  CONTROL_MODE,
  CONTROL_HELP,
  DEBUG_MODE,
  Ė̷͓̝R̶͙̱̈́̇̾͜R̸̤̝̎Ō̶̳̱R̴̖̩͙̎̎͛͜_̴̳̼̰̔͘U̶̻̅N̴̠͉̱͆͂͝Ȁ̷͔̅̇U̸̢̹̓͝T̷̪̮͑H̶͇̯͐͊̚O̶̳̠͝R̵̙̋Ĭ̵̞̘͊̏Z̶͉̼͇͛E̶͖̥͕̍͋D̸̰̄_̸̙̓A̵̦̹̎C̶͇͐̓͂C̴͍͐E̷̗̱̋̚͝S̷̜̯̋̎̎S
} system_state;



extern HRTIM_HandleTypeDef hhrtim;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c4;

extern LPTIM_HandleTypeDef hlptim2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void CountUpdate(pulseCounter_t* c);
void EncUpdate(encoder_t* e);
void userInit(void);
void userLoop(void);
void displayLoop(void);
void setFPS(uint8_t fps);

uint8_t getConfig(void);

void setGPIO(GPIO_t gpio, GPIO_PinState state);

GPIO_PinState getGPIO(GPIO_t gpio);


#endif /* INC_CUSTOMMAIN_H_ */
