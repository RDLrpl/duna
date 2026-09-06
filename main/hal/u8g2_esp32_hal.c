#include "u8g2_esp32_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "u8g2_hal";
static spi_device_handle_t handle_spi;
static u8g2_esp32_hal_t hal;

void u8g2_esp32_hal_init(u8g2_esp32_hal_t hal_param)
{
    hal = hal_param;
}

uint8_t u8g2_esp32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_BYTE_SET_DC:
        gpio_set_level(hal.dc, arg_int);
        break;

    case U8X8_MSG_BYTE_INIT: {
        spi_bus_config_t bus_config = {
            .sclk_io_num = hal.clk,
            .mosi_io_num = hal.mosi,
            .miso_io_num = -1,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));

        spi_device_interface_config_t dev_config = {
            .clock_speed_hz = 10 * 1000 * 1000,
            .spics_io_num = hal.cs,
            .queue_size = 1,
            .mode = 0,
        };
        ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &handle_spi));
        break;
    }

    case U8X8_MSG_BYTE_SEND: {
        spi_transaction_t trans = {0};
        trans.tx_buffer = arg_ptr;
        trans.length = arg_int * 8;
        ESP_ERROR_CHECK(spi_device_polling_transmit(handle_spi, &trans));
        break;
    }
    }
    return 0;
}

uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: {
        uint64_t mask = 0;
        if (hal.dc != U8G2_ESP32_HAL_UNDEFINED) mask |= (1ULL << hal.dc);
        if (hal.reset != U8G2_ESP32_HAL_UNDEFINED) mask |= (1ULL << hal.reset);
        gpio_config_t io_conf = {
            .pin_bit_mask = mask,
            .mode = GPIO_MODE_OUTPUT,
        };
        gpio_config(&io_conf);
        break;
    }
    case U8X8_MSG_DELAY_MILLI:
        vTaskDelay(pdMS_TO_TICKS(arg_int));
        break;
    case U8X8_MSG_GPIO_DC:
        if (hal.dc != U8G2_ESP32_HAL_UNDEFINED)
            gpio_set_level(hal.dc, arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        if (hal.reset != U8G2_ESP32_HAL_UNDEFINED)
            gpio_set_level(hal.reset, arg_int);
        break;
    default:
        return 0;
    }
    return 1;
}