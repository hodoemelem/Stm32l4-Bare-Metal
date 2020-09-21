#include "stm32l4xx.h"                  // Device header
void SysClock_Config(void);
void systickDelayMS(int n);
void SysTick_Handler(void);
//Configure Sstick interrupt to toggle LED
//at 1Hz, system clock is 16MHz
int main(void)
{
	SysClock_Config();
	__disable_irq(); // disable global interrupts request initially
	
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	//Config Systick (Generic User guide pg 249
	SysTick->LOAD = 16000000 - 1; // 1sec at 16MHz clk
	SysTick->VAL = 0;             // Current value 
	SysTick->CTRL = 7;            // SysTick enalble,clksource enable and interrupt enable
	
	__enable_irq();    // enable global interrupt
	
	while(1);
	
}

void SysTick_Handler(void)
{
		GPIOA->ODR ^= (1<<5); // Toggle LED at PA5
	
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
