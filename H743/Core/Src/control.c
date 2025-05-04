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

// Test times [ms]
#define STEPTESTTIME 5000
#define SEQUENCETESTTIME 100000
uint8_t sequenceFlag = 0;
uint32_t pc = 0;
uint32_t controlCounter = 0;
ControlMode controlMode = IDLE;
uint16_t stepRef = 0;

void controlInit(void) {
	HAL_TIM_Base_Start_IT(&htim17);
}

// 1kHz control loop
void controlLoop(void) {
	int32_t servoEnc;
	int32_t servoCount;
	int32_t error;
	switch (controlMode) {
	case IDLE:
		break;
	case GAME:
		break;
	case SEQUENCE:
		float freq = (float)(controlCounter) *0.0001 + 0.001;
		float rpm = 3000*sin((freq-0.01)*freq);
		setMotorSpeed((int16_t)(rpm), servo[0]);
		servoEnc = servo[0].position + __HAL_TIM_GET_COUNTER(&htim2);
		servoCount = servo[2].position + __HAL_TIM_GET_COUNTER(&htim4);
		error = servoCount*20 - servoEnc;
		send_int32_uart(error);
		controlCounter++;
		if (controlCounter >= SEQUENCETESTTIME) {
			setMotorSpeed(0, servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;
	case SPEED:
		int32_t encoderValue = __HAL_TIM_GET_COUNTER(&htim5);
		setMotorSpeed((int16_t)(encoderValue*ENCRPMGAIN), servo[0]);
		break;
	case STEP:
		setMotorSpeed(stepRef,servo[0]);
		servoEnc = servo[0].position + __HAL_TIM_GET_COUNTER(&htim2);
		servoCount = servo[2].position + __HAL_TIM_GET_COUNTER(&htim4);
		error = servoCount*20 - servoEnc;
		send_int32_uart(error);
		controlCounter++;
		if (controlCounter >= STEPTESTTIME) {
			setMotorSpeed(0, servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;
	case REALIGN:
		if (controlCounter == 1000) {
			int32_t encCounter = __HAL_TIM_GET_COUNTER(&htim2);
			servoEnc = servo[0].position + encCounter;
			int32_t offset = 10000 - (servoEnc%10000);
			__HAL_TIM_SET_COUNTER(&htim4,0);
			servo[2].position = 0;
			__HAL_TIM_SET_AUTORELOAD(&htim4, 1000);
		} else if (controlCounter > 1000) {
			setMotorSpeed(50,servo[0]);
		}
		controlCounter++;
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
		htim4.Instance->CR1 |=  TIM_CR1_DIR;
	} else {
		htim4.Instance->CR1 &= ~TIM_CR1_DIR;
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

}

void send_int32_uart(int32_t value) {
    uint8_t buffer[4];

    // Convert int32_t to 4-byte buffer (little-endian)
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);

    HAL_UART_Transmit(&huart2, buffer, 4, HAL_MAX_DELAY);
}
