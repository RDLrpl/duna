#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "handlers.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/gpio.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct {
    int center;
    int min_mv;
    int max_mv;
} AxisCal;

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t cali_handle = NULL;
static bool do_calibration = false;

static AxisCal cal_x = { JY_CENTER_DEFAULT_MV, 0, JY_ADC_VREF_MV };
static AxisCal cal_y = { JY_CENTER_DEFAULT_MV, 0, JY_ADC_VREF_MV };

static PadHandler duna_pad_handler = {
    .deltax = 0,
    .deltay = 0,
    .long_clk = false,
    .clk = false
};

static inline uint32_t millis(void) {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

static int read_stable_mv(adc_channel_t channel) {
    long sum = 0;
    int count = 0;

    for (int i = 0; i < FILTER_SAMPLES; i++) {
        int raw_val = 0;
        if (adc_oneshot_read(adc1_handle, channel, &raw_val) != ESP_OK) {
            continue;
        }
        int mv = 0;
        if (do_calibration) {
            if (adc_cali_raw_to_voltage(cali_handle, raw_val, &mv) != ESP_OK) {
                continue;
            }
        } else {
            mv = (raw_val * JY_ADC_VREF_MV) / JY_ADC_MAX_RAW;
        }
        sum += mv;
        count++;
    }
    if (count == 0) return -1;
    return (int)(sum / count);
}

static void init_axis(AxisCal *a, int center) {
    a->center = center;
    a->min_mv = center - JY_INIT_RANGE_NEG_MV;
    a->max_mv = center + JY_INIT_RANGE_POS_MV;
    if (a->min_mv < 0) a->min_mv = 0;
}

void init_joystick(void) {
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, PIN_JY_XPOS, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, PIN_JY_YPOS, &config));

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    do_calibration = (adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle) == ESP_OK);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_JY_KEY),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    long sum_x = 0, sum_y = 0;
    int n = 0;
    for (int i = 0; i < JY_CENTER_SAMPLES; i++) {
        int x = read_stable_mv(PIN_JY_XPOS);
        int y = read_stable_mv(PIN_JY_YPOS);
        if (x >= 0 && y >= 0) {
            sum_x += x;
            sum_y += y;
            n++;
        }
        vTaskDelay(pdMS_TO_TICKS(JY_CENTER_DELAY_MS));
    }

    int cx = JY_CENTER_DEFAULT_MV;
    int cy = JY_CENTER_DEFAULT_MV;
    if (n > 0) {
        cx = sum_x / n;
        cy = sum_y / n;
    }
    if (abs(cx - JY_CENTER_DEFAULT_MV) > JY_CENTER_MAX_ERR_MV) cx = JY_CENTER_DEFAULT_MV;
    if (abs(cy - JY_CENTER_DEFAULT_MV) > JY_CENTER_MAX_ERR_MV) cy = JY_CENTER_DEFAULT_MV;

    init_axis(&cal_x, cx);
    init_axis(&cal_y, cy);
}

int apply_deadzone(int value, int threshold) {
    int a = abs(value);
    if (a <= threshold) return 0;
    int out = ((a - threshold) * 100) / (100 - threshold);
    if (out > 100) out = 100;
    return (value > 0) ? out : -out;
}

static int process_axis(AxisCal *a, int mv) {
    if (mv > a->max_mv) a->max_mv = mv;
    if (mv < a->min_mv) a->min_mv = mv;

    int delta = mv - a->center;
    int span = (delta >= 0) ? (a->max_mv - a->center) : (a->center - a->min_mv);
    if (span < JY_MIN_VALID_SPAN_MV) span = JY_MIN_VALID_SPAN_MV;

    int norm = (delta * 100) / span;
    norm = (norm * 100) / (100 - JY_OUTER_ZONE_PCT);
    if (norm > 100) norm = 100;
    if (norm < -100) norm = -100;

    return apply_deadzone(norm, JY_DEADZONE_PCT);
}

void handle_joystick(void) {
    int mv_x = read_stable_mv(PIN_JY_XPOS);
    int mv_y = read_stable_mv(PIN_JY_YPOS);

    if (mv_x >= 0 && mv_y >= 0) {
        duna_pad_handler.deltax = process_axis(&cal_x, mv_x);
        duna_pad_handler.deltay = -process_axis(&cal_y, mv_y);
    } else {
        duna_pad_handler.deltax = 0;
        duna_pad_handler.deltay = 0;
    }

    duna_pad_handler.clk = false;
    duna_pad_handler.long_clk = false;

    static int stable_state = 1;
    static int last_raw = 1;
    static uint32_t last_change_time = 0;
    static uint32_t press_start_time = 0;
    static bool long_click_done = false;

    int raw = gpio_get_level(PIN_JY_KEY);
    uint32_t now = millis();

    if (raw != last_raw) {
        last_raw = raw;
        last_change_time = now;
    }

    if (raw != stable_state && (now - last_change_time) >= DEBOUNCE_MS) {
        stable_state = raw;
        if (stable_state == 0) {
            press_start_time = now;
            long_click_done = false;
        } else if (!long_click_done) {
            duna_pad_handler.clk = true;
        }
    }

    if (stable_state == 0 && !long_click_done &&
        (now - press_start_time) >= LONG_PRESS_MS) {
        duna_pad_handler.long_clk = true;
        long_click_done = true;
    }
}

PadHandler get_pad_handler(void) {
    return duna_pad_handler;
}