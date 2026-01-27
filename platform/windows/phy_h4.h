//
// Created by he104 on 2026/1/6.
//

#ifndef BT_STACK_PHY_H4_H
#define BT_STACK_PHY_H4_H

#include <stdint.h>

uint8_t phybusif_input(void);
// uint8_t bt_send(uint8_t *data, uint16_t len);
void bt_reset(void);
void bt_get(uint8_t *data, uint16_t len);
uint8_t bt_init(void);
void bt_timer_init(void);
extern uint16_t data_len;

uint8_t phy_send(uint8_t *data, uint16_t len);
void bt_tick_init(void);
uint8_t bt_uart_init_windows(const char *portName, uint32_t baudRate);

#endif //BT_STACK_PHY_H4_H