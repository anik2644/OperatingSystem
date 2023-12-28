#include<stdint.h>
#include "led.h"
#include "RCC.h"
#include "GPIO.h"


void delay(uint32_t count)
{
  for(uint32_t i = 0 ; i < count ; i++);
}

void led_init(uint8_t input_led, uint8_t output_led)
{
	RCC->AHB1ENR |= (1<<0); //for portA
	GPIOA->MODER |= (0<<(input_led*2)) | (1<<(output_led*2)); 
	
}

void led_on(uint8_t led_no)
{
	GPIOA->BSRR |= (1<<led_no);

}

void led_off(uint8_t led_no)
{
	GPIOA->BSRR |= (1<<led_no) << 16;
}


