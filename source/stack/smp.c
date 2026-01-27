#include "smp.h"
#include "smp_key.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "hci.h"
#include "l2cap.h"
#include "gap.h"
uint8_t request_cmd[7] = {0};
uint8_t response_cmd[7] = {0x02, 0x03, 0x00, 0x01, 0x10, 0x00, 0x00};

const uint8_t tk[16] = {0};
uint8_t Ca[16] = {0};
uint8_t Cb[16] = {0};
uint8_t Na[16] = {0};
uint8_t Nb[16] = {0};

uint16_t smp_data_recv(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer)
{
    uint8_t *smp_rx_buffer = rx_buffer;
    uint8_t *smp_tx_buffer = tx_buffer;
    uint16_t smp_len = len;

    // 解析SMP数据包
    uint8_t Opcode;
    STREAM_TO_UINT8(Opcode, smp_rx_buffer);

    // 处理SMP数据包
    switch (Opcode)
    {
    case PAIRING_REQUEST:
        smp_rx_buffer--;
        memcpy(request_cmd, smp_rx_buffer, smp_len);
        UINT8_TO_STREAM(smp_tx_buffer, PAIRING_RESPONSE);
        // 无输入,无输出,MITI(先写死,后面根据config改
        UINT8_TO_STREAM(smp_tx_buffer, 0X03);
        UINT8_TO_STREAM(smp_tx_buffer, 0X00);
        UINT8_TO_STREAM(smp_tx_buffer, 0X01);
        UINT8_TO_STREAM(smp_tx_buffer, 0X10);
        UINT8_TO_STREAM(smp_tx_buffer, 0X00);
        UINT8_TO_STREAM(smp_tx_buffer, 0X00);
        return 7;
    case PAIRING_CONFIRM:
        UINT8_TO_STREAM(smp_tx_buffer, PAIRING_CONFIRM);
        smp_c1(tk, Nb, request_cmd, response_cmd, peer_address, peer_address_type, personal_address, 0, Cb);
        memcpy(smp_tx_buffer, Cb, 16);
        return 17;
    case PAIRING_RANDOM:

        UINT8_TO_STREAM(smp_tx_buffer, PAIRING_RANDOM);
        memcpy(smp_tx_buffer, Na, 16);
        memcpy(Nb, smp_rx_buffer, 16);
        smp_s1(tk, Na, Nb, ltk);
        return 17;
    default:
        printf("not supported smp function\r\n");
        break;
    }

    return 0;
}

static void smp_identify_address_info(void *param)
{
    uint8_t *smp_tx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 8);
    uint8_t *tx_buffer = smp_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, IDENTIFY_ADDRESS);
    UINT8_TO_STREAM(tx_buffer, 0x01);
    memcpy(tx_buffer, personal_address, 6);
    L2CAP_Send_Data(0x0006, smp_tx_buffer, 8);
    free(smp_tx_buffer);
}

static void smp_identify_info(void *param)
{
    uint8_t *smp_tx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 17);
    uint8_t *tx_buffer = smp_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, IDENTIFY_INFO);
    *(uint32_t *)tx_buffer = rand();
    *(uint32_t *)(tx_buffer + 4) = rand();
    *(uint32_t *)(tx_buffer + 8) = rand();
    *(uint32_t *)(tx_buffer + 12) = rand();
    L2CAP_Send_Data(0x0006, smp_tx_buffer, 17);
    free(smp_tx_buffer);
    timer_create(10, smp_identify_address_info, NULL);
}

static void smp_master_identification(void *param)
{
    uint8_t *smp_tx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 11);
    uint8_t *tx_buffer = smp_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, MASTER_IDENT);
    *(uint32_t *)tx_buffer = rand();
    *(uint32_t *)(tx_buffer + 4) = rand();
    *(uint32_t *)(tx_buffer + 6) = rand();
    L2CAP_Send_Data(0x0006, smp_tx_buffer, 11);
    free(smp_tx_buffer);
    timer_create(10, smp_identify_info, NULL);
}

// 这里得改
void smp_enc_info(void)
{
    uint8_t *smp_tx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * 17);
    uint8_t *tx_buffer = smp_tx_buffer;
    UINT8_TO_STREAM(tx_buffer, ENC_INFO);
    smp_s1(tk, Nb, ltk, ltk);
    memcpy(tx_buffer, ltk, 16);
    L2CAP_Send_Data(0x0006, smp_tx_buffer, 17); // 17为固定长度
    free(smp_tx_buffer);
    timer_create(10, smp_master_identification, NULL);
}
