/*
 * comms.c
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 *
 *      Communication functions. USART2 for USART to usb. USART3 is extra.
 *      USB interface.
 */

#include <customMain.h>
#include "comms.h"

uint32_t messageCount = 0;
// UART communication buffers
uint8_t RxUART[64];
float received_floats[4];
uint8_t tempData[2];
int bufIdx = 0;
float dt;
float tau[3];
float cutoff[3] = {0.001,0.001,0.001};
float scale[3] = {-0.05,-0.01,-0.01};
float lambda[3] = {30, 1, 1};
float bias[3] = {0};
prev_vals prevValues[3] = {0};
int32_t posRefs[3] = {0};
bool commsFlag = false;

uint8_t TxBufA[64];
uint8_t TxBufB[64];
uint8_t* writeBuf = TxBufA;
uint8_t* sendBuf  = TxBufB;
int writeIdx = 0;
volatile bool uart_busy = false;
uint32_t commsTimer;

void commsInit(void) {
	commsTimer = HAL_GetTick();
	for (int i = 0; i < 3; i++) {
		tau[i] = 1/(2*M_PI*cutoff[i]);
	}
	HAL_UART_Receive_IT(&huart2, RxUART, 16);
}

void sendValData(uint32_t loopIteration) {
	uint8_t buffer[32];
	uint32_t servoCount;
	uint32_t servoEnc;

    buffer[0] = (uint8_t)(loopIteration & 0xFF);
    buffer[1] = (uint8_t)((loopIteration >> 8) & 0xFF);
    buffer[2] = (uint8_t)((loopIteration >> 16) & 0xFF);
    buffer[3] = (uint8_t)((loopIteration >> 24) & 0xFF);

    uint8_t bufIdx = 4;
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			if (i == 2) {
				servoCount = servo[i].counter.count*10;
			} else {
				servoCount = servo[i].counter.count*20;
			}
			servoEnc = servo[i].encoder.position;
			for (int j = 0; j < 4; j++) {
				buffer[bufIdx + j] = (uint8_t)((servoCount >> (j*8)) & 0xFF);
				buffer[bufIdx + 4 + j] = (uint8_t)((servoEnc >> (j*8)) & 0xFF);
			}
			bufIdx += 8;
		}
	}
	HAL_UART_Transmit(&huart3, buffer, bufIdx, HAL_MAX_DELAY);
}

void sendPosData(uint32_t loopIteration) {
	uint8_t buffer[32];
	uint32_t servoEnc;

    buffer[0] = (uint8_t)(loopIteration & 0xFF);
    buffer[1] = (uint8_t)((loopIteration >> 8) & 0xFF);
    buffer[2] = (uint8_t)((loopIteration >> 16) & 0xFF);
    buffer[3] = (uint8_t)((loopIteration >> 24) & 0xFF);

    uint8_t bufIdx = 4;
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			servoEnc = servo[i].encoder.position;
			for (int j = 0; j < 4; j++) {
				buffer[bufIdx + j] = (uint8_t)((servoEnc >> (j*8)) & 0xFF);
			}
			bufIdx += 4;
		}
	}
	if (!uart_busy) {
		uart_busy = true;
		HAL_UART_Transmit_IT(&huart3, buffer, bufIdx);
	}

}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart3) {
        uart_busy = false;
    }
}

uint32_t executionTime = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Receive UART data
	if (huart == &huart2) {
		// [accSurge][accSway][accYaw][speed]
		memcpy(received_floats, RxUART, 16);
		for (int i = 0; i < 3; i++) {
			received_floats[i] = received_floats[i]*scale[i];
		}
		uint32_t newTime = HAL_GetTick();
		dt = (float)(newTime-commsTimer) /1000;
		commsTimer = newTime;

		if (dt > 0.1) {
			for (int i = 0; i < 3; i++) {
				prevValues[i].filt_u = 0;
				prevValues[i].filt_y = 0;
				prevValues[i].intV_y = 0;
				prevValues[i].intX_y = 0;
			}
			HAL_UART_Receive_IT(&huart2, RxUART, 16);
			return;
		}
		bias[0] = speed2bias(received_floats[3]);
		hpf(received_floats);
		fEuler(received_floats);
		leakyInt(received_floats);
		invKin(received_floats);
		limit(received_floats);
		setRefs(received_floats);
		commsFlag = true;
		messageCount++;
		executionTime = HAL_GetTick() - commsTimer;
		HAL_UART_Receive_IT(&huart2, RxUART, 16);

	}
}

void hpf(float floats[4]) {
    for (int i = 0; i < 3; i++) {
        float u = floats[i];
        float alpha = tau[i]/(tau[i] + dt);
        floats[i] = alpha * (prevValues[i].filt_y + u - prevValues[i].filt_u);
        prevValues[i].filt_y = floats[i];
        prevValues[i].filt_u = u;
    }
}

void fEuler(float floats[4]) {
    for (int i = 0; i < 3; i++) {
        float u = floats[i];
        floats[i] = prevValues[i].intV_y + u * dt;
        prevValues[i].intV_y = floats[i];
    }
}

float speed2bias(float speed) {
	speed = fminf(fmaxf(speed, V_MIN), V_MAX);
	float bias = BIAS_MIN + (BIAS_MAX - BIAS_MIN) * ((speed-V_MIN) / (V_MAX-V_MIN));
	return bias;
}

void leakyInt(float floats[4]) {
	for (int i = 0; i < 3; i++) {
		float u = floats[i];
		float beta = 1-lambda[i]*dt;
		floats[i] = beta * prevValues[i].intX_y + lambda[i]*dt*bias[i] + dt*u;
		prevValues[i].intX_y = floats[i];
	}
}

void invKin(float floats[4]) {
	float AL[2];
	float AG[2];
	float theta = floats[2];
	float yM = floats[1];
	float L;
	AL[0] = -D_X*cos(theta) + D_Y*sin(theta);
	AL[1] = yM - D_X*sin(theta) - D_Y*cos(theta);
	AG[0] = AL[0] + G_X;
	AG[1] = AL[1] - G_Y;
	L = sqrtf(AG[0]*AG[0] + AG[1]*AG[1]);
	floats[1] = L-L_0;						// x_F
	AL[0] = D_X*cos(theta) - D_Y*sin(theta);
	AL[1] = yM + D_X*sin(theta) + D_Y*cos(theta);
	AG[0] = AL[0] - G_X;
	AG[1] = AL[1] + G_Y;
	L = sqrtf(AG[0]*AG[0] + AG[1]*AG[1]);
	floats[2] = L-L_0;						// x_B
}

void limit(float floats[4]) {
	for (int i = 0; i < 3; i++) {
		if (floats[i] > POS_MAX) {
			floats[i] = POS_MAX;
		}
		if (floats[i] < POS_MIN) {
			floats[i] = POS_MIN;
		}
	}
}

void setRefs(float floats[4]) {
	for (int i = 0; i < 3; i++) {
		posRefs[i] = (int32_t)(floats[i]*2500000);
	}
}
