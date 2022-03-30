/*
 * uart_proto.c
 *
 *  Created on: 2022. 3. 30.
 *      Author: donggyu
 */

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "uart_proto.h"

extern UART_HandleTypeDef huart2;


#define UPACK_STX 0xAA
#define UPACK_ETX 0x55

void init_uart_proto() {

}

static inline uint8_t checksum(uint8_t* dat, uint8_t len) {
	uint8_t chk = 0;
	for(uint8_t i=0;i<len;i++) {
		chk ^= dat[i];
	}
	return chk;
}

int generate_uart_packet(uart_ids_t id, uint8_t* dst, uint8_t* src, uint8_t src_len) {
	if(dst == NULL || src == NULL) {
		return -1;
	}
	dst[0] = UPACK_STX;
	dst[1] = id;
	dst[2] = src_len;

	for(uint8_t i=0;i<src_len;i++) {
		dst[3+i] = src[i];
	}

	dst[3+src_len] = checksum(dst, src_len+3);
	dst[4+src_len] = UPACK_ETX;
	return src_len + 5;
}

void send_uart_data(uart_ids_t id, uint8_t *data, uint8_t len) {
	uint8_t* dst = malloc(len);
	uint8_t dst_len;
	if(dst == NULL) {
		return;
	}
	dst_len = generate_uart_packet(0, dst, data, len);
	if(dst_len > 0) {
		HAL_UART_Transmit(&huart2, (uint8_t *)dst, dst_len, 0xFFFF);
	}
	free(dst);
}
