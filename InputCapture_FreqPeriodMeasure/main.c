#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);

int period;
int freq;
int main(void)
{
	SysClock_Config();
	
	int last_value = 0;
	int current_value = 0;
	
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A  
	GPIOA->MODER = 0xABFFEBFF;  // PA5 and PA6 in alternate mode 
	
	
	GPIOA->AFR[0] |= 1<<20;     // AF register low[0], AFSEL5 = AF1 TIM2 CH1 out on PA5
	
	
	//TIM2 config 
	RCC->APB1ENR1 |= (1<<0); // enable TIM2 clk
	TIM2->PSC = 1600 - 1;    // Divide Fclk(16MHz) by 1600 = 10KHz
	TIM2->ARR = 10000 - 1;   // counting to 10,000 is 1 sec
	TIM2->CCMR1 = 0x30;      // compare mode, toggle OUT bit on match pg 1071
	TIM2->CCR1  = 0;         // value to compare to CNT current value (here at overflow)
	TIM2->CCER  |= 1<<0;     // enable channel 1 (CCR1)
	TIM2->CNT = 0;           // clear the timer counter
	TIM2->CR1 = 1<<0;        // enable timer in default upcounting  mode
	
	//config PA6 as input of TIM3 CH1
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A 
	//GPIOA->MODER = 0xABFFEBFF;  // PA5 and PA6 in alternate mode 
	GPIOA->AFR[0] |= 1<<25;     // AF register low[0], AFSEL6 = AF2 TIM3 CH1 out on PA6
	
	//TIM3 config 
	RCC->APB1ENR1 |= (1<<1); // enable TIM3 clk
	TIM3->PSC = 16000 - 1;    // Divide Fclk(16MHz) by 16000 = 1KHz
	TIM3->CCMR1 = 0x41;       // capture at every edge
	TIM3->CCER  = 0x0B;     // enable channel 1 (CCR1), and capture at both edges
	TIM3->CR1 = 1<<0;        // enable timer in default upcounting  mode
	
	/*
	  PA5 connected to PA6 by jumper wire
		PA5 toggles every 1 sec, hence PA6 gets edges every 1 sec
		1 bit period of TIM3 counter is 0.001 sec
    CCR1 is enabled on both edges,thus one caputre every 1 sec
	*/
	
	while(1)
	{
		while(!(TIM3->SR & 2)); // wait until an edge is captured
		current_value = (int)TIM3->CCR1;
		period = current_value - last_value; // no. of counts btw captures
		last_value = current_value;
		freq = (1000/period);
		/*
		   Freq_count = (1/count) * Freq_timer
		*/
		last_value = current_value;
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