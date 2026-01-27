#ifndef __HEART_RATE_H__
#define __HEART_RATE_H__

#include "att.h"
#include "gatt.h"
#include "stdio.h"

#define HEART_RATE_SERVER_NOTIFICATION_INTERVAL 1000
void Heart_Rate_Server_init(void);
void Heart_Rate_Server_Notification_Start(void);
void Heart_Rate_Server_Notification_Stop(void);
#endif // !__HEART_RATE_H__
