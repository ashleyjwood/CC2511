/* Assembler directives */

.syntax unified /* Use modern syntax                                                    */
.text           /* The "text" section of the executable is where code is placed.        */
.thumb_func     /* Specifies that main is a thumb mode function and will be offset by 1 */ 
.global main    /* Declare "main" as a global symbol (callable from other files).       */
.align 4        /* Make sure the starting address is a multiple of 4.                   */

/* Definitions */
/* These are equivalent to C macros. However, note that they use a comma. */

.EQU LED_PIN,                 0x02000000
.EQU WAIT_LOOPS_ON,           1000000
.EQU WAIT_LOOPS_OFF,          1000000

.EQU PAD_CONTROL_BASE,        0x4001c000
.EQU PAD_CONTROL_LED,         PAD_CONTROL_BASE + 0x68

.EQU GPIO_CONTROL_BASE,       0x40014000
.EQU GPIO_CONTROL_LED,        GPIO_CONTROL_BASE + 0x0cc

.EQU GPIO_OE_SET_BASE,        0xd0000000
.EQU REG_GPIO_OE_SET,         GPIO_OE_SET_BASE + 0x024
.EQU GPIO_OUT_SET,            GPIO_OE_SET_BASE + 0x014
.EQU GPIO_OUT_CLR,            GPIO_OE_SET_BASE + 0x018

/* Main function */
main:
  push {lr}

  /* TODO - Calculate any values that will be used throughout the program */

  /* TODO - Initialise 3 registers, as in Lab 4 */ 

// Enable
ldr r0, =PAD_CONTROL_LED            /* Set r0 to be PAD_CONTROL_LED */
ldr r1, [r0]                        /* Set r1 to be the content of PAD_CONTROL_LED */
movs r2, #0x40
ands r1, r1, r2                     /* Bitshift a 1, 6 places to the left */
str r1, [r0]                        /* Store the changed content of r0 into the oringal r0 */

// Select the GPIO function (F5)
ldr r0, =GPIO_CONTROL_LED           /* Set r1 to be GPIO_CONTROL_LED */
ldr r1, [r0]                        /* Set r2 to be the content of GPIO_CONTROL_LED */
movs r2, #0x1f                      /* Reset */
orrs r1, r1, r2
movs r3, #0x1a
eors r1, r3, r1                      /* XOR with itself  */
str r1, [r0]                        /* Store the changed content of r1 into the original r1 */

/* Set the data direction */
ldr r0, =REG_GPIO_OE_SET            /* Set r2 to be REG_GPIO_OE_SET */
ldr r1, [r0]                        /* Set r3 to be the content of REG_GPIO_OE_SET */
ldr r2, =LED_PIN
orrs r1, r1, r2                         /* Set content of REG_GPIO_OE_SET to be 25 */
str r1, [r0]                        /* Store the changed content of r2 into the original r2 */


  /** TODO 
   *  Run in an endless loop that:
   *    (i)    Turns on the LED
   *    (ii)   Waits, by using an inner loop
   *    (iii)  Turns off the LED
   *    (iv)   Waits, by using an inner loop
   *    (v)    Returns to the top of the loop 
   */

movs r3, #1
loop1:

ldr r0, =GPIO_OUT_SET
ldr r1, [r0]
ldr r2, =LED_PIN 
orrs r1, r1, r2
str r1, [r0]

ldr r0, =WAIT_LOOPS_ON /* Number of seconds on (work out with clock speed) */
waiton:
subs r0, #1 /* minus 1 for each "second" */
cmp r0, #0
bne waiton

ldr r0, =GPIO_OUT_CLR
ldr r1, [r0]
ldr r2, =LED_PIN 
orrs r1, r1, r2
str r1, [r0]

ldr r0, =WAIT_LOOPS_OFF /* Number of seconds off (work out with clock speed) */
waitoff:
subs r0, #1 /* minus 1 for each "second" */
cmp r0, #0
bne waitoff

cmp r3, #0
bne loop1

/* Set output to be enabled */

	pop {pc}  /* Return from this function, by popping the return address into the program counter (pc) */