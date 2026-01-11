/*
 * SRXL2.h (WIP)
 *
 *  Created on: June 7, 2025
 *      Author: leecurrent04
 *      Email : leecurrent04@inha.edu
 *
 *  Description :
 *  Opaque pointer 방식으로 재작성중임.
 */

#ifndef SRXL2_H_
#define SRXL2_H_


/* Macro ---------------------------------------------------------------------*/
#define SRXL_MAX_BUFFER_SIZE    (80)
#define SRXL_MIN_BUFFER_SIZE    (5)

typedef struct SRXL2 SRXL2;

/* Functions 1 ---------------------------------------------------------------*/
SRXL2* SRXL2_create();

int SRXL2_connect(SRXL2* self);
int SRXL2_getControlData(SRXL2* self);
uint8_t SRXL2_getRssi(SRXL2* self);

int SRXL2_readByteIRQ2(SRXL2* self, const uint8_t data);

#endif
