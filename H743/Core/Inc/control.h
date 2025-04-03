/*
 * control.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define CLOCKFREQ 240000000
#define BASE 32768
#define RPMLIMIT 3000

void setMotorSpeed(int16_t rpm);

#endif /* INC_CONTROL_H_ */
