#include "phy_h4.h"
#include "hci.h"
#include <stdio.h>
#include "fifo.h"
#include <string.h>
uint16_t read_pos = 0;
uint16_t event_param_len;
uint16_t acl_param_len;
bt_h4_read_status_t h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
uint8_t get_data_flag = 0;
extern uint8_t USART2_RX_BUF[1024];

#if 1

#define UART_BT_FIFO_SIZE 1024

KFIFO uart_bt_fifo;
uint8_t uart_bt_fifo_data[UART_BT_FIFO_SIZE];
uint8_t uart_data;
// uint8_t
uint8_t uart_bt_init(void)
{
    memset(uart_bt_fifo_data, 0, UART_BT_FIFO_SIZE);
#if BTSNOOP_ENABLE
    btsnoop_open();
#endif
    return kfifo_init(&uart_bt_fifo, uart_bt_fifo_data, UART_BT_FIFO_SIZE);
}

int uart_bt_send(uint8_t *data, uint16_t len)
{
#if BTSNOOP_ENABLE
    btsnoop_write(data[0], 0, data, len);
#endif
    return HAL_UART_Transmit(&huart2, data, len, 1000);
    // return 0;
}

void phy_send(uint8_t *data, uint16_t len)
{
    uart_bt_send(data, len);
}


int uart_bt_get_it(void)
{
    kfifo_put_ch(&uart_bt_fifo, uart_data);
    HAL_UART_Receive_IT(&huart2, &uart_data, 1);

    return 0;
}

void uart_bt_get(uint8_t *data, uint16_t len)
{
    kfifo_put(&uart_bt_fifo, data, len);
#if BTSNOOP_ENABLE
    btsnoop_write(data[0], 1, data, len);
#endif
}

int uart_bt_read(uint8_t *data, uint16_t len)
{
#if 0
    for (int index = 0; index < len; index++) {
        kfifo_get_ch(&uart_bt_fifo, data);
        data++;
    }
#else
    kfifo_get(&uart_bt_fifo, data, len);
#endif
    return 0;
}

uint8_t uart_bt_data_ready(void)
{
    return !kfifo_is_empty(&uart_bt_fifo);
}

#endif

void uart_bt_reset(void)
{
    kfifo_reset(&uart_bt_fifo);
}

void uart_bt_finish(void)
{
#if BTSNOOP_ENABLE
    btsnoop_close();
#endif
    printf("finished\r\n");
}

void Uart_Receive_IDLE(void)
{
    uint8_t len;

    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2); // 清除标志
        HAL_UART_DMAStop(&huart2);          // 停止DMA接收，防止数据出

        len = 1024 - huart2.hdmarx->Instance->CNDTR; // 获取DMA中传输的数据个数
        printf("data_get_complete:%d\n", len);
        uart_bt_get(USART2_RX_BUF, len);
        HAL_UART_Receive_DMA(&huart2, USART2_RX_BUF, 1024); // 打开DMA接收，数据存入rx_buffer数组中
    }
}

uint8_t phybusif_input(void)
{
    // 若未收到数据，则轮空，不执行
    while (uart_bt_data_ready())
    {

        switch (h4_read_status)
        {
        case BT_H4_W4_TRANSPORT_TYPE:
            uart_bt_read(bt_rx_buffer, 1);
            if (bt_rx_buffer[0] == BT_H4_TYPE_EVT)
            {
                h4_read_status = BT_H4_W4_EVT_HDR;
            }
            else if (bt_rx_buffer[0] == BT_H4_TYPE_ACL)
            {
                h4_read_status = BT_H4_W4_ACL_HDR;
            }
            read_pos = 1;
            break;
        case BT_H4_W4_EVT_HDR:
            uart_bt_read(bt_rx_buffer + read_pos, 2);
            printf("evt hdr: %02x %02x\r\n", bt_rx_buffer[read_pos], bt_rx_buffer[read_pos + 1]); // 这里显示有问题，不影响运行，应该是有调试没删
            event_param_len = bt_rx_buffer[read_pos + 1];
            read_pos += 2;
            h4_read_status = BT_H4_W4_EVT_PARAM;
            break;
        case BT_H4_W4_ACL_HDR:
            uart_bt_read(bt_rx_buffer + read_pos, 4);
            printf("connect info: %02x %02x\r\n", bt_rx_buffer[read_pos], bt_rx_buffer[read_pos + 1]);
            acl_param_len = (bt_rx_buffer[read_pos + 3] << 8) + bt_rx_buffer[read_pos + 2];
            printf("acl_param_len = %d\n", acl_param_len);
            read_pos += 4;
            h4_read_status = BT_H4_W4_ACL_PARAM;
            break;
        case BT_H4_W4_EVT_PARAM:
            /*读取event*/
            uart_bt_read(bt_rx_buffer + read_pos, event_param_len);
            /* event处理 */
            bt_evt_data_process(bt_rx_buffer, read_pos + event_param_len);
            read_pos = 0;
            h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
            break;
        case BT_H4_W4_ACL_PARAM:
            /*读取acl*/
            uart_bt_read(bt_rx_buffer + read_pos, acl_param_len);
            /*acl处理，丢给hci*/
            bt_acl_data_process(bt_rx_buffer, read_pos + event_param_len);
            read_pos = 0;
            h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
            break;
        default:
            break;
        }
    }
    return 0;
}

int fputc(int ch, FILE *f)
{

    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
