#ifndef __BATTERY_H__
#define __BATTERY_H__
#include "config.h"
#include	"tiny_timer.h"
#include "gatt.h"
#include <stdint.h>
#include <stdio.h>
#define BATTERY_SERVER_NOTIFICATION_INTERVAL 1000


extern uint8_t Battery_level;
void Battery_Server_init(void);
void Battery_Server_Notification_Start(void);
void Battery_Server_Notification_Stop(void);
void ble_battery_service_init(void);
#endif

