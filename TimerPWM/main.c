//switch led at PA5 using  PWM 

#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);

int main(void)
{
	SysClock_Config();
	
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A  
	GPIOA->MODER = 0xABFFFBFF;  // PA5  in alternate mode 
	GPIOA->AFR[0] |= 1<<20;     // AF register low[0], AFSEL5 = AF1 TIM2 out on PA5
	
	
	//TIM2 config 
	RCC->APB1ENR1 |= (1<<0); // enable TIM2 clk
	TIM2->PSC = 10 - 1;    // Divide Fclk(16MHz) by 10 = 1.6MHz
	TIM2->ARR = 26667 - 1;   // period is 0.0166sec or 60Hz PWM
	TIM2->CNT = 0;           // clear the timer counter
	TIM2->CCMR1 = 0x60;      // PWM mode
	TIM2->CCER  |= 1<<0;     // enable channel 1 (CCR1)	
	TIM2->CCR1  = 8889 - 1;  // pulse width to 1/3 the period
	TIM2->CR1 = 1<<0;        // enable timer in default upcounting  mode
	
	
	while(1)
	{
	
	}
	
	
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