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
#define UART_BUFF_SIZE 128
uint8_t RxUART[UART_BUFF_SIZE];
float received_floats[4];
uint8_t tempData[2];
int recBufIdx = 0;
bool msgCorruptedFlag = false;

int bufIdx = 0;
float dt;
float tau[3];
float cutoff[3] = {2.25,7.5,2.5};

float lambda[3] = {1, 0.6, 0.6};
float bias[3] = {0};

float scale[3] = {-0.2,0.2,0.05};
float wn[3] = {1.1547, 0.8165, 0.8165};
float k1[3];
float k2[3];
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
		float zeta = 0.7;
		k1[i] = 2*zeta*wn[i];
		k2[i] = wn[i]*wn[i];
		tau[i] = 1/(2*M_PI*cutoff[i]);
	}
	HAL_UART_Receive_IT(&huart2, tempData, 1);
}




void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2) {
        uart_busy = false;
    }
}

uint32_t executionTime = 0;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->ErrorCode & HAL_UART_ERROR_ORE) {
        // Overrun error occurred
    	setGPIO(LEDs[8],GPIO_PIN_SET);
    }

    // Restart reception if needed
    HAL_UART_Receive_IT(huart, tempData, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Receive UART data
	if (huart == &huart2) {
		memcpy(RxUART+recBufIdx, tempData, 1);
		if (tempData[0] == '\n') {
			if (!msgCorruptedFlag) {
				parseMessage();
			} else {
				msgCorruptedFlag = false;
			}
			recBufIdx = 0;
		} else if (++recBufIdx >= UART_BUFF_SIZE) {
				recBufIdx = 0;
				msgCorruptedFlag = true;
			}
		}
		HAL_UART_Receive_IT(&huart2, tempData, 1);
}

void parseMessage(void) {
    char temp_buffer[UART_BUFF_SIZE];
    float values[4];
    char *token;
    int i = 0;

    // Find length up to '\n'
    size_t len = strcspn((char *)RxUART, "\n");
    if (len == UART_BUFF_SIZE || RxUART[len] != '\n') return; // Invalid message

    memcpy(temp_buffer, RxUART, len);
    temp_buffer[len] = '\0'; // Null-terminate

    token = strtok(temp_buffer, ",");
    while (token && i < 4) {
        values[i++] = strtof(token, NULL);
        token = strtok(NULL, ",");
    }

    if (i == 4) {
        process_data(values);
    }
}

void process_data(float recfloats[4]) {
	uint32_t newTime = HAL_GetTick();
	dt = (float)(newTime-commsTimer) /1000;
	commsTimer = newTime;

	for (int i = 0; i < 3; i++) {
		if ((i != 2) && (recfloats[i] > 25)) {
			recfloats[i] = 25;
		} else if (recfloats[i] > 100) {
			recfloats[i] = 100; 	// Higher acc limit for yaw
		}
		recfloats[i] = recfloats[i]*scale[i];
	}
	if (dt > 0.1) {
		for (int i = 0; i < 3; i++) {
			prevValues[i].filt_u = 0;
			prevValues[i].filt_u2 = 0;
			prevValues[i].filt_y = 0;
			prevValues[i].filt_y2 = 0;
			prevValues[i].intV_y = 0;
			prevValues[i].intX_y = 0;
		}
		return;
	}
	/*		First MCA
	 *
	 *
	 *
	 */
	bias[0] = speed2bias(recfloats[3]);
	hpf(recfloats);
	fEuler(recfloats);
	leakyInt(recfloats);
	//recfloats[2] = 0;

	//hpf2(recfloats);

	invKin(recfloats);
	limit(recfloats);
	setRefs(recfloats);
	commsFlag = true;
	messageCount++;
}

void hpf2(float floats[4]) {
	for (int i = 0; i < 3; i++) {
		float u = floats[i];

		float dts = dt*dt;
		float b0 = 4/dts;
		float b1 = -8/dts;
		float b2 = 4/dts;

		float a0 = b0 + k1[i]*2/dt + k2[i];
		float a1 = -2 * b0 + 2* k2[i];
		float a2 = b0 - k1[i]*2/dt + k2[i];

		floats[i] = (b0*u + b1*prevValues[i].filt_u + b2*prevValues[i].filt_u2 - a1*prevValues[i].filt_y - a2*prevValues[i].filt_y2) / a0;

		prevValues[i].filt_y2 = prevValues[i].filt_y;
		prevValues[i].filt_y = floats[i];
		prevValues[i].filt_u2 = prevValues[i].filt_u;
		prevValues[i].filt_u = u;

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
				servoCount = servo[i].pulseCounter.count*10;
			} else {
				servoCount = servo[i].pulseCounter.count*20;
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
		HAL_UART_Transmit_IT(&huart2, buffer, bufIdx);
	}
}

void sendRpmSP(uint32_t loopIteration, int16_t rpm) {
	uint8_t buffer[32];

    buffer[0] = (uint8_t)(loopIteration & 0xFF);
    buffer[1] = (uint8_t)((loopIteration >> 8) & 0xFF);
    buffer[2] = (uint8_t)((loopIteration >> 16) & 0xFF);
    buffer[3] = (uint8_t)((loopIteration >> 24) & 0xFF);

    //uint8_t bufIdx = 8;

    int32_t rppm = (int32_t)rpm;
    buffer[4] = (uint8_t)(rppm & 0xFF);
    buffer[5] = (uint8_t)((rppm >> 8) & 0xFF);
    buffer[6] = (uint8_t)((rppm >> 16) & 0xFF);
    buffer[7] = (uint8_t)((rppm >> 24) & 0xFF);
	if (!uart_busy) {
		//uart_busy = true;
		HAL_UART_Transmit(&huart2, buffer, 8, HAL_MAX_DELAY);
	}
}
