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

#define maxRPMchange 500

int16_t prevRPM = 0;
uint32_t pc = 0;
uint32_t startTime;

void setMotorSpeed(int16_t rpm, servo_t servo) {
	int16_t rpmChange = rpm-prevRPM;
	if (abs(rpmChange) > maxRPMchange) {				// Gradual speed change
		if (rpmChange > 0) {
			rpm = prevRPM + maxRPMchange;
		} else if (rpmChange < 0) {
			rpm = prevRPM - maxRPMchange;
		}
	}
	prevRPM = rpm;

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
	startTime = HAL_GetTick();
	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

