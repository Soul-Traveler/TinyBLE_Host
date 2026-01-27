#include "gatt.h"
#include "stdio.h"
#include "../Server/Server.h"
#include <stdint.h>
#include "att.h"
#include "hci.h"
#include <string.h>

uint16_t gatts_handle_find_by_type_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_gatt_data_recv(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint8_t att_opencode;
    uint16_t att_size = len - 1;
    uint16_t att_len = 0;

    STREAM_TO_UINT8(att_opencode, att_rx_buffer);
//先switch-case吧，后面就搞指针便宜那一套，传入参数基本都是(att_rx_buffer, att_size, att_tx_buffer)
    switch (att_opencode)
    {
    case ATT_RSP_ERROR:
        printf("ATT_RSP_ERROR\n");
        /* code */
        break;
    case ATT_REQ_MTU:
        printf("ATT_REQ_MTU\n");
        att_len = att_exchange_mtu(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_MTU:
        printf("ATT_RSP_MTU\n");
        break;
    case ATT_REQ_FIND_INFO:
        printf("ATT_REQ_FIND_INFO\n");
        att_len = atts_handle_find_info_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_FIND_INFO:
        printf("ATT_RSP_FIND_INFO\n");
        break;
    case ATT_REQ_FIND_TYPE_VALUE:
        att_len = gatts_handle_find_by_type_req(att_rx_buffer, att_size, att_tx_buffer);
        // att_len = att_not_found(att_tx_buffer);
        printf("ATT_REQ_FIND_TYPE_VALUE\n");
        break;
    case ATT_RSP_FIND_TYPE_VALUE:
        printf("ATT_RSP_FIND_TYPE_VALUE\n");
        break;
    case ATT_REQ_READ_BY_TYPE:
        printf("ATT_REQ_READ_BY_TYPE\n");
        att_len = gatts_handle_read_type_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_READ_BY_TYPE:
        printf("ATT_RSP_READ_BY_TYPE\n");
        break;
    case ATT_REQ_READ:
        printf("ATT_REQ_READ\n");
        att_len = att_handle_read_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_READ:
        printf("ATT_RSP_READ\n");
        break;
    case ATT_REQ_READ_BLOB:
        printf("ATT_REQ_READ_BLOB\n");
        att_len = att_handle_read_blob_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_READ_BLOB:
        printf("ATT_RSP_READ_BLOB\n");
        break;
    case ATT_REQ_READ_MULTI:
        printf("ATT_REQ_READ_MULTI\n");
        break;
    case ATT_RSP_READ_MULTI:
        printf("ATT_RSP_READ_MULTI\n");
        break;
    case ATT_REQ_READ_BY_GRP_TYPE:
        printf("ATT_REQ_READ_BY_GRP_TYPE\n");
        att_len = gatts_handle_read_grp_type_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_READ_BY_GRP_TYPE:
        printf("ATT_RSP_READ_BY_GRP_TYPE\n");
        break;
    case ATT_REQ_WRITE:
        printf("ATT_REQ_WRITE\n");
        att_len = att_handle_wrtie_req(att_rx_buffer, att_size, att_tx_buffer);
        break;
    case ATT_RSP_WRITE:
        printf("ATT_RSP_WRITE\n");
    case ATT_CMD_WRITE:
        printf("ATT_CMD_WRITE\n");
        att_len = att_handle_write_cmd(att_rx_buffer, att_size, att_tx_buffer);
        break;
    default:
        printf("input error\n");
        break;
    }
    return att_len;
}

#if 1


#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1
gatt_server_manager_t gatt_server_manager = {0};
gatt_server_pri_service_t gatt_server_pri_service[GATT_PRI_SERVICE_MAX_COUNT] = {0};

uint8_t gatt_gap_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GAP_SERVER)};
uint8_t gatt_gap_characteristic_name[] = {GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(GATT_GAP_NAME_HANDLE), BT_LE_U16_TO_ARRAY(GATT_UUID_GAP_DEVICE_NAME)};
uint8_t gatt_gap_characteristic_appearance[] = {GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(GATT_GAP_APPEARANCE_HANDLE), BT_LE_U16_TO_ARRAY(GATT_UUID_GAP_ICON)};
uint8_t gatt_appearance_value[] = {0XC2,0X03};
gatt_server_service_t gap_service[] =
    {
        {GATT_GAP_SERVICE_HANDLE, GATT_UUID_PRI_SERVICE, NULL,
         gatt_gap_uuid, sizeof(gatt_gap_uuid), GATT_PERM_READ},
        {GATT_GAP_CHARACTERISTIC_HANDLE, GATT_UUID_CHAR_DECLARE, NULL,
         gatt_gap_characteristic_name, sizeof(gatt_gap_characteristic_name), GATT_PERM_READ},
        {GATT_GAP_NAME_HANDLE, GATT_UUID_GAP_DEVICE_NAME, NULL,
         (uint8_t *)GATT_GAP_NAME, sizeof(GATT_GAP_NAME), GATT_PERM_READ},
        {GATT_GAP_CHARACTERISTIC_HANDLE_APPRERANCE, GATT_UUID_CHAR_DECLARE, NULL,
         gatt_gap_characteristic_appearance, sizeof(gatt_gap_characteristic_appearance), GATT_PERM_READ},
        {GATT_GAP_APPEARANCE_HANDLE, GATT_UUID_GAP_ICON, NULL,
         gatt_appearance_value, sizeof(gatt_appearance_value), GATT_PERM_READ}, //

};

uint8_t gatt_server_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GATT_SERVER)};
uint8_t gatt_characteristic_services_changed[] = {GATT_CHAR_PROP_BIT_INDICATE, BT_LE_U16_TO_ARRAY(GATT_SER_CHA_HANDLE), BT_LE_U16_TO_ARRAY(GATT_UUID_GATT_SRV_CHGD)};
uint8_t gatt_characteristic_supported_features[] = {GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE, BT_LE_U16_TO_ARRAY(GATT_SER_SUP_FEATURE), BT_LE_U16_TO_ARRAY(GATT_UUID_SER_SUP_FEA)};
uint8_t gatt_server_support_feature[] = {0x00};
gatt_server_service_t gatt_service[] =
    {
        {GATT_SERVICE_HANLE, GATT_UUID_PRI_SERVICE, NULL,
         gatt_server_uuid, sizeof(gatt_server_uuid), GATT_PERM_READ},
        {GATT_CHARACTERISTIC_HANDLE_SER_CHA, GATT_UUID_CHAR_DECLARE, NULL,
         gatt_characteristic_services_changed, sizeof(gatt_characteristic_services_changed), GATT_PERM_READ},
        {GATT_SER_CHA_HANDLE, GATT_UUID_GATT_SRV_CHGD, NULL,
         gatt_gap_characteristic_appearance, sizeof(gatt_gap_characteristic_appearance), GATT_PERM_READ},
        {GATT_SER_CHA_HANDLE_CLI, GATT_UUID_CHAR_CLIENT_CONFIG, NULL,
         gatt_gap_characteristic_appearance, sizeof(gatt_gap_characteristic_appearance), GATT_PERM_READ},
        {GATT_CHARACTERISTIC_HANDLE_SUP_FEA, GATT_UUID_CHAR_DECLARE, NULL,
         gatt_characteristic_supported_features, sizeof(gatt_characteristic_supported_features), GATT_PERM_READ},
        {GATT_SER_SUP_FEATURE, GATT_UUID_SER_SUP_FEA, NULL,
         gatt_server_support_feature, sizeof(gatt_server_support_feature), GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE},

};

// 添加服务api
// uint8_t gatt_server_add_pri_service(gatt_server_service_t *service, uint16_t start_handle, uint16_t end_handle,
//                                     uint8_t service_count, uint16_t pri_uuid, uint8_t *pri_uuid128, gatt_pri_service_cbs_t *cb)
// {
//     uint8_t server_pri_service_cnt = gatt_server_manager.gatt_server_pri_service_count;
//     gatt_server_pri_service[server_pri_service_cnt].serivce_count = service_count;
//     gatt_server_pri_service[server_pri_service_cnt].start_handle = start_handle;
//     gatt_server_pri_service[server_pri_service_cnt].end_handle = end_handle;
//     gatt_server_pri_service[server_pri_service_cnt].pri_uuid = pri_uuid;
//     gatt_server_pri_service[server_pri_service_cnt].gatt_server_service = service;
//     gatt_server_pri_service[server_pri_service_cnt].cb = cb;
//     if (pri_uuid128)
//         memcpy(gatt_server_pri_service[server_pri_service_cnt].pri_uuid128, pri_uuid128, 16);
//     gatt_server_manager.gatt_server_pri_service_count++;
//     att_add_handle_grop(service, service_count);
//     return 0;
// }





uint8_t gatt_server_add_pri_service(gatt_server_service_t *service, uint16_t start_handle, uint16_t end_handle,
                                    uint8_t service_count, uint16_t pri_uuid, uint8_t *pri_uuid128, gatt_pri_service_cbs_t *cb)
{
    uint8_t server_pri_service_cnt = gatt_server_manager.gatt_server_pri_service_count;
    gatt_server_pri_service[server_pri_service_cnt].serivce_count = service_count;

    gatt_server_pri_service[server_pri_service_cnt].pri_uuid = pri_uuid;
    gatt_server_pri_service[server_pri_service_cnt].gatt_server_service = service;
    gatt_server_pri_service[server_pri_service_cnt].cb = cb;
    if (pri_uuid128)
        memcpy(gatt_server_pri_service[server_pri_service_cnt].pri_uuid128, pri_uuid128, 16);
    gatt_server_manager.gatt_server_pri_service_count++;

    uint16_t att_start_handle = att_add_handle_grop(service, service_count);


//更改：开始句柄与结束句柄由att层在注册的时候自动生成，自动解决。
    gatt_server_pri_service[server_pri_service_cnt].start_handle = att_start_handle;
    gatt_server_pri_service[server_pri_service_cnt].end_handle = att_start_handle + service_count - 1;


//改为自动分配以后，由此处进行对特征声明的自动更新。
    for (int i = 0; i < service_count; i++)
    {
        // 检查是不是特征声明 (0x2803)
        if (service[i].uuid16 == GATT_UUID_CHAR_DECLARE)
        {
            // 逻辑：特征声明后面紧跟的一定是特征值 (Value)
            // 所以 Value Handle = 当前声明的 handle + 1
            // 或者更稳妥地：直接取数组里下一个元素的 handle (service[i+1].handle)

            // 这里假设 service[i].handle 已经被 att_add_handle_grop 填好了
            // 如果 service[i].handle 是空的，可以用 (att_start_handle + i) 来算
            uint16_t current_handle = service[i].handle;
            uint16_t value_handle = current_handle + 1; // 也就是 service[i+1].handle

            // 获取声明数组的指针 (gatt_battery_level_characteristic)
            uint8_t *p_data = service[i].value;

            // 写入小端序 Handle (第 1、2 字节)
            // 格式: [Prop] [Handle_Low] [Handle_High] [UUID...]
            if (p_data != NULL)
            {
                p_data[1] = (uint8_t)(value_handle & 0xFF);
                p_data[2] = (uint8_t)((value_handle >> 8) & 0xFF);

                // 打印一下，看着舒服
                // printf("GATT Patch: Decl at 0x%04X -> Points to 0x%04X\n", current_handle, value_handle);
            }
        }
    }
    return 0;
}




uint16_t gatts_handle_read_grp_type_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Starting_Handle;
    uint16_t Ending_Handle;
    uint16_t Attribute_Type;
    STREAM_TO_UINT16(Starting_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Ending_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Attribute_Type, att_rx_buffer);
    // 帮助定位
    //  printf("Starting_Handle: %d, Ending_Handle: %d, Attribute_Type: %d\n", Starting_Handle, Ending_Handle, Attribute_Type);
    for (uint8_t i = 0; i < gatt_server_manager.gatt_server_pri_service_count; i++)
    {
        if (Starting_Handle <= gatt_server_pri_service[i].start_handle)
        {
            UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_READ_BY_GRP_TYPE);
            if (!gatt_server_pri_service[i].pri_uuid)
            {
                UINT8_TO_STREAM(att_tx_buffer, 0x14);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].start_handle);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].end_handle);
                memcpy(att_tx_buffer, gatt_server_pri_service[i].pri_uuid128, 16);
                return 0X16;
            }
            else
            {
                UINT8_TO_STREAM(att_tx_buffer, 0x06);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].start_handle);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].end_handle);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].pri_uuid);
                return 0X08;
            }
        }
    }
    return att_not_found(att_tx_buffer,ATT_REQ_READ_BY_GRP_TYPE);
}

uint16_t gatts_handle_read_type_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Starting_Handle;
    uint16_t Ending_Handle;
    uint16_t Attribute_Type; // 其实就是特定特征的UUID
    STREAM_TO_UINT16(Starting_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Ending_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Attribute_Type, att_rx_buffer);

    for (uint8_t i = 0; i < gatt_server_manager.gatt_server_pri_service_count; i++)
    {
        if (gatt_server_pri_service[i].start_handle <= Starting_Handle)
        {
            for (uint8_t j = 0; j < gatt_server_pri_service[i].serivce_count; j++)
            {
                if (gatt_server_pri_service[i].gatt_server_service[j].handle >= Starting_Handle)
                {
                    if (gatt_server_pri_service[i].gatt_server_service[j].uuid16 == Attribute_Type)
                    {
                        UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_READ_BY_TYPE);
                        UINT8_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].gatt_server_service[j].value_length + 2);
                        UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].gatt_server_service[j].handle);
                        memcpy(att_tx_buffer, gatt_server_pri_service[i].gatt_server_service[j].value, gatt_server_pri_service[i].gatt_server_service[j].value_length);
                        return gatt_server_pri_service[i].gatt_server_service[j].value_length + 4;
                    }
                }
            }
        }
        else if (gatt_server_pri_service[i].start_handle > Ending_Handle)
            break;
    }
    return att_not_found(att_tx_buffer,ATT_REQ_READ_BY_TYPE);
}
// 查找对应服务的handle域
uint16_t gatts_handle_find_by_type_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Starting_Handle;
    uint16_t Ending_Handle;
    uint16_t Attribute_Type;
    uint16_t UUID;
    STREAM_TO_UINT16(Starting_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Ending_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Attribute_Type, att_rx_buffer);
    STREAM_TO_UINT16(UUID, att_rx_buffer);
    for (uint8_t i = 0; i < gatt_server_manager.gatt_server_pri_service_count; i++)
    {
        if (gatt_server_pri_service[i].start_handle >= Starting_Handle)
        {
            if (gatt_server_pri_service[i].pri_uuid == UUID)
            {
                UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_FIND_TYPE_VALUE);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].start_handle);
                UINT16_TO_STREAM(att_tx_buffer, gatt_server_pri_service[i].end_handle);
                return 5;
            }
        }
    }

    return att_not_found(att_tx_buffer,ATT_REQ_FIND_TYPE_VALUE);
}
uint8_t gatt_server_init(void)
{
    gatt_server_manager.server_mtu = GATT_BLE_MTU_SIZE;
    gatt_server_manager.gatt_server_pri_service_count = 0;
    gatt_server_add_pri_service((gatt_server_service_t *)&gap_service, GATT_GAP_SERVICE_HANDLE, GATT_GAP_APPEARANCE_HANDLE,
                                sizeof(gap_service) / sizeof(gatt_server_service_t), BT_UUID_SERVCLASS_GAP_SERVER, NULL, NULL);
    gatt_server_add_pri_service((gatt_server_service_t *)&gatt_service, GATT_SERVICE_HANLE, GATT_SER_SUP_FEATURE,
                                sizeof(gatt_service) / sizeof(gatt_server_service_t), BT_UUID_SERVCLASS_GATT_SERVER, NULL, NULL);
    return 0;
}
#endif
