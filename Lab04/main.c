/**************************************************************
Lab 4
Write a blink program without using the pico SDK. 
**************************************************************/

#include <stdbool.h>

int main(void)
{
// TODO - Initialise components and variables
#define LED_PIN (1 << 25)
#define RED_PIN (1 << 11)
#define GREEN_PIN (1 << 12)
#define BLUE_PIN (1 << 13)
#define BLINK_SPEED 0.25            // Time light is on/off in seconds
#define ITERATION_COUNT 16 000 000  // Number of times code iterates. Based off of the clock speed (I think).

// DEFINE MEMORY ADDRESSES AND OFFSETS //
// All memory addresses and offsets are from the pico documentation

// Pad Controls
#define PAD_CONTROL_BASE 0x4001c000
#define PAD_CONTROL_LED (PAD_CONTROL_BASE + 0x68)
#define PAD_CONTROL_RED (PAD_CONTROL_BASE + 0x30)
#define PAD_CONTROL_GREEN (PAD_CONTROL_BASE + 0x34)
#define PAD_CONTROL_BLUE (PAD_CONTROL_BASE + 0x38)

// GPIO Controls
#define GPIO_CONTROL_BASE 0x40014000
#define GPIO_CONTROL_LED (GPIO_CONTROL_BASE + 0x0cc)
#define GPIO_CONTROL_RED (GPIO_CONTROL_BASE + 0x05c)
#define GPIO_CONTROL_GREEN (GPIO_CONTROL_BASE + 0x064)
#define GPIO_CONTROL_BLUE (GPIO_CONTROL_BASE + 0x06c)

// GPIO Output Enabled
#define GPIO_OE_SET_BASE 0xd0000000
#define REG_GPIO_OE_SET (GPIO_OE_SET_BASE + 0x024)
#define GPIO_OUT_SET (GPIO_OE_SET_BASE + 0x014)
#define GPIO_OUT_CLR (GPIO_OE_SET_BASE + 0x018)

  typedef unsigned long uint32_t;

#define CONTENT_OF(addr) (*(volatile uint32_t *)addr)

  // RESET AND ENABLE THE PADS //
  // Reset
  CONTENT_OF(PAD_CONTROL_LED) = 0x0;
  CONTENT_OF(PAD_CONTROL_RED) = 0x0;
  CONTENT_OF(PAD_CONTROL_GREEN) = 0x0;
  CONTENT_OF(PAD_CONTROL_BLUE) = 0x0;

  // Enable
  CONTENT_OF(PAD_CONTROL_LED) = (1 << 6);
  CONTENT_OF(PAD_CONTROL_RED) = (1 << 6);
  CONTENT_OF(PAD_CONTROL_GREEN) = (1 << 6);
  CONTENT_OF(PAD_CONTROL_BLUE) = (1 << 6);

  // Select the GPIO function (F5)
  CONTENT_OF(GPIO_CONTROL_LED) = 0x1f;    // reset
  CONTENT_OF(GPIO_CONTROL_RED) = 0x1f;
  CONTENT_OF(GPIO_CONTROL_GREEN) = 0x1f;
  CONTENT_OF(GPIO_CONTROL_BLUE) = 0x1f;

  CONTENT_OF(GPIO_CONTROL_LED) = 0x1a ^ CONTENT_OF(GPIO_CONTROL_LED);
  CONTENT_OF(GPIO_CONTROL_RED) = 0x1a ^ CONTENT_OF(GPIO_CONTROL_RED);
  CONTENT_OF(GPIO_CONTROL_GREEN) = 0x1a ^ CONTENT_OF(GPIO_CONTROL_GREEN);
  CONTENT_OF(GPIO_CONTROL_BLUE) = 0x1a ^ CONTENT_OF(GPIO_CONTROL_BLUE);

  // Set the data direction (is it an output or an input)
  // Set output to be enabled
  CONTENT_OF(REG_GPIO_OE_SET) = LED_PIN;
  CONTENT_OF(REG_GPIO_OE_SET) = RED_PIN;
  CONTENT_OF(REG_GPIO_OE_SET) = GREEN_PIN;
  CONTENT_OF(REG_GPIO_OE_SET) = BLUE_PIN;

  while (true)
  {
    // TODO - Repeated code here
    CONTENT_OF(GPIO_OUT_SET) = LED_PIN;
    for (uint32_t i = 0; i < (BLINK_SPEED * ITERATION_COUNT); i++)
    {
      __asm volatile("nop");
    }
    CONTENT_OF(GPIO_OUT_SET) = RED_PIN;
    for (uint32_t i = 0; i < (BLINK_SPEED * ITERATION_COUNT); i++)
    {
      __asm volatile("nop");
    }
    CONTENT_OF(GPIO_OUT_SET) = GREEN_PIN;
    for (uint32_t i = 0; i < (BLINK_SPEED * ITERATION_COUNT); i++)
    {
      __asm volatile("nop");
    }
    CONTENT_OF(GPIO_OUT_SET) = BLUE_PIN;
    for (uint32_t i = 0; i < (BLINK_SPEED * ITERATION_COUNT); i++)
    {
      __asm volatile("nop");
    }
    CONTENT_OF(GPIO_OUT_CLR) = LED_PIN;
    CONTENT_OF(GPIO_OUT_CLR) = RED_PIN;
    CONTENT_OF(GPIO_OUT_CLR) = GREEN_PIN;
    CONTENT_OF(GPIO_OUT_CLR) = BLUE_PIN;
    for (uint32_t i = 0; i < (BLINK_SPEED * ITERATION_COUNT); i++)
    {
      __asm volatile("nop");
    }
  }
}