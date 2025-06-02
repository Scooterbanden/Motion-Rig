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
#define STROKE_T 350
#define STROKE_LR 350
#define MM2PULSE 2500

// Forward declaration
struct servo_t;
struct encoder_t;
struct pulseCounter_t;

struct encoder_t {
	TIM_HandleTypeDef* encoder;
	int32_t position;
	GPIO_t encZpin;
	uint32_t lastZ;
	int32_t last_count;
};

typedef struct encoder_t encoder_t;

struct pulseCounter_t {
	TIM_HandleTypeDef* timer;
	int32_t count;
	int32_t last_count;
};

typedef struct pulseCounter_t pulseCounter_t;

// Define the struct (No `typedef` here)
struct servo_t {
	bool readyFlag;
	GPIO_t readyPin;
	bool TreachFlag;
	GPIO_t TreachPin;
	bool enableFlag;
	GPIO_t enablePin;
	GPIO_t directionPin;

	uint8_t TIM_sourceFlag;
	TIM_HandleTypeDef* pulseTimerGP;
	uint32_t TIM_CH_GP;
	uint32_t TIM_CH_HR;
	encoder_t encoder;
	bool ParkedFlag;
	pulseCounter_t pulseCounter;
	int16_t prevRpm;
};

typedef struct servo_t servo_t;

// External reference to the struct array
extern int activeServos;
extern int currentServo;
extern encoder_t pulseCounters[];
extern servo_t servo[];

// Function prototypes
void UpdateReady(servo_t* s);
void UpdateTreach(servo_t* s);
void UpdateEncZ(servo_t* s);
int32_t get_sent_pulses(pulseCounter_t* c);
int32_t get_servo_position(servo_t* s);
void servoInit(void);
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
