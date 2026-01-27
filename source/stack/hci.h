#ifndef __BT_HCI_H__
#define __BT_HCI_H__
#include <stdint.h>
#include "tiny_timer.h"

#define UINT8_TO_STREAM(p, u8)  \
    {                           \
        *(p)++ = (uint8_t)(u8); \
    }
#define UINT16_TO_STREAM(p, u16)        \
    {                                   \
        *(p)++ = (uint8_t)(u16);        \
        *(p)++ = (uint8_t)((u16) >> 8); \
    }
#define STREAM_TO_UINT8(u8, p) \
    {                          \
        u8 = *(p);             \
        (p)++;                 \
    }

#define STREAM_TO_UINT16(u16, p) \
    {                            \
        u16 = *(p);              \
        u16 |= (*(p + 1) << 8);  \
        (p) += 2;                \
    }

#define HCI_OGF_CONTROLLER_BB (0x03 << 10)
#define HCI_OGF_VENDOR_SPEC   (0x3F << 10)
#define HCI_OGF_LE_CONTROLLER (0x08 << 10)
#define HCI_OGF_INFO_PARAM    (0x04 << 10)

#define HCI_RESET_OP          (0x0003 | HCI_OGF_CONTROLLER_BB)
#define HCI_WRITE_LOCAL_NAME  (0x0013 | HCI_OGF_CONTROLLER_BB)
#define HCI_WRITE_SCAN_MODE   (0x001A | HCI_OGF_CONTROLLER_BB)

// WB03
#define HCI_LE_SET_EVENT_MASK                             (0x0001 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_READ_BUFFER_SIZE                           (0x0002 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES              (0x0003 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_RANDOM_ADDRESS                         (0x0005 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_ADVERTISING_PARAMETERS                 (0x0006 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER          (0x0007 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_ADVERTISING_DATA                       (0x0008 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_SCAN_RESPONSE_DATA                     (0x0009 | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_ADVERTISING_ENABLE                     (0x000A | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_SCAN_PARAMETERS                        (0x000B | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_SET_SCAN_ENABLE                            (0x000C | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_CREATE_CONNEDCTION                         (0x000D | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_CREATE_CONNEDCTION_CANCEL                  (0x000E | HCI_OGF_LE_CONTROLLER)

#define HCI_LE_LONG_TERM_LEY_REQUEST_REPLY                (0x001A | HCI_OGF_LE_CONTROLLER)
#define HCI_LE_REMOTE_CONNECTION_PARAMETERS_REQUEST_REPLY (0x0020 | HCI_OGF_LE_CONTROLLER)

#define HCI_VENDOR_OP                                     (0x0000 | HCI_OGF_VENDOR_SPEC)
#define HCI_LE_READ_REMOTE_VERSION_INFO                   0X041D
#define HCI_READ_LOCAL_VERSION                            (0x0001 | HCI_OGF_INFO_PARAM)
#define HCI_READ_BD_ADDR                                  (0x0009 | HCI_OGF_INFO_PARAM)


// LE_meta
#define HCI_LE_CONNECTION_UPDATE_COMPLETE           0x03
#define HCI_LE_REMOTE_CONNECTION_PARAMETERS_REQUEST 0x06
#define HCI_LE_LONG_TERM_KEY_REQUEST_REPLY          0x05
#define HCI_LE_ENHANCED_CONNECTION_COMPLETE         0x0A

#define HCI_EVT_ENC_CHANGE                          0x08
#define HCI_EVT_DISCON															0X05
#define HCI_EVT_CMD_CMPL                            0x0E
#define HCI_EVT_LE_META                             0X3E
#define HCI_EVT_VS_EVT                              0xFF

typedef enum {
    BT_H4_TYPE_CMD = 0x01,
    BT_H4_TYPE_ACL = 0x02,
    BT_H4_TYPE_SCO = 0x03,
    BT_H4_TYPE_EVT = 0x04,
} bt_h4_type_t;

typedef enum {
    BT_H4_W4_TRANSPORT_TYPE,
    BT_H4_W4_EVT_HDR,
    BT_H4_W4_ACL_HDR,
    BT_H4_W4_EVT_PARAM,
    BT_H4_W4_ACL_PARAM,
} bt_h4_read_status_t;

void bt_hci_vendor(uint8_t *data, uint16_t len);
void bt_hci_write_scan_mode(void);
void bt_hci_reset(void);
void bt_hci_write(void);
void bt_hci_reset_timeout(void *param);
void bt_evt_data_process(uint8_t *data, uint16_t len);
void bt_acl_data_process(uint8_t *data, uint16_t len);
extern uint8_t bt_tx_buffer[];
extern uint8_t bt_rx_buffer[];


void bt_acl_data_send(uint8_t *data, uint16_t len);

#endif // !__BT_HCI_

