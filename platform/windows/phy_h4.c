#include <stdio.h>
#include <windows.h>
#include "phy_h4.h"
#include "hci.h"
#include "Config.h"
#include "fifo.h"
#include "btsnoop.h"

// ---------------------------------------------------------
// 串口部分 (Windows API)
// ---------------------------------------------------------
static HANDLE hSerial = INVALID_HANDLE_VALUE;

uint8_t bt_uart_init_windows(const char *portName, uint32_t baudRate)
{
    hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port: %s\n", portName);
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    // 开启 RTS/CTS 硬件流控
    dcbSerialParams.fOutxCtsFlow = TRUE;
    dcbSerialParams.fRtsControl  = RTS_CONTROL_HANDSHAKE;
    dcbSerialParams.fOutxDsrFlow = FALSE;
    dcbSerialParams.fDtrControl  = DTR_CONTROL_DISABLE;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return 1;
    }

    // 配置超时 (模拟非阻塞读取)
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = MAXDWORD; // 字节间超过1ms没收到就返回
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hSerial, &timeouts);

    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
    printf("Serial opened: %s @ %d (RTS/CTS Enabled)\n", portName, baudRate);
    return 0;
}

// 封装后的串口读取函数
int bt_uart_read(uint8_t *buffer, uint16_t max_len)
{
    if (hSerial == INVALID_HANDLE_VALUE) return 0;
    DWORD bytesRead = 0;
    // 这里会稍微阻塞一下(由Timeouts决定)，等待数据到来
    ReadFile(hSerial, buffer, max_len, &bytesRead, NULL);
    return (int)bytesRead;
}

uint8_t phy_send(uint8_t *data, uint16_t len)
{
    if (hSerial == INVALID_HANDLE_VALUE) return 1;
    DWORD bytesWritten;
    if (!WriteFile(hSerial, data, len, &bytesWritten, NULL)) {
        printf("Serial write error\n");
        return 1;
    }
    // 发送的数据记录到 btsnoop (方向: Host->Controller)
    // data[0] 是 Packet Type (CMD/ACL/SCO)
    // 0 表示发送
    btsnoop_write(data[0], 0, data, len); 
    return 0;
}

void bt_uart_close(void)
{
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

// ---------------------------------------------------------
// H4 协议与 FIFO 管理
// ---------------------------------------------------------
volatile uint8_t timmer_running_flag = 0;
uint16_t read_pos = 0;
uint16_t event_param_len;
uint16_t acl_param_len;
bt_h4_read_status_t h4_read_status = BT_H4_W4_TRANSPORT_TYPE;

KFIFO bt_fifo;
uint8_t bt_fifo_data[2048]; //稍微加大一点缓冲

uint8_t bt_init(void)
{
    memset(bt_fifo_data, 0, sizeof(bt_fifo_data));
    return kfifo_init(&bt_fifo, bt_fifo_data, sizeof(bt_fifo_data));
}

void bt_reset(void)
{
    kfifo_reset(&bt_fifo);
}

// 内部辅助函数：从FIFO读数据
static int bt_read(uint8_t *data, uint16_t len)
{
    // 如果FIFO里数据不够，这里其实会出问题。
    // 但因为我们在 phybusif_input 里是一步步 check 的，
    // 只要 FIFO 里没数据，状态机就不会往下走，所以这里直接 get 是安全的。
    return kfifo_get(&bt_fifo, data, len);
}

// ---------------------------------------------------------
// 核心调度函数
// ---------------------------------------------------------
uint8_t phybusif_input(void)
{
    // 1. 【新增】从 Windows 串口捞数据，喂给 FIFO
    uint8_t rx_temp[256];
    int len = bt_uart_read(rx_temp, sizeof(rx_temp));
    if (len > 0) {
        // 存入 FIFO
        kfifo_put(&bt_fifo, rx_temp, len);
        // 注意：接收方向的 btsnoop 比较难在这里记，因为还不知道包类型。
        // 如果想看原始数据，可以在这里打印或写入 raw log。
        // 或者在下面状态机解析出完整包后再 write btsnoop。
    }

    // 2. 只有 FIFO 不为空时，才跑状态机
    while (!kfifo_is_empty(&bt_fifo)) 
    {
        switch (h4_read_status)
        {
        case BT_H4_W4_TRANSPORT_TYPE:
            // 需要 1 字节
            if (kfifo_len(&bt_fifo) < 1) return 0;
            
            bt_read(bt_rx_buffer, 1);
            if (bt_rx_buffer[0] == BT_H4_TYPE_EVT) {
                h4_read_status = BT_H4_W4_EVT_HDR;
            } else if (bt_rx_buffer[0] == BT_H4_TYPE_ACL) {
                h4_read_status = BT_H4_W4_ACL_HDR;
            }
            read_pos = 1;
            break;

        case BT_H4_W4_EVT_HDR:
            // 需要 2 字节 (EventCode + Len)
            if (kfifo_len(&bt_fifo) < 2) return 0;

            bt_read(bt_rx_buffer + read_pos, 2);
            // printf("evt hdr: %02x %02x\r\n", bt_rx_buffer[read_pos], bt_rx_buffer[read_pos + 1]);
            event_param_len = bt_rx_buffer[read_pos + 1];
            read_pos += 2;
            h4_read_status = BT_H4_W4_EVT_PARAM;
            break;

        case BT_H4_W4_ACL_HDR:
            // 需要 4 字节 (Handle + Len)
            if (kfifo_len(&bt_fifo) < 4) return 0;

            bt_read(bt_rx_buffer + read_pos, 4);
            acl_param_len = (bt_rx_buffer[read_pos + 3] << 8) + bt_rx_buffer[read_pos + 2];
            read_pos += 4;
            h4_read_status = BT_H4_W4_ACL_PARAM;
            break;

        case BT_H4_W4_EVT_PARAM:
            // 需要 payload 长度
            if (kfifo_len(&bt_fifo) < event_param_len) return 0;

            bt_read(bt_rx_buffer + read_pos, event_param_len);
            
            // 收到完整包了！在这里记 BTSnoop (接收方向)
            // 1 表示接收
            btsnoop_write(BT_H4_TYPE_EVT, 1, bt_rx_buffer, read_pos + event_param_len);

            bt_evt_data_process(bt_rx_buffer, read_pos + event_param_len);
            read_pos = 0;
            h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
            break;

        case BT_H4_W4_ACL_PARAM:
            // 需要 payload 长度
            if (kfifo_len(&bt_fifo) < acl_param_len) return 0;

            bt_read(bt_rx_buffer + read_pos, acl_param_len);

            // 收到完整包了！在这里记 BTSnoop
            btsnoop_write(BT_H4_TYPE_ACL, 1, bt_rx_buffer, read_pos + acl_param_len);

            bt_acl_data_process(bt_rx_buffer, read_pos + acl_param_len);
            read_pos = 0;
            h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
            break;

        default:
            h4_read_status = BT_H4_W4_TRANSPORT_TYPE;
            break;
        }
    }

    // 3. 处理定时器任务
    if (timmer_running_flag == 1)
    {
        timmer_running_flag = 0;
        // 注意：这里调用的是 extern 的 timer_tick
        // 确保你在 timer/bt_timer.c 里实现了这个函数
        timer_running();
    }
    Sleep(1);
    return 0;
}

// 心跳线程
DWORD WINAPI Heartbeat_Thread(LPVOID lpParam)
{
    while (1) {
        Sleep(1); // 1ms 心跳
        timmer_running_flag = 1; 
        // 也可以直接在这里调 timer_tick()，但为了线程安全，
        // 在 phybusif_input (主线程) 里调更稳妥，避免 data race。
        timer_tick();

    }
    return 0;
}

void bt_tick_init(void)
{
    HANDLE hThread = CreateThread(NULL, 0, Heartbeat_Thread, NULL, 0, NULL);
    if (hThread) {
        // CloseHandle(hThread);
        printf("Heartbeat thread started.\n");
    }
}