#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ui/animation/Transition.h"
#include "ui/ui.h"
#include "encoder.h" 
#include "api/handlers.h"

static const char *TAG = "DUNA";

#define PIN_CLK   18
#define PIN_MOSI  23
#define PIN_CS       5
#define PIN_DC       2
#define PIN_RST      4

#define ENCODER_A_PIN 33
#define ENCODER_B_PIN 27
#define ENCODER_KEY_PIN 25

void app_main(void)
{
    // init
    rotary_encoder_config_t config = ROTARY_ENCODER_DEFAULT_CONFIG();

    config.pin_a = (gpio_num_t)ENCODER_A_PIN;
    config.pin_b = (gpio_num_t)ENCODER_B_PIN;
    config.pin_btn = (gpio_num_t)ENCODER_KEY_PIN;
    config.btn_pressed_level = 0; 
    config.enable_internal_pullup = true;
    config.callback = encoder_event_handler;
    config.callback_ctx = NULL;


    u8g2_esp32_hal_t hal = U8G2_ESP32_HAL_DEFAULT;
    hal.clk   = PIN_CLK;
    hal.mosi  = PIN_MOSI;
    hal.cs    = PIN_CS;
    hal.dc    = PIN_DC;
    hal.reset = PIN_RST;
    u8g2_esp32_hal_init(hal);

    static u8g2_t u8g2;
    u8g2_Setup_ssd1306_128x64_noname_f(
        &u8g2,
        U8G2_R0,
        u8g2_esp32_spi_byte_cb,
        u8g2_esp32_gpio_and_delay_cb
    );

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    // welcome
    draw_welcome(&u8g2);

    // APIs
    rotary_encoder_handle_t encoder_handle = NULL;
    ESP_ERROR_CHECK(rotary_encoder_create(&config, &encoder_handle));
    // xTaskCreate(encoder_states_update, "encoder_update", 1024, NULL, 5, NULL);

    vTaskDelay(1650 / portTICK_PERIOD_MS);

    while (1) {
        ui_update(&u8g2);

        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
    
}