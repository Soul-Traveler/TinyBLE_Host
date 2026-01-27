#ifndef __BTSNOOP_H__
#define __BTSNOOP_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define TRANSPORT_TYPE_CMD 0x01
#define TRANSPORT_TYPE_EVENT 0x04
#define TRANSPORT_TYPE_ACL 0x02
#define TRANSPORT_TYPE_SCO 0x03

#define FLAG_COMMAND_SENT 0x02
#define FLAG_COMMAND_RECEIVED 0x03
#define FLAG_EVENT_SENT 0x02
#define FLAG_EVENT_RECEIVED 0x03
#define FLAG_DATA_SENT 0x00
#define FLAG_DATA_RECEIVED 0x01

extern uint64_t bt_time;
uint8_t btsnoop_open(void);
uint8_t btsnoop_close(void);
uint8_t btsnoop_write(uint8_t type, uint8_t in, uint8_t *data, uint16_t data_len);
void btsnoop_sync(void);
#endif