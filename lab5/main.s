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

    ; Define Full Step Sequence
FullStep  DCD 0x9, 0xA, 0x6, 0x5      ; Full step sequence

    ; Rotate Clockwise
	MOV r3, #48                  ; Number of steps (48 steps × 7.5° per step = 360°)
rotate_clockwise
    LDR r2, =FullStep            ; Load Full Step sequence
    LDR r1, [r2], #4             ; Load current step
    MOV r0, #3                   ; Counter for FullStep array index
loop_clockwise
    AND r4, r1, #0x8             ; Extract Winding A value
    LSR r4, r4, #3               ; Shift to get the actual value
    AND r5, r1, #0x4             ; Extract Winding A value
    LSR r5, r5, #2               ; Shift to get the actual value
    AND r6, r1, #0x2             ; Extract Winding B value
    LSR r6, r6, #1               ; Shift to get the actual value
    AND r7, r1, #0x1             ; Extract Winding B value
    STR r4, [r0, #GPIO_ODR]  ; Set Winding A
    STR r5, [r0, #GPIO_ODR]  ; Set Winding A
    STR r6, [r1, #GPIO_ODR]  ; Set Winding B
    STR r7, [r1, #GPIO_ODR]  ; Set Winding B
    MOV r4, #6000                ; Delay
delay_loop_clockwise
    SUBS r4, r4, #1
    BNE delay_loop_clockwise
    SUBS r0, r0, #1
    BPL loop_clockwise
    SUBS r3, r3, #1
    BNE rotate_clockwise

    ; Check Button State
    LDR r0, =GPIOC_BASE          ; Load GPIOC base address
    LDR r1, [r0, #GPIO_IDR]      ; Read input data register
    TST r1, #(1 << BUTTON_PIN)   ; Test button state
    BEQ rotate_clockwise         ; If button is pressed, rotate clockwise
    B rotate_counterclockwise    ; Otherwise, rotate counterclockwise

    ; Rotate Counterclockwise
    MOV r3, #48                  ; Number of steps (48 steps × 7.5° per step = 360°)
rotate_counterclockwise
    LDR r2, =FullStep            ; Load Full Step sequence
    LDR r1, [r2, #12]            ; Load current step
    MOV r0, #3                   ; Counter for FullStep array index
loop_counterclockwise
    AND r4, r1, #0x8             ; Extract Winding A value
    LSR r4, r4, #3               ; Shift to get the actual value
    AND r5, r1, #0x4             ; Extract Winding A value
    LSR r5, r5, #2               ; Shift to get the actual value
    AND r6, r1, #0x2             ; Extract Winding B value
    LSR r6, r6, #1               ; Shift to get the actual value
    AND r7, r1, #0x1             ; Extract Winding B value
    STR r4, [r0, #GPIO_ODR]  ; Set Winding A
    STR r5, [r0, #GPIO_ODR]  ; Set Winding A
    STR r6, [r1, #GPIO_ODR]  ; Set Winding B
    STR r7, [r1, #GPIO_ODR]  ; Set Winding B
    MOV r4, #6000                ; Delay
delay_loop_counterclockwise
    SUBS r4, r4, #1
    BNE delay_loop_counterclockwise
    SUBS r0, r0, #1
    BPL loop_counterclockwise
    SUBS r3, r3, #1
    BNE rotate_counterclockwise

    ENDP

; Define Data Section
	AREA myData, DATA, READWRITE
	ALIGN

array   DCD 1, 2, 3, 4            ; Define an array of 4 integers
pressed_offset DCD 0               ; Variable to store the button state

	END