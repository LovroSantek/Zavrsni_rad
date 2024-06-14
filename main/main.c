#include "driver/spi_master.h"
#include "esp_log.h"
#include "9DOF2_driver.h"

#include <string.h>  // For memset()

// For delay
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "MadgwickAHRS.h" // Data processing

#include "ble_init.h"
#include "ble_server.h"

void app_main() 
{
    spi_setup();
    bluetooth_init();
    bluetooth_server_init();

    ESP_LOGI("SPI: ", "Who am I has a value of: %02X", read_register(0));

    if (write_register(PWR_MGMT_REG_ADRESS, WAKE_UP_MASK))
    {
        ESP_LOGI("SPI: ", "Device is no longer in sleep mode");
    }

    //float roll = 0.f;
    //float pitch = 0.f;
    //float yaw = 0.f;

    while(1) {
        float gx = read_gyro(GYRO_XOUT_H);
        float gy = read_gyro(GYRO_YOUT_H);
        float gz = read_gyro(GYRO_ZOUT_H);
        float ax = read_accel(ACCEL_XOUT_H);
        float ay = read_accel(ACCEL_YOUT_H);
        float az = read_accel(ACCEL_ZOUT_H);

        MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az);
        //getAngles(&roll, &pitch, &yaw);
        //ESP_LOGI("Angles: ", "x = %.3f  y = %.3f  z = %.3f", roll, pitch, yaw);

        //ESP_LOGI("Acceleration: ", "x = %.3f  y = %.3f  z = %.3f", ax, ay, az);
        //ESP_LOGI("Gyroscope: ", "x = %.3f  y = %.3f  z = %.3f", gx, gy, gz);

        vTaskDelay(800 / portTICK_PERIOD_MS);  // Dodano samo za delay
        //getAngles(&roll, &pitch, &yaw);
        //ESP_LOGI("Angles: ", "x = %.3f  y = %.3f  z = %.3f", roll, pitch, yaw);
        ESP_LOGI("Acceleration: ", "x = %.3f  y = %.3f  z = %.3f", ax, ay, az);
    }
}