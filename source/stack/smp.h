#ifndef __SMP_H__
#define __SMP_H__

#include "config.h"
#include <stdint.h>

#define PAIRING_REQUEST  0x01
#define PAIRING_RESPONSE 0x02
#define PAIRING_CONFIRM  0x03
#define PAIRING_RANDOM   0x04

#define ENC_INFO         0x06
#define MASTER_IDENT     0x07
#define IDENTIFY_INFO    0x08
#define IDENTIFY_ADDRESS 0x09

uint16_t smp_data_recv(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
void smp_enc_info(void);
#endif // !__SMP_H__
