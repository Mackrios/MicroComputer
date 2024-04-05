; Load Constant Definitions
	INCLUDE core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s

; Define Pin Mappings
; Green LED <--> PA.5, Button <--> PC.13
LED_PIN EQU 5
BUTTON_PIN EQU 13
MOTOR_1A_PIN EQU 2
MOTOR_2A_PIN EQU 3
MOTOR_3A_PIN EQU 1
MOTOR_4A_PIN EQU 0

; Define Code Section
	AREA main, CODE, READONLY
	EXPORT __main              ; make __main visible to linker

; Subroutine to rotate the stepper motor clockwise
_motor_clockwise PROC
	push {LR} 
    LDR r1, =FullStep      ; Load the address of the FullStep array
    MOV r2, #12            ; Set the number of steps (12 steps x 15° per step = 180°)

rotate_step_loop
    LDR r3, [r1], #4       ; Load the next full step sequence from the FullStep array
    BL delay               ; Call the delay subroutine
    STR r3, [r0, #GPIO_ODR] ; Set the GPIO output data register with the step sequence
    SUBS r2, r2, #1        ; Decrement the step count
    BNE rotate_step_loop   ; Repeat until all steps are completed
	pop {LR} 
    BX LR                  ; Return from subroutine
	ENDP

; Subroutine to introduce a delay
delay PROC
    MOV r4, #3000          ; Set the delay value
delay_loop
    SUBS r4, r4, #1        ; Decrement the delay counter
    BNE delay_loop         ; Repeat until the delay counter reaches zero
    BX LR                  ; Return from subroutine
	ENDP

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

    ; Set Pin 5 of Port A with no pull-up, no pull-down
    ; Configure Pin 5 of Port A with no pull-up or pull-down
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_PUPDR]
    BIC r1, r1, #(3<<(2*LED_PIN)) ; Clear bits for Pin 5
    STR r1, [r0, #GPIO_PUPDR]

    ; Set Pin 13 of Port C with pull-up
    ; Configure Pin 13 of Port C with pull-up
    LDR r0, =GPIOC_BASE
    LDR r1, [r0, #GPIO_PUPDR]
    ORR r1, r1, #(1<<(2*BUTTON_PIN)) ; Set pull-up for Pin 13
    STR r1, [r0, #GPIO_PUPDR]

    ; Set Pin 5 of Port A as push-pull output
    ; Configure Pin 5 of Port A as push-pull output
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_OTYPER]
    BIC r1, r1, #(1<<LED_PIN) ; Clear bit for Pin 5
    STR r1, [r0, #GPIO_OTYPER]

    ; Set Motor Driver Inputs
    LDR r0, =GPIOC_BASE
    LDR r1, [r0, #GPIO_MODER]
    BIC r1, r1, #(3<<(2*MOTOR_1A_PIN))  ; Configure Pin PC2 as output
    BIC r1, r1, #(3<<(2*MOTOR_2A_PIN))  ; Configure Pin PC3 as output
    BIC r1, r1, #(3<<(2*MOTOR_3A_PIN))  ; Configure Pin PC1 as output
    BIC r1, r1, #(3<<(2*MOTOR_4A_PIN))  ; Configure Pin PC0 as output
    ORR r1, r1, #(1<<(2*MOTOR_1A_PIN))  ; Set Pin PC2 as output
    ORR r1, r1, #(1<<(2*MOTOR_2A_PIN))  ; Set Pin PC3 as output
    ORR r1, r1, #(1<<(2*MOTOR_3A_PIN))  ; Set Pin PC1 as output
    ORR r1, r1, #(1<<(2*MOTOR_4A_PIN))  ; Set Pin PC0 as output
    STR r1, [r0, #GPIO_MODER]

    ; Set Pin 5 of Port A with low speed
    ; Configure Pin 5 of Port A with low speed
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_OSPEEDR]
    BIC r1, r1, #(3<<(2*LED_PIN)) ; Clear bits for Pin 5
    STR r1, [r0, #GPIO_OSPEEDR]

    ; Set Pin 13 of Port C with low speed
    ; Configure Pin 13 of Port C with low speed
    LDR r0, =GPIOC_BASE
    LDR r1, [r0, #GPIO_OSPEEDR]
    BIC r1, r1, #(3<<(2*BUTTON_PIN)) ; Clear bits for Pin 13
    STR r1, [r0, #GPIO_OSPEEDR]

main_loop
    ; Call the subroutine to rotate the stepper motor clockwise
    BL _motor_clockwise

    ; Infinite loop to keep the program running

    B main_loop
	
    ALIGN

    ; Define Data Section
    AREA myData, DATA, READWRITE
    ALIGN

pressed_offset DCD 0               ; Variable to store the button state

; Define Full Step Sequence
FullStep  DCD 0x9, 0xA, 0x6, 0x5      ; Full step sequence


    END
