;******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attention
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from the book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************

    INCLUDE core_cm4_constants.s  ; Load Constant Definitions
    INCLUDE stm32l476xx_constants.s

    ; Green LED <--> PA.5, Button <--> PC.13
LED_PIN EQU 5
BUTTON_PIN EQU 13

    AREA main, CODE, READONLY
    EXPORT __main              ; make __main visible to linker
    ENTRY           

__main  PROC

    ; Enable the clock to GPIO Port A    
    LDR r0, =RCC_BASE
    LDR r1, [r0, #RCC_AHB2ENR]
    ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
    STR r1, [r0, #RCC_AHB2ENR]
    
    ; MODE: 00 - Input mode, 01 - General-purpose output mode
    ;       10 - Alternate function mode, 11 - Analog mode (reset state)
    LDR r0, =GPIOA_BASE
    LDR r1, [r0, #GPIO_MODER]
    BIC r1, r1, #(3<<(2*LED_PIN))
    ORR r1, r1, #(1<<(2*LED_PIN))
    STR r1, [r0, #GPIO_MODER]

    ; Enable the clock to GPIO Port C
    LDR r0, =RCC_BASE
    LDR r1, [r0, #RCC_AHB2ENR]
    ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
    STR r1, [r0, #RCC_AHB2ENR]

    ; Set Pin 13 as input mode for GPIOC
    LDR r0, =GPIOC_BASE
    LDR r1, [r0, #GPIO_MODER]
    BIC r1, r1, #(3<<(2*BUTTON_PIN)) ; Clear bits for Pin 13
    STR r1, [r0, #GPIO_MODER]

    ; Set Pin 13 with no pull-up, no pull-down
    LDR r1, [r0, #GPIO_PUPDR]
    BIC r1, r1, #(3<<(2*BUTTON_PIN)) ; Clear bits for Pin 13
    STR r1, [r0, #GPIO_PUPDR]

    ; Turn on the green LED (Assuming 1 indicates turning on)
    LDR r1, [r0, #GPIO_ODR]
    ORR r1, r1, #(1<<LED_PIN)
    STR r1, [r0, #GPIO_ODR]

while_loop
    ; Check if Pin 13 is high (button pressed)
    LDR r1, [r0, #GPIO_IDR]
    TST r1, #(1<<BUTTON_PIN)   ; Test bit for Pin 13
    BEQ while_loop             ; Branch if Pin 13 is low

    ; Check the state of Pin 5 (assuming 1 means LED is on)
    LDR r1, [r0, #GPIO_ODR]
    TST r1, #(1<<LED_PIN)      ; Test bit for Pin 5
    CMP r1, #(1<<LED_PIN)      ; Compare bit for Pin 5
    MOVNE r2, #1               ; If not equal, set 'pressed' to 1
    MOVEQ r2, #0               ; If equal, set 'pressed' to 0
	LDR r3, =pressed_offset      ; Load the address of pressed_offset into r3
	STR r2, [r0, r3]             ; Update the 'pressed' variable




    B while_loop             ; Repeat the loop

stop    B   stop                ; dead loop & program hangs here

    ENDP

    ALIGN

    AREA myData, DATA, READWRITE
    ALIGN
array   DCD 1, 2, 3, 4
pressed_offset DCD 0  ; Variable to store the button state

    END
