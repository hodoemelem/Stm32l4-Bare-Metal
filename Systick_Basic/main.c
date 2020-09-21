#include "stm32l4xx.h"                  // Device header
void SysClock_Config(void);



int main(void)
{
	SysClock_Config();
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	//Config Systick (Generic User guide pg 249
	SysTick->LOAD = 32000000 - 1; // 200 ms at 16MHz
	SysTick->VAL = 0;             // Current value 
	SysTick->CTRL = 5;            // SysTick enale
	
  while(1)
	{
      if(SysTick->CTRL & 0x10000) // check if count flag is high
			{
				GPIOA->ODR ^= (1<<5); // Toggle LED at PA5
			}
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


