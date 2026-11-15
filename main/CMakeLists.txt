idf_component_register(
    SRCS 
        "main.c" 
        "api/fs/fs.c"
        "api/GraphicalApi/ga.c"
        "api/GraphicalApi/toolbar.c"
        "api/GraphicalApi/render.c"
        "api/handlers/handlers.c"
        "api/timemod/rtctime.c"
        "api/wifi/wifidrive.c"

        "ui/animation/Transition.c"

        "ui/gapps/mainmenu.c"
        "ui/gapps/control.c"
        "ui/gapps/settings/settings.c"
        "ui/gapps/settings/submenu/timesettings.c"

    INCLUDE_DIRS 
        "." 
        "api"
        "api/fs"
        "api/GraphicalApi"
        "api/handlers"
        "api/timemod"
        "api/wifi"
        "ui/animation"
        "ui/gapps"
        "ui/gapps/settings/submenu"
        "ui/gapps/settings"
        "ui"
    PRIV_REQUIRES driver esp_driver_gpio esp_driver_spi esp_lcd
    REQUIRES lvgl nvs_flash esp_wifi esp_adc esp_timer esp_driver_i2c esp_lvgl_port freertos spi_flash esp_partition app_update fatfs vfs ds1302
)
