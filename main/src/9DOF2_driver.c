#include <string.h>
#include "esp_log.h"

#include "9DOF2_driver.h"

spi_device_handle_t spi_handle;

void spi_setup()
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = MAX_TRANSFER_SIZE
    };

    esp_err_t result = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to initialize bus: %s", esp_err_to_name(result));
        return;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,  // Clock 1 MHz
        .mode = 0,  // SPI mode 0
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .flags = 0,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .input_delay_ns = 0
    };

    result = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to setup SPI: %s", esp_err_to_name(result));
        spi_bus_free(VSPI_HOST);
    }
    else
    {
        ESP_LOGI("SPI", "SPI setup successful");
    }
}

int8_t read_register(uint8_t adress)
{
    uint8_t send_buf[2] = {SPI_READ_MASK | adress, 0x00};
    uint8_t recv_buf[2] = {0};

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));  // Zero out the transaction
    t.length = 16;             // Total bits to transfer
    t.tx_buffer = send_buf;    // Transmit buffer
    t.rx_buffer = recv_buf;    // Receive buffer
    t.flags = 0;

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to transmit: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPI", "Received: 0x%02X", recv_buf[1]);
    }

    return recv_buf[1];
}

bool write_register(uint8_t address, uint8_t value)
{
    uint8_t tx_data[2];
    tx_data[0] = SPI_WRITE_MASK & address;  // Clear MSB to indicate a write operation
    tx_data[1] = value;

    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(spi_transaction_t));  // Zero out the transaction struct
    transaction.length = 16;
    transaction.tx_buffer = tx_data;
    transaction.rx_buffer = NULL;

    esp_err_t result = spi_device_transmit(spi_handle, &transaction);
    if (result != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to write to sensor: %s", esp_err_to_name(result));
        return false;
    }
    else
    {
        ESP_LOGI("SPI", "Data 0x%X written to register 0x%X successfully", value, address);
        return true;
    }
}

bool select_user_bank(uint8_t user_bank)
{
    uint8_t tx_data[2];
    tx_data[0] = SPI_WRITE_MASK;  // Clear the MSB for write operation
    tx_data[1] = user_bank;

    spi_transaction_t t;
    memset(&t, 0, sizeof(spi_transaction_t));  // Clear transaction structure
    t.length = 16;                             // Command and data, 16 bits total
    t.tx_buffer = tx_data;                     // Pointer to transmit buffer
    t.rx_buffer = NULL;                        // No data expected to return

    esp_err_t ret = spi_device_transmit(spi_handle, &t);  // Perform the transaction
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to select user bank: %s", esp_err_to_name(ret));
        return false;
    } else {
        ESP_LOGI("SPI", "User bank %d selected successfully", user_bank);
        return true;
    }
}

void read_gyro_x(void) {
    uint8_t reg_address_high = 0x2D;  // Register address for ACCEL_XOUT_H
    uint8_t reg_address_low = 0x2E;   // Register address for ACCEL_XOUT_L

    // Prepare the command to read the high byte
    uint8_t tx_data_high[2] = {0x80 | reg_address_high, 0x00};  // Set the read bit (MSB = 1)
    uint8_t rx_data_high[2] = {0};
    spi_transaction_t trans_high = {
        .length = 16,  // Command and data, 16 bits total
        .tx_buffer = tx_data_high,
        .rx_buffer = rx_data_high
    };
    spi_device_transmit(spi_handle, &trans_high);  // Transmit!

    // Prepare the command to read the low byte
    uint8_t tx_data_low[2] = {SPI_READ_MASK | reg_address_low, 0x00};  // Set the read bit (MSB = 1)
    uint8_t rx_data_low[2] = {0};
    spi_transaction_t trans_low = {
        .length = 16,  // Command and data, 16 bits total
        .tx_buffer = tx_data_low,
        .rx_buffer = rx_data_low
    };
    spi_device_transmit(spi_handle, &trans_low);  // Transmit!

    // Combine the high and low bytes
    int16_t accel_x = ((int16_t)rx_data_high[1] << 8) | rx_data_low[1];

    float g_force = accel_x / ACCEL_SENSITIVITY;  // Convert to g-force
    float acceleration = g_force * 9.81;          // Convert g-force to m/s² if needed

    ESP_LOGI("ACCEL", "Acceleration in m/s^2: %.3f m/s^2\n", acceleration);
}