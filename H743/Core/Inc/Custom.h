/*
 * Custom.h
 *
 *  Created on: Apr 1, 2025
 *      Author: jmk
 */

#ifndef INC_CUSTOM_H_
#define INC_CUSTOM_H_

#include "main.h"

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

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void userInit(void);
void userLoop(void);


#endif /* INC_CUSTOM_H_ */
