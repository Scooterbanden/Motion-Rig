/*
 * comms.h
 *
 *  Created on: Apr 3, 2025
 *      Author: jakob
 */

#ifndef INC_COMMS_H_
#define INC_COMMS_H_

#define COMMSTS 0.0167f
#define V_MIN 0.0f
#define V_MAX 250.0f
#define BIAS_MIN -0.1f
#define BIAS_MAX 0.1f

#define D_X 0.71715f
#define D_Y 0.27061f
#define G_X 0.6f
#define G_Y 0.35018f
#define L_0 0.60726f

#define POS_MAX 0.15f
#define POS_MIN -0.15f

typedef struct{
	float filt_u;
	float filt_u2;
	float filt_y;
	float filt_y2;
	float intV_y;
	float intX_y;

}prev_vals;

extern int32_t posRefs[3];
extern bool commsFlag;

void commsInit(void);

void sendValData(uint32_t loopIteration);
void sendPosData(uint32_t loopIteration);
void sendRpmSP(uint32_t loopIteration, int16_t rpm);
void process_sample(float recfloats[4]);

void hpf(float floats[4]);
void fEuler(float floats[4]);
float speed2bias(float speed);
void leakyInt(float floats[4]);
void invKin(float floats[4]);
void limit(float floats[4]);
void setRefs(float floats[4]);
/*
void add2Buffer(uint8_t bufIdx, int32_t count, int32_t encoder);

void sendUART(void);
*/

#endif /* INC_COMMS_H_ */
