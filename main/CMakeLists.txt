idf_component_register(
    SRCS 
        "main.c" 
        "hal/u8g2_esp32_hal.c"
        "ui/animation/Transition.c"
        "ui/ui.c"
        "api/handlers.c"
    INCLUDE_DIRS 
        "." 
        "hal"
        "ui"
        "api"
        "ui/animation"
    PRIV_REQUIRES driver esp_driver_gpio esp_driver_spi
    REQUIRES u8g2 freertos spi_flash esp_partition app_update
)
