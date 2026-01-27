#ifndef __SERVER_H__
#define __SERVER_H__

#include "config.h"
#include "Battery/Battery.h"
#include <stdint.h>

uint16_t server_data_req(uint16_t Starting_Handle, uint16_t Ending_Handle, uint16_t Attribute_Type, uint8_t *tx_buffer);
uint16_t server_info_req(uint16_t Starting_Handle, uint16_t Ending_Handle, uint8_t *tx_buffer);
uint16_t Server_write(uint16_t Attribute_Handle, uint16_t len, uint8_t *rx_buffer);
uint16_t Server_read(uint16_t Attribute_Handle, uint8_t *tx_buffer,uint8_t *rx_buffer);
uint16_t Server_read_blob(uint16_t Attribute_Handle, uint8_t *tx_buffer,uint8_t *rx_buffer);
void Services_init(void);
#endif

