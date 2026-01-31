#include "hid_mouse.h"
#include "gpio.h"
/* HID over LE */
uint8_t hid_info[] = {0x11, 0x01, 0x00, 0X03};
uint8_t hid_report_data[95] = {
    0X05, 0X01, 0X09, 0X02, 0XA1, 0X01, 0X85, 0X01, 0X09, 0X01, 0XA1, 0X00, 0X05, 0X09, 0X19, 0X01, 0X29, 0X08, 0X15,
    0X00, 0X25, 0X01,
    0X75, 0X01, 0X95, 0X08, 0X81, 0X02, 0X05, 0X01, 0X16, 0X08, 0XFF, 0X26, 0XFF, 0X00, 0X75, 0X10, 0X95, 0X02, 0X09,
    0X30, 0X09, 0X31,
    0X81, 0X02, 0X15, 0X81, 0X25, 0X7F, 0X75, 0X08, 0X95, 0X01, 0X09, 0X38, 0X81, 0X06, 0XC0, 0XC0, 0X05, 0X0C, 0X09,
    0X01, 0XA1, 0X01,
    0X85, 0X02, 0X15, 0X00, 0X25, 0X01, 0X75, 0X01, 0X95, 0X08, 0X09, 0XCD, 0X09, 0XB6, 0X09, 0XB5, 0X09, 0XEA, 0X09,
    0XE9, 0X09, 0XE2,
    0X09, 0XCC, 0X09, 0XB7, 0X81, 0X06, 0XC0
};
uint8_t gatt_hid_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_LE_HID)};
uint8_t gatt_hid_characteristic[] = {
    GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(0x0017), BT_LE_U16_TO_ARRAY(GATT_UUID_HID_INFORMATION)
};
uint8_t hid_con_int[] = {
    GATT_CHAR_PROP_BIT_WRITE_NR, BT_LE_U16_TO_ARRAY(0x0019), BT_LE_U16_TO_ARRAY(GATT_UUID_HID_CONTROL_POINT)
};
uint8_t hid_rep_map[] = {
    GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(0x001b), BT_LE_U16_TO_ARRAY(GATT_UUID_HID_REPORT_MAP)
};
uint8_t hid_int_rep[] = {
    GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY, BT_LE_U16_TO_ARRAY(0x001d),
    BT_LE_U16_TO_ARRAY(GATT_UUID_HID_BT_MOUSE_INPUT)
};
uint8_t hid_rep[] = {
    GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY, BT_LE_U16_TO_ARRAY(0x0020),
    BT_LE_U16_TO_ARRAY(GATT_UUID_HID_REPORT)
};
uint8_t hid_rep1[] = {
    GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY, BT_LE_U16_TO_ARRAY(0x0024),
    BT_LE_U16_TO_ARRAY(GATT_UUID_HID_REPORT)
};
uint8_t report_reference1[] = {0x01, 0x01};
uint8_t report_reference2[] = {0x02, 0x01};
uint8_t hid_reoprt_data_1[] = {0x00, 0x0A, 0x00, 0x00, 0x00, 0x00};
uint8_t hid_reoprt_data_2[] = {0x00};
uint8_t gatt_hid_characteristic_config[] = {0x00, 0x00};
gatt_server_service_t hid_service[] =
{
    {
        0x0015, GATT_UUID_PRI_SERVICE, NULL,
        gatt_hid_uuid, sizeof(gatt_hid_uuid), GATT_PERM_READ_ENCRYPTED
    },
    {
        0x0016, GATT_UUID_CHAR_DECLARE, NULL,
        gatt_hid_characteristic, sizeof(gatt_hid_characteristic), GATT_PERM_READ
    },
    {
        0x0017, GATT_UUID_HID_INFORMATION, NULL,
        hid_info, sizeof(hid_info), GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY
    }, //
    {
        0x0018, GATT_UUID_CHAR_DECLARE, NULL,
        hid_con_int, sizeof(hid_con_int), GATT_PERM_READ
    },
    {
        0x0019, GATT_UUID_HID_CONTROL_POINT, NULL,
        NULL, sizeof(NULL), GATT_PERM_READ
    }, //
    {
        0x001a, GATT_UUID_CHAR_DECLARE, NULL,
        hid_rep_map, sizeof(hid_rep_map), GATT_PERM_READ
    },
    {
        0X001b, GATT_UUID_HID_REPORT_MAP, NULL,
        hid_report_data, sizeof(hid_report_data), GATT_PERM_READ
    }, //
    {
        0x001c, GATT_UUID_CHAR_DECLARE, NULL,
        hid_int_rep, sizeof(hid_int_rep), GATT_PERM_READ
    },
    {
        0x001d, GATT_UUID_HID_BT_MOUSE_INPUT, NULL,
        hid_rep, sizeof(hid_rep), GATT_PERM_READ
    }, //
    {
        0x001e, GATT_UUID_CHAR_CLIENT_CONFIG, NULL,
        gatt_hid_characteristic_config, sizeof(gatt_hid_characteristic_config), GATT_PERM_READ | GATT_PERM_WRITE
    },
    {
        0x001f, GATT_UUID_CHAR_DECLARE, NULL,
        hid_rep, sizeof(hid_rep), GATT_PERM_READ
    },
    {
        0x0020, GATT_UUID_HID_REPORT, NULL,
        hid_reoprt_data_1, sizeof(hid_reoprt_data_1), GATT_PERM_READ
    }, //
    {
        0x0021, GATT_UUID_RPT_REF_DESCR, NULL,
        report_reference1, sizeof(report_reference1), GATT_PERM_READ
    }, //
    {
        0x0022, GATT_UUID_CHAR_CLIENT_CONFIG, NULL,
        gatt_hid_characteristic_config, sizeof(gatt_hid_characteristic_config), GATT_PERM_READ | GATT_PERM_WRITE
    }, //
    {
        0x0023, GATT_UUID_CHAR_DECLARE, NULL,
        hid_rep1, sizeof(hid_rep1), GATT_PERM_READ
    },
    {
        0x0024, GATT_UUID_HID_REPORT, NULL,
        hid_reoprt_data_2, sizeof(hid_reoprt_data_2), GATT_PERM_READ
    }, //
    {
        0x0025, GATT_UUID_RPT_REF_DESCR, NULL,
        report_reference2, sizeof(report_reference2), GATT_PERM_READ
    }, //
    {
        0x0026, GATT_UUID_CHAR_CLIENT_CONFIG, NULL,
        gatt_hid_characteristic_config, sizeof(gatt_hid_characteristic_config), GATT_PERM_READ
    }, //
};

int hid_notification_timer = 0;

void HID_Server_Notification(void *param);

void HID_Server_Notification_Call(void) {
    att_notification(hid_service[11].handle, hid_reoprt_data_1, sizeof(hid_reoprt_data_1)); // 0x0020
    hid_notification_timer = timer_create(1500, HID_Server_Notification, NULL);
}

void HID_Server_Notification(void *param) {
    HID_Server_Notification_Call();
}

void HID_Server_Notification_Start(void) {
    hid_notification_timer = timer_create(1500, HID_Server_Notification, NULL);
}

void HID_Server_Notification_Stop(void) {
    timer_cancel(hid_notification_timer);
    hid_notification_timer = 0;
}

void HID_Server_Notification_Other2(void *param);

void HID_Server_Notification_Other1(void *param) {
    hid_reoprt_data_2[0] = 0X08;
    att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
    timer_create(500, HID_Server_Notification_Other2, NULL);
}

void HID_Server_Notification_Other2(void *param) {
    hid_reoprt_data_2[0] = 0X00;
    att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
    timer_create(3000, HID_Server_Notification_Other1, NULL);
}

void hid_read_cb(uint16_t handle) {
    // __NOP();
}

/*                 按键测试函数                  */

void HID_Server_Notification_key(void *param) {
#ifdef STM32F103xB
    static uint8_t key_status = 0;
    static uint8_t key_status_passed = 0;
    key_status = volum_down_press();
    if (key_status != key_status_passed) {
        switch (key_status) {
            case 0:
                hid_reoprt_data_2[0] = 0X00;
                key_status_passed = 0;
                att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
                break;
            case 1:
                hid_reoprt_data_2[0] = 0X08;
                key_status_passed = 1;
                att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
                break;
            case 2:
                hid_reoprt_data_2[0] = 0X10;
                key_status_passed = 2;
                att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
                break;
            case 3:
                hid_reoprt_data_2[0] = 0X20;
                key_status_passed = 3;
                att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
                break;
            default:
                hid_reoprt_data_2[0] = 0X00;
                key_status_passed = 1;
                att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
                break;
        }
    }
#else
    static uint8_t status = 0;
    static uint8_t count = 0;
    if (count++ > 5) {
        if (status == 0) {
            status = 8;
        } else {
            status = 0;
            count = 0;
        }

        hid_reoprt_data_2[0] = status;
        att_notification(hid_service[15].handle, hid_reoprt_data_2, sizeof(hid_reoprt_data_2)); // 0x0024
    }

#endif
}

void hid_write_cb(uint16_t handle, uint8_t *value, uint8_t value_len) {
    if (handle == hid_service[13].handle) // 0x0022
    {
        if (value[0] == 0x01) {
            HID_Server_Notification_Start();
        } else {
            HID_Server_Notification_Stop();
        }
    }
    if (handle == hid_service[17].handle) // 0x0024
    {
        if (value[0] == 0x01) {
            // timer_create(3000, HID_Server_Notification_Other1, NULL); //这里是音量减少的互相唤醒，现在改成启动按键扫描
            timer_create_cycle(100, HID_Server_Notification_key, NULL);
        } else {
            // HID_Server_Notification_Stop();
        }
    }
}

gatt_pri_service_cbs_t hid_service_cbs = {
    hid_read_cb,
    hid_write_cb
};

void hid_mouse_server_init(void) {
    // hid功能注册
    gatt_server_add_pri_service((gatt_server_service_t *) &hid_service, 0x0015, 0x0026,
                                sizeof(hid_service) / sizeof(gatt_server_service_t), BT_UUID_SERVCLASS_LE_HID, NULL,
                                &hid_service_cbs);
}
