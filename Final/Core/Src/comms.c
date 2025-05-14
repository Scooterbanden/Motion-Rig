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


void add2Buffer(int32_t count, int32_t encoder) {
	if (writeIdx + 8 <= sizeof(TxBufA)) {
	    for (int i = 0; i < 4; i++) {
	        writeBuf[writeIdx + i] = (uint8_t)((count >> (i*8)) & 0xFF);
	        writeBuf[writeIdx + 4 + i] = (uint8_t)((encoder >> (i*8)) & 0xFF);
	    }
	    writeIdx += 8;
	}
}


void sendUART(void) {
    if (!uart_busy && writeIdx > 0) {
        uart_busy = true;

        // Swap buffers
        uint8_t* temp = sendBuf;
        sendBuf = writeBuf;
        writeBuf = temp;

        int len = writeIdx;
        writeIdx = 0;

        HAL_UART_Transmit_DMA(&huart2, sendBuf, len);
    }
}


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
