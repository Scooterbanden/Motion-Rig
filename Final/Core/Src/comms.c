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
uint8_t RxUART[256];
uint8_t tempData[2];
int bufIdx = 0;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Receive UART data
	if (huart == &huart2) {
		memcpy(RxUART+bufIdx, tempData, 1);
		if (++bufIdx >= 256) bufIdx = 0;
		HAL_UART_Receive_IT(&huart2, tempData, 1);
	}
}
