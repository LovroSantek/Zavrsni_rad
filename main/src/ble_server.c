#include "ble_server.h"
#include "esp_log.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#define GATTS_TAG "BLE_SERVER"

#define DEVICE_NAME "Zavrsni_rad_ble"

static const uint16_t char_uuid_accel_x = CHAR_UUID_ACCEL_X;
static const uint16_t char_uuid_accel_y = CHAR_UUID_ACCEL_Y;
static const uint16_t char_uuid_accel_z = CHAR_UUID_ACCEL_Z;

static uint16_t connection_id = 0xFFFF;

static bool is_notification_enabled_x = false;
static bool is_notification_enabled_y = false;
static bool is_notification_enabled_z = false;

static uint16_t cccd_handle_x;
static uint16_t cccd_handle_y;
static uint16_t cccd_handle_z;

// UUIDs should be constants or defined correctly
static const uint16_t service_uuid = SERVICE_UUID;
static const uint16_t client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;


// Define characteristic properties
static const uint8_t char_property_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const esp_gatts_attr_db_t gatt_db[] = {
    // Service Declaration
    [0] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&service_uuid, ESP_GATT_PERM_READ, 
         sizeof(service_uuid), sizeof(service_uuid), (uint8_t *)&service_uuid}
    },
    // Characteristic Declaration for Acceleration X
    [1] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_x, ESP_GATT_PERM_READ, 
         sizeof(uint8_t), sizeof(char_property_read_write_notify), (uint8_t *)&char_property_read_write_notify}
    },
    // Characteristic Value for Acceleration X
    [2] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_x, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         sizeof(float), sizeof(float), NULL}
    },
    // CCCD for Acceleration X
    [3] = {
    {ESP_GATT_AUTO_RSP}, 
    {ESP_UUID_LEN_16, (uint8_t *)&client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
     sizeof(uint16_t), sizeof(uint16_t), NULL}
    },
    // Characteristic Declaration for Acceleration Y
    [4] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_y, ESP_GATT_PERM_READ, 
         sizeof(uint8_t), sizeof(char_property_read_write_notify), (uint8_t *)&char_property_read_write_notify}
    },
    // Characteristic Value for Acceleration Y
    [5] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_y, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         sizeof(float), sizeof(float), NULL}
    },
    // CCCD for Acceleration Y
    [6] = {
    {ESP_GATT_AUTO_RSP},
    {ESP_UUID_LEN_16, (uint8_t *)&client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
     sizeof(uint16_t), sizeof(uint16_t), NULL}
    },
    // Characteristic Declaration for Acceleration Z
    [7] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_z, ESP_GATT_PERM_READ, 
         sizeof(uint8_t), sizeof(char_property_read_write_notify), (uint8_t *)&char_property_read_write_notify}
    },
    // Characteristic Value for Acceleration Z
    [8] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_z, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         sizeof(float), sizeof(float), NULL}
    },
    // CCCD for Acceleration Z
    [9] = {
    {ESP_GATT_AUTO_RSP},
    {ESP_UUID_LEN_16, (uint8_t *)&client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
     sizeof(uint16_t), sizeof(uint16_t), NULL}
    },
};

static uint16_t gatts_interface;

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            // Save the interface for later use
            gatts_interface = gatts_if;
            // Register the attribute table
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, sizeof(gatt_db) / sizeof(gatt_db[0]), 0);
        break;

        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status == ESP_GATT_OK)
            {
                uint16_t start_handle = param->add_attr_tab.handles[0];
                ESP_LOGI(GATTS_TAG, "Attribute table created, start handle = %d", start_handle);
                cccd_handle_x = start_handle + 3;
                cccd_handle_y = start_handle + 6;
                cccd_handle_z = start_handle + 9;
            }
            else
            {
                ESP_LOGE(GATTS_TAG, "Failed to create attr table, error code = %x", param->add_attr_tab.status);
            }
        break;

        case ESP_GATTS_CONNECT_EVT:
            connection_id = param->connect.conn_id;
            gatts_interface = gatts_if;
            ESP_LOGI(GATTS_TAG, "Device connected, conn_id = %d", connection_id);
        break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TAG, "Device disconnected, conn_id = %d", connection_id);
            connection_id = 0xFFFF;
        break;

        case ESP_GATTS_WRITE_EVT:
            if (param->write.handle == cccd_handle_x && param->write.len == 2) {
                uint16_t config_value = param->write.value[0] | (param->write.value[1] << 8);
                is_notification_enabled_x = (config_value == 0x0001);
            }
            else if (param->write.handle == cccd_handle_y && param->write.len == 2) {
            uint16_t config_value = param->write.value[0] | (param->write.value[1] << 8);
            is_notification_enabled_y = (config_value == 0x0001);
            }
            else if (param->write.handle == cccd_handle_z && param->write.len == 2) {
                uint16_t config_value = param->write.value[0] | (param->write.value[1] << 8);
                is_notification_enabled_z = (config_value == 0x0001);
            }
        break;

        default:
            ESP_LOGI(GATTS_TAG, "Unhandled GATT event %d", event);
        break;
    }
}

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
};

void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            ESP_LOGI(GATTS_TAG, "BLE advertising started");
            break;
        default:
            ESP_LOGI(GATTS_TAG, "Unhandled GAP event %d", event);
            break;
    }
}

void ble_advertising_start(void)
{
    static uint8_t service_uuid[16] = {
        // Your Service UUID here, 16 bytes if using 128-bit UUIDs
        0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
        0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
    };

    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .min_interval = 0x0006,
        .max_interval = 0x0010,
        .appearance = 0x00,
        .manufacturer_len = 0,
        .p_manufacturer_data = NULL,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = sizeof(service_uuid),
        .p_service_uuid = service_uuid,
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
    };
    
    esp_err_t status = esp_ble_gap_config_adv_data(&adv_data);
    if (status != ESP_OK)
    {
        ESP_LOGE(GATTS_TAG, "Failed to config adv data: %s", esp_err_to_name(status));
    }
    else
    {
        ESP_LOGI(GATTS_TAG, "Advertising data set up");
    }
}

void bluetooth_server_init(void)
{
    ESP_LOGI(GATTS_TAG, "Initializing GATT server...");
    esp_err_t ret = esp_ble_gap_set_device_name(DEVICE_NAME);
    if (ret != ESP_OK)
    {
        ESP_LOGE(GATTS_TAG, "Setting device name failed: %s", esp_err_to_name(ret));
    }
    else 
    {
        ESP_LOGI(GATTS_TAG, "Name of Bluetooth device set to: %s", DEVICE_NAME);
    }
    esp_ble_gatts_register_callback(gatts_event_handler);
    ESP_LOGI(GATTS_TAG, "GATT server callback registered.");

    esp_ble_gatts_app_register(0);
    ESP_LOGI(GATTS_TAG, "Registering application profile with GATT server...");

    esp_ble_gap_register_callback(gap_event_handler);
    ESP_LOGI(GATTS_TAG, "GAP callback registered.");

    ble_advertising_start();
    ESP_LOGI(GATTS_TAG, "Advertising start requested.");
}