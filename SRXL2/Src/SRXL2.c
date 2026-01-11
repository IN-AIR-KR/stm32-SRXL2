/*
 * FC_RC/SRXL2.c
 *
 *  Created on: Mar 7, 2025
 *      Author: leecurrent04
 *      Email : leecurrent04@inha.edu
 */


/* Includes ------------------------------------------------------------------*/
#include <FC_RC/Protocol/SRXL2/SRXL2_module.h>


/* Variables -----------------------------------------------------------------*/
struct SRXL2 SRXL2
{
	uint8_t rxCount;
	uint8_t buffer[SRXL_MAX_BUFFER_SIZE];
	SRXL2_Header header;
	SRXL2_Handshake_Data handshake_info;
}

SRXL2* SRXL2_create(const uint8_t src_id, const uint8_t Priority, const uint8_t BaudRate, const uint8_t Info, const uint8_t UID)
{
	SRXL2* p = (SRXL2*)malloc(sizeof(SRXL2));

	memset(p, 0, sizeof(SRXL2));

	p->handshake_info.SrcID = src_id;
	p->handshake_info.DestID = 0x00;	// init
	p->handshake_info.Priority = Priority;
	p->handshake_info.BaudRate = BaudRate;
	p->handshake_info.Info = Info;
	p->handshake_info.UID = UID;

	return p;
}

/*
 * @brief Create SRXL2 as default setting
 * @detail 115200bps, no smart ESC/Telem/Forward Programming, default UID
 * @parm none
 * @retval SRXL2* p
 */
SRXL2* SRXL2_createDefault(const uint8_t fc_id)
{
	return SRXL2_create(SRXL2_FC_DEVICE_ID, 0x60, SRXL_BAUD_115200, 0x00, 0x12345678);
}

// const uint8_t SRXL_FC_DEVICE_ID = 0x30;


/* Functions 1 ---------------------------------------------------------------*/
/*
 * RadioControl.c 에서 호출하는 함수
 */

/*
 * @brief 수신기와 연결
 * @detail 수신기와 연결하기 위한 Handshake 절차 수행
 * @parm SRXL2* self
 * @retval 0 : 연결 완료
 *         2 : 이미 연결됨
 */
int SRXL2_connect(SRXL2* self){
	while(1)
	{
		if(SRXL2_isReceived()!=0) continue;
		SRXL2_parsePacket();

		switch(self->header.pType)
		{
		case SRXL_CTRL_ID:
			// TODO : request bind 
			break;
		case SRXL_HANDSHAKE_ID:
			SRXL2_Handshake_Packet* rx_packet = (SRXL2_Handshake_Packet *)self->buffer;

			// check if device is recevier
			const uint8_t src_id = rx_packet->data.SrcID;
			if((src_id&0x10) != 0x10) continue;

			self->handshake_info.DestID = rx_packet->data.SrcID;
			break;
		default:
			continue;
		}
		break;
	}

	// transmit handshake packet;
	while(SRXL2_doHandshake(&tx_packet));

	/*
	 * 조종기가 꺼진 상황에서 Bind 하는 경우
	 */
	while(SRXL2_getControlData());
	if(SRXL2_getRssi()==0) SRXL2_connect();
	return 0;
}


/*
 * @brief 조종 데이터 로딩
 * @detail RC_GetData()에서 실행됨
 * @retval 0 : 정상 수신
 * 		  -1 : 수신 버퍼 없음
 * 		  -2 : 조종 데이터가 아님
 * 	    0xf2 : FailSafe
 */
int SRXL2_getControlData(SRXL2* self){
	
	if(SRXL2_isReceived()!=0) return -1;
	SRXL2_parsePacket();

	if(self->header.pType != SRXL_CTRL_ID) return -2;
	SRXL2_Control_Packet* rx = (SRXL2_Control_Packet*)self->buffer;

	// TODO: rssi, frameLoss, Fail-safe
	switch(rx->Command){
	case SRXL_CTRL_CMD_CHANNEL:
		// SRXL2_SendTelemetryData();
		SRXL2_parseControlData((SRXL2_Control_Packet*)RC_Buffer);
		break;
	case SRXL_CTRL_CMD_CHANNEL_FS:
		return RC_setFailsafe(0x1<<RC_PROTOCOL_SRXL2);
		break;
	case SRXL_CTRL_CMD_VTX:
		break;
	case SRXL_CTRL_CMD_FWDPGM:
		break;
	}

	return 0;
}


/*
 * @brief RSSI 수신 값 로드
 * @retval rssi
 */
uint8_t SRXL2_getRssi(void)
{
	return (uint8_t)((SRXL2_Control_Packet*)RC_Buffer)->data.rssi;
}


/*
 * @brief 수신 인터럽트 IRQ 2
 * @detail UART IRQ1에서 호출됨
 * 		   RC_Buffer에 저장
 * @retval 0 : 패킷 전체 수신 완료
 * @retval 1 : 1byte 수신 완료
 * @retval -1 : 헤더 에러
 * @retval -2 : 버퍼 설정 안됨
 */
int SRXL2_readByteIRQ2(SRXL2* self, const uint8_t data)
{
	static uint8_t cnt = 0;
	static uint8_t maxLen = 0;

	if(RC_isBufferInit()!=0) return -2;
	if(cnt>=SRXL_MAX_BUFFER_SIZE) return -2;

	switch(cnt)
	{
	case 0:
		if(data == SPEKTRUM_SRXL_ID){
			self->buffer[cnt] = data;
			cnt++;
		}
		break;
	case 1:
		switch(data){
		case SRXL_HANDSHAKE_ID:
			maxLen = 14;
			break;
		case SRXL_BIND_ID:
			maxLen = 21;
			break;
		case SRXL_PARAM_ID:
			maxLen = 14;
			break;
		case SRXL_RSSI_ID:
			maxLen = 10;
			break;
		case SRXL_TELEM_ID:
			maxLen = 22;
			break;
		case SRXL_CTRL_ID:
			maxLen = 80;
			break;
		default :
			cnt = 0;
			return -1;
		}
		self->buffer[cnt] = data;
		cnt++;
		break;
	default :
		self->buffer[cnt] = data;

		/*
			* Control Packet은 사이즈가 가변적임
			* 3번째 바이트가 패킷의 크기를 결정함
			*/
		if(maxLen == 80) maxLen = self->buffer[cnt];

		if(cnt == maxLen-1){
			cnt=0;
			return 0;
		}
		else{
			cnt++;
		}
		break;
	}
	return 1;
}


/* Functions 2 ---------------------------------------------------------------*/
/*
 * @brief 장치간 Handshake 동작 수행
 * 		  Bus내 연결된 장치 정보 알림
 *
 * @parm SRXL2* self
 * @retval 0 : 송신 완료
 * 		  -1 : 송신 실패
 * 		  -2 : 패킷 크기와 정보가 불일치
 * TODO : timeout
 */
int SRXL2_doHandshake(SRXL2* self)
{
	// Handshake packet for tx
	SRXL2_Handshake_Packet packet;

	packet.header.speckrum_id = SPEKTRUM_SRXL_ID;
	packet.header.pType = SRXL_HANDSHAKE_ID;
	packet.header.len = sizeof(SRXL2_Handshake_Packet);

	packet.handshake_info = self->handshake_info;

	packet.crc = calculate_crc((uint_8*)packet, packet.header.len);

	while(1)
	{
		if(SRXL2_isReceived()!=0) continue;
		SRXL2_parseControlData();

		const uint8_t rx_packetType = self->packet.header.pType;
		if(rx_packetType != SRXL_HANDSHAKE_ID) continue;

		SRXL2_Handshake_Packet* rx_packet = (SRXL2_Handshake_Packet *) self->buffer;
		const uint8_t rx_SrcID = rx_packet->data.src_id;
		const uint8_t rx_DestID = rx_packet->data.DestID;

		if(rx_SrcID != packet.handshake_info.DestID) continue;
		if(rx_DestID != packet.handshake_info.SrcID) continue;

		break;
	}

	return RC_halfDuplex_Transmit((uint8_t*)packet, len);
}


/*
 * @brief ControlData 파싱
 * @detail packet에서 ControlData 파싱 수행.
 *		   data 정규화 수행.
 *		   data 범위 조정(1000us~2000us), 반전, 트림, Dead-zone 적용.
 * 		   RC_Channel[]에 저장
 * @parm SRXL_Control_Pack *rx : (SRXL2_Control_Packet*)packet
 */
int SRXL2_parseControlData(SRXL2_Control_Packet *rx)
{
	uint8_t channelCnt = 0;
	static uint32_t channelMask = 0;
	static uint32_t previousTime = 0;

	for(int i=0; i<SRXL_MAX_CHANNEL; i++)
	{
		if(!((rx->data.mask>>i)&0x01)) continue;
		if(i>=RC_CHANNEL_MAX) break;

		uint16_t value = rx->data.values[channelCnt];

		channelCnt++;

		// Specktrum raw 값 정규화
		value = RC_applyChannelNormMinMax(value, SRXL_CTRL_VALUE_MIN, SRXL_CTRL_VALUE_MAX);

		// raw 범위에서 1000-2000 범위로 mapping
		value = map(value, SRXL_CTRL_VALUE_MIN, SRXL_CTRL_VALUE_MAX, 1000, 2000);

		RC_MSG_setChannelValue(value, i);
	}

	/*
	 * SRXL2에서 rssi가 양수면 %값, 음수면 dBm 값임.
	 * MAVLink는 %값을 0-254 범위로 표현함
	 */
	uint8_t rssi = ((rx->data.rssi>>8)&0x1)?msg.RC_channels.rssi:map(rx->data.rssi, 0, 100, 0, 254);

	// 2초마다 채널 마스크 정보 갱신
	if(msg.system_time.time_boot_ms - previousTime > 2000){
		previousTime = msg.system_time.time_boot_ms;
		channelMask = 0;
	}

	channelMask |= rx->data.mask;
	RC_MSG_setChannelInfo(countSetBits(channelMask), rssi);

	return 0;
}


/*
 * @brief 수신이 있는 지 확인
 * @detail IRQ2가 실행되었는지 확인
 * @retval 0 : 수신 완료
 * 		   1 : 수신 인터럽트 없음
 *         2 : CRC 불일치
 */
int SRXL2_isReceived(SRXL2* self){
	if(IS_FL_RX == 0) return 1;
	if(self == NULL) return 999;


	uint8_t *buffer = &(self->buffer[0]);
	uint8_t len = buffer[2];
	uint16_t crc = combine_crc_bytes(buffer[len-2], buffer[len-1]);

	if (len < SRXL_MIN_PACKET_SIZE || len > SRXL_MAX_BUFFER_SIZE) return 10;
	if(calculate_crc(buffer, len) != crc) return 2;

	// flag clear
	CLEAR_FL_RX();

	return 0;
}


int SRXL2_parsePacket(SRXL2* self)
{
	uint8_t *buffer = &(self->buffer[0]);
	uint8_t len = buffer[2];

	self->header.speckrum_id = SPEKTRUM_SRXL_ID;
	self->header.pType = buffer[1];
	self->header.len = buffer[2];

	self->crc = combine_crc_bytes(buffer[len-2], buffer[len-1]);
	
	return 0;
}


/* Functions 3 ---------------------------------------------------------------*/
/*
 * 입력 받은 바이트를 2byte CRC 값으로 변환
 */
uint16_t combine_crc_bytes(uint8_t CRC_Hi, uint8_t CRC_Lo)
{
	return ((uint16_t)CRC_Hi << 8 | CRC_Lo);
}


/*
 * crc 계산
 * @detail : Big-endian
 * @parm const uint8_t* data : data address
 * @parm uint8_t len : sizeof(data)
 * @retval uint16_t crc
 */
uint16_t calculate_crc(const uint8_t *data, uint8_t len)
{
	uint16_t crc = 0x0000;
	for (uint8_t i = 0; i < len-2; i++) {
		crc ^= ((uint16_t)data[i] << 8);
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc = (crc << 1);
		}
	}

	return crc;
}


/*
 * crc 계산 후 삽입
 * @detail : Big-endian
 * @parm uint8_t* data : data address
 * @parm uint8_t len : sizeof(data)
 * @retval uint16_t crc
 */
uint16_t insert_crc(uint8_t *data, uint8_t len)
{
	uint16_t crc = calculate_crc(data, len);

	data[len -2] = (uint8_t)(crc >> 8);
	data[len -1] = (uint8_t)(crc & 0xFF);

	return crc;
}


/*
 *	Set bit(1)의 갯수를 세는 함수
 *	@parm uint32_t i : bits
 *	@retval uint8_t num of set bits
 */
uint8_t countSetBits(uint32_t i)
{
	// C or C++: use uint32_t
	i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
	i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
	return (uint8_t)((i * 0x01010101) >> 24);          // horizontal sum of bytes
}
