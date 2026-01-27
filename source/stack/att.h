#ifndef __ATT_H__
#define __ATT_H__

#include "config.h"
#include "stdint.h"
//#include "gatt.h"


#define ATT_RSP_ERROR 0x01
#define ATT_REQ_MTU 0x02
#define ATT_RSP_MTU 0x03
#define ATT_REQ_FIND_INFO 0x04
#define ATT_RSP_FIND_INFO 0x05
#define ATT_REQ_FIND_TYPE_VALUE 0x06
#define ATT_RSP_FIND_TYPE_VALUE 0x07
#define ATT_REQ_READ_BY_TYPE 0x08
#define ATT_RSP_READ_BY_TYPE 0x09
#define ATT_REQ_READ 0x0A
#define ATT_RSP_READ 0x0B
#define ATT_REQ_READ_BLOB 0x0C
#define ATT_RSP_READ_BLOB 0x0D
#define ATT_REQ_READ_MULTI 0x0E
#define ATT_RSP_READ_MULTI 0x0F
#define ATT_REQ_READ_BY_GRP_TYPE 0x10
#define ATT_RSP_READ_BY_GRP_TYPE 0x11
#define ATT_REQ_WRITE 0x12
#define ATT_RSP_WRITE 0x13

#define ATT_CMD_WRITE 0x52
#define ATT_REQ_PREPARE_WRITE 0x16
#define ATT_RSP_PREPARE_WRITE 0x17
#define ATT_REQ_EXEC_WRITE 0x18
#define ATT_RSP_EXEC_WRITE 0x19
#define ATT_HANDLE_VALUE_NOTIFICATION 0x1B
#define ATT_HANDLE_VALUE_IND 0x1D
#define ATT_HANDLE_VALUE_CONF 0x1E
#define ATT_SIGN_CMD_WRITE 0xD2

#define ATT_MTU_MAX 23

// extern gatt_server_service_t *att_vaul_space;

#define BT_LE_U16_TO_ARRAY(u16) (u16 & 0xff), (u16 >> 8 & 0xff)
#define MAX_UUID_SIZE 16

typedef struct
{
#define LEN_UUID_16 2
#define LEN_UUID_32 4
#define LEN_UUID_128 16
  uint16_t len;
  union
  {
    uint16_t uuid16;
    uint32_t uuid32;
    uint8_t uuid128[MAX_UUID_SIZE];
  } uu;

} bt_uuid_t;


uint16_t att_exchange_mtu(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_data_recv(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_find_grp_type(uint16_t START_HANDLE, uint8_t *tx_buffer);
uint16_t att_not_found(uint8_t *tx_buffer,uint8_t opencode);
uint16_t att_handle_read_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_handle_wrtie_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_handle_write_cmd(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
uint16_t att_handle_read_blob_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);
// 通知服务调用
uint16_t att_notification(uint16_t Attribute_Handle, uint8_t *att_data, uint16_t len);

uint16_t atts_handle_find_info_req(uint8_t *rx_buffer, uint16_t len, uint8_t *tx_buffer);


///////////////////////////////////////////////////

#endif /* ATT_H */
