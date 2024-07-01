#include "driver/spi_master.h"
#include "esp_log.h"
#include "9DOF2_driver.h"

#include <string.h>
#include <stdbool.h>

// For delay
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "Fusion.h"

#define SAMPLE_PERIOD (0.1f) // replace this with actual sample period

void app_main() 
{
    spi_setup();

    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);

    ESP_LOGI("SPI: ", "Who am I has a value of: %02X", read_register(0));

    if (write_register(PWR_MGMT_REG_ADRESS, WAKE_UP_MASK))
    {
        ESP_LOGI("SPI: ", "Device is no longer in sleep mode");
    }

    while(1) {
        float gx = read_gyro(GYRO_XOUT_H);
        float gy = read_gyro(GYRO_YOUT_H);
        float gz = read_gyro(GYRO_ZOUT_H);
        float ax = read_accel(ACCEL_XOUT_H);
        float ay = read_accel(ACCEL_YOUT_H);
        float az = read_accel(ACCEL_ZOUT_H);

        const FusionVector gyroscope = {.axis = {gx, gy, gz}}; // in degrees/s
        const FusionVector accelerometer = {.axis = {ax, ay, az}}; // in g

        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        //ESP_LOGI("Acceleration: ", "x = %.3f  y = %.3f  z = %.3f", ax, ay, az);
        //ESP_LOGI("Gyroscope: ", "x = %.3f  y = %.3f  z = %.3f", gx, gy, gz);
        //ESP_LOGI("Euler angles: ","Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}