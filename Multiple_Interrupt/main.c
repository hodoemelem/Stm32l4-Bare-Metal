#include "stm32l4xx.h"                  // Device header

//Led: PA5
//push Button:PC13
//PB10 conneted to GND or VCC

void SysClock_Config(void);
void systickDelayMS(int n);
void EXTI15_10_IRQHandler(void); // External interrupt callback function

volatile int pb10_counter;
volatile int pc13_counter;
int main(void)
{
	SysClock_Config();
	
	__disable_irq(); // disable global interrupts request initially
	
	RCC->AHB2ENR |= (1<<0)|(1<<1)|(1<<2);    // GPIO A,B,C clk enable
	RCC->APB2ENR |=  (1<<0);   // enable clock for SYSCFG
	
	
	GPIOA->MODER = 0xABFFF7FF; // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output mode
	GPIOB->MODER = 0xFFCFFEBF; // MODE10 for PB10, input mode
	GPIOC->MODER = 0xF3FFFFFF; // ie 1111 0011 1111 ...  MODE13 is PC13, 00 for input mode
	
	
	SYSCFG->EXTICR[3] |= 0x20;  // select EXTI13 for PC13 interrupt with 0x20
	SYSCFG->EXTICR[2] |= 0x100; // select EXTI10 for PB10 interrupt with 0x100
	
	EXTI->IMR1 |= 0x2000;      // unmask EXTI13
	EXTI->FTSR1 |= 0x2000;     // select falling edge intrreupt trigger on pin 13
	
	EXTI->IMR1 |= 0x400;       // unmask EXTI10
	EXTI->FTSR1 |= 0x400;      // select falling edge trigger for PB10
	EXTI->RTSR1 = 1<<10;
	GPIOB->PUPDR =0x100000;   //  enable pullup resistor on PB10
 
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);  // enable IRQ for EXT115 to 10
	 
	__enable_irq();    // enable global interrupt
	
	while(1);
}


//ISR for  EXTI15 to 10
void EXTI15_10_IRQHandler(void)
{
	
	if(EXTI->PR1 & 0x2000){ // PC13 
		EXTI->PR1 = 0x2000;         // clear pin 13 interrupt pending flag
		pc13_counter++;
		
		GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
   	systickDelayMS(300);
	  GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	  systickDelayMS(300);
		
		
	}
	else if(EXTI->PR1 & 0x0400){ //PB10
		
		EXTI->PR1 = 0x0400;          // clear pin 10 interrupt pending flag
		pb10_counter++;
		
		GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
	  systickDelayMS(300);
	  GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	  systickDelayMS(300);
	
  	GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
	  systickDelayMS(300);
	  GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	  systickDelayMS(300);
		
		GPIOA->BSRR  = (1<<5);         // or 0x20;   set PA5 High
	  systickDelayMS(300);
	  GPIOA->BSRR  = 1<<(5+16);    // or 0x002;   reset PA5
	  systickDelayMS(300);
		
		
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
