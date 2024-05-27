/**************************************************************
 * main.c
 * rev 1.0 15-Aug-2023 yelps
 * Lab3
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>

#define PICO_LED 25
#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13

#define RED_SWITCH 2
#define GREEN_SWITCH 3
#define BLUE_SWITCH 4

int main(void)
{
  stdio_init_all();

  // Initialise GPIO pins
  gpio_init(PICO_LED);
  gpio_init(RED_LED);
  gpio_init(GREEN_LED);
  gpio_init(BLUE_LED);

  gpio_init(RED_SWITCH);
  gpio_init(GREEN_SWITCH);
  gpio_init(BLUE_SWITCH);

  // Set direction of pins
  gpio_set_dir(PICO_LED, GPIO_OUT);
  gpio_set_dir(RED_LED, GPIO_OUT);
  gpio_set_dir(GREEN_LED, GPIO_OUT);
  gpio_set_dir(BLUE_LED, GPIO_OUT);

  gpio_set_dir(RED_SWITCH, GPIO_IN);
  gpio_set_dir(GREEN_SWITCH, GPIO_IN);
  gpio_set_dir(BLUE_SWITCH, GPIO_IN);

  while (true)
  {
    gpio_put(RED_LED, !gpio_get(RED_SWITCH));
    gpio_put(GREEN_LED, !gpio_get(GREEN_SWITCH));
    gpio_put(BLUE_LED, !gpio_get(BLUE_SWITCH));

    while (!gpio_get(RED_SWITCH) && !gpio_get(GREEN_SWITCH) && !gpio_get(BLUE_SWITCH))
    {
      // Read the state of switches
      gpio_put(RED_LED, !gpio_get(RED_SWITCH));
      gpio_put(GREEN_LED, !gpio_get(GREEN_SWITCH));
      gpio_put(BLUE_LED, !gpio_get(BLUE_SWITCH));

      // Turn the PICO_LED on
      gpio_put(PICO_LED, 1);
    }

    // Turn the PICO_LED off when any of the switches are released
    gpio_put(PICO_LED, 0);
  }

  return 0;
}