#ifndef _9DOF2_DRIVER_H
#define _9DOF2_DRIVER_H

#include "driver/spi_master.h"

// Pin definitions for SPI
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

#define MAX_TRANSFER_SIZE 512
#define SPI_WRITE_MASK 0x7F
#define SPI_READ_MASK 0X80
 
#define USER_BANK_REG_ADDRESS 0X7F
#define PWR_MGMT_REG_ADRESS 0X06
#define WAKE_UP_MASK 0X01
#define SLEEP_MASK 0X41

// Accelerometer result registers
#define ACCEL_XOUT_H 0x2D
#define ACCEL_YOUT_H 0X2F
#define ACCEL_ZOUT_H 0X31

// Gyroscope result registers
#define GYRO_XOUT_H 0X33
#define GYRO_YOUT_H 0X35
#define GYRO_ZOUT_H 0X37

#define ACCEL_SENSITIVITY 16384.0  // Sensitivity setting for the accelerometer (+-2g option)

#define GYRO_SENSITIVITY 131.0  // Sensitivity setting for the gyroscope ()

#define MAG_SENSITIVITY 0.15  // Sensitivity setting for the magnetometer

extern spi_device_handle_t spi_handle;  // Declaration of the SPI device handle

void spi_setup();
uint8_t read_register(uint8_t adress);
bool write_register(uint8_t address, uint8_t value);
bool select_user_bank(uint8_t user_bank);
float read_accel(uint8_t accel_out_adress);
float read_gyro(uint8_t gyro_out_adress);

#endif