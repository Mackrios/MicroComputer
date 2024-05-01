#include "stm32l476xx.h"
#include "SysClock.h"
#include <stdio.h>
// [0] = left = 10
// [1] = middle sensor front = 11
// [2] = right sensor = 12 
// [3] = middle sensor = 13 
// PC.10, PC.11, PC.12, PC.14 pins for reflectance sensors
#define SENSOR_PORT GPIOC
#define SENSOR_PIN_1  10
#define SENSOR_PIN_2  11
#define SENSOR_PIN_3  12
#define SENSOR_PIN_4  14
#define BUTTON_PIN 		13
int threshold[3] ; 
int sensor_pin_map[] = {SENSOR_PIN_1,SENSOR_PIN_2,SENSOR_PIN_3,SENSOR_PIN_4} ;
int calb_low[3];
int calb_high[3];
volatile int finail_data_sensor[4];
volatile int black = 0 , white = 0 ;
int data[3];
int left = 0;
int middle = 0;
int right = 0;
void delayMicroseconds(uint32_t us) {
    for (int i = 0; i < us * 6; ++i) {
        __NOP(); // No operation, compiler intrinsic to introduce a delay
    }
}
/*
///////////////////////////////////////////////
///////////////////////////////////////////////
beggning of the sensor logic 
*//////////////////////////////////////////////

 int sensor_pin_odr_bits (int sensor_index){
 
 return ((0x1)<< sensor_pin_map[sensor_index]);
 }
 
  int sensor_pin_moder_bits (int sensor_index){
 
return ((0x1)<< 2* (sensor_pin_map[sensor_index]));
 }

void configureSensorsAndButton(){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    // Set sensor pins (PC10, PC11, PC12, PC14) to input mode
    SENSOR_PORT->MODER &= ~((0x3 <<	SENSOR_PIN_1 * 2) | (0x3 <<	SENSOR_PIN_2 * 2) | (0x3 <<	SENSOR_PIN_3 * 2) | (0x3 <<	SENSOR_PIN_4 * 2) | (0x3 <<	BUTTON_PIN * 2) );
    //SENSOR_PORT->ODR |= (1U << (10 + 2));    // Set SENSOR_PIN_i high
		GPIOC->PUPDR &= ~(0x03<<(26)); // set pull up pull down
    //delayMicroseconds(10); // Wait for 10 microseconds

    //SENSOR_PORT->MODER &= ~(0x3 << (20 + 2)); // Set SENSOR_PIN_i back to input mode
    // Configure button pin (PC13) to input mode with pull-up configuration
}
int measureReflectance(int sensor_index) {
        SENSOR_PORT->MODER |= (sensor_pin_moder_bits (sensor_index)); // Set SENSOR_PIN_i to output mode
        SENSOR_PORT->ODR |= (sensor_pin_odr_bits (sensor_index));    // Set SENSOR_PIN_i high

        delayMicroseconds(10); // Wait for 10 microseconds

        SENSOR_PORT->MODER &= ~(sensor_pin_moder_bits (sensor_index)); // Set SENSOR_PIN_i back to input mode
         int count = 0;
        while ((SENSOR_PORT->IDR & (sensor_pin_odr_bits (sensor_index))) != 0 &&(count < 2000)) {
            count++;
					
        }
				
        return count ;
    }
int checkbutton_state(){
return ((GPIOC->IDR) & (0x1 << BUTTON_PIN )) == 0 ;
			}
    	// main logic for navagting the maze 
    // Check conditions based on sensor readings
enum PROGRAM_STATE {
		PROGRAM_STATE_CALB_WHITE,
		PROGRAM_STATE_RUN
};

/*
///////////////////////////////////////////////
///////////////////////////////////////////////
beggning of the motor instization/functions
*//////////////////////////////////////////////
void motor_instiazation(){
int mask = 0x330F;
int set = 0x1105;
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; 
	
	GPIOA->MODER =GPIOA->MODER &~ mask | set;
	GPIOA->PUPDR &= ~(0x505);
}

void motor_left(){
	GPIOA->ODR = GPIOA->ODR| 0x10; 
}

void motor_right(){
GPIOA->ODR = GPIOA->ODR| 0x01;
}

void motor_forward(){
GPIOA->ODR = GPIOA->ODR | 0x11;
}

void motor_stop(){
GPIOA->ODR = GPIOA->ODR & (0x00);
}

void motor_back(){
GPIOA->ODR = GPIOA->ODR| 0x42;
}
/// every movemnt is after a stop 
void motor_logic_moevemnt(){
	int i = 0 ;
	for(i=0;i<3;i++){
	data[i]= measureReflectance(i);
	}															 
	left = (data[0]>threshold[0]?1:0);					 //checking left
	middle = (data[1]>threshold[1]?1:0);					//checking middle 															
	right = (data[2]>threshold[2]?1:0);					//checking right 

if (middle == 1 && right == 0 && left == 0 ){
		motor_forward();
		delayMicroseconds(500);
		motor_stop();
}else if  (middle == 1 && right == 1 && left == 0 ){
		motor_right();
		delayMicroseconds(500);
		motor_stop();
		motor_forward();
		delayMicroseconds(500);
		motor_stop();
}else if (middle == 1 && right == 0 && left == 1 ){
		motor_left();
		delayMicroseconds(500);
		motor_stop();
		motor_forward();
		delayMicroseconds(500);
		motor_stop();
}else if (middle == 0 && right == 1 && left == 0 ){
		motor_right();
		delayMicroseconds(500);
		motor_stop();
}else if (middle == 0 && right == 0 && left == 1 ){
		motor_left();
		delayMicroseconds(500);
		motor_stop();
}else if (middle == 1 && right == 1 && left == 1 ){
		motor_forward();
		delayMicroseconds(500);
		motor_stop();
}else {
		//motor_back();
		delayMicroseconds(400);
		motor_stop();
		motor_left();
		delayMicroseconds(400);
		motor_stop();
		motor_right();
		motor_stop();
}
}



int main(void) {
    //System_Clock_Init();
	motor_instiazation();
	configureSensorsAndButton();
int button_pressed_last_frame = 0;
int state = PROGRAM_STATE_CALB_WHITE;
int arrays[4] ;
int tres = 0 ;
	//motor_forward();
    while (1) {
			// motor_forward();
			int button_pressed = checkbutton_state();
			if (button_pressed && ! button_pressed_last_frame){
			switch (state){
				case PROGRAM_STATE_CALB_WHITE:
							for (int i = 0 ; i<3;i++){
								calb_low[i]= (measureReflectance(i)*1.1);
								threshold[i]=calb_low[i];
							}
							state   = PROGRAM_STATE_RUN ;
						break ;
				default:
						break ;
			}
			}
			button_pressed_last_frame = button_pressed ;
   if (state == PROGRAM_STATE_RUN){
//		 for (int i = 0;i<3;i++ ){																		/// for debugging puropses 
//					finail_data_sensor[i] = measureReflectance(i);
//			 		arrays[i] = finail_data_sensor[i] ;
//			 //if (i )
//					tres = measureReflectance(i);
//		 }
		 //motor_back();
		 motor_logic_moevemnt();
	 }
    }
}
