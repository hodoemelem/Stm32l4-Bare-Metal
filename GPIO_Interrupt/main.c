#include "stm32l4xx.h"                  // Device header

//Led: PA5
//push Button:PC13
void SysClock_Config(void);
void systickDelayMS(int n);
void EXTI15_10_IRQHandler(void); // External interrupt callback function

int main(void)
{
	SysClock_Config();
	
	__disable_irq(); // disable global interrupts request initially
	
	RCC->AHB2ENR |= (1<<0);    // enable GPIOA clk, port A is Bit 0 on the register
	RCC->AHB2ENR |= (1<<2);    // enable GPIOC clk, port C is Bit 2 on the register
	RCC->APB2ENR |=  (1<<0);   // enable clock for SYSCFG
	
	
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output mode
	GPIOC->MODER = 0xF3FFFFFF; // ie 1111 0011 1111 ...  MODE13 is PC13, 00 for input mode
	SYSCFG->EXTICR[3] = 0x20;  // select EXTI13 for PC13 interrupt with 0x20
	
	EXTI->IMR1 |= 0x2000;      // unmask EXTI13
	EXTI->FTSR1 |= 0x2000;     // select falling edge intrreupt trigger on pin 13
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);  // enable IRQ for EXT115 to 10
	 
	__enable_irq();    // enable global interrupt
	
	while(1);
}


//ISR for  EXTI15 to 10
void EXTI15_10_IRQHandler(void)
{
	GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
	systickDelayMS(300);
	GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	systickDelayMS(300);
	
	GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
	systickDelayMS(300);
	GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	systickDelayMS(300);
	
	EXTI->PR1 |= 0x2000;         // clear interrupt pending flag
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
