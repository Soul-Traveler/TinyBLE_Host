#ifndef __BT_TIMER_H__
#define __BT_TIMER_H__

#include "stdint.h"

#define BT_TIMER_COUNT_MAX 10

typedef struct
{
    uint8_t used;                  // 进程是否被使用
    uint8_t cycle;                 // 是否循环
    uint32_t Timer;                // 时间
    uint32_t Time_next;            // 执行间隔
    void *param;                   // 参数
    void (*pFunction)(void *para); // 执行操作
} bt_timer_t;

void timer_running(void);
void timer_cancel(int index);
int timer_create(uint32_t timeout, void (*pFunction)(void *), void *param);
int timer_create_cycle(uint32_t timeout, void (*pFunction)(void *), void *param);
void timer_init(void);
void timer_tick(void);
#endif
