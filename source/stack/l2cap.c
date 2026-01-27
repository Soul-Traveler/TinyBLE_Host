#include "l2cap.h"
#include "config.h"
#include "hci.h"
#include "att.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "smp.h"
uint16_t L2CAP_Get_Packet(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *l2cap_rx_buffer = rx_buffer;
    uint8_t *l2cap_tx_buffer = tx_buffer;
    uint16_t l2_len;
    /*好像这个结构体不是很必要的样子*/
    l2cap_hdr_t rx_hdr;
    l2cap_hdr_t tx_hdr;
    STREAM_TO_UINT16(rx_hdr.len, l2cap_rx_buffer);
    STREAM_TO_UINT16(rx_hdr.cid, l2cap_rx_buffer);

    switch (rx_hdr.cid)
    {
    case L2CAP_Signaling_channel:
        // 暂时不用
        break;
    case Connectionless_channel:
        // 暂时不用
        break;
    case AMP_Manager_Protocol:
        // 暂时不用
        break;
    case Attribute_Protocol:
        l2_len = att_data_recv(l2cap_rx_buffer, rx_hdr.len, l2cap_tx_buffer + 4);
        tx_hdr.len = l2_len;
        tx_hdr.cid = Attribute_Protocol;
        break;
    case Low_Energy_L2CAP_Signaling_channel:
        break;
    case Security_manager_protocol:
        tx_hdr.len = smp_data_recv(l2cap_rx_buffer, rx_hdr.len, l2cap_tx_buffer + 4);
        tx_hdr.cid = Security_manager_protocol;
        break;
    case AMP_Test_Manager_Protocol:
        // 暂时不用
        break;
    default:
        break;
    }
    printf("rx_hdr.len = %d,rx_hdr.cid = %d\n", rx_hdr.len, rx_hdr.cid);
    UINT16_TO_STREAM(l2cap_tx_buffer, tx_hdr.len);
    UINT16_TO_STREAM(l2cap_tx_buffer, tx_hdr.cid);
    if (tx_hdr.len)
        return tx_hdr.len + 4;
    else
        return 0;
}


void L2CAP_Send_Data(uint16_t cid, uint8_t *data, uint16_t len)
{
    // uint8_t *l2cap_tx_buffer = (uint8_t *)malloc(len + 4); // cid和len加起来共4的字节
    uint8_t l2cap_tx_buffer[256];
    uint8_t *tx_buffer = l2cap_tx_buffer;
    UINT16_TO_STREAM(tx_buffer, len);
    UINT16_TO_STREAM(tx_buffer, cid);
    memcpy(tx_buffer, data, len);
    bt_acl_data_send(l2cap_tx_buffer, len + 4);
    // free(l2cap_tx_buffer);
}
