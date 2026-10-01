idf_component_register(
    SRCS 
        "main.c" 
        "api/fs/fs.c"
        "api/GraphicalApi/ga.c"
        "api/GraphicalApi/toolbar.c"
        "api/handlers/handlers.c"

        "ui/animation/Transition.c"
        "ui/gapps/util.c"
        "ui/gapps/gapps.c"
        "driver.c"

        
    INCLUDE_DIRS 
        "." 
        "api"
        "api/fs"
        "api/GraphicalApi"
        "api/handlers"
        "ui/animation"
        "ui/gapps"
        "ui"
    PRIV_REQUIRES driver esp_driver_gpio esp_driver_spi esp_lcd
    REQUIRES lvgl esp_adc esp_driver_i2c esp_lvgl_port freertos spi_flash esp_partition app_update fatfs vfs
)
