#include "stm32l4xx.h"                  // Device header


void SysClock_Config(void);
void systickDelayMS(int n);



int main(void)
{
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	SysClock_Config();
	
	while(1){
		  systickDelayMS(500);
	   	GPIOA->ODR ^= (1<<5);    // toggle LED at PA5
		 
	}
	
}


void systickDelayMS(int n)
{
	// Based  on 16MHz clk
	SysTick->LOAD = 16000 - 1; // 1m sec event @16MHz clk
	SysTick->VAL = 0;          // Clear current value reg.
	SysTick->CTRL =0x5;        // Enable Systick
	
	for(int i = 0; i<n; i++)
	{
		while((SysTick->CTRL & 0x10000) == 0);
	}
	SysTick->CTRL = 0; // Disable SysTick
}



void SysClock_Config(void)
{
	RCC->CR = RCC_CR_MSION;  // MSI ON
	
	RCC->CFGR &=~ RCC_CFGR_SW; // select MSI as clk source
	
	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
	
	RCC->CR &=~ RCC_CR_MSIRANGE;            // clear the range register
	RCC->CR |= RCC_CR_MSIRANGE_8;           // select 16MHz range
	
	
	RCC->CR |= RCC_CR_MSIRGSEL;             // Use the  MSIRANGE  in CR 
	
	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
	
}
