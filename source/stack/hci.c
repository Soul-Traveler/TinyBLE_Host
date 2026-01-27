#include "hci.h"
#include "tiny_timer.h"
#include "l2cap.h"
#include "stdio.h"
#include "phy_h4.h"
#include "config.h"
#include "phy_h4.h"
#include "string.h"
#include "gap.h"
int bt_hci_reset_timer;
uint8_t local_device_name[248] = "hello,this is my first bringup code && 你好蓝牙";
uint8_t bt_tx_buffer[1024] = {0};
uint8_t bt_rx_buffer[1024] = {0};
uint8_t bt_le_event_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F};
#if 0
uint8_t bt_le_adv_data[]       = {0x42, 0x54, 0x5F, 0x44, 0x45, 0x4D, 0x4F};
#else
uint8_t bt_le_adv_data[] = CUSTOM_DEVICE_NAME;
#endif

void bt_send(uint8_t *data, uint16_t len)
{
    phy_send(data, len);
}


void bt_hci_reset_timeout(void *param)
{
    printf("bt_hci_reset_timeout\r\n");
    bt_hci_reset();
}

void bt_hci_reset(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_RESET_OP);
    // 无后续参数,直接填0
    UINT8_TO_STREAM(tx_buffer, 0);
    // 发送数据
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    // 创建超时定时器,如果超时则重新发送
    bt_hci_reset_timer = timer_create(2000, bt_hci_reset_timeout, NULL);
}

void bt_hci_write_local_name(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_WRITE_LOCAL_NAME);
    UINT8_TO_STREAM(tx_buffer, 248);
    memcpy(tx_buffer, local_device_name, sizeof(local_device_name));
    tx_buffer += sizeof(local_device_name);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
}

void bt_hci_write_scan_mode(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_WRITE_SCAN_MODE);
    UINT8_TO_STREAM(tx_buffer, 8);
    UINT8_TO_STREAM(tx_buffer, 3);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
}

void bt_le_set_evt_mask(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_LE_SET_EVENT_MASK);
    UINT8_TO_STREAM(tx_buffer, 8);
    memcpy(tx_buffer, bt_le_event_mask, sizeof(bt_le_event_mask));
    tx_buffer += sizeof(bt_le_event_mask);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_set_adv_param(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_LE_SET_ADVERTISING_PARAMETERS);
    UINT8_TO_STREAM(tx_buffer, 15);
    UINT16_TO_STREAM(tx_buffer, 0X0020); // 20ms
    UINT16_TO_STREAM(tx_buffer, 0X0020); // 50ms
    UINT8_TO_STREAM(tx_buffer, 0X00);    // ADV_IND
    UINT8_TO_STREAM(tx_buffer, 0X00);
    UINT8_TO_STREAM(tx_buffer, 0X00);
    UINT16_TO_STREAM(tx_buffer, 0X3831);
    UINT16_TO_STREAM(tx_buffer, 0X0009);
    UINT16_TO_STREAM(tx_buffer, 0X9461);
    UINT8_TO_STREAM(tx_buffer, 0X07);
    UINT8_TO_STREAM(tx_buffer, 0X00);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_set_adv_data(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_LE_SET_ADVERTISING_DATA);
    UINT8_TO_STREAM(tx_buffer, 32);
    memset(tx_buffer, 0, 32);
    UINT8_TO_STREAM(tx_buffer, sizeof(bt_le_adv_data) + 13);

    // 广播设备标识
    UINT8_TO_STREAM(tx_buffer, 0x02);
    UINT8_TO_STREAM(tx_buffer, 0x01);
    UINT8_TO_STREAM(tx_buffer, 0x06);
    // 广播为hid设备
    UINT8_TO_STREAM(tx_buffer, 0x03);
    UINT8_TO_STREAM(tx_buffer, 0x03);
    UINT16_TO_STREAM(tx_buffer, 0x1812);

    // 广播为键盘
    UINT8_TO_STREAM(tx_buffer, 0x03);
    UINT8_TO_STREAM(tx_buffer, 0x19);
    UINT16_TO_STREAM(tx_buffer, 0x03C1);

    // 广播名称
    UINT8_TO_STREAM(tx_buffer, sizeof(bt_le_adv_data) + 1);
    UINT8_TO_STREAM(tx_buffer, 0x09);
    memcpy(tx_buffer, bt_le_adv_data, sizeof(bt_le_adv_data) - 1);
    tx_buffer += sizeof(bt_le_adv_data) - 1;

    bt_send(bt_tx_buffer, 36);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_set_adv_enable(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    // 一个字节的type
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    // cmd的命令向量(opencode)
    UINT16_TO_STREAM(tx_buffer, HCI_LE_SET_ADVERTISING_ENABLE);
    UINT8_TO_STREAM(tx_buffer, 0x01);
    UINT8_TO_STREAM(tx_buffer, 0x01);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_read_buffer_size(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);

    UINT16_TO_STREAM(tx_buffer, HCI_LE_READ_BUFFER_SIZE);
    UINT8_TO_STREAM(tx_buffer, 0);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_read_local_supported_features(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);

    UINT16_TO_STREAM(tx_buffer, HCI_LE_READ_LOCAL_SUPPORTED_FEATURES);
    UINT8_TO_STREAM(tx_buffer, 0);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_read_adcertising_channel_tx_power(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);

    UINT16_TO_STREAM(tx_buffer, HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER);
    UINT8_TO_STREAM(tx_buffer, 0);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

void bt_le_read_local_version(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);

    UINT16_TO_STREAM(tx_buffer, HCI_READ_LOCAL_VERSION);
    UINT8_TO_STREAM(tx_buffer, 0);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

// 发送读取本地mac命令
void bt_le_read_bd_addr(void)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    UINT16_TO_STREAM(tx_buffer, HCI_READ_BD_ADDR);
    UINT8_TO_STREAM(tx_buffer, 0);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

// 读取本地mac
void bt_le_read_bd_addr_rsp(uint8_t *event_buf)
{
    memcpy(personal_address, event_buf, 6);

    printf("personal_address:%x %x %x %x %x %x\n", personal_address[0], personal_address[1], personal_address[2], personal_address[3], personal_address[4], personal_address[5]);
}

static void bt_le_remote_conn_param_req(uint8_t *evt_buffer)
{
    // 读取

    uint16_t Connection_Handle;
    uint16_t Connection_Interval_Min;
    uint16_t Connection_Interval_Max;
    uint16_t Connection_Latency;
    uint16_t Supervision_Timeout;
    STREAM_TO_UINT16(Connection_Handle, evt_buffer);
    STREAM_TO_UINT16(Connection_Interval_Min, evt_buffer);
    STREAM_TO_UINT16(Connection_Interval_Max, evt_buffer);
    STREAM_TO_UINT16(Connection_Latency, evt_buffer);
    STREAM_TO_UINT16(Supervision_Timeout, evt_buffer);

    // 写入
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    UINT16_TO_STREAM(tx_buffer, HCI_LE_REMOTE_CONNECTION_PARAMETERS_REQUEST_REPLY);
    UINT8_TO_STREAM(tx_buffer, 0x0E);
    UINT16_TO_STREAM(tx_buffer, Connection_Handle);
    // UINT16_TO_STREAM(tx_buffer, Connection_Interval_Min);
    UINT16_TO_STREAM(tx_buffer, 0X0018);
    // UINT16_TO_STREAM(tx_buffer, Connection_Interval_Max);
    UINT16_TO_STREAM(tx_buffer, 0X0028);
    // UINT16_TO_STREAM(tx_buffer, Connection_Latency);
    UINT16_TO_STREAM(tx_buffer, 0X0005);
    // UINT16_TO_STREAM(tx_buffer, Supervision_Timeout);
    UINT16_TO_STREAM(tx_buffer, 0X01F4);
    UINT16_TO_STREAM(tx_buffer, 0x00);
    UINT16_TO_STREAM(tx_buffer, 0x00);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

// 为了配对,需要存储一些有关对端的数据
void bt_le_get_extra_data(uint8_t *evt_buffer)
{
    uint8_t status;
    STREAM_TO_UINT8(status, evt_buffer);
    STREAM_TO_UINT16(connection_handle, evt_buffer);
    STREAM_TO_UINT8(role, evt_buffer);
    STREAM_TO_UINT8(peer_address_type, evt_buffer);
    memcpy(peer_address, evt_buffer, 6);
    evt_buffer += 6;
    memcpy(local_address, evt_buffer, 6);
    evt_buffer += 6;
    memcpy(local_private_address, evt_buffer, 6);
    evt_buffer += 6;

    if (status)
    {
        printf("bt_le_get_extra_data error\n");
        while (1)
            ;
    }
}

// 发送长期密钥给controller
void bt_le_set_ltk(uint8_t *evt_buffer)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    uint16_t connected_handle;
    STREAM_TO_UINT16(connected_handle, evt_buffer);
    if (connected_handle != connection_handle)
        // while (1) {
        printf("connection_handle is 0X%x/n", connection_handle);
    // }
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_CMD);
    UINT16_TO_STREAM(tx_buffer, HCI_LE_LONG_TERM_LEY_REQUEST_REPLY);
    UINT8_TO_STREAM(tx_buffer, 0x012);
    UINT16_TO_STREAM(tx_buffer, connected_handle);
    memcpy(tx_buffer, ltk, 16);
    tx_buffer += 16;
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer);
    for (int index = 0; index < tx_buffer - bt_tx_buffer; index++)
    {
        printf("0x%02x ", bt_tx_buffer[index]);
    }
}

//常规初始化流程
void bt_evt_data_process(uint8_t *data, uint16_t len)
{
    printf("bt_evt_data_process\r\n");
    printf("\r\n------------------------------------------------\r\n");
    for (int index = 0; index < len; index++)
    {
        printf("0x%02x ", data[index]);
    }
    printf("\r\n-------------------------------------------\r\n");

    uint8_t *evt_buffer = data;
    uint8_t type;
    uint8_t evt_code;
    uint8_t param_len;
    STREAM_TO_UINT8(type, evt_buffer);
    STREAM_TO_UINT8(evt_code, evt_buffer);
    STREAM_TO_UINT8(param_len, evt_buffer);

    printf("\r\ntype:0x%02x evt_code:0x%02x param_len:%d\r\n",
           type, evt_code, param_len);
    switch (evt_code)
    {
    case HCI_EVT_CMD_CMPL:
    {
        uint8_t num_packet;
        uint16_t opcode;
        uint8_t status;
        STREAM_TO_UINT8(num_packet, evt_buffer);
        STREAM_TO_UINT16(opcode, evt_buffer);
        STREAM_TO_UINT8(status, evt_buffer);

        switch (opcode)
        {
        case HCI_RESET_OP:
        {
            timer_cancel(bt_hci_reset_timer);
            bt_le_set_evt_mask();
            break;
        }
        case HCI_LE_SET_EVENT_MASK:
        {
            printf("HCI_LE_SET_EVENT_MASK\r\n");
            bt_le_read_local_version();
            break;
        }
        case HCI_READ_LOCAL_VERSION:
        {
            printf("HCI_READ_LOCAL_VERSION\r\n");
            bt_le_read_buffer_size();

            break;
        }
        case HCI_LE_READ_BUFFER_SIZE:
        {
            printf("HCI_READ_BUFFER_SIZE\r\n");
            bt_le_read_local_supported_features();
            break;
        }
        case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
        {
            printf("HCI_LE_READ_LOCAL_SUPPORTED_FEATURES\r\n");
            bt_le_read_adcertising_channel_tx_power();
            break;
        }
        case HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER:
        {
            printf("HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER\r\n");
            bt_le_set_adv_param();

            break;
        }
        case HCI_LE_SET_ADVERTISING_PARAMETERS:
        {
            printf("HCI_LE_SET_ADVERTISING_PARAMETERS\r\n");
            bt_le_set_adv_data();
            break;
        }
        case HCI_LE_SET_ADVERTISING_DATA:
        {
            printf("HCI_LE_SET_ADVERTISING_DATA\r\n");
            bt_le_set_adv_enable();
            break;
        }
        case HCI_LE_SET_ADVERTISING_ENABLE:
        {
            bt_le_read_bd_addr();
            break;
        }
        case HCI_READ_BD_ADDR:
        {
            printf("HCI_READ_BD_ADDR\r\n");
            bt_le_read_bd_addr_rsp(evt_buffer);
            break;
        }
        default:
            break;
        }
        break;
    }
    //断开链接
    case HCI_EVT_DISCON:
    {
        timer_init();//清空定时器
        bt_le_set_adv_enable();
        break;
    }
    case HCI_EVT_LE_META:
    {
        uint8_t Subevent_Code;
        STREAM_TO_UINT8(Subevent_Code, evt_buffer);

        switch (Subevent_Code)
        {
        case HCI_LE_REMOTE_CONNECTION_PARAMETERS_REQUEST:
            bt_le_remote_conn_param_req(evt_buffer);
            break;
        case HCI_LE_ENHANCED_CONNECTION_COMPLETE:
            bt_le_get_extra_data(evt_buffer);
            break;
        case HCI_LE_LONG_TERM_KEY_REQUEST_REPLY:
            bt_le_set_ltk(evt_buffer);
            break;
        default:
            break;
        }
        break;
    }
    case HCI_EVT_ENC_CHANGE:
    {
        // smp_enc_info();
        break;
    }
    default:
        break;
    }
}

/// @brief 处理acl的函数入口,后续处理在l2cap层
/// @param data 数据
/// @param len 长度
void bt_acl_data_process(uint8_t *data, uint16_t len)
{
	#if 0
    printf("bt_acl_data_process");
    printf("\r\n------------------------------------------------\r\n");
    for (int index = 0; index < len; index++)
    {
        printf("0x%02x ", data[index]);
    }
    printf("\r\n-------------------------------------------\r\n");
	#endif

    uint8_t *acl_buffer = data;
    uint8_t type;
    uint16_t connection_handle;
    uint8_t PB_flag;
    uint8_t BC_flag;
    uint16_t param_len;
    STREAM_TO_UINT8(type, acl_buffer);
    STREAM_TO_UINT16(connection_handle, acl_buffer);
    /*目前传输数据还没出现需要大量拆分的地方，就先不进行处理*/
    // PB_flag           = connection_handle >> 12;
    // BC_flag           = PB_flag >> 2;
    // PB_flag           = PB_flag & 0b0011;
    // connection_handle = connection_handle & 0x0FFF;
    STREAM_TO_UINT16(param_len, acl_buffer);

    uint8_t *tx_buffer = bt_tx_buffer;
    uint16_t tx_len;
    /*进入L2CAP层*/
    tx_len = L2CAP_Get_Packet(acl_buffer, param_len, tx_buffer + 5);
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_ACL);
    UINT16_TO_STREAM(tx_buffer, connection_handle);
    UINT16_TO_STREAM(tx_buffer, tx_len);
    if (tx_len)
    {
        bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer + tx_len);
    }
}

void bt_acl_data_send(uint8_t *data, uint16_t len)
{
    uint8_t *tx_buffer = bt_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, BT_H4_TYPE_ACL);
    UINT16_TO_STREAM(tx_buffer, connection_handle);
    UINT16_TO_STREAM(tx_buffer, len);
    memcpy(tx_buffer, data, len);
    bt_send(bt_tx_buffer, tx_buffer - bt_tx_buffer + len);
}
