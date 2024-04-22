#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

void TIM2_Init(){
 RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // Enable TIMER 1 clock

	TIM2->CR1 &= ~TIM_CR1_DIR; // Counting direction: 0 = Up-counting, 1 =Down-counting
	//PrescaLer, slow down the input clock by a factor of (1 + prescaLer)
	TIM2->PSC=39; //4 MHz I (1 + 39) = we KHz
	TIM2->ARR = 999; //PWM period = (999 + 1) * 111eeKHz = 0 .01s
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; //Clear output compare mode bits for channel 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1M_2; //Select PWM Mode 1 output on channel 1 (OC1M = 110)
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //Output 1 preLoad enable
	TIM2->CCER &= ~TIM_CCER_CC1NP; //Select output polarity: 0 = Active high, 1 = Active Low	OC1N = OCREF + CC1NP
	TIM2->CCER |= TIM_CCER_CC1NE; //Enable complementary output of channel 1 (CH1N)
	TIM2->BDTR |= TIM_BDTR_MOE; //Main output enable (MOE): 0 = Disable, = 1 Enable
	TIM2->CCR1 = 500;//Output Compare Register for channel 1 Initial duty cycle 50%
	TIM2->CR1 |= TIM_CR1_CEN; //II Enable counter
}
int main(void){

	
	
	
	
	
	//System_Clock_Init(); // Switch System Clock = 80 MHz
	
	  // Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   // this enables the GPIOA
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	 
	GPIOA->MODER &= ~(0xC00);
	GPIOA->MODER |=(0x800);
 	GPIOA->AFR[0] &= ~(0xF0000); 
  GPIOA->AFR[0] |= (0x10000);
	
	GPIOC->MODER &= ~(0x5) ; 
	GPIOC->PUPDR &= ~(0x5); /// // set pull up pull down 
	GPIOC->ODR |= (0x3)  ; // thios truns the light on 
	
	GPIOC->MODER &= ~(0x03<<(26));
	GPIOC->PUPDR &= ~(0x03<<(26)); // set pull up pull down 
//  // Dead loop & program hangs here
  
//Variable for the button press
	int pressed = 0;
	while(1){

	}

	

}
