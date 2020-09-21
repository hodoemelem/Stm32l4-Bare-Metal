#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);
void systickDelayMS(int n);
void TIM2_IRQHandler(void);

//Create TIM2 to toggle LED at 1Hz
//System clk is 16HZ

int main(void)
{
	SysClock_Config();
	__disable_irq();
	
	
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	
	RCC->APB1ENR1 |= (1<<0); // enable TIM2 clk source
	TIM2->PSC = 16000 - 1;   // Divide Fclk by 1600 = 1KHz
	TIM2->ARR = 1000 - 1;    // counting to 1000 is 1 sec(1Hz)
	TIM2->CNT = 0;           // clear the timer counter
	TIM2->CR1 = 1<<0;        // enable TIM2
	TIM2->DIER = 1<<0;       // overflow/update interrupt enable (UIE)
	
	NVIC_EnableIRQ(TIM2_IRQn);
	
	__enable_irq();
	while(1);

}

void TIM2_IRQHandler(void)
{
	TIM2->SR = 0; // clear interrupt flag
	GPIOA->ODR ^= 0x20;
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