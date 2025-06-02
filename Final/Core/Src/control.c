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
int32_t stepTestTime = 5000;
int32_t seqTestTime = 200000;

int32_t parkPos = 437500;

int valDir = 1;
int valSpeed = 200;
double speedChanges = 0;	// For speedShaping parameter calculation mu and sigma
double mu = 1;
double sigma = 3;

float freqScale;
float rpmAmplitude;
float freqInitial = 0.001;

bool calibrated = false;
uint32_t controlCounter = 0;
uint32_t loopIteration = 0;
ControlMode controlMode = IDLE;
ValidationMode validationMode;
ControlMode requestedMode;		// To allow CALIBRATE to return to the mode that started it
uint16_t stepRef = 0;

void startControl(void) {
	HAL_TIM_Base_Start_IT(&htim17);
}
void stopControl(void) {
	HAL_TIM_Base_Stop_IT(&htim17);
}

// 1kHz control loop
void controlLoop(void) {

	// This loop also also handles encoder tracking (some are 16bit, so they are counted in a 32bit var), same
	int32_t servoEnc;
	int32_t servoCount;
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			int32_t curr = (int32_t)__HAL_TIM_GET_COUNTER(servo[i].encoder.encoder);  // Get current counter value
			int32_t delta = (curr - servo[i].encoder.last_count);     // Calculate delta

			// Handle wraparound (up/down counting)
			if (delta > 32767) {  // Overflow detected (the counter reset to 0)
			    delta -= 65536;  // Correct the overflow
			} else if (delta < -32768) {  // Underflow detected (the counter reset from 0)
			    delta += 65536;  // Correct the underflow
			}

			servo[i].encoder.position += delta;  // Update position with delta
			servo[i].encoder.last_count = curr;  // Store current count for next iteration

			if (servo[i].pulseCounter.timer != NULL) {
				curr = (int32_t)__HAL_TIM_GET_COUNTER(servo[i].pulseCounter.timer);
				delta = (curr - servo[i].pulseCounter.last_count);

				// Handle wraparound (up/down counting)
				if (delta > 32767) {  // Overflow detected (the counter reset to 0)
					delta -= 65536;  // Correct the overflow
				} else if (delta < -32768) {  // Underflow detected (the counter reset from 0)
					delta += 65536;  // Correct the underflow
				}

				servo[i].pulseCounter.count += delta;  // Update position with delta
				servo[i].pulseCounter.last_count = curr;  // Store current count for next iteration
			}
		}
	}

	// Control, each mode is semi hardcoded, SEQUENCE, SPEED and STEP validation modes use only first servo, rest uses all available servos
	/* IDLE: Passive, waiting for mode selection
	 * GAME: Actual motion, with references calculated from received game data
	 *
	 * 		Test modes
	 * SEQUENCE: First validation sequence, only one implemented is the chirp signal for error-torque dynamic identification
	 * SPEED: Motor speed is set by user input encoder (Currently
	 * STEP: Motor speed is set to selected value for a specified time
	 * REALIGN: Just added due to varying torque in test setup, realigns motor to initial angle
	 * CONSTSPEED: Constant fixed speed, also just added for testing
	 *
	 * 		Utility modes
	 * CALIBRATE: Moves at fixed low speed until endstop (detected with Treach pin) then resets position and goes to PARK
	 * PARK: Moves to middle of stroke then resets position.
	 *
	 * 		Validation modes (Submodes of VALIDATION)
	 * INDIVIDUAL_CONSTANT: Moves each servo individually at constant speed steps back and forth till rated speed
	 * INDIVIDUAL_VARYING: Same idea but with a Wendell distribution looking shape, quick peak and slow decline in speed
	 * ALL_CONSTANT: All at once constant speed
	 * ALL_VARYING: All at once, the shaped speeds
	 */
	switch (controlMode) {
	case IDLE:
		break;
	case GAME:
		if (!calibrated) {
			requestedMode = controlMode;
			controlMode = CALIBRATE;
			break;
		}
		uint8_t refIdx = 0;
		for (int i = 0; i < 4; i++) {
			if (servo[i].enableFlag) {
				int32_t error = posRefs[refIdx] - servo[i].encoder.position;
				float temp = (float)error * 0.08f;

				if (temp > INT16_MAX) temp = INT16_MAX;
				else if (temp < INT16_MIN) temp = INT16_MIN;

				int16_t rpm = (int16_t)temp;

				if (i == 0) {
					loopIteration++;
					sendRpmSP(loopIteration,rpm);
					setMotorSpeed(rpm, &servo[i]);
				}
				refIdx++;
			}
		}
		controlCounter++;
		if (commsFlag) {
			controlCounter = 0;
			commsFlag = false;
		}
		if (controlCounter > 100) {
			posRefs[0] = 0;
			posRefs[1] = 0;
			posRefs[2] = 0;
		}
		//loopIteration++;

		//sendPosData(loopIteration);



		break;
	case SEQUENCE:
		float freq = (float)(controlCounter) *freqScale + freqInitial;
		float rpm = rpmAmplitude*sin(2*3.14*((freq-freqInitial)*freq));
		setMotorSpeed((int16_t)(rpm), &servo[0]);
	    servoCount = servo[0].pulseCounter.count;
		servoEnc = servo[0].encoder.position;
		send_int32_uart(servoCount*20,servoEnc);
		controlCounter++;
		if (controlCounter >= seqTestTime) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = CONSTSPEED;
		}
		break;
	case SPEED:
		int32_t encoderValue = __HAL_TIM_GET_COUNTER(&htim5);
		setMotorSpeed((int16_t)(encoderValue*ENCRPMGAIN), &servo[0]);
		break;
	case STEP:
		setMotorSpeed(stepRef,&servo[0]);
	    servoCount = servo[0].pulseCounter.count;
		servoEnc = servo[0].encoder.position;
		send_int32_uart(servoCount,servoEnc);
		controlCounter++;
		if (controlCounter >= stepTestTime) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;

	case REALIGN:
		if (controlCounter == 1000) {
			servoEnc = get_servo_position(&servo[0]);
			int32_t offset = 10000 - (servoEnc%10000);
			__HAL_TIM_SET_COUNTER(&htim4,0);
			servo[2].encoder.position = 0;
			__HAL_TIM_SET_AUTORELOAD(&htim4, offset/20);
		} else if (controlCounter > 1000) {
			setMotorSpeed(50,&servo[0]);
		}
		controlCounter++;
		break;

	case CONSTSPEED:
		if (controlCounter > 1000) {
			setMotorSpeed(200,&servo[0]);
		}
		controlCounter++;
		if (controlCounter > 6000) {
			setMotorSpeed(0, &servo[0]);
			controlCounter = 0;
			controlMode = REALIGN;
		}
		break;

	case CALIBRATE:
		int running = 0;
		for (int i = 0; i < 4; i++) {
			if (servo[i].enableFlag) {
				if (!(servo[i].TreachFlag)) {
					running++;
					setMotorSpeed(-CALIBRATIONSPEED,&servo[i]);
				} else {
					setMotorSpeed(0,&servo[i]);
				}

			}
		}
		if (running == 0) {
			controlCounter++;
			if (controlCounter > 500) { //Wait 0.5s for 0 speed
				for (int i = 0; i < 4; i++) {
					if (servo[i].enableFlag) {
						__HAL_TIM_SET_COUNTER(servo[i].encoder.encoder,0);
						servo[i].encoder.position = 0;
						servo[i].encoder.last_count = 0;
						if (servo[i].pulseCounter.timer != NULL) {
							__HAL_TIM_SET_COUNTER(servo[i].pulseCounter.timer,0);
						}
						servo[i].pulseCounter.count = 0;
						servo[i].pulseCounter.last_count = 0;
						servo[i].TreachFlag = false;
					}
				}
				calibrated = true;
				controlCounter = 0;
				controlMode = PARK;
			}

		}
		break;

	case PARK:
		int parking = 0;
		for (int i = 0; i < 4; i++) {
			if (servo[i].enableFlag) {
				if (servo[i].encoder.position >= (int32_t)STROKE_T/2*MM2PULSE) { //(int32_t)STROKE_T/2*MM2PULSE
					servo[i].ParkedFlag = true;
					setMotorSpeed(0,&servo[i]);
				}
				if (!(servo[i].ParkedFlag)) {
					parking++;
					setMotorSpeed(CALIBRATIONSPEED,&servo[i]);
				}
			}
		}
		if (parking == 0) {
			controlMode = requestedMode;
			loopIteration = 0;
			for (int i = 0; i < 4; i++) {
				if (servo[i].enableFlag) {
					__HAL_TIM_SET_COUNTER(servo[i].encoder.encoder,0);
					servo[i].encoder.position = 0;
					servo[i].encoder.last_count = 0;
					if (servo[i].pulseCounter.timer != NULL) {
						__HAL_TIM_SET_COUNTER(servo[i].pulseCounter.timer,0);
					}
					servo[i].pulseCounter.count = 0;
					servo[i].pulseCounter.last_count = 0;
				}
			}
			for (int i = 0; i < 4; i++) {
				servo[i].ParkedFlag = false;
			}
		}
		break;
	case VALIDATION:
		if (!calibrated) {
			requestedMode = controlMode;
			controlMode = CALIBRATE;
			break;
		}


		float rpmSP = ((float)(valSpeed*valDir));
		float rpmPeak;
		int going = 0;

		switch (validationMode) {
		case INDIVIDUAL_CONSTANT:
			if (!limitCheck(servo[currentServo], rpmSP)) {
				setMotorSpeed((int16_t)rpmSP,&servo[currentServo]);
			} else {
				servo[currentServo].ParkedFlag = false;
				if (!nextSpeed()) {
					setMotorSpeed(0, &servo[currentServo]);
					int prevServo = currentServo;
					for (int i = currentServo+1; i < 4; i++) {
						if (servo[i].enableFlag) {
							currentServo = i;
							break;
						}
					}
					if (currentServo == prevServo) {
						controlCounter = 0;
						currentServo = 0;
						speedChanges = 0;
						validationMode = INDIVIDUAL_VARYING;
					}
				}
			}
			break;

		case INDIVIDUAL_VARYING:
			rpmPeak = rpmSP;
			rpmSP = shapedSpeed(controlCounter, mu, sigma, (double)rpmPeak);


			if (!limitCheck(servo[currentServo], rpmSP)) {
				setMotorSpeed((int16_t)rpmSP,&servo[currentServo]);
				controlCounter++;
			} else {
				controlCounter = 0;
				servo[currentServo].ParkedFlag = false;
				if (!nextSpeed()) {
					setMotorSpeed(0, &servo[currentServo]);
					int prevServo = currentServo;
					for (int i = currentServo+1; i < 4; i++) {
						if (servo[i].enableFlag) {
							currentServo = i;
							break;
						}
					}
					if (currentServo == prevServo) {
						validationMode = ALL_CONSTANT;
					}
					speedChanges = 0;
					mu = (10-speedChanges*2) *0.1;
					sigma = (10-speedChanges) *0.3;
				} else {
					mu = (10-speedChanges*2) *0.1;
					sigma = (10-speedChanges) *0.3;
				}
			}
			break;

		case ALL_CONSTANT:
			for (int i = 0; i < 4; i++) {
				if (servo[i].enableFlag) {
					if (!limitCheck(servo[i], rpmSP)) {
						setMotorSpeed((int16_t)rpmSP, &servo[i]);
						going++;
					} else {
						servo[i].ParkedFlag = true;
						setMotorSpeed(0, &servo[i]);
					}
				}
			}

			if (going == 0) {
				for (int i = 0; i < 4; i++) {
					if (servo[i].enableFlag) {
						servo[i].ParkedFlag = false;
					}
				}
				if (!nextSpeed()) {
					controlCounter = 0;
					speedChanges = 0;
					validationMode = ALL_VARYING;
				}
			}
			break;

		case ALL_VARYING:
			rpmPeak = rpmSP;
			rpmSP = shapedSpeed(controlCounter, mu, sigma, rpmPeak);

			for (int i = 0; i < 4; i++) {
				if (servo[i].enableFlag) {
					if (!limitCheck(servo[i], rpmSP) && !servo[i].ParkedFlag) {
						setMotorSpeed((int16_t)rpmSP, &servo[i]);
						going++;
					} else {
						servo[i].ParkedFlag = true;
						setMotorSpeed(0, &servo[i]);
					}
				}
			}

			if (going == 0) {
				controlCounter = 0;
				if (!nextSpeed()) {
					// validation complete
					controlMode = IDLE;
					for (int i = 0; i < 4; i++) {
						setMotorSpeed(0, &servo[i]);
						setGPIO(servo[i].enablePin, GPIO_PIN_RESET);
						servo[i].enableFlag = 0;
					}
				} else {

					mu = (10 - speedChanges * 2) * 0.1;
					sigma = (10 - speedChanges) * 0.3;

					for (int i = 0; i < 4; i++) {
						if (servo[i].enableFlag) {
							servo[i].ParkedFlag = false;
						}
					}
				}
			} else {
				controlCounter++;
			}
			break;
		}
		loopIteration++;
		sendValData(loopIteration);
		/*
		for (int i = 0; i < 4; i++) {
			if (servo[i].enableFlag) {
			    servoCount = servo[i].counter.count;
				servoEnc = servo[i].encoder.position;
				if (i != 2) {
					send_int32_uart(servoCount*20, servoEnc);
				} else {
					send_int32_uart(servoCount*10, servoEnc);
				}
				//add2Buffer(servoCount*20, servoEnc);
			}
		}
		//sendUART();
		*/
		break;
	}
}

float shapedSpeed(uint32_t k, double mu, double sigma, double maxSpeed) {
	// Lod-normal shaped speed profile calculator (just a quick rise, slow fall, wendell distribution looking thing)
	double t = ((double)k) * TCONTROL;
    if (t <= 0.0f) return 0.0f;  // avoid log(0) or negative input

    double logPart = logf(t / mu);  // natural log
    double exponent = -(logPart * logPart) / (2.0f * sigma * sigma);
    double shape = expf(exponent);

    return (float)(maxSpeed * shape);
}

bool nextSpeed(void) {
	bool keepGoing = true;
	if (valDir == 1) {
		valDir = -1;
		for (int i = 0; i < 4; i++) {
			servo[i].ParkedFlag = 0;
		}
	} else {
		valDir = 1;
		for (int i = 0; i < 4; i++) {
			servo[i].ParkedFlag = 0;
		}
		valSpeed = valSpeed*2;
		speedChanges++;
		if (valSpeed == 3200) {
			valSpeed = 3000;
		} else if (valSpeed > 3000) {
			valSpeed = 200;
			keepGoing = false;
		}
	}
	return keepGoing;
}

bool limitCheck(servo_t servo, float rpmSP) {
	int32_t estimatedPosition = servo.encoder.position + (int32_t)(rpmSP*RPM2FREQ*TCONTROL);
	bool willReachForwardLimit = (valDir == 1) && (estimatedPosition >= UPPERLIMIT);
	bool willReachReverseLimit = (valDir == -1) && (estimatedPosition <= LOWERLIMIT);

	bool limitReached = willReachForwardLimit || willReachReverseLimit;
	return limitReached;
}

void setMotorSpeed(int16_t rpm, servo_t* servo) {

	if (rpm == 0) {
		HAL_TIM_OC_Stop(servo->pulseTimerGP, servo->TIM_CH_GP);
		return;
	}
	int32_t ARR;
	int32_t freqTarget;
	GPIO_PinState direction = GPIO_PIN_RESET;

	if (rpm < 0) {
		direction = GPIO_PIN_SET;
		rpm = abs(rpm);
		if (servo->pulseCounter.timer != NULL) {
			servo->pulseCounter.timer->Instance->CR1 |=  TIM_CR1_DIR;
		}
	} else {
		if (servo->pulseCounter.timer != NULL) {
			servo->pulseCounter.timer->Instance->CR1 &=  ~TIM_CR1_DIR;
		}
	}
	if (rpm > 7000) { rpm = 7000; }
	if (rpm < 30) {
		/*
		if (controlMode == VALIDATION) {
			servo->ParkedFlag = true;
		}*/
		rpm = 0;
		HAL_TIM_OC_Stop(servo->pulseTimerGP, servo->TIM_CH_GP);
		return;
	}
	int16_t delta = rpm - (servo->prevRpm);
	if (abs(delta) > MAX_DELTA) {
	    rpm = servo->prevRpm + MAX_DELTA * delta/abs(delta);
	}
	servo->prevRpm = rpm;
	freqTarget = rpm*RPM2FREQ;
	ARR = CLOCKFREQ/freqTarget;
	if (ARR > 65535) {			// Max ARR (min speed)
		ARR = 65535;
	} else if (ARR < 1) {	// Min ARR (max speed)
		ARR = 1;
	}
	HAL_TIM_OC_Stop(servo->pulseTimerGP, servo->TIM_CH_GP);
	setGPIO(servo->directionPin, direction);
	__HAL_TIM_SET_AUTORELOAD(servo->pulseTimerGP, ARR);
	__HAL_TIM_SET_COMPARE(servo->pulseTimerGP, servo->TIM_CH_GP,ARR/2);
	HAL_TIM_OC_Start(servo->pulseTimerGP, servo->TIM_CH_GP);
}

void send_int32_uart(int32_t value, int32_t value2) {
	// modified to send two int32_t for both pulse and encoder count
    uint8_t buffer[8];

    // Convert int32_t to 4-byte buffer (little-endian)
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);
    buffer[4] = (uint8_t)(value2 & 0xFF);
    buffer[5] = (uint8_t)((value2 >> 8) & 0xFF);
    buffer[6] = (uint8_t)((value2 >> 16) & 0xFF);
    buffer[7] = (uint8_t)((value2 >> 24) & 0xFF);

    HAL_UART_Transmit(&huart2, buffer, 8, HAL_MAX_DELAY);
}
