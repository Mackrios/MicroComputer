#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
 int SIZE = 60 ;

 void  Keypad_Pin_Init()
 {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;  


// MODE: 00: Input mode, 01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)


    GPIOC->MODER &=~(0x03F003FF); // Clear bit  //IN PRE
    GPIOC->MODER |= (0x000000055);  //SETS THE BITS TO OUPUT
    GPIOC->PUPDR &=~(0x03F003FF);

 }
 
unsigned char keypad_scan (void) 
{

    unsigned char key_map [4][4] = {
    {'1', '2', '3','A'}, //II 1st row
    {'4', '5', '6','B'}, //II 2nd row
    {'7', '8', '9','C'}, //II 3rd row
    {'*', '0', '#','D'}, //II 4th row
    };
    unsigned char row, Col, ColumnPressed;
    unsigned char key = (0xFF);
    uint32_t inputMask = 0 , outputMask = 0;
    int rows [] = {0, 1, 2, 3}; // Output pin numbers, assuming using Port C
    int cols[] = {4, 10, 11 ,12}; // Input pin numbers, assuming using Port C

    for (Col = 0; Col <= 4; Col++)
    {
        inputMask |= 1<<cols [Col];
    }

    for (row = 0;row < 4; row++)
    {
        outputMask |= 1<<rows [row];
    }

    // Check whether any key has been pressed
    // 1. Output zeros on all row pins
    // 2. Delay briefly,and read inputs of column pins
    // 3. If inputs are 1 for all columns, then no key has been pressed

    GPIOC ->ODR &= ~outputMask;
    GPIOC ->ODR &= (0x0); 

    for(volatile int i=0; i<100000; i++);

    if ((GPIOC->IDR & inputMask) == inputMask) // If no key is pressed, return exi 
        return (0xFF);

    // Identify the column of the key pressed
    for (Col = 0; Col < 4; Col++) { // CoLumn scan
        if ( (GPIOC->IDR & (1<<cols[Col])) == 0 )
    ColumnPressed = Col;
    }

    // Identify the row of the key pressed
    for (row = 0; row < 4; row++) {// Row scan
        // Set up the row outputs
        GPIOC->ODR |= outputMask ;
        GPIOC->ODR &= ~(1<< rows[row]) ; // could be rows 
		
    for(volatile int i=0; i<10000; i++){}
        
        
       if ( (GPIOC->IDR & (1<<cols[ColumnPressed])) == 0 )
        {
            key = key_map [row] [ColumnPressed];
             return key;
        }
    }
    // Wait until the key is released
    return 0xFF;

}

int main(void){
System_Clock_Init(); // Switch System Clock = 80 MHz
unsigned char key,previous_key;

    char message[64] = "";
    unsigned char len = 0;
    int i;
    Keypad_Pin_Init();
    I2C_GPIO_init();
    I2C_Initialization(I2C1);
    ssd1306_Init();
    ssd1306_Fill(White);
    ssd1306_SetCursor(2,0);
    ssd1306_WriteString(message, Font_11x18, Black);        
    ssd1306_UpdateScreen();

while (1)
    {
key = keypad_scan();
        if(previous_key == key)
            continue;
        switch (key)
        {
        case 0xFF:
            break;
        default:
            if (len >= 6){
                len = 6;
                for(i = 0;i<len;i++){
                    if(i == 5)
                        message[i] = key;
                    else
                        message[i]=message[i+1];
                }
            }else{
                message[len] = key;
            }
            len++;
            previous_key = key;
        }
        //output the message
        ssd1306_Fill(White);
        ssd1306_SetCursor(2,0);
        ssd1306_WriteString(message, Font_11x18, Black);        
        ssd1306_UpdateScreen();
    }
}

