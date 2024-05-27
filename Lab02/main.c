/**************************************************************
Lab 2
Blink infinite blink code for the onboard LED
**************************************************************/

#include "pico/stdlib.h"
#include <string.h>

#define LED_PIN 25

int main(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Loop for ever
    while (true)
    {
        gpio_put(LED_PIN, 1); // Switch on the LED
        sleep_ms(250);        // Wait for a while
        gpio_put(LED_PIN, 0); // Switch off the LED
        sleep_ms(250);        // Wait for a while
    }
}