#ifndef _SRXL_H_
#define _SRXL_H_

int SRXL2_readByteIRQ2(const uint8_t data);

#define SRXL_MAX_BUFFER_SIZE 100

#define SPEKTRUM_SRXL_ID        (0xA6)

#define SRXL_HANDSHAKE_ID       (0x21)
#define SRXL_BIND_ID            (0x41)
#define SRXL_PARAM_ID           (0x50)
#define SRXL_RSSI_ID            (0x55)
#define SRXL_TELEM_ID           (0x80)
#define SRXL_CTRL_ID            (0xCD)


#endif
