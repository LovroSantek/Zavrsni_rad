#include "driver/spi_master.h"
#include "esp_log.h"
#include "9DOF2_driver.h"

#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "Fusion.h"

#include "WIFI.h"
#include "httpServer.h"

#define SAMPLE_PERIOD (0.01f)

float global_yaw = 0.0, global_pitch = 0.0, global_roll = 0.0;

void app_main() 
{
    spi_setup();
    wifi_init();
    http_server_start();

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

        const FusionVector gyro = {.axis = {gx, gy, gz}};
        const FusionVector accel = {.axis = {ax, ay, az}};

        FusionAhrsUpdateNoMagnetometer(&ahrs, gyro, accel, SAMPLE_PERIOD);
        const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

        global_yaw = euler.angle.yaw;
        global_pitch = euler.angle.pitch;
        global_roll = euler.angle.roll;

        //ESP_LOGI("Acceleration: ", "x = %.3f  y = %.3f  z = %.3f", ax, ay, az);
        //ESP_LOGI("Gyroscope: ", "x = %.3f  y = %.3f  z = %.3f", gx, gy, gz);
        //ESP_LOGI("Euler angles: ","Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);

        vTaskDelay((int) (1000 * SAMPLE_PERIOD) / portTICK_PERIOD_MS);
    }
}