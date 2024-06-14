#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include "esp_gatts_api.h"

// UUIDs for the service and characteristics
#define SERVICE_UUID        0x00FF
#define CHAR_UUID_ACCEL_X   0xFF01
#define CHAR_UUID_ACCEL_Y   0xFF02
#define CHAR_UUID_ACCEL_Z   0xFF03

// Function to initialize the GATT server
void bluetooth_server_init(void);

#endif
