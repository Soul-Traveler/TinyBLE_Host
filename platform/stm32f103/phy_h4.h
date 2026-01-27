
#ifndef __PHY_H4_H__
#define __PHY_H4_H__
#include "usart.h"
#include "main.h"
//#include "bt_uart.h"

uint8_t phybusif_input(void);
void Uart_Receive_IDLE(void);
void phy_send(uint8_t *data, uint16_t len);

uint8_t uart_bt_init(void);
int uart_bt_get_it(void);
int uart_bt_send(uint8_t *data, uint16_t len);
void uart_bt_get(uint8_t *data,uint16_t len);
int uart_bt_read(uint8_t *data, uint16_t len);
uint8_t uart_bt_data_ready(void);
void uart_bt_reset(void);
void uart_bt_finish(void);
#endif
