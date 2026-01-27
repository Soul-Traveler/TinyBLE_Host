#include "tiny_timer.h"
#include <string.h>
bt_timer_t timer[BT_TIMER_COUNT_MAX];

/// @brief 初始化定时器
/// @param
void timer_init(void)
{
#if 0
    for(int index = 0; index < BT_TIMER_COUNT_MAX; index++)
    {
        timer[index].used = 0;
        timer[index].Timer = 0;
        timer[index].pFunction = NULL;
        timer[index].param = NULL;
    }
#endif
    memset(&timer, 0, sizeof(timer));
}

/// @brief 查找空闲的定时器
/// @param
/// @return 空闲定时器的标号
static int find_timer(void)
{
    uint8_t index = 0;
    for (index = 0; index < BT_TIMER_COUNT_MAX; index++)
    {
        if (timer[index].used == 0)
        {
            return index;
        }
    }
    return -1;
}

/// @brief 创建软件定时器
/// @param timeout 软件定时器超时时间
/// @param pFunction 软件定时器Hander
/// @param param Hander的参数
/// @return 软件定时器的编号
int timer_create(uint32_t timeout, void (*pFunction)(void *), void *param)
{
    int index = find_timer();
    if (index == -1)
    {
        return -1;
    }
    timer[index].used = 1;
    timer[index].Timer = timeout;
    timer[index].pFunction = pFunction;
    timer[index].param = param;
    return index;
}

/// @brief 创建软件定时器
/// @param timeout 软件定时器超时时间
/// @param pFunction 软件定时器Hander
/// @param param Hander的参数
/// @return 软件定时器的编号
int timer_create_cycle(uint32_t timeout, void (*pFunction)(void *), void *param)
{
    int index = find_timer();
    if (index == -1)
    {
        return -1;
    }
    timer[index].used = 1;
    timer[index].Timer = timeout;
    timer[index].cycle = 1;
    timer[index].Time_next = timeout;
    timer[index].pFunction = pFunction;
    timer[index].param = param;
    return index;
}



/// @brief 删除定时器
/// @param index 定时器编号
void timer_cancel(int index)
{
    if (1 == timer[index].used)
    {
        memset(&timer[index], 0, sizeof(timer[index]));
    }
}

/// @brief 软件定时器运行
/// @param
void timer_running(void)
{
    int index = 0;
    for (index = 0; index < BT_TIMER_COUNT_MAX; index++)
    {
        if (timer[index].used == 1)
        {
            // timer[index].Timer--;
            if (0 == timer[index].Timer)
            {
                timer[index].pFunction(timer[index].param);
                if (timer[index].cycle == 1)
                {
                    timer[index].Timer = timer[index].Time_next;
                }
                else
                memset(&timer[index], 0, sizeof(timer[index]));
            }
        }
    }
}

/// @brief 软件定时器时钟
/// @param
void timer_tick(void)
{
    int index = 0;
    for (index = 0; index < BT_TIMER_COUNT_MAX; index++)
    {
        if (timer[index].used == 1)
        {
            if (timer[index].Timer > 0)
                timer[index].Timer--;
            // if (0 == timer[index].Timer)
            // {
            //     timer[index].pFunction(timer[index].param);
            //     memset(&timer[index], 0, sizeof(timer[index]));
            // }
        }
    }
}
