#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
 void Keypad_Init(){
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;  

// MODE: 00: Input mode, 01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)

GPIOC->MODER &=~(0x03F003FF); // Clear bit  //IN PRE
GPIOC->MODER |= (0x000000055);  //SETS THE BITS TO OUPUT
unsigned char key_map [4][3] = {
{'1', '2', '3','A'}, //II 1st row
{'4', '5', '6','B'}, //II 2nd row
{'7', '8', '9','C'}, //II 3rd row
{'*', '0', '#','D'}, //II 4th row
};
 }
 
unsigned char Keypad_scan() {
    unsigned char row, col, ColumnPressed = 0; 	
    unsigned char key = 0xFF;

    // Check whether any key has been pressed
    // Output zeros on all row pins
    for (row = 0; row <= 3; row++) {
        // Set up the row outputs using ODR
        GPIOC->ODR = ~(1 << row);  // Assuming GPIOC is the GPIO port you are using

        // Delay shortly (adjust the delay duration according to your requirements)
        for (volatile int i = 0; i < 1000; i++);  // Adjust the delay as needed

        // Read inputs of column pins
        // Check the column inputs
        for (col = 4; col <= 12; col++) {
            // If the input from the column pin ColumnPressed is zero
            if ((GPIOC->IDR & (1 << col)) == 0) {
                ColumnPressed = col;
            }
            if (col == 10 && ((GPIOC->IDR & (1 << col)) == 0) )
              ColumnPressed = col - 6 ;
            if (col == 11 && ((GPIOC->IDR & (1 << col)) == 0) )
              ColumnPressed = col - 7 ;
            if (col == 12 && ((GPIOC->IDR & (1 << col)) == 0) )
              ColumnPressed = col - 7 ;
        }


        // If inputs are 1 for all columns, then no key has been pressed
        if ((GPIOC->IDR & 0x07) == 0x07) {
            return 0xFF; // If no key pressed, return 0xFF
        }

        // Identify the row of the column pressed
        // Set up the row outputs using ODR
        GPIOC->ODR = (1 << row);  // Assuming GPIOC is the GPIO port you are using

        // Read the column inputs after a short delay
        // Check the column inputs
        for (int i = 0; i < 1000; i++);  // Adjust the delay as needed

        if ((GPIOC->IDR & (1 << ColumnPressed)) == 0) {
            key = key_map[row][ColumnPressed];
        }
    }

    return key;
}

int main(void){
System_Clock_Init(); // Switch System Clock = 80 MHz

 // Enable the clock to GPIO Port B
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;  

// MODE: 00: Input mode, 01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)

GPIOC->MODER &=~(0x03F003FF); // Clear bit  //IN PRE
GPIOC->MODER |= (0x000000055);  //SETS THE BITS TO OUPUT

// GPIOC->PUPDR &=~(0x0FFF03FF);   //SETS TO PULL UP PULL DOWN
//
// GPIOC->OTYPER &= ~(0x00001F1F); //OUTPUT TYPE REGISTER    IN PRE

while(1){

}
}


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	
//	
//int main(void){
//	volatile int i;
//	int count = 0;
//	char message[64]="";
//	
//	System_Clock_Init(); // Switch System Clock = 80 MHz
//	I2C_GPIO_init();
//	I2C_Initialization(I2C1);

//	ssd1306_Init();
//	
//	while(1){
//		for(i=0; i<100000; i++);
//		sprintf(message, "%d", count);
//		ssd1306_Fill(White);
//		ssd1306_SetCursor(2,0);
//		ssd1306_WriteString(message, Font_11x18, Black);
//		ssd1306_UpdateScreen();	
//		count++;
//	}
//}

