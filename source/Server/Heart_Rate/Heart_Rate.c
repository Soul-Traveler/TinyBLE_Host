#include "Heart_Rate.h"
#include <stdint.h>
#include "tiny_timer.h"

int Heart_Rate_notification_timer;
uint8_t Heart_Rate_value[2] = {0};

/*Heart Rate*/
// 服务 UUID: 0x180D (Heart Rate Service)
uint8_t hr_svc_uuid[] = {BT_LE_U16_TO_ARRAY(0x180D)};

// 特征声明 (Declaration)
// 属性: Notify (0x10)
// 指针: 指向下一个 Handle (0x0032)
// UUID: 0x2A37
uint8_t hr_char_decl[] = {
    GATT_CHAR_PROP_BIT_NOTIFY,
    BT_LE_U16_TO_ARRAY(0x0000),
    BT_LE_U16_TO_ARRAY(0x2A37)
};

// 特征值 (Value): 真正放数据的地方
// 格式: [Flags, HeartRateValue]
uint8_t hr_meas_val[] = {0x00, 0x00};

// CCCD: 客户端配置 (必须有，手机才能订阅)
uint8_t hr_meas_cccd[] = {0x00, 0x00};


// --- B. 服务表定义 (GATT Table) ---
// 我们手动分配 Handle: 0x0030 ~ 0x0033

gatt_server_service_t heart_rate_service[] = {
    // [0x0030] 服务声明
    {0x0030, GATT_UUID_PRI_SERVICE, NULL, hr_svc_uuid, sizeof(hr_svc_uuid), GATT_PERM_READ},

    // [0x0031] 特征声明
    {0x0031, GATT_UUID_CHAR_DECLARE, NULL, hr_char_decl, sizeof(hr_char_decl), GATT_PERM_READ},

    // [0x0032] 特征值 (Value) <-- 发数据往这发!
    {0x0032, 0x2A37, NULL, hr_meas_val, sizeof(hr_meas_val), 0},

    // [0x0033] CCCD (开关)
    {0x0033, GATT_UUID_CHAR_CLIENT_CONFIG, NULL, hr_meas_cccd, sizeof(hr_meas_cccd), GATT_PERM_READ | GATT_PERM_WRITE}
};


void Heart_Rate_Server_Notification(void *param);

void Heart_Rate_Server_Notification_Call(void) {
    // 这里可以采集心率
    Heart_Rate_value[1]++;
    att_notification(heart_rate_service[2].handle, Heart_Rate_value, sizeof(Heart_Rate_value));
    Heart_Rate_notification_timer = timer_create(1000, Heart_Rate_Server_Notification, NULL);
}

void Heart_Rate_Server_Notification(void *param) {
    printf("Heart_Rate level is %d\r\n", Heart_Rate_value[1]);
    Heart_Rate_Server_Notification_Call();
}

void Heart_Rate_Server_Notification_Start(void) {
    Heart_Rate_notification_timer = timer_create(1000, Heart_Rate_Server_Notification, NULL);
}

void Heart_Rate_Server_Notification_Stop(void) {
    timer_cancel(Heart_Rate_notification_timer);
    Heart_Rate_notification_timer = 0;
}

void hr_write_cb(uint16_t handle, uint8_t *value, uint8_t len) {
    // 手机写 CCCD (Handle 0x0033)
    if (handle == heart_rate_service[3].handle) {
        if (value[0] == 0x01) {
            Heart_Rate_Server_Notification_Start(); // 手机点订阅 -> 开始发
        } else {
            Heart_Rate_Server_Notification_Stop(); // 手机取消 -> 停止发
        }
    }
}

gatt_pri_service_cbs_t heart_rate_service_cbs = {
    NULL,
    &hr_write_cb
};

void Heart_Rate_Server_init(void) {
    gatt_server_add_pri_service((gatt_server_service_t *) &heart_rate_service, 0x0030, 0x0033,
                                sizeof(heart_rate_service) / sizeof(gatt_server_service_t),
                                BT_UUID_SERVCLASS_HEART_RATE, NULL, &heart_rate_service_cbs);
}
