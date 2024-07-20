#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

const uint8_t PIN_LED = 15;

const uint8_t PIN_SCL = 27;
const uint8_t PIN_SDA = 26;

const uint8_t PIN_ADC = 28;
const uint16_t RANGE_MAX = 1400;
const uint16_t RANGE_MIN = 3200;

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

    printf("sending reset\n");

    printf("done\n");
}

float map_within(uint16_t min_value, uint16_t max_value, uint16_t value) {
    float range = max_value - min_value;
    float result = (float)(value-min_value) / range;
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

    while (true) {
        uint16_t value = adc_read();
        float mapped = map_within(RANGE_MIN, RANGE_MAX, value);
        printf("%.0f %% humidity\n", mapped);
        sleep_ms(500);
    }
}


