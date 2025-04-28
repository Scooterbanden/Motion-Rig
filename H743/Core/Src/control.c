/*
 * control.c
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 *
 *      Functions for servo motor control
 *
 *      setMotorSpeed(rpm) - sets pulse frequency for the desired rpm
 */

#include <customMain.h>
#include "control.h"

uint8_t sequenceFlag = 0;
uint32_t pc = 0;
uint32_t controlCounter = 0;

void controlInit(void) {
	HAL_TIM_Base_Start_IT(&htim15);
}

// 1kHz control loop
void controlLoop(void) {
	if (sequenceFlag) {
		float freq = (float)(controlCounter) *0.0001 + 0.001;
		float rpm = 3000*sin((freq-0.01)*freq);
		setMotorSpeed((int16_t)(rpm), servo[0]);
		controlCounter++;
		if (controlCounter >= 200000) {
			sequenceFlag = 0;
			HAL_TIM_OC_Stop(servo[0].pulseTimerGP, servo[0].TIM_CH_GP);
		}
	}
}

void setMotorSpeed(int16_t rpm, servo_t servo) {

	if (rpm == 0) {
		HAL_TIM_OC_Stop(servo.pulseTimerGP, servo.TIM_CH_GP);
		return;
	}
	int32_t ARR;
	int32_t freqTarget;
	GPIO_PinState direction = GPIO_PIN_RESET;

	if (rpm < 0) {
		direction = GPIO_PIN_SET;
		rpm = abs(rpm);
	}
	if (rpm > 3000) { rpm = 3000; }


	freqTarget = rpm*RPM2FREQ;
	ARR = CLOCKFREQ/freqTarget;
	if (ARR > 65535) {			// Max ARR (min speed)
		ARR = 65535;
	} else if (ARR < 1) {	// Min ARR (max speed)
		ARR = 1;
	}
	HAL_TIM_OC_Stop(servo.pulseTimerGP, servo.TIM_CH_GP);
	setGPIO(servo.directionPin, direction);
	__HAL_TIM_SET_AUTORELOAD(servo.pulseTimerGP, ARR);
	__HAL_TIM_SET_COMPARE(servo.pulseTimerGP, servo.TIM_CH_GP,ARR/2);
	HAL_TIM_OC_Start(servo.pulseTimerGP, servo.TIM_CH_GP);

	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

