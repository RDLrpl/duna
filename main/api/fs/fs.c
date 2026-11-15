#include "config.h"
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

static bool sd_card_mounted = false;
static bool spi_bus_initialized = false;
esp_err_t ret;
sdmmc_card_t *card;

static const char *TAG = "SDCARD";

static esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = true,
    .max_files = 4,
    .allocation_unit_size = 16 * 1024
};

static sdmmc_host_t host = SDSPI_HOST_DEFAULT();
static sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();

bool* is_mounted(void) {
    return &sd_card_mounted;
}

void init_sd(void) {
    if (spi_bus_initialized) {
        slot_config.gpio_cs = PIN_SD_CS;
        slot_config.host_id = host.slot;
        return;
    }

    host.slot = SPI3_HOST; 

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        return;
    }

    spi_bus_initialized = true;

    slot_config.gpio_cs = PIN_SD_CS;
    slot_config.host_id = host.slot;
}

void mount_sd(void) {
    if (sd_card_mounted) {
        return;
    }

    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Mount Failed: 0x000");
        } else {
            ESP_LOGE(TAG, "Mount Failed: %s", esp_err_to_name(ret));
        }
        return;
    }

    sd_card_mounted = true;
}

void unmount_sd(void) {
    if (!sd_card_mounted) {
        return;
    }

    ret = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    if (ret == ESP_OK) {
        sd_card_mounted = false;
        ESP_LOGI(TAG, "Card unmounted, VFS memory freed.");
    } else {
        ESP_LOGE(TAG, "Failed to unmount card: %s", esp_err_to_name(ret));
    }
}