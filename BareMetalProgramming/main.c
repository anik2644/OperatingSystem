#include<stdio.h>
#include "led.h"
#include "RCC.h"
#include "SystemClockConfig.h"
#include "GPIO.h"

void task1_handler(void);

int main(void)
{
	SystemClockConfig();

	led_init(1,4);   //input PA1, output PA4

	//task1_handler();
	while(1){	
		uint32_t input = GPIOA->IDR;
		
		if((input >> 1) & 1) 
			led_on(4);
		
		else
			led_off(4);
	}

	for(;;);
}


// void task1_handler(void)
// {
// 	while(1){	
// 		uint32_t input = GPIOA->IDR;
		
// 		if((input >> 1) & 1) 
// 			led_on(4);
		
// 		else
// 			led_off(4);
		
// 	}

// }
