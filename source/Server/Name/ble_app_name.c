#include "ble_app_name.h"

#include <stdio.h>
#include "ble_app_name.h"

/*Device Infomation*/
uint8_t pnp_id[] = "\x02\x5E\x04\x40\x00\x00\x03";
uint8_t manufacturer_name[] = "Entropy_Ruins";
uint8_t dev_info_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_DEVICE_INFO)};
uint8_t gatt_name_string_characteristic[] = {GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(GATT_MANUFACTURER_NAME_STRING), BT_LE_U16_TO_ARRAY(GATT_UUID_MANU_NAME)};
uint8_t gatt_pnp_id_characteristic[] = {GATT_CHAR_PROP_BIT_READ, BT_LE_U16_TO_ARRAY(GATT_PNP_ID_HANDLE), BT_LE_U16_TO_ARRAY(GATT_UUID_PNP_ID)};
gatt_server_service_t dev_info[] = {
    {GATT_DEV_INFO_SERVICE_HANDLE, GATT_UUID_PRI_SERVICE, NULL,
     dev_info_uuid, sizeof(dev_info_uuid), GATT_PERM_READ_ENCRYPTED},
    {GATT_CHARACTERISTIC_MANUFACTURER_NAME_STRING_HANDLE, GATT_UUID_CHAR_DECLARE, NULL,
     gatt_name_string_characteristic, sizeof(gatt_name_string_characteristic), GATT_PERM_READ},
    {GATT_MANUFACTURER_NAME_STRING, GATT_UUID_MANU_NAME, NULL,
     manufacturer_name, sizeof(manufacturer_name), GATT_PERM_READ},
    {GATT_CHARACTERISTIC_PNP_ID, GATT_UUID_CHAR_DECLARE, NULL,
     gatt_pnp_id_characteristic, sizeof(gatt_pnp_id_characteristic), GATT_PERM_READ},
    {GATT_PNP_ID_HANDLE, GATT_UUID_PNP_ID, NULL,
     pnp_id, sizeof(pnp_id) - 1, GATT_PERM_READ},
};

void ble_app_name_service_init(void) {
 gatt_server_add_pri_service((gatt_server_service_t *)&dev_info, GATT_DEV_INFO_SERVICE_HANDLE, GATT_PNP_ID_HANDLE,
                                sizeof(dev_info) / sizeof(gatt_server_service_t), BT_UUID_SERVCLASS_DEVICE_INFO, NULL, NULL);
}


















