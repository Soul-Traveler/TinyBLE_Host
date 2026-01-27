#include "./btsnoop.h"
#include <windows.h>

FILE *btsnoop_file = NULL;

// uint64_t bt_time = 0;
// uint32_t total, mfree;   // file objects
// uint32_t byteswritten;   /* File write counts */
// uint32_t bytesread;      /* File read counts */
// int btsnoop_fd = -1;

void be_store_32(uint8_t *buffer, uint32_t value)
{
    uint8_t index   = 0;
    buffer[index++] = (uint8_t)(value >> 24);
    buffer[index++] = (uint8_t)(value >> 16);
    buffer[index++] = (uint8_t)(value >> 8);
    buffer[index++] = (uint8_t)(value);
}

// 以8位数据位基，顺序调换。高位在前->低位在前
void be_store_64(uint8_t *buffer, uint64_t value)
{
    uint8_t index   = 0;
    buffer[index++] = (uint8_t)(value >> 56);
    buffer[index++] = (uint8_t)(value >> 48);
    buffer[index++] = (uint8_t)(value >> 40);
    buffer[index++] = (uint8_t)(value >> 32);
    buffer[index++] = (uint8_t)(value >> 24);
    buffer[index++] = (uint8_t)(value >> 16);
    buffer[index++] = (uint8_t)(value >> 8);
    buffer[index++] = (uint8_t)(value);
}

uint8_t btsnoop_open(void)
{
    SYSTEMTIME st;
    char file_name[64] = {0};
    GetLocalTime(&st);
    sprintf(file_name, "btsnoop_%04d-%02d-%02d_%02d-%02d-%02d.log",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    btsnoop_file = fopen(file_name, "wb");
    if (btsnoop_file == NULL)
        return 1;
    const char header[] = "btsnoop\0\0\0\0\1\0\0\x3\xea";
    fwrite(header, 1, sizeof(header) - 1, btsnoop_file);
    fflush(btsnoop_file); // 强制刷入磁盘

    return 0;
}

uint8_t btsnoop_close(void)
{
    if (btsnoop_file != NULL) {
        fclose(btsnoop_file);
        btsnoop_file = NULL;
    }
    return 0;
}

static uint64_t get_btsnoop_timestamp(void)
{
#ifdef _WIN32
    // Windows FileTime 是自 1601年1月1日以来的 100ns 间隔
    // 2000年1月1日距离 1601年1月1日的偏移量 (单位: 100ns)
    const uint64_t OFFSET_1601_TO_2000 = 125911584000000000ULL;

    FILETIME ft;
    ULARGE_INTEGER li;

    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    // 1. 减去偏移量，对齐到 2000年
    // 2. 除以 10，将 100ns 转换为 微秒 (us)
    if (li.QuadPart < OFFSET_1601_TO_2000) return 0; // 防御性检查
    return (li.QuadPart - OFFSET_1601_TO_2000) / 10;
#else
    // 如果是 Linux，可以用 gettimeofday 并减去 2000年的 offset
    return 0;
#endif
}

uint8_t btsnoop_write(uint8_t type, uint8_t in, uint8_t *data, uint16_t data_len)
{
    if (btsnoop_file == NULL) return 1;
    uint32_t orig_len;
    uint32_t include_len;
    uint32_t flags;
    uint32_t drop;
    uint64_t timestamp;

    switch (type) {
        case TRANSPORT_TYPE_CMD:
            flags = FLAG_COMMAND_SENT;
            break;
        case TRANSPORT_TYPE_SCO:
        case TRANSPORT_TYPE_ACL:
            flags = in ? FLAG_DATA_RECEIVED : FLAG_DATA_SENT;
            break;
        case TRANSPORT_TYPE_EVENT:
            flags = FLAG_EVENT_RECEIVED;
            break;
        default:
            break;
    }

    uint64_t time_now = get_btsnoop_timestamp();
    be_store_32((uint8_t *)&orig_len, data_len);
    be_store_32((uint8_t *)&include_len, data_len);
    be_store_32((uint8_t *)&flags, flags);
    be_store_32((uint8_t *)&drop, 0); // 一般不drop数据
    be_store_64((uint8_t *)&timestamp, time_now);

    //后续可以直接用结构体一键写入
    fwrite(&orig_len, sizeof(orig_len), 1, btsnoop_file);
    fwrite(&include_len, sizeof(include_len), 1, btsnoop_file);
    fwrite(&flags, sizeof(flags), 1, btsnoop_file);
    fwrite(&drop, sizeof(drop), 1, btsnoop_file);
    fwrite(&timestamp, sizeof(timestamp), 1, btsnoop_file);
    fwrite(data, data_len, 1, btsnoop_file);
    fflush(btsnoop_file);
    return 0;
}

void btsnoop_sync(void)
{
    if (btsnoop_file != NULL) {
        fflush(btsnoop_file);
    }
}
