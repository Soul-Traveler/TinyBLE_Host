#include <stdio.h>
#include "phy_h4.h"
#include "hci.h"
#include "Server.h"
#include "tiny_timer.h"
#include "btsnoop.h"
int main(void) {
    printf("bt_stack start!\n");

    // 1. 第一步：打开 BTSnoop 日志 (保证能抓到后续所有包)
    btsnoop_open();

    // 2. 第二步：初始化串口 (保证硬件就绪)
    // 建议加个判断，如果串口打不开直接报错
    if (bt_uart_init_windows("COM4", 115200) != 0) {
        printf("UART init failed!\n");
        return -1;
    }

    // 3. 初始化协议栈各模块
    timer_init();
    bt_tick_init();
    bt_init();
    gatt_server_init();
    Services_init();

    // 4. 最后一步：发送 HCI Reset 命令
    // 此时串口和日志都准备好了，这一包会被成功发出并记录
    bt_hci_reset();

    printf("bt_stack init complete\n");
    while (1) {
        phybusif_input();
    }
    return 0;
}