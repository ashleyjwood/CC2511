/**************************************************************
Lab 7

**************************************************************/

// Header Files
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "stdio.h"
#include "string.h"
#include "terminal.h"

// UART Configuration
#define UART_TX_PIN 0           // Specify the UART Transmit Pin
#define UART_RX_PIN 1           // Specify the UART Receive Pin
#define UART_ID uart0           // Specify the UART module (e.g., uart0, uart1, etc.)
#define BAUD_RATE 115200        // Set the Baud Rate to 115200 bits per second
#define DATA_BITS 8             // Configure 8 data bits per UART frame
#define STOP_BITS 1             // Use 1 stop bit per UART frame
#define PARITY UART_PARITY_NONE // Set UART Parity to None (no parity bit used)

// LED PWM Configuration
#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13
// #define STEP 25       // Step increment
#define MAX_LEVEL 255 // 250 = 100%
#define MIN_LEVEL 0   // 0 = 0%

uint8_t red;
uint8_t green;
uint8_t blue;

void display_header()
{
  term_move_to(1, 1);
  term_set_color(clrWhite, clrBlue); // Setting white text on blue background for the header
  printf("************************* LED CONTROLLER UI *************************");
  term_set_color(clrWhite, clrBlack); // Resetting the colour for the reset of the terminal to default
}

void display_pwm_info(uint8_t red, uint8_t green, uint8_t blue)
{
  term_move_to(1, 3);
  printf("Red LED: %d\n", red);

  term_move_to(1, 4);
  printf("Green LED: %d\n", green);

  term_move_to(1, 5);
  printf("Blue LED: %d\n", blue);
}

void display_instructions()
{
  term_move_to(1, 7);
  printf("LED Control Instructions:");
  term_move_to(1, 8);
  printf("type: red n");
  term_move_to(1, 9);
  printf("type: green n");
  term_move_to(1, 10);
  printf("type: blue n");
}

void display_code_lines()
{
  term_move_to(1, 12);
}

void update_ui()
{
  term_cls(); // Clear the screen
  display_header();
  display_pwm_info(red, green, blue);
  display_instructions();
  display_code_lines();
}

volatile bool input_ready = false;

static int chars_rxed = 0;

volatile char buf[100];
volatile unsigned int buf_index = 0;

int brightness;

// RX interrupt handler
void on_uart_rx()
{
  while (uart_is_readable(UART_ID))
  {
    uint8_t ch = uart_getc(UART_ID);
    uart_putc(UART_ID, ch);

    // When in this section, a character has been received, and the interrupt (ISR) is called.

    // Store the received character in a buffer
    buf[buf_index] = ch; // Save the character
    buf_index++;         // Increment the index

    if (ch == '\r') // Check for newline character (Enter key)
    {
      uart_putc(UART_ID, '\n'); // Send a newline character back
      ch = '\n';
      buf[buf_index] = '\0'; // Null-terminate the buffer
      buf_index = 0;

      term_cls(); // Clear the screen
      display_header();
      display_pwm_info(red, green, blue);
      display_instructions();
      display_code_lines();

      if (sscanf((char *)buf, "red %d", &brightness) == 1)
      {
        // Ensure brightness is within bounds
        brightness = (brightness > MAX_LEVEL) ? MAX_LEVEL : (brightness < MIN_LEVEL) ? MIN_LEVEL
                                                                                     : brightness;
        red = brightness;
        pwm_set_gpio_level(RED_LED, red * red); // Set the red LED brightness
        update_ui();
        printf("Red LED has been set to %d \r\n", red);
        input_ready = true;
      }

      else if (sscanf((char *)buf, "green %d", &brightness) == 1)
      {
        // Ensure brightness is within bounds
        brightness = (brightness > MAX_LEVEL) ? MAX_LEVEL : (brightness < MIN_LEVEL) ? MIN_LEVEL
                                                                                     : brightness;
        green = brightness;
        pwm_set_gpio_level(GREEN_LED, green * green); // Set the green LED brightness
        update_ui();
        printf("Green LED has been set to %d \r\n", green);
        input_ready = true;
      }

      else if (sscanf((char *)buf, "blue %d", &brightness) == 1)
      {
        // Ensure brightness is within bounds
        brightness = (brightness > MAX_LEVEL) ? MAX_LEVEL : (brightness < MIN_LEVEL) ? MIN_LEVEL
                                                                                     : brightness;
        blue = brightness;
        pwm_set_gpio_level(BLUE_LED, blue * blue); // Set the blue LED brightness
        update_ui();
        printf("Blue LED has been set to %d \r\n", blue);
        input_ready = true;
      }

      else if (strcmp((char *)buf, "off") == 0)
      {
        // Turn off the LEDs
        pwm_set_gpio_level(RED_LED, MIN_LEVEL);
        pwm_set_gpio_level(GREEN_LED, MIN_LEVEL);
        pwm_set_gpio_level(BLUE_LED, MIN_LEVEL);
        update_ui();
        printf("LED is off \r\n");
        input_ready = true;
      }

      else
      {
        printf("Incorrect format, please refer to above\r\n"); //
        update_ui();
        input_ready = true;
      }
    }
  }
}

int main()
{
  stdio_init_all(); // Initialize stdio (for printing)
  uart_init(UART_ID, 2400); // Initialize UART communication with a baud rate of 2400.

  // Set the TX and RX pins by using the function select on the GPIO
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  int ch; // Variable to hold characters received over UART

  // The call will return the actual baud rate selected, which will be as close as possible to that requested
  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

  uart_set_hw_flow(UART_ID, false, false); // Set UART flow control CTS/RTS, we don't want these, so turn them off

  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Set the UART data format

  uart_set_fifo_enabled(UART_ID, false); // Turn off FIFOs, process characters one at a time

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select the correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  irq_set_exclusive_handler(UART_IRQ, on_uart_rx); // Set up the UART RX interrupt handler
  irq_set_enabled(UART_IRQ, true);

  uart_set_irq_enables(UART_ID, true, false); // Enable UART to send interrupts (RX only)

  // Send a basic string out over UART
  uart_puts(UART_ID, "\nHello, uart interrupts\n");

  //**************** PWM INITIALISATION ***********************
  // Initialize PWM for controlling LEDs
  gpio_set_function(RED_LED, GPIO_FUNC_PWM);
  gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);
  gpio_set_function(BLUE_LED, GPIO_FUNC_PWM);

  uint slice_num_RED = pwm_gpio_to_slice_num(RED_LED);
  uint slice_num_GREEN = pwm_gpio_to_slice_num(GREEN_LED);
  uint slice_num_BLUE = pwm_gpio_to_slice_num(BLUE_LED);

  int RED_LEVEL = 0;
  int GREEN_LEVEL = 0;
  int BLUE_LEVEL = 0;

  pwm_set_enabled(slice_num_RED, true);
  pwm_set_enabled(slice_num_GREEN, true);
  pwm_set_enabled(slice_num_BLUE, true);

  pwm_set_gpio_level(RED_LED, RED_LEVEL * RED_LEVEL);
  pwm_set_gpio_level(GREEN_LED, GREEN_LEVEL * GREEN_LEVEL);
  pwm_set_gpio_level(BLUE_LED, BLUE_LEVEL * BLUE_LEVEL);
  //**********************************************************

  while (1)
  {
    while (input_ready)
    {
      __asm("wfi"); // Puts the system in low power mode until an interrupt occurs
    }
    // Reset input_ready to false, so the loop doesn't keep processing the same data
    input_ready = false;
  }
}