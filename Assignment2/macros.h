/******************************************************************************
 * Project Name: CC2511 Assignment 2 - Macros
 * Description:  Macros for all constants used in main.c
 * Authors:      Crabb. L., Wood. A., Van Duren. R.  (Group 88)
 * Date:         27/10/2023
 ******************************************************************************/

// X control pins
#define HOME_X_PIN          2
#define STEP_X_PIN          3
#define DIRECTION_X_PIN     6
#define FAULT_X_PIN         7

// Y control pins
#define HOME_Y_PIN          8
#define STEP_Y_PIN          9
#define DIRECTION_Y_PIN     10
#define FAULT_Y_PIN         11

// Z control pins
#define HOME_Z_PIN          12
#define STEP_Z_PIN          13
#define DIRECTION_Z_PIN     14
#define FAULT_Z_PIN         15

// Stepper control pins
#define RESET_PIN   16
#define SLEEP_PIN   17
#define DECAY_PIN   18
#define ENABLE_PIN  19
#define MODE0_PIN   20
#define MODE1_PIN   21
#define MODE2_PIN   22

// ACC pins
#define SPINDLE_PIN     26
#define ERROR_LED_PIN   27

// UART pins
#define UART_TX0_PIN    0
#define UART_RX0_PIN    1
#define UART_TX1_PIN    4
#define UART_RX1_PIN    5

// Onboard LED pin
#define LED_PIN     25

// UART Definitions
#define UART_ID         uart0
#define BAUD_RATE       115200
#define DATA_BITS       8
#define STOP_BITS       1
#define PARITY          UART_PARITY_NONE

// Spindle Control
#define SPINDLE_PIN         26
#define CLOCK_FREQUENCY     125e6
#define SPINDLE_PWM_FREQUENCY       30e3

// Position Definitions (mm)
#define MAX_BOARD_X         150.0
#define MIN_BOARD_X         0.0
#define MAX_BOARD_Y         100.0
#define MIN_BOARD_Y         0.0
#define MAX_Z_POSITION      0.0
#define MIN_Z_POSITION      -40.0

// Stepper MACROS
#define STEPS_PER_MM                        55
#define DRIVE_STEP_DIRECTION_POSITIVE       0
#define DRIVE_STEP_DIRECTION_NEGATIVE       1
#define STEPPER_PWM_FREQUENCY               900 //Hz

#define WRAP_VALUE  65500

#define CLOCK_DIV_INT           256
#define CLOCK_DIV_FRACTION      0
#define CLOCK_DIV               ((float)CLOCK_DIV_INT + ((float)CLOCK_DIV_FRACTION / 16.0f))

#define SETUP_TIME_MS   1