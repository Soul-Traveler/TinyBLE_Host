#include "Battery.h"


uint8_t gatt_battery_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_BATTERY)};
uint8_t gatt_battery_level_characteristic[] = {
    GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_NOTIFY,
    BT_LE_U16_TO_ARRAY(0X0000),//已经不需要了，后面会自动分配。
    BT_LE_U16_TO_ARRAY(GATT_UUID_BATTERY_LEVEL)
};
uint8_t gatt_battery_level_value[] = {0x64};
uint8_t gatt_battery_level_characteristic_config[] = {0x00, 0x00};
gatt_server_service_t battery_service[] = {
    {
        0x0000, GATT_UUID_PRI_SERVICE, NULL,
        gatt_battery_uuid, sizeof(gatt_battery_uuid), GATT_PERM_READ
    },
    {
        0X0000, GATT_UUID_CHAR_DECLARE, NULL,
        gatt_battery_level_characteristic, sizeof(gatt_battery_level_characteristic), GATT_PERM_READ
    },
    {
        0X0000, BT_UUID_SERVCLASS_BATTERY, NULL,
        gatt_battery_level_value, sizeof(gatt_battery_level_value),
        GATT_PERM_READ | GATT_PERM_WRITE | GATT_CHAR_PROP_BIT_NOTIFY
    },
    {
        0X0000, GATT_UUID_CHAR_CLIENT_CONFIG, NULL,
        gatt_battery_level_characteristic_config, sizeof(gatt_battery_level_characteristic_config), GATT_PERM_READ
    },
};
volatile int battery_notification_timer = -1;


void Battery_Server_Notification(void *param);

void Battery_Server_Notification_Call(void) {
    att_notification(battery_service[2].handle, gatt_battery_level_value, sizeof(Battery_level));
    battery_notification_timer = timer_create(BATTERY_SERVER_NOTIFICATION_INTERVAL, Battery_Server_Notification, NULL);
    if (-1 == battery_notification_timer)
        printf("btttery_server_notification_timer_create_error\n");
}

void Battery_Server_Notification(void *param) {
    printf("Battery level is %d\r\n", gatt_battery_level_value[0]);
    Battery_Server_Notification_Call();
    gatt_battery_level_value[0]--;
}

void Battery_Server_Notification_Start(void) {
    battery_notification_timer = timer_create(BATTERY_SERVER_NOTIFICATION_INTERVAL, Battery_Server_Notification, NULL);
}

void Battery_Server_Notification_Stop(void) {
    timer_cancel(battery_notification_timer);
    battery_notification_timer = 0;
}

void battery_write_cb(uint16_t handle, uint8_t *value, uint8_t value_len) {
    if (handle == battery_service[3].handle) {
        if (value[0] == 0x01) {
            Battery_Server_Notification_Start();
        } else {
            Battery_Server_Notification_Stop();
        }
    }
}

void battery_read_cb(uint16_t handle) {
    if (handle == battery_service[2].handle) {
        if (gatt_battery_level_value[0] > 0)
            gatt_battery_level_value[0]--;
        else
            gatt_battery_level_value[0] = (uint8_t) 0X0064;
    }
}

gatt_pri_service_cbs_t battery_service_cbs = {
    battery_read_cb,
    battery_write_cb
};


void ble_battery_service_init(void) {
    gatt_server_add_pri_service((gatt_server_service_t *) &battery_service, 0X0011, 0X0014,
                                sizeof(battery_service) / sizeof(gatt_server_service_t), BT_UUID_SERVCLASS_BATTERY,
                                NULL, &battery_service_cbs);
}
