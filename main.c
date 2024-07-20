#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "ssd1306_i2c.h"

const uint8_t PIN_LED = 15;

const uint8_t PIN_SCL = 27;
const uint8_t PIN_SDA = 26;

const uint8_t PIN_ADC = 28;
const uint16_t RANGE_MAX = 1400;
const uint16_t RANGE_MIN = 3200;

#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128
#define SSD1306_PAGE_HEIGHT _u(8)
#define SSD1306_NUM_PAGES (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)

volatile bool led_state = false;

static int addr = 0x68;

bool repeating_toogle_led() {
    led_state = !led_state;
    gpio_put(PIN_LED, led_state);
    return true;
}

struct repeating_timer *setup_led() {
    printf("initializing led\n");
    gpio_set_dir(PIN_LED, true);
    gpio_put(PIN_LED, true);

    struct repeating_timer *led_timer =
        calloc(1, sizeof(struct repeating_timer));
    add_repeating_timer_ms(500, repeating_toogle_led, NULL, led_timer);
    printf("done\n");
    return led_timer;
}

void setup_i2c() {
    printf("initializing i2c\n");
    i2c_init(i2c1, 400 * 1000);

    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    printf("done init\n");
}

float map_within(uint16_t min_value, uint16_t max_value, uint16_t value) {
    float range = max_value - min_value;
    float result = (float)(value - min_value) / range;
    return result * 100.0f;
}

int main() {
    stdio_init_all();

    sleep_ms(2000);

    printf("initializing\n");

    const uint PIN_MASK = (1 << PIN_LED) | (1 << PIN_SCL) | (1 << PIN_SDA);
    gpio_init_mask(PIN_MASK);

    struct repeating_timer *led_timer = setup_led();

    adc_init();

    adc_gpio_init(PIN_ADC);
    adc_select_input(2);

    setup_i2c();

    SSD1306_init();

    RenderArea frame_area = {
        start_col : 0,
        end_col : SSD1306_WIDTH - 1,
        start_page : 0,
        end_page : SSD1306_NUM_PAGES - 1
    };

    calc_render_area_buflen(&frame_area);

    uint8_t buf[SSD1306_BUF_LEN];

    while (true) {
        uint16_t value = adc_read();
        float mapped = map_within(RANGE_MIN, RANGE_MAX, value);
        printf("%.0f %% humidity\n", mapped);

        for (int i = 0; i < 3; i++) {
            SSD1306_send_cmd(SSD1306_SET_ALL_ON); // Set all pixels on
            sleep_ms(500);
            SSD1306_send_cmd(SSD1306_SET_ENTIRE_ON
            ); // go back to following RAM for pixel state
            sleep_ms(500);
        }
        sleep_ms(1000);
    }
}
