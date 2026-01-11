/*
 * SRXL2_module.h
 *
 *  Created on: Mar 7, 2025
 *      Author: leecurrent04
 *      Email : leecurrent04@inha.edu
 */

#ifndef SRXL2_MODULE_H_
#define SRXL2_MODULE_H_


/* Includes ------------------------------------------------------------------*/

#include <spm_srxl.h>
#include <SRXL2.h>
#include <SRXL2_type.h>

// 아래의 모듈을 제거하도록 코드를 재 작성해야함.
// #include <main.h>
//#include <FC_RC/RC_module.h>
//#include <FC_Serial/MiniLink/MiniLink.h>


/* Macro ---------------------------------------------------------------------*/
#define SPEKTRUM_SRXL_ID        (0xA6)
#define SRXL_MAX_DEVICES        (16)

#define SRXL_CTRL_VALUE_MIN 	(10912)
#define SRXL_CTRL_VALUE_MAX 	(54612)


/* Variables -----------------------------------------------------------------*/
// extern uint8_t* RC_Buffer;
// extern SRXL2_Packet packet;
// extern SRXL2_Handshake_Data receiver_info;
extern const uint8_t SRXL_FC_DEVICE_ID;


/* Functions 2 ---------------------------------------------------------------*/
int SRXL2_doHandshake(SRXL2* self);
int SRXL2_parseControlData(SRXL2_Control_Packet *rx);
int SRXL2_isReceived(void);


/* Functions 3 ---------------------------------------------------------------*/
uint16_t calculate_crc(const uint8_t *data, uint8_t len);
uint16_t insert_crc(uint8_t *data, uint8_t len);
uint8_t countSetBits(uint32_t i);


/* Functions 3 ---------------------------------------------------------------*/
/*
 * In Progress!
 * SRXL_testing.c
 */
int SRXL2_doBind(SRXL2_Bind_Packet* tx_packet);
SRXL2_SignalQuality_Data SRXL2_reqSignalQuality(void);
int SRXL2_SendTelemetryData(void);

#endif /* INC_RC_SRXL2_H_ */
