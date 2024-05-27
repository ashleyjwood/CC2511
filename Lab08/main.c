#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include <stdio.h>

#define LDR 26        // GPIO 26
#define RED_LED 11    // GPIO 11
#define GREEN_LED 12  // GPIO 12
#define BLUE_LED 13   // GPIO 13
#define MAX_PWM 3000  // Upper bounds for light level received
#define MIN_PWM 100   // Lower bounds for light level received
#define VOLTAGE_CONV_FACTOR 3.3f / MAX_PWM
#define PWM_CONV_FACTOR 255.0f / (MAX_PWM - MIN_PWM)

int main(void)
{
  uint16_t ADC;
  stdio_init_all();

  // Configure the PWM for the LEDs
  uint slice_num_RED = pwm_gpio_to_slice_num(RED_LED);
  gpio_set_function(RED_LED, GPIO_FUNC_PWM);

  // Enable the PWM slice for the Red LED
  pwm_set_enabled(slice_num_RED, true);

  // Initialise the ADC
  adc_init();
  adc_gpio_init(LDR);
  adc_select_input(0);

  while (true)
  {
    // Read the ADC value from the LDR
    ADC = adc_read();

    // Clamp ADC value to the specified range
    if (ADC < MIN_PWM)
    {
      ADC = MIN_PWM;
    }
    else if (ADC > MAX_PWM)
    {
      ADC = MAX_PWM;
    }

    // ADC value to LED intensity
    uint8_t pwm_value = (uint8_t)((ADC - MIN_PWM) * PWM_CONV_FACTOR);
    uint8_t brightness_percent = (pwm_value / 255.0f) * 100;

    // Set the PWM level for RED_LED
    pwm_set_gpio_level(RED_LED, pwm_value * pwm_value);

    // Print ADC value, voltage, brightness%
    printf("ADC value: %hi, Voltage: %.2lf V, PWM Value: %i, Brightness: %i%%\r\n", ADC, ADC * VOLTAGE_CONV_FACTOR, pwm_value, brightness_percent);

    sleep_ms(100); // Add a delay for stability
  }
}
