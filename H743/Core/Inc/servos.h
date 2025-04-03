/*
 * servo.h
 *
 *  Created on: Apr 2, 2025
 *      Author: jmk
 */

#ifndef INC_SERVOS_H_
#define INC_SERVOS_H_

#include <customMain.h>
#include "stm32h7xx_hal.h"

// Forward declaration
struct servo_t;

// Define the struct (No `typedef` here)
struct servo_t {
	uint8_t readyFlag;
	GPIO_t readyPin;
	uint8_t TreachFlag;
	GPIO_t TreachPin;
	uint8_t enableFlag;
	GPIO_t enablePin;
	uint8_t directionFlag;
	GPIO_t directionPin;
	uint8_t TIM_sourceFlag;
	TIM_HandleTypeDef* pulseTimerGP;
	uint32_t TIM_CH_GP;
	uint32_t TIM_CH_HR;
	TIM_HandleTypeDef* encoder;
	uint32_t position;
	uint32_t lastZ;
};

// Make `MyStruct` a typedef after the struct is fully defined
typedef struct servo_t servo_t;

// External reference to the struct array
extern servo_t extiStructs[];

// Function prototypes
void UpdateReady(servo_t* s);
void UpdateTreach(servo_t* s);
void UpdateEncZ(servo_t* s);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/*
// Struct definition
typedef struct {
	uint8_t readyFlag;
	void (*updateReadyFunc)(struct servo_t*);
	GPIO_t enablePin;
	uint8_t enableFlag;
	GPIO_t TreachPin;
	void (*updateTreachFunction)(struct servo_t*);
	TIM_HandleTypeDef pulseTimerGP;
	HRTIM_HandleTypeDef pulseTimerHR;
	TIM_HandleTypeDef encoder;
} servo_t;

// External reference to the struct array (so main.c or other files can use it)
extern servo_t extiStructs[];

// Function prototypes
void UpdateStruct(servo_t* s);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
*/

#endif /* INC_SERVOS_H_ */
