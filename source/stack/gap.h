#ifndef __BT_GAP_H__
#define __BT_GAP_H__



#include <stdint.h>

extern uint16_t connection_handle;
extern uint8_t role;
extern uint8_t peer_address_type;
extern uint8_t peer_address[6];
extern uint8_t local_address[6];
extern uint8_t local_private_address[6];
extern uint8_t personal_address[6];

extern uint8_t ltk[16];//长期密钥



#endif // __BT_GAP_H__
