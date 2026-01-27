#ifndef __L2CAP_H__
#define __L2CAP_H__

#include "stdint.h"

#define L2CAP_Signaling_channel            0x0001
#define Connectionless_channel             0x0002
#define AMP_Manager_Protocol               0x0003
#define Attribute_Protocol                 0x0004
#define Low_Energy_L2CAP_Signaling_channel 0x0005
#define Security_manager_protocol          0x0006
#define AMP_Test_Manager_Protocol          0x0007

typedef struct
{
    uint16_t len; // L2CAP帧长度
    uint16_t cid; // L2CAP逻辑通道CID
} l2cap_hdr_t;

uint16_t L2CAP_Get_Packet(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
void L2CAP_Send_Data(uint16_t cid, uint8_t *data, uint16_t len);
#endif
