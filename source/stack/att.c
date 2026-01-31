#include "../Server/Server.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "string.h"
#include "att.h"
#include "hci.h"
#include "l2cap.h"
#include "gatt.h"
extern gatt_server_pri_service_t gatt_server_pri_service[GATT_PRI_SERVICE_MAX_COUNT];

uint16_t att_data_recv(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t att_len = att_gatt_data_recv(att_rx_buffer, len, att_tx_buffer);

    return att_len;
}

// 交换ATT_mtu,其实没啥用,接收暂时没这个限制
uint16_t att_exchange_mtu(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t att_mtu;
    STREAM_TO_UINT16(att_mtu, att_rx_buffer);
    UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_MTU);
    UINT16_TO_STREAM(att_tx_buffer, ATT_MTU_MAX);
    gatt_server_manager.server_mtu = att_mtu;
    return 3;
}

// att无此服务/句柄
uint16_t att_not_found(uint8_t *tx_buffer, uint8_t opencode)
{
    uint8_t *att_tx_buffer = tx_buffer;
    UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_ERROR);
    UINT8_TO_STREAM(att_tx_buffer, opencode);
    UINT16_TO_STREAM(att_tx_buffer, 0x0001);
    UINT8_TO_STREAM(att_tx_buffer, 0X0A);
    return 5;
}

uint16_t att_wrtie_req(uint8_t *tx_buffer)
{
    uint8_t *att_tx_buffer = tx_buffer;
    UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_WRITE);
    return 1;
}

// 读取句柄对应的值

// att通知,句柄,值和长度
uint16_t att_notification(uint16_t Attribute_Handle, uint8_t *att_data, uint16_t len)
{
    // uint8_t *att_tx_buffer = (uint8_t *) malloc(len + 3); // 除去值,剩下的就是opencode和handle,共3
    uint8_t att_tx_buffer[256];
    uint8_t *tx_buffer = att_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, ATT_HANDLE_VALUE_NOTIFICATION);
    UINT16_TO_STREAM(tx_buffer, Attribute_Handle);
    memcpy(tx_buffer, att_data, len);
    L2CAP_Send_Data(Attribute_Protocol, att_tx_buffer, len + 3);
    // free(att_tx_buffer);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gatt_server_service_t *att_vaul_space[100] = {NULL};

// void att_server_add_pri_handle(gatt_server_service_t *service)
// {
//     static uint16_t att_handle_point = 0;
//     att_vaul_space[service->handle] = service;
// }

// 将注册进行重写，直接由att层对对应的handle进行分配

uint16_t att_server_add_pri_handle(gatt_server_service_t *service)
{
    static uint16_t att_handle_point = 0x0000;
    att_vaul_space[att_handle_point] = service;
    service->handle = att_handle_point++;
    return att_handle_point - 1; // 把当前的handle值返回给上一层
}

// void att_add_handle_grop(gatt_server_service_t *service, uint8_t service_count) {
//     uint8_t i = 0;
//     for (i = 0; i < service_count; i++) {
//         att_server_add_pri_handle(&service[i]);
//     }
// }

// 返回的是开始的handle，有service_count，上层就直接求出值吧
uint16_t att_add_handle_grop(gatt_server_service_t *service, uint8_t service_count)
{
    uint8_t i = 0;
    uint16_t att_start_handle = 0;
    for (i = 0; i < service_count; i++)
    {
        if (0 == i)
            att_start_handle = att_server_add_pri_handle(&service[i]);
        else
            att_server_add_pri_handle(&service[i]);
    }
    return att_start_handle;
}

uint16_t att_handle_write_cmd(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint16_t Attribute_Handle;
    STREAM_TO_UINT16(Attribute_Handle, att_rx_buffer);
    memcpy(att_vaul_space[Attribute_Handle]->value, att_rx_buffer, len - 2);
    att_vaul_space[Attribute_Handle]->value_length = len - 2;
    return 0;
}

uint16_t att_handle_read_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Attribute_Handle;
    STREAM_TO_UINT16(Attribute_Handle, att_rx_buffer);
    // printf("Attribute_Handle = %d\r\n", Attribute_Handle);
    UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_READ);

    for (uint8_t i = 0; i < gatt_server_manager.gatt_server_pri_service_count; i++)
    {
        if (Attribute_Handle >= gatt_server_pri_service[i].start_handle && Attribute_Handle <= gatt_server_pri_service[i].end_handle)
        {
            if (gatt_server_pri_service[i].cb != NULL)
                gatt_server_pri_service[i].cb->gatt_db_read(Attribute_Handle);
        }
    }

    if (gatt_server_manager.server_mtu > (att_vaul_space[Attribute_Handle]->value_length + 2))
    {
        memcpy(att_tx_buffer, att_vaul_space[Attribute_Handle]->value, att_vaul_space[Attribute_Handle]->value_length);
        return att_vaul_space[Attribute_Handle]->value_length + 1;
    }
    else
    {
        memcpy(att_tx_buffer, att_vaul_space[Attribute_Handle]->value, gatt_server_manager.server_mtu - 1);
        return gatt_server_manager.server_mtu;
    }
}

uint16_t atts_handle_find_info_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Starting_Handle;
    uint16_t Ending_Handle;
    STREAM_TO_UINT16(Starting_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Ending_Handle, att_rx_buffer);
    UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_FIND_INFO);
    // 这个01不知道是干啥的
    UINT8_TO_STREAM(att_tx_buffer, 0X01);
    for (uint16_t i = Starting_Handle; i <= Ending_Handle; i++)
    {
        UINT16_TO_STREAM(att_tx_buffer, i);
        if (att_vaul_space[i]->uuid16)
        {
            UINT16_TO_STREAM(att_tx_buffer, att_vaul_space[i]->uuid16);
        }
        else
        {
            // TODO
            //  UINT16_TO_STREAM(att_tx_buffer, att_vaul_space[Starting_Handle]->uuid128);
        }
    }
    return att_tx_buffer - tx_buffer;
}

uint16_t att_handle_read_blob_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Attribute_Handle;
    uint16_t Offset;
    STREAM_TO_UINT16(Attribute_Handle, att_rx_buffer);
    STREAM_TO_UINT16(Offset, att_rx_buffer);

    // ---检查 Handle 是否合法 ---
    if (/*Attribute_Handle >= gatt_server_manager.gatt_server_pri_service_count ||*/ att_vaul_space[Attribute_Handle] == NULL)
    {
        uint8_t *err_ptr = tx_buffer;
        UINT8_TO_STREAM(err_ptr, ATT_RSP_ERROR);
        UINT8_TO_STREAM(err_ptr, ATT_REQ_READ_BLOB);
        UINT16_TO_STREAM(err_ptr, Attribute_Handle);
        UINT8_TO_STREAM(err_ptr, 0x01); // 0x01: Invalid Handle
        return 5;
    }


    if (Offset < att_vaul_space[Attribute_Handle]->value_length)
    {
        UINT8_TO_STREAM(att_tx_buffer, ATT_RSP_READ_BLOB);
        if (gatt_server_manager.server_mtu > (att_vaul_space[Attribute_Handle]->value_length + 2 - Offset))
        {
            memcpy(att_tx_buffer, att_vaul_space[Attribute_Handle]->value + Offset,
                   att_vaul_space[Attribute_Handle]->value_length - Offset);
            return att_vaul_space[Attribute_Handle]->value_length - Offset + 1;
        }
        else
        {
            memcpy(att_tx_buffer, att_vaul_space[Attribute_Handle]->value + Offset, gatt_server_manager.server_mtu - 1);
            return gatt_server_manager.server_mtu;
        }
    }
    else
    {
        // 越界处理
        uint8_t *err_ptr = tx_buffer;
        UINT8_TO_STREAM(err_ptr, ATT_RSP_ERROR);     // Opcode: 0x01
        UINT8_TO_STREAM(err_ptr, ATT_REQ_READ_BLOB); // 出错的请求 Opcode: 0x0C
        UINT16_TO_STREAM(err_ptr, Attribute_Handle); // 出错的句柄
        UINT8_TO_STREAM(err_ptr, 0x07);              // Error Code: Invalid Offset (0x07)
        return 5;
    }
}

uint16_t att_handle_wrtie_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *att_rx_buffer = rx_buffer;
    uint8_t *att_tx_buffer = tx_buffer;
    uint16_t Attribute_Handle;
    STREAM_TO_UINT16(Attribute_Handle, att_rx_buffer);
    memcpy(att_vaul_space[Attribute_Handle]->value, att_rx_buffer, len - 2);
    for (uint8_t i = 0; i < gatt_server_manager.gatt_server_pri_service_count; i++)
    {
        if (Attribute_Handle >= gatt_server_pri_service[i].start_handle && Attribute_Handle <= gatt_server_pri_service[i].end_handle)
        {
            if (gatt_server_pri_service[i].cb != NULL)
                gatt_server_pri_service[i].cb->gatt_db_write(Attribute_Handle, att_rx_buffer, len - 2);
        }
    }
    return att_wrtie_req(att_tx_buffer);
}
