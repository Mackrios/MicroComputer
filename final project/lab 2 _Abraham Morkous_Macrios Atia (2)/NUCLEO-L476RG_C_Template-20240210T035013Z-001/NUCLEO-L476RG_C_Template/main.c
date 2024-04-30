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

int sensor_pin_map[] = {SENSOR_PIN_1,SENSOR_PIN_2,SENSOR_PIN_3,SENSOR_PIN_4} ;
int calb_low[4];
int calb_high[4];
volatile int finail_data_sensor[4];
volatile int black = 0 , white = 0 ;

void delayMicroseconds(uint32_t us) {
    for (int i = 0; i < us * 6; ++i) {
        __NOP(); // No operation, compiler intrinsic to introduce a delay
    }
}

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
void motor_logic_moevemnt(){
int i = 0 ;
//motor_back();
int	sensor_num = 0 ;
if(finail_data_sensor[1]>=60){ // checking for moving foward
	//motor_forward();
	if (finail_data_sensor[0]<=150 && finail_data_sensor[2]<=150){     // no side line move forward 
		//delayMicroseconds(100);
		//motor_stop();
		motor_forward();
		//delayMicroseconds(100);
		//motor_stop();
	}
		if (finail_data_sensor[0]>=150 && finail_data_sensor[2]<=150 ){
			delayMicroseconds(1000);
			motor_stop();
//		delayMicroseconds(1000);
		motor_left();
//		delayMicroseconds(50);
//		motor_stop();
	}else if (finail_data_sensor[0]<=150 && finail_data_sensor[2]>=150 ){
		delayMicroseconds(1000);
		motor_stop();
//		delayMicroseconds(50);
		motor_right();
//		delayMicroseconds(50);
//		motor_stop();
		}
}else if(finail_data_sensor[1]<=60 && finail_data_sensor[0]<=60 && finail_data_sensor[2]<=60 ){ //checking for no line in fornt so we back up until line is found  
	motor_stop();
	delayMicroseconds(50);
	motor_back();
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
		PROGRAM_STATE_CALB_BLACK,
		PROGRAM_STATE_RUN
};

int measruments_calbrated(int i){
	int raw_data = measureReflectance(i);
	int scaled_data = (raw_data - calb_low[i]) * 1000 / (calb_high[i] - calb_low[i]) ;
		if (scaled_data < 0){return 0;}
		if (scaled_data > 1000){return 1000;}
		else {
		return scaled_data ;
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
							for (int i = 0 ; i<4;i++){
								calb_low[i]= measureReflectance(i);
							}
							state   = PROGRAM_STATE_CALB_BLACK ;
						break ;
				case PROGRAM_STATE_CALB_BLACK:
							for (int i = 0 ; i<4;i++){
								calb_high[i]= measureReflectance(i);
							}
							state = PROGRAM_STATE_RUN ;
						break ;
				default:
						break ;
			}
			}
			button_pressed_last_frame = button_pressed ;
   if (state == PROGRAM_STATE_RUN){
		 for (int i = 0;i<4;i++ ){
					finail_data_sensor[i] = measruments_calbrated(i);
			 		arrays[i] = finail_data_sensor[i] ;
			 //if (i )
					tres = measruments_calbrated(i);
		 }
		 motor_logic_moevemnt();
	 }
    }
}
