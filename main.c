#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "rust/bindings.h"

const uint8_t PIN_LED_RED = 16;
const uint8_t PIN_LED_YELLOW = 17;
const uint8_t PIN_LED_GREEN = 18;
const uint8_t PIN_LED_BLUE = 19;

const uint8_t PIN_ADC = 28;
const uint16_t RANGE_MAX = 1400;
const uint16_t RANGE_MIN = 3200;

volatile bool repeating_timer_set = false;
volatile uint8_t active_pin = 16;
volatile bool led_state = false;

const uint PIN_MASK = (1 << PIN_LED_RED) | (1 << PIN_LED_BLUE) |
                      (1 << PIN_LED_GREEN) | (1 << PIN_LED_YELLOW);

bool repeating_toogle_led() {
    led_state = !led_state;
    gpio_put(active_pin, led_state);
    return true;
}

struct repeating_timer *setup_led() {
    printf("initializing led\n");
    gpio_set_dir(PIN_LED_RED, true);
    gpio_put(PIN_LED_RED, true);

    struct repeating_timer *led_timer =
        calloc(1, sizeof(struct repeating_timer));
    add_repeating_timer_ms(500, repeating_toogle_led, NULL, led_timer);
    printf("done\n");
    return led_timer;
}

float map_within(uint16_t min_value, uint16_t max_value, uint16_t value) {
    float range = max_value - min_value;
    float result = (float)(value - min_value) / range;
    return result * 100.0f;
}

void turn_leds_off() {
    gpio_put_masked(PIN_MASK, false);
    printf("all leds off\n");
}

int main() {
    stdio_init_all();

    sleep_ms(2000);

    printf("initializing\n");

    gpio_init_mask(PIN_MASK);
    gpio_set_dir_out_masked(PIN_MASK);
    gpio_put(PIN_LED_BLUE, true);

    struct repeating_timer *led_timer =
        calloc(1, sizeof(struct repeating_timer));

    adc_init();

    adc_gpio_init(PIN_ADC);
    adc_select_input(2);

    while (true) {
        printf("reading\n");
        uint16_t value = adc_read();
        float mapped = map_within(RANGE_MIN, RANGE_MAX, value);

        printf("analyzing\n");
        uint16_t status = analyze(mapped);
        printf("status: %d\n", status);

        bool cancelled = cancel_repeating_timer(led_timer);
        turn_leds_off();
        printf("%.0f %% humidity, status %d\n", mapped, status);

        switch (status) {
        case 0:
            active_pin = PIN_LED_RED;
            add_repeating_timer_ms(500, repeating_toogle_led, NULL, led_timer);
            break;
        case 1:
            gpio_put(PIN_LED_RED, true);
            break;
        case 2:
            active_pin = PIN_LED_YELLOW;
            add_repeating_timer_ms(500, repeating_toogle_led, NULL, led_timer);
            break;
        case 3:
            gpio_put(PIN_LED_YELLOW, true);
            break;
        case 4:
            gpio_put(PIN_LED_GREEN, true);
            break;
        case 5:
            gpio_put(PIN_LED_BLUE, true);
            break;
        }

        sleep_ms(1000);
    }
}
