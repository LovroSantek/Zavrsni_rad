#include "ble_server.h"
#include "esp_log.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#define GATTS_TAG "BLE_SERVER"

// UUIDs should be constants or defined correctly
static const uint16_t service_uuid = SERVICE_UUID;
static const uint16_t char_uuid_accel_x = CHAR_UUID_ACCEL_X;

// Define characteristic properties
static const uint8_t char_property_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const esp_gatts_attr_db_t gatt_db[] = {
    // Service Declaration
    [0] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&service_uuid, ESP_GATT_PERM_READ, 
         sizeof(uint16_t), sizeof(service_uuid), (uint8_t *)&service_uuid}
    },
    // Characteristic Declaration
    [1] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&char_uuid_accel_x, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         sizeof(float), 0, (uint8_t *)&char_property_read_write_notify}
    },
};

static uint16_t gatts_if;

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            // Save the interface for later use
            gatts_if = gatts_if;
            // Register the attribute table
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, sizeof(gatt_db) / sizeof(gatt_db[0]), 0);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status == ESP_GATT_OK) {
                ESP_LOGI(GATTS_TAG, "Attribute table created, handle = %d", param->add_attr_tab.num_handle);
            } else {
                ESP_LOGE(GATTS_TAG, "Failed to create attr table, error code = %x", param->add_attr_tab.status);
            }
            break;
            
        default:
            ESP_LOGI(GATTS_TAG, "Unhandled GATT event %d", event);
            break;
    }
}

static uint8_t raw_adv_data[] = {
    /* Flags */
    0x02, 0x01, 0x06,  // General discoverable, BR/EDR not supported

    /* Service UUID */
    0x03, 0x03, 0xFF, 0xFF,  // Length, Type, UUID

    /* Device Name */
    0x0F, 0x09, 'E', 'x', 'a', 'm', 'p', 'l', 'e', ' ', 'D', 'e', 'v', 'i', 'c', 'e',

    /* Transmission Power */
    0x02, 0x0A, 0xEB  // Length, Type, Power
};

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
    esp_ble_gatts_register_callback(gatts_event_handler);
    ESP_LOGI(GATTS_TAG, "GATT server callback registered.");

    // Register application profile
    ESP_LOGI(GATTS_TAG, "Registering application profile with GATT server...");
    esp_err_t register_app_result = esp_ble_gatts_app_register(0);
    if (register_app_result == ESP_OK) {
        ESP_LOGI(GATTS_TAG, "Application profile registered successfully.");
    } else {
        ESP_LOGE(GATTS_TAG, "Failed to register application profile, error code: %s", esp_err_to_name(register_app_result));
    }

    // Register the GAP callback
    esp_ble_gap_register_callback(gap_event_handler);
    ESP_LOGI(GATTS_TAG, "GAP callback registered.");

    // Start advertising
    ble_advertising_start();
    ESP_LOGI(GATTS_TAG, "Advertising start requested.");
}