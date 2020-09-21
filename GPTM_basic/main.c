#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);

int main(void)
{
	SysClock_Config();
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	//TIM2 config for 1Hz delay
	RCC->APB1ENR1 |= (1<<0);
	TIM2->PSC = 1600 - 1;    // Divide Fclk by 1600 = 10KHz
	TIM2->ARR = 10000 - 1;   // counting to 10,000 is 1 sec
	TIM2->CNT = 0;           // clear the timer counter
	TIM2->CR1 = 1<<0;        // enable TIM2
	
	while(1)
	{
		
		while(!(TIM2->SR & 1));    // wait for overflow
		  TIM2->SR &=~1;           // clear the overflow flag
	   	GPIOA->ODR ^= (1<<5);    // toggle LED on PA5
		
	}
}



void SysClock_Config(void)
{
	RCC->CR = RCC_CR_MSION;  // MSI ON
	
	RCC->CFGR &=~ RCC_CFGR_SW; // select MSI as clk source
	
	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
	
	RCC->CR &=~ RCC_CR_MSIRANGE;            // clear the range register
	RCC->CR |= RCC_CR_MSIRANGE_8;           // sel. 16MHz range
	
	
	RCC->CR |= RCC_CR_MSIRGSEL;             // Use the  MSIRANGE  in CR 
	
	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
	
}

