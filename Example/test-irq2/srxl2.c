#include <stdint.h>
#include "srxl2.h"

uint8_t RC_Buffer[SRXL_MAX_BUFFER_SIZE];
int RC_isBufferInit() { return 0; }


int SRXL2_readByteIRQ2(const uint8_t data)
{
	static uint8_t cnt = 0;
	static uint8_t maxLen = 0;

	if(RC_isBufferInit()!=0) return -2;
	if(cnt>=SRXL_MAX_BUFFER_SIZE) return -2;

	switch(cnt)
	{
	case 0:
		if(data != SPEKTRUM_SRXL_ID){ cnt=0; return -1;}
		break;
	case 1:
		switch(data){
		case SRXL_HANDSHAKE_ID:maxLen = 14; break;
		case SRXL_BIND_ID:maxLen = 21; break;
		case SRXL_PARAM_ID: maxLen = 14; break;
		case SRXL_RSSI_ID: maxLen = 10; break;
		case SRXL_TELEM_ID: maxLen = 22; break;
		case SRXL_CTRL_ID: maxLen = 80; break;
		default : cnt = 0; return -1;
		}
		break;
	default :
		if(cnt == 2 && maxLen == 80){ maxLen = data;} // Control Packet은 사이즈가 가변적임
		if(cnt != maxLen-1){ break;}	// return 0 if the last byte is received.

		RC_Buffer[cnt] = data;
		cnt=0;
		return 0;
	}

	RC_Buffer[cnt] = data;
	cnt++;

	return 1;
}
