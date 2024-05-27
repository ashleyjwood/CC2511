/**************************************************************
Lab 3
Turning on and off the RGB light using the keyboard
**************************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>

#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13

// TODO - Initialise components and variables
int main(void)
{
  stdio_init_all();

  // Initialise all LED pins
  gpio_init(RED_LED);
  gpio_init(GREEN_LED);
  gpio_init(BLUE_LED);

  // Set the direction of the pins
  gpio_set_dir(RED_LED, GPIO_OUT);
  gpio_set_dir(GREEN_LED, GPIO_OUT);
  gpio_set_dir(BLUE_LED, GPIO_OUT);

  while (true)
  {
    // TODO - Repeated code here
    printf("Reading from pico board\r\n");
    sleep_ms(0);
    int keypress = getchar_timeout_us(0);
    if (keypress != PICO_ERROR_TIMEOUT)
    {

      switch (keypress)
      {
      // Red switch
      case 'r':
        // Put the state of the RED_LED to be not the current state
        gpio_put(RED_LED, !gpio_get(RED_LED));
        printf("RED PRESSED\r\n");
        break;

      // Green switch
      case 'g':
        gpio_put(GREEN_LED, !gpio_get(GREEN_LED));
        printf("GREEN PRESSED\r\n");
        break;

      // Blue switch
      case 'b':
        gpio_put(BLUE_LED, !gpio_get(BLUE_LED));
        printf("BLUE PRESSED\r\n");
        break;

      // Turn LED off
      case 'o':
        // Put the state off the LEDs to be 0 (off)
        gpio_put(RED_LED, 0);
        gpio_put(GREEN_LED, 0);
        gpio_put(BLUE_LED, 0);
        printf("LED TURNED OFF\r\n");
        break;

      // White LED on boot
      default:
        // Put the state off the LEDs to be 1 (on)
        gpio_put(RED_LED, 1);
        gpio_put(GREEN_LED, 1);
        gpio_put(BLUE_LED, 1);
        printf("LED SET TO WHITE\r\n");
        break;
      }
    }
  }
  return false;
}