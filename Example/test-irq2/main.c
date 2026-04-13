#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "srxl2.h"



// --- 테스트 실행 함수 ---
void run_test(char* name, uint8_t* packet, int len) {
    printf("[%s] Start\n", name);
    for(int i = 0; i < len; i++) {
        int result = SRXL2_readByteIRQ2(packet[i]);
        printf("Byte[%02d]: 0x%02X -> Result: %2d, maxLen: %d\n", i, packet[i], result, 0); // maxLen은 내부 static이라 관찰 불가하나 로직 흐름 확인
        if(result == 0) printf(">> SUCCESS: Packet Received!\n");
        if(result == -1) printf(">> ERROR: Invalid Packet!\n");
    }
    printf("\n");
}

int main() {
    // 테스트 1: Handshake 패킷 (14바이트 고정)
    uint8_t handshake[] = {0xA6, 0x21, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    run_test("Handshake Test", handshake, 14);

    // 테스트 2: Control 패킷 (가변 길이, 여기서는 8바이트로 설정)
    uint8_t control[] = {0xA6, 0xCD, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55};
    run_test("Control Packet Test", control, 8);

    return 0;
}
