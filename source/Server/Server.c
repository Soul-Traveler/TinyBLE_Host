#include "Server.h"
#include "string.h"

#include "Name/ble_app_name.h"
#include "Heart_Rate/Heart_Rate.h"
#include "Hid_mouse/hid_mouse.h"
uint16_t enable;



void Services_init(void)
{
    ble_app_name_service_init();
    ble_battery_service_init();
    Heart_Rate_Server_init();
    hid_mouse_server_init();
}
