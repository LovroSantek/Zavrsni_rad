#include "driver/spi_master.h"
#include "esp_log.h"
#include "9DOF2_driver.h"

#include <string.h>  // For memset

#include "freertos/FreeRTOS.h" //Dodano samo za delay
#include "freertos/task.h" //Dodano samo za delay
#include "esp_system.h" //Dodano samo za delay

void app_main() 
{
    esp_err_t spi_setup_err = spi_setup();
    if (spi_setup_err != ESP_OK)
    {
        ESP_LOGE("SPI", "Failed to setup SPI: %s", esp_err_to_name(spi_setup_err));
    }
    else
    {
        ESP_LOGI("SPI", "SPI setup successful");
    }
    
    const char* TAG = "Timer"; // Dodano samo za delay

    write_to_sensor(0X06, 0X01);

    while(1) {
        ESP_LOGI(TAG, "One second has passed.");
        read_gyro_x();
        vTaskDelay(500 / portTICK_PERIOD_MS); //Dodano samo za delay
    }
}