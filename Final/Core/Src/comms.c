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

// UART communication buffers
uint8_t RxUART[64];
uint8_t tempData[2];
int bufIdx = 0;

uint8_t TxBufA[64];
uint8_t TxBufB[64];
uint8_t* writeBuf = TxBufA;
uint8_t* sendBuf  = TxBufB;
int writeIdx = 0;
volatile bool uart_busy = false;

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
	HAL_UART_Transmit(&huart2, buffer, bufIdx, HAL_MAX_DELAY);
}

/*
void add2Buffer(uint8_t bufIdx, int32_t count, int32_t encoder) {
	for (int i = 0; i < 4; i++) {
		writeBuf[writeIdx + i] = (uint8_t)((count >> (i*8)) & 0xFF);
		writeBuf[writeIdx + 4 + i] = (uint8_t)((encoder >> (i*8)) & 0xFF);
	}
}


void sendUART(void) {
    if (!uart_busy && writeIdx > 0) {
        uart_busy = true;

         Swap buffers
        uint8_t* temp = sendBuf;
        sendBuf = writeBuf;
        writeBuf = temp;

        int len = writeIdx;
        writeIdx = 0;

        HAL_UART_Transmit_DMA(&huart2, sendBuf, len);
    }
}
*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2) {
        uart_busy = false;
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Receive UART data
	if (huart == &huart2) {
		memcpy(RxUART+bufIdx, tempData, 1);
		if (++bufIdx >= 256) bufIdx = 0;
		HAL_UART_Receive_IT(&huart2, tempData, 1);
	}
}
