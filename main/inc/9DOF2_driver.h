#ifndef _9DOF2_DRIVER_H
#define _9DOF2_DRIVER_H

#include "driver/spi_master.h"

// Pin definitions for SPI
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

// SPI register addresses
#define USER_BANK_REG_ADDRESS 0X7F

// Sensitivity settings for the accelerometer
#define ACCEL_SENSITIVITY 16384.0

// Declaration of the SPI device handle (defined in the corresponding .c file)
extern spi_device_handle_t spi_handle;

// Function prototypes for SPI-related operations
esp_err_t spi_setup(void);
void read_from_sensor(void);
void select_user_bank(uint8_t user_bank);
void read_gyro_x(void);
void write_to_sensor(uint8_t address, uint8_t value);

#endif // SENSOR_SPI_H
