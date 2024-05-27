/******************************************************************************
 * Project Name: CC2511 Assignment 2 - Main
 * Description:  This C program is designed to control a CNC machine through 
 *               UART communication, allowing the user to send instructions for 
 *               moving the machine's tool, setting spindle speed, and more, 
 *               with functions and error handling in place.
 * Authors:      Crabb. L., Wood. A., Van Duren. R.  (Group 88)
 * Date:         27/10/2023
 ******************************************************************************/

// -------------------- DIRECTIVES -------------------- //
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "macros.h"
// -------------------- END OF DIRECTIVES -------------------- //

// -------------------- GLOBAL VARIABLES -------------------- //
// Volatile Flags
volatile bool input_ready = false; // Flag to indicate if new input is ready

// UART Data Buffer
volatile char Buffer[100];              // Buffer to store incoming UART data
volatile unsigned int Buffer_index = 0; // Index to track the buffer

// Home Position and Spindle Control
int8_t home_position[] = {0, 0, 0}; // Home position array {x, y, z}
float current_position[3];          // Global variable to track current position of the tool

// GPIO Pin Arrays for Initialisation
uint gpio_pins_output[] = {RESET_PIN, SLEEP_PIN, HOME_X_PIN, HOME_Y_PIN, HOME_Z_PIN, MODE0_PIN, MODE1_PIN,
                           MODE2_PIN, DIRECTION_X_PIN, DIRECTION_Y_PIN, DIRECTION_Z_PIN,
                           ERROR_LED_PIN};                        // Initialisation of GPIO_OUT pins
uint gpio_pins_input[] = {FAULT_X_PIN, FAULT_Y_PIN, FAULT_Z_PIN}; // Initialisation of GPIO_IN pins

// -------------------- END OF GLOBAL VARIABLES -------------------- //

// -------------------- FUNCTIONS -------------------- //

//--------------PRINT INSTRUCTION-------------//
int print_instructions()
{
  printf("//-----Instructions-----// \n\r");
  printf("on       /turns on stepper motors  \n\r");
  printf("off      /turns off stepper motors and spindle \n\r");
  printf("x10      /move tool 10mm in the positive x direction \n\r");
  printf("x-10     /move tool 10mm in the negative x direction \n\r ");
  printf("y10      /move tool 10mm along y axis \n\r");
  printf("z10      /move tool 10mm along z axis \n\r");
  printf("spd50    /Sets spindle speed to 50%% \n\r");
  printf("home x   /returns tool to the starting x position \n\r");
  printf("home all /returns tool to the starting position \n\r");
}
// ---------- Spindle Control Function ---------- //
// Set spindle speed using PWM
bool set_spindle_speed(uint pin, uint frequency, uint duty_cycle_percent) // Spindle Function (Pin, Frequency, Duty Cycle Percent)
{
  // Check if the duty cycle is within the valid range (0-100%)
  if (duty_cycle_percent >= 0 && duty_cycle_percent <= 100) // Set spindle speed boundaries
  {
    // Calculate the period (top) based on frequency
    uint top = (uint)(float)(CLOCK_FREQUENCY / (float)frequency) - 1;

    // Configure the PWM settings for spindle control
    uint pin_slice_number = pwm_gpio_to_slice_num(pin);
    uint pin_channel_number = pwm_gpio_to_channel(pin);
    pwm_set_enabled(pin_slice_number, true);
    pwm_set_wrap(pin_slice_number, top);
    pwm_set_phase_correct(pin_slice_number, false);

    // Set the duty cycle based on the specified percentage
    pwm_set_chan_level(pin_slice_number, pin_channel_number, top * (duty_cycle_percent / 100.0f));

    // Print a message indicating the spindle speed
    printf("Spindle speed set to: %u%%\n\r", duty_cycle_percent);

    return true;
  }
  else
  {
    // Print an error message for out-of-bounds input
    printf("Out-of-bounds error! (0 ≤ input ≤ 100)\n\r");
    return false;
  }
}
// ---------- End of Spindle Control Function ---------- //

// ---------- Stepper Control Function ---------- //
// Function to control stepper motors
int stepper_control(uint8_t step_pin, uint8_t direction_pin, int16_t move_distance)
{
  // Determine the direction based on the sign of move_distance (+/-)
  if (move_distance < 0)
  {
    // Set direction pin to move in the negative direction
    gpio_put(direction_pin, DRIVE_STEP_DIRECTION_NEGATIVE);
  }
  else
  {
    // Set direction pin to move in the positive direction
    gpio_put(direction_pin, DRIVE_STEP_DIRECTION_POSITIVE);
  }
  sleep_ms(1);

  // Setup PWM for Stepper Motors
  uint32_t frequency = STEPPER_PWM_FREQUENCY;
  uint step_pulse_us = (1 / (float)frequency) * 1e6;
  uint slice_num_step = pwm_gpio_to_slice_num(step_pin);
  pwm_set_clkdiv(slice_num_step, CLOCK_DIV);
  uint top = (uint)(float)(CLOCK_FREQUENCY / CLOCK_DIV / (float)frequency) - 1;
  pwm_set_wrap(slice_num_step, top);
  pwm_set_chan_level(slice_num_step, PWM_CHAN_B, top / 2); // 50% Duty Cycle
  uint32_t on_time = step_pulse_us * STEPS_PER_MM * abs(move_distance);

  // Enable the Stepper motor
  pwm_set_enabled(slice_num_step, true);
  /// wait for calculated on time
  sleep_us(on_time);
  pwm_set_enabled(slice_num_step, false);
  // Disable the stepper motor
}
// ---------- End of Stepper Control Function ---------- //

//------------ Diagonal Control Function ---------------//
int diag_stepper_control(uint8_t step_pin_x, uint8_t step_pin_y, uint8_t direction_pin_x, int8_t direction_pin_y, int16_t move_distance_x, int16_t move_distance_y)
{
  // Determine the direction based on the sign of move_distance (+/-)
  if (move_distance_x < 0)
  {
    // Set direction pin to move in the negative direction
    gpio_put(direction_pin_x, DRIVE_STEP_DIRECTION_NEGATIVE);
  }
  else
  {
    // Set direction pin to move in the positive direction
    gpio_put(direction_pin_x, DRIVE_STEP_DIRECTION_POSITIVE);
  }

  if (move_distance_y < 0)
  {
    // Set direction pin to move in the negative direction
    gpio_put(direction_pin_y, DRIVE_STEP_DIRECTION_NEGATIVE);
  }
  else
  {
    // Set direction pin to move in the positive direction
    gpio_put(direction_pin_y, DRIVE_STEP_DIRECTION_POSITIVE);
  }

  sleep_ms(1);
  uint32_t frequency = STEPPER_PWM_FREQUENCY;
  uint step_pulse_us = (1 / (float)frequency) * 1e6;
  uint slice_num_step_x = pwm_gpio_to_slice_num(step_pin_x);
  uint slice_num_step_y = pwm_gpio_to_slice_num(step_pin_y);
  pwm_set_clkdiv(slice_num_step_x, CLOCK_DIV);
  pwm_set_clkdiv(slice_num_step_y, CLOCK_DIV);
  uint top = (uint)(float)(CLOCK_FREQUENCY / CLOCK_DIV / (float)frequency) - 1;
  pwm_set_wrap(slice_num_step_x, top);
  pwm_set_wrap(slice_num_step_y, top);
  pwm_set_chan_level(slice_num_step_x, PWM_CHAN_B, top / 2);
  pwm_set_chan_level(slice_num_step_y, PWM_CHAN_B, top / 2);
  uint32_t on_time = step_pulse_us * STEPS_PER_MM * abs(move_distance_x);

  // Enable Stepper motors to move for calculated time
  pwm_set_enabled(slice_num_step_x, true);
  pwm_set_enabled(slice_num_step_y, true);
  sleep_us(on_time);
  pwm_set_enabled(slice_num_step_x, false);
  pwm_set_enabled(slice_num_step_y, false);
  // Disable the stepper motor
}
//------------- end diag control function ----------------//

// ---------- RX Interrupt Handler Function ---------- //
void on_uart_rx()
{
  while (uart_is_readable(UART_ID))
  {
    uint8_t key_press = uart_getc(UART_ID);
    uart_putc(UART_ID, key_press);

    // Save the character in the buffer and increment the index
    if (Buffer_index < 99)
    {
      Buffer[Buffer_index++] = key_press;
    }

    // Check if the buffer is overflowed when a newline character is received
    if ((key_press == '\r' || key_press == '\n')) // Check for newline character (Enter key)
    {
      Buffer[Buffer_index] = '\0';
      input_ready = true;
    }
  }
}
// ---------- End of RX Interrupt Handler Function ---------- //

// ---------- Blink LED Function ---------- //
void blink_error_led()
{
  while (!(gpio_get(FAULT_X_PIN) == 1 || gpio_get(FAULT_Y_PIN) == 1 || gpio_get(FAULT_Z_PIN) == 1))
  {
    gpio_put(ERROR_LED_PIN, 1); // Switch on the LED
    sleep_ms(1000);             // Wait for 1 second
    gpio_put(ERROR_LED_PIN, 0); // Switch off the LED
    sleep_ms(1000);             // Wait for 1 second
    printf("ERROR DETECTED!\n\r");
  }
}
// ---------- End of Blink LED Function ---------- //

// ---------- Input Handler Function ---------- //
int input_handler(volatile char input_command[10])
{
  // Define variables for the Input Handler
  char axis[2];
  int move_distance;
  int move_distance_x;
  int move_distance_y;
  uint spindle_speed;
  uint circle_radius;

  // String checks for inputs
  if (sscanf((char *)input_command, "x %d", &move_distance) == 1)
  {
    // Check if the move distance is within the valid range
    if ((MIN_BOARD_Y - current_position[0]) <= move_distance && move_distance <= (MAX_BOARD_X - current_position[0]))
    {
      // Print a message indicating the move distance for the X-axis
      printf("Moving along the X-axis: %d mm\n\r", move_distance);

      // Update the current position along the X-axis
      current_position[0] = current_position[0] + move_distance;

      // Control the stepper motor for the X-axis
      stepper_control(STEP_X_PIN, DIRECTION_X_PIN, move_distance);

      // Print the updated current position
      printf("Current position: (%f, %f, %f)\n\r", current_position[0], current_position[1], current_position[2]);
    }
    else
    {
      printf("tried to move out of range\n\r");
    }

    // -------------------- END OF MOVE COMMAND: X-AXIS -------------------- //
  }
  else if (sscanf((char *)input_command, "y %d", &move_distance) == 1)
  {
    if ((MIN_BOARD_Y - current_position[1]) <= move_distance && move_distance <= (MAX_BOARD_Y - current_position[1]))
    {
      // Print a message indicating the move distance for the Y-axis
      printf("Moving along the Y-axis: %d mm\n\r", move_distance);

      // Update the current position along the Y-axis
      current_position[1] = current_position[1] + move_distance;

      // Control the stepper motor for the Y-axis
      stepper_control(STEP_Y_PIN, DIRECTION_Y_PIN, move_distance);

      // Print the updated current position
      printf("Current position: (%f, %f, %f)\n\r", current_position[0], current_position[1], current_position[2]);
    }
    else
    {
      printf("tried to move out of range\n\r");
    }
  }
  else if (sscanf((char *)input_command, "z %d", &move_distance) == 1)
  {
    if ((MIN_Z_POSITION - current_position[2]) <= move_distance && move_distance <= (MAX_Z_POSITION - current_position[2]))
    {
      // Print a message indicating the move distance for the Z-axis
      printf("Moving along the Y-axis: %d mm\n\r", move_distance);

      // Update the current position along the Z-axis
      current_position[2] = current_position[2] + move_distance;

      // Control the stepper motor for the Z-axis
      stepper_control(STEP_Z_PIN, DIRECTION_Z_PIN, move_distance);

      // Print the updated current position
      printf("Current position: (%f, %f, %f)\n\r", current_position[0], current_position[1], current_position[2]);
    }
    else
    {
      printf("tried to move out of range\n\r");
    }
  }
  // Diagonal Command -> Rory // replace with function
  else if (sscanf((char *)input_command, "d %d %d", &move_distance_x, &move_distance_y) == 2)
  {
    if ((MIN_BOARD_X - current_position[0]) <= move_distance_x && move_distance_x <= (MAX_BOARD_X - current_position[0]) && (MIN_BOARD_Y - current_position[1]) <= move_distance_y && move_distance_y <= (MAX_BOARD_Y - current_position[1]))
    {
      // Update the current position along the XY-axis
      current_position[0] = current_position[0] + move_distance_x;
      current_position[1] = current_position[1] + move_distance_y;

      // Print a message indicating the move distance for the XY-axis
      printf("Moving along the XY-axis: %d mm, %d mm\n\r", move_distance_x, move_distance_y);

      // Control the stepper motor for the XY-axis
      diag_stepper_control(STEP_X_PIN, STEP_Y_PIN, DIRECTION_X_PIN, DIRECTION_Y_PIN, move_distance_x, move_distance_y);

      // Print the updated current position
      printf("Current position: (%f, %f, %f)\n\r", current_position[0], current_position[1], current_position[2]);
    }
    else
    {
      printf("tried to move out of range x or y\n\r");
    }
  }
  // Spindle Control

  if (sscanf((char *)input_command, "spd%u", &spindle_speed) == 1)
  {
    if (set_spindle_speed(SPINDLE_PIN, SPINDLE_PWM_FREQUENCY, spindle_speed) == 1)
      ;
    printf("spindle speed set: %u\n\r", spindle_speed);
  }

  // Command to home the x-direction
  else if (strstr((char *)input_command, "home x"))
  {
    printf("Homing x\n\r");
    stepper_control(STEP_Z_PIN, DIRECTION_Z_PIN, (float)home_position[2] - current_position[2]);
    stepper_control(STEP_X_PIN, DIRECTION_X_PIN, (float)home_position[0] - current_position[0]);
    current_position[0] = (float)(home_position[0]);
  }
  // Command to home the y-direction
  else if (strstr((char *)input_command, "home y"))
  {
    printf("Homing y\n\r");
    stepper_control(STEP_Z_PIN, DIRECTION_Z_PIN, (float)home_position[2] - current_position[2]);
    stepper_control(STEP_Y_PIN, DIRECTION_Y_PIN, (float)home_position[1] - current_position[1]);
    current_position[1] = (float)(home_position[1]);
  }
  // Command to home the z-direction
  else if (strstr((char *)input_command, "home z"))
  {
    printf("Homing z\n\r");

    stepper_control(STEP_Z_PIN, DIRECTION_Z_PIN, (float)home_position[2] - current_position[2]);
    current_position[2] = (float)(home_position[2]);
  }
  // Command to home all directions
  else if (strstr((char *)input_command, "home all"))
  {
    printf("Homing all\n\r");
    stepper_control(STEP_Z_PIN, DIRECTION_Z_PIN, (float)home_position[2] - current_position[2]);
    stepper_control(STEP_Y_PIN, DIRECTION_Y_PIN, (float)home_position[1] - current_position[1]);
    stepper_control(STEP_X_PIN, DIRECTION_X_PIN, (float)home_position[0] - current_position[0]);
    for (uint8_t i = 0; i < 3; i++)
    {
      current_position[i] = (float)(home_position[i]);
    }
  }
  // Command to turn on the stepper motors
  else if (strstr((char *)input_command, "on"))
  {
    // Setup and activate stepper motors
    gpio_put(SLEEP_PIN, 1);
    gpio_put(RESET_PIN, 1);
    sleep_us(2);
    gpio_put(ENABLE_PIN, 0);
    sleep_us(2);

    // Set full steps
    gpio_put(MODE0_PIN, 0);
    gpio_put(MODE1_PIN, 0);
    gpio_put(MODE2_PIN, 0);
  }
  // Command to turn off the stepper motors
  else if (strstr((char *)input_command, "off"))
  {
    // Deactivate stepper motors
    gpio_put(SLEEP_PIN, 0);
    gpio_put(RESET_PIN, 0);
    sleep_us(2);
    gpio_put(ENABLE_PIN, 1);
    sleep_us(2);
  }
  // Reset the blink_error_led function
  else
  {
    printf("invalid command");
    print_instructions();
  }

  return 0;
}
// ---------- End of Input Handler ---------- //
// -------------------- END OF FUNCTIONS -------------------- //

// -------------------- MAIN -------------------- //
int main()
{
  stdio_init_all();

  // ---------- UART Initialisation ---------- //
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID, 2400);
  gpio_set_function(UART_TX0_PIN, GPIO_FUNC_UART); // Configure the TX pin for UART communication
  gpio_set_function(UART_RX0_PIN, GPIO_FUNC_UART); // Configure the RX pin for UART communication
  int key_press;
  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);
  uart_set_hw_flow(UART_ID, false, false);
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
  uart_set_fifo_enabled(UART_ID, false);
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(UART_ID, true, false);
  uart_puts(UART_ID, "\nTerminal Connected \n\r");
  print_instructions();

  // Set up error LED interrupt handling
  irq_set_exclusive_handler(GPIO_IRQ_EDGE_RISE, blink_error_led);
  irq_set_enabled(ERROR_LED_PIN, true);

  // ---------- End of UART Initialisation ---------- //

  // Initialise GPIO pins for input and output
  int8_t i;

  // Initialise Input Pins
  for (i = 0; i < 3; i++)
  {
    gpio_init(gpio_pins_input[i]);             // Initialise GPIO
    gpio_set_dir(gpio_pins_input[i], GPIO_IN); // Set direction to input
  }

  // Initialise Output Pins
  for (i = 0; i < 11; i++)
  {
    gpio_init(gpio_pins_output[i]);              // Initialise GPIO
    gpio_set_dir(gpio_pins_output[i], GPIO_OUT); // Set direction to output
  }
  
  // Initialise PWM Pins
  gpio_set_function(SPINDLE_PIN, GPIO_FUNC_PWM);
  gpio_set_function(STEP_X_PIN, GPIO_FUNC_PWM);
  gpio_set_function(STEP_Y_PIN, GPIO_FUNC_PWM);
  gpio_set_function(STEP_Z_PIN, GPIO_FUNC_PWM);

  // Wait for input, handle interrupts, and enter low power mode
  while (1)
  {
    if (input_ready)
    {
      // Process incoming data with the Input Handler
      input_handler(Buffer);
      
      //Reset Buffer
      Buffer[0] = '\0';
      Buffer_index = 0;
      input_ready = false;
    }
    // Reset the buffer and related variables

    else
    {
      // Enter low power mode (WFI - Wait for Interrupt)
      __asm("wfi");
    }
  }
}
// -------------------- END OF MAIN -------------------- //