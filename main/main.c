#include "driver/spi_master.h"
#include "esp_log.h"
#include "9DOF2_driver.h"

#include <string.h>  // For memset

#include "freertos/FreeRTOS.h" //Dodano samo za delay
#include "freertos/task.h" //Dodano samo za delay
#include "esp_system.h" //Dodano samo za delay

void app_main() 
{
    spi_setup();

    ESP_LOGI("SPI: ", "Who am I has a value of: %02X", read_register(0));

    if (write_register(PWR_MGMT_REG_ADRESS, WAKE_UP_MASK))
    {
        ESP_LOGI("SPI: ", "Device is no longer in sleep mode");
    }

    while(1) {
        ESP_LOGI("Timer: ", "One second has passed.");
        read_gyro_x();
        vTaskDelay(500 / portTICK_PERIOD_MS); //Dodano samo za delay
    }
}