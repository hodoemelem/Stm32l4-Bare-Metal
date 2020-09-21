//#include "stm32l4xx.h"                  // Device header

//void SysClock_Config(void);

//int main(void)
//{
//	SysClock_Config();
//	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
//	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
//	
//	//TIM2 config for 1Hz delay
//	RCC->APB1ENR1 |= (1<<0);
//	TIM2->PSC = 1600 - 1;    // Divide Fclk by 1600 = 10KHz
//	TIM2->ARR = 10000 - 1;   // counting to 10,000 is 1 sec
//	TIM2->CNT = 0;           // clear the timer counter
//	TIM2->CR1 = 1<<0;        // enable TIM2
//	
//	while(1)
//	{
//		
//		while(!(TIM2->SR & 1));    // wait for overflow
//		  TIM2->SR &=~1;           // clear the overflow flag
//	   	GPIOA->ODR ^= (1<<5);    // toggle LED on PA5
//		
//	}
//}



//void SysClock_Config(void)
//{
//	RCC->CR = RCC_CR_MSION;  // MSI ON
//	
//	RCC->CFGR &=~ RCC_CFGR_SW; // select MSI as clk source
//	
//	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
//	
//	RCC->CR &=~ RCC_CR_MSIRANGE;            // clear the range register
//	RCC->CR |= RCC_CR_MSIRANGE_8;           // sel. 16MHz range
//	
//	
//	RCC->CR |= RCC_CR_MSIRGSEL;             // Use the  MSIRANGE  in CR 
//	
//	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
//	
//}











//  toggle PA5 led with TIM2 compare mode

#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);


int main(void)
{
	SysClock_Config();
	
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A  
	GPIOA->MODER = 0xABFFFBFF;  // PA5 in alternate mode(B)
	GPIOA->AFR[0] |= 1<<20;     // AF register low[0], AFSEL5 = AF1 TIM2 out on PA5
	
	//TIM2 config 
	RCC->APB1ENR1 |= (1<<0); // enable TIM2 clk
	TIM2->PSC = 1600 - 1;    // Divide Fclk(16MHz) by 1600 = 10KHz
	TIM2->ARR = 10000 - 1;   // counting to 10,000 is 1 sec
	TIM2->CCMR1 = 0x30;      // compare mode, toggle OUT bit on match pg 1071
	TIM2->CCR1  = 0;         // value to compare to CNT current value (here at overflow)
	TIM2->CCER  |= 1<<0;     // enable channel 1 (CCR1)
	TIM2->CNT = 0;           // clear the timer counter
	TIM2->CR1 = 1<<0;        // enable TIM2 in default upcounting  mode
	
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