#ifndef __HID_MOUSE_H__
#define __HID_MOUSE_H__

#include "att.h"
#include "gatt.h"
#include "stdint.h"
#include "tiny_timer.h"
void Hid_Mouse_test_Notification_Start(void);
void Hid_Mouse_test_Notification_Stop(void);
void hid_mouse_server_init(void);
extern uint8_t hid_report_data[95];

#endif // !__HID_MOUSE_H__

