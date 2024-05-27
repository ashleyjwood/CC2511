/**************************************************************
Lab 5

**************************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include <stdio.h>

#define CONTENT_OF(addr) (*(volatile uint32_t *)addr)
#define PAD_CONTROL_BASE 0x4001c000
#define REG_PAD_CONTROL_GPIO15 (PAD_CONTROL_BASE + 0x40)
#define CTRL_PDE 0x4
#define CTRL_PUE 0x8

#define RED_LED   11
#define GREEN_LED 12
#define BLUE_LED  13
#define STEP      25    // Steps up from 0% to 100% in 10% increments
#define MAX_LEVEL 250   // Set to 250 instead 255 for simplicity in design and output
#define MIN_LEVEL 0
#define INPUT_PIN 15

int main(void)
{
  stdio_init_all();

  gpio_set_function(RED_LED, GPIO_FUNC_PWM);
  gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);
  // gpio_set_function(BLUE_LED, GPIO_FUNC_PWM);

  uint slice_num_RED = pwm_gpio_to_slice_num(RED_LED);
  uint slice_num_GREEN = pwm_gpio_to_slice_num(GREEN_LED);
  // uint slice_num_BLUE = pwm_gpio_to_slice_num(BLUE_LED);

  int RED_LEVEL = 0;
  int GREEN_LEVEL = 0;
  // int BLUE_LEVEL = 0;

  pwm_set_enabled(slice_num_RED, true);
  pwm_set_enabled(slice_num_GREEN, true);
  // pwm_set_enabled(slice_num_BLUE, true);

  pwm_set_gpio_level(RED_LED, RED_LEVEL);
  pwm_set_gpio_level(GREEN_LED, GREEN_LEVEL);
  // pwm_set_gpio_level(BLUE_LED, BLUE_LEVEL);

  int ch;

  // Configure internal pull-up and pull-down resistors for GPIO15
  // CONTENT_OF(REG_PAD_CONTROL_GPIO15) &= ~(CTRL_PUE | CTRL_PDE);

  // GPIO pin for digital input

  // int input_pin = 15;
  gpio_init(INPUT_PIN);
  gpio_set_dir(INPUT_PIN, GPIO_IN);

  gpio_init(BLUE_LED);
  gpio_set_dir(BLUE_LED, GPIO_OUT);
  // int c;

  while (true)
  {
    // printf("Connected\r\n");
    sleep_ms(100);
    ch = getchar_timeout_us(0);
    // sleep_ms(50);
    if (ch != PICO_ERROR_TIMEOUT)
    {
      // Adjust brightness levels based on keypress
      switch (ch)
      {
      // 'r' key is pressed
      case 'r':
        /* Set the new LEVEL to be the old LEVEL + STEP unless 
        it is greater than or equal to the max level if it is 
        more, then set to the max level. */
        RED_LEVEL = (RED_LEVEL + STEP <= MAX_LEVEL) ? RED_LEVEL + STEP : MAX_LEVEL;
        printf("Red Increase %d \r\n", RED_LEVEL);
        break;
              // 'R' key is pressed
      case 'R':
        /* Set the new LEVEL to be the old LEVEL - STEP unless 
        it is less than or equal to the min level if it is more, 
        then set to the min level. */
        RED_LEVEL = (RED_LEVEL - STEP >= MIN_LEVEL) ? RED_LEVEL - STEP : MIN_LEVEL;
        printf("Red Decrease %d \r\n", RED_LEVEL);
        break;
      case 'g':
        GREEN_LEVEL = (GREEN_LEVEL + STEP <= MAX_LEVEL) ? GREEN_LEVEL + STEP : MAX_LEVEL;
        printf("Green Increase %d \r\n", GREEN_LEVEL);
        break;
      case 'G':
        GREEN_LEVEL = (GREEN_LEVEL - STEP >= MIN_LEVEL) ? GREEN_LEVEL - STEP : MIN_LEVEL;
        printf("Green Increase %d \r\n", GREEN_LEVEL);
        break;
      // case 'b':
      //   BLUE_LEVEL = (BLUE_LEVEL + STEP <= MAX_LEVEL) ? BLUE_LEVEL + STEP : MAX_LEVEL;
      //   break;
      // case 'B':
      //   BLUE_LEVEL = (BLUE_LEVEL - STEP >= MIN_LEVEL) ? BLUE_LEVEL - STEP : MIN_LEVEL;
      //   break;
      case 'u': //
        CONTENT_OF(REG_PAD_CONTROL_GPIO15) |= CTRL_PUE;
        CONTENT_OF(REG_PAD_CONTROL_GPIO15) &= ~CTRL_PDE;
        printf("Pullup on, pulldown off\r\n");
        break;

      case 'd': //
        CONTENT_OF(REG_PAD_CONTROL_GPIO15) &= ~CTRL_PUE;
        CONTENT_OF(REG_PAD_CONTROL_GPIO15) |= CTRL_PDE;
        printf("Pullup off, pulldown on\r\n");
        break;

      case 'o': //
        CONTENT_OF(REG_PAD_CONTROL_GPIO15) &= ~(CTRL_PUE | CTRL_PDE);
        printf("Pullup off, pulldown off\r\n");
        break;
      default:
        break;
      }
      // Read the digital input from the chosen pin

      // Control the blue LED based on the input state
      // if (input_state)
      // {
      //   BLUE_LEVEL = MAX_LEVEL;
      // //   GREEN_LEVEL = MAX_LEVEL;
      // //   RED_LEVEL = MAX_LEVEL;
      // }
      // // else
      // {
      //   BLUE_LEVEL = MIN_LEVEL;
      // //   GREEN_LEVEL = MIN_LEVEL;
      // //   RED_LEVEL = MIN_LEVEL;
      // }

      // // Set the LED brightness
      pwm_set_gpio_level(RED_LED, RED_LEVEL * RED_LEVEL);
      pwm_set_gpio_level(GREEN_LED, GREEN_LEVEL * GREEN_LEVEL);
      // pwm_set_gpio_level(BLUE_LED, BLUE_LEVEL * BLUE_LEVEL);

      {

        // Delay
        sleep_ms(100);
      }
    }
    bool input_state = gpio_get(INPUT_PIN);

    gpio_put(BLUE_LED, gpio_get(input_state));
  }
}