/*
 * uart_proto.h
 *
 *  Created on: 2022. 3. 30.
 *      Author: donggyu
 */

#ifndef INC_UART_PROTO_H_
#define INC_UART_PROTO_H_

typedef enum {
	UART_CMD_START = 1,
	UART_CMD_JOY = UART_CMD_START,
	UART_CMD_MAX
} uart_ids_t;


void send_uart_data(uart_ids_t id, uint8_t *data, uint8_t len);

#endif /* INC_UART_PROTO_H_ */
