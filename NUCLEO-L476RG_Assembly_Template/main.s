
; Load Constant Definitions
	INCLUDE core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s

; Define Pin Mappings
; Green LED <--> PA.5, Button <--> PC.13
LED_PIN EQU 5
BUTTON_PIN EQU 13

; Define Code Section
	AREA main, CODE, READONLY
	EXPORT __main              ; make __main visible to linker
	ENTRY           

__main  PROC

    ; Initialize system clock

    ; Enable the clock to GPIO Port A    
    LDR r0, =RCC_BASE
    LDR r1, [r0, #RCC_AHB2ENR]
    ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
    STR r1, [r0, #RCC_AHB2ENR]

    ; Set Pin 5 of Port A as output mode
    ; LED is configured for general-purpose output mode
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_MODER]
    BIC r1, r1, #(3<<(2*LED_PIN))
    ORR r1, r1, #(1<<(2*LED_PIN))
    STR r1, [r0, #GPIO_MODER]

    ; Enable the clock to GPIO Port C
    ; Enable GPIOC clock
    LDR r0, =RCC_BASE
    LDR r1, [r0, #RCC_AHB2ENR]
    ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
    STR r1, [r0, #RCC_AHB2ENR]

    ; Set Pin 13 as input mode for GPIOC
    ; Configure Pin 13 of Port C as input
    LDR r0, =GPIOC_BASE
    LDR r1, [r0, #GPIO_MODER]
    BIC r1, r1, #(3<<(2*BUTTON_PIN)) ; Clear bits for Pin 13
    STR r1, [r0, #GPIO_MODER]

    ; Set Pin 13 with no pull-up, no pull-down
    ; Configure Pin 13 of Port C with no pull-up or pull-down
    LDR r1, [r0, #GPIO_PUPDR]
    BIC r1, r1, #(3<<(2*BUTTON_PIN)) ; Clear bits for Pin 13
    STR r1, [r0, #GPIO_PUPDR]

while_loop
    ; Check if Pin 13 is low (button pressed)
    LDR r1, =GPIOC_BASE
    LDR r0, [r1, #GPIO_IDR]
    TST r0, #(1 << BUTTON_PIN)   ; Test bit for Pin 13
    BEQ button_not_pressed        ; If button is not pressed, go to button_not_pressed

    ; Button is pressed, check the LED state
    LDR r1, =GPIOA_BASE
    LDR r0, [r1, #GPIO_ODR]
    TST r0, #(1 << LED_PIN)      ; Test bit for LED Pin
    BEQ led_off                   ; If LED is off, go to led_off

    ; LED is on, set pressed to 0
    MOV r2, #0                    ; pressed = 0
    B continue_while_loop         ; Continue the while loop

led_off
    ; LED is off, set pressed to 1
    MOV r2, #1                    ; pressed = 1
    B continue_while_loop         ; Continue the while loop

button_not_pressed
    ; Button is not pressed, check if pressed is 1
    CMP r2, #1                    ; Compare pressed with 1
    BEQ turn_on_led               ; If pressed is 1, turn on LED
    B turn_off_led                ; Otherwise, turn off LED

turn_on_led
    ; Turn on LED
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_ODR]
    ORR r1, r1, #(1 << LED_PIN)  ; Set LED Pin
    STR r1, [r0, #GPIO_ODR]
    B continue_while_loop         ; Continue the while loop

turn_off_led
    ; Turn off LED
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_ODR]
    BIC r1, r1, #(1 << LED_PIN)  ; Clear LED Pin
    STR r1, [r0, #GPIO_ODR]

continue_while_loop
    ; Continue the while loop
    B while_loop

    ENDP

    ALIGN

    ; Define Data Section
    AREA myData, DATA, READWRITE
    ALIGN

array   DCD 1, 2, 3, 4            ; Define an array of 4 integers
pressed_offset DCD 0               ; Variable to store the button state

    END
