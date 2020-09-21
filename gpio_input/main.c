#include "stm32l4xx.h"   // Device header
	
int main(void)
{
	RCC->AHB2ENR |= (1<<0);    // enable GPIOA clk, port A is Bit 0 on the register
	RCC->AHB2ENR |= (1<<2);    // enable GPIOC clk, port C is Bit 2 on the register
	
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output mode
	GPIOC->MODER = 0xF3FFFFFF; // ie 1111 0011 1111 ...  MODE13 is PC13, 00 for input mode
	
	while(1)
	{
		if(GPIOC->IDR & (1<<13))         // if PC13 is high, ie button released
		{ 
				GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5, ie low
		}
		else{                            // PC13 is low , ie button pushed
			GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
		}
  }
}