#ifndef _9DOF2_DRIVER_H
#define _9DOF2_DRIVER_H

#include "driver/spi_master.h"

// Pin definitions for SPI
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

#define MAX_TRANSFER_SIZE 512  // Max transfer bit size
#define SPI_WRITE_MASK 0x7F
#define SPI_READ_MASK 0X80

#define USER_BANK_REG_ADDRESS 0X7F
#define PWR_MGMT_REG_ADRESS 0X06
#define WAKE_UP_MASK 0X01
#define SLEEP_MASK 0X41

#define ACCEL_SENSITIVITY 16384.0  // Sensitivity settings for the accelerometer

extern spi_device_handle_t spi_handle;  // Declaration of the SPI device handle

void spi_setup();
int8_t read_register(uint8_t adress);
bool write_register(uint8_t address, uint8_t value);
bool select_user_bank(uint8_t user_bank);

void read_gyro_x();

#endif