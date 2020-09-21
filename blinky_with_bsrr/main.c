//PA5
//AHB2
//RCC->AHB2ENR
//GPIOx_MODER
//GPIOx_ODR

#include "stm32l4xx.h"   // Device header

void delayMS(int);
	
int main(void)
{
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	while(1){
	   	GPIOA->BSRR  = (1<<5);    // or 0x20;   set PA5 High
		  delayMS(500);        
		  GPIOA->BSRR  = 1<<(5+16);//or use 0x00200000;   // reset PA5 low
	    delayMS(500); 
	}
}

void delayMS(int delay){
	
//	int i;
//	
//	for(; delay>0; delay--){
//		
//		for(i = 0; i<3195;i++);
//		
//	}
	
	unsigned ticks = (delay * (SystemCoreClock/ 8)) / 1000 ;
 
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
 
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}



