#include "stm32l4xx.h"                  // Device header

void SysClock_Config(void);
void systickDelayMS(int n);
void USART2_Init(void);
void LED_play(int);
void USART2_IRQHandler(void);


int main(void)
{
	SysClock_Config();
	
	__disable_irq(); // disable global interrupts request initially
	USART2_Init();   // UART initiallization

	USART2->CR1 |= 1<<5;      // RE interrupt enable
	NVIC_EnableIRQ(USART2_IRQn);  // enable USART2 IRQ 
	 
	__enable_irq();    // enable global interrupt
	
	while(1);
}


//ISR for UART
void USART2_Init(void)
{

	//PA2 TX
	//PA3 Rx
	RCC->APB1ENR1 |= 1<<17;     // enable USART2 clk source BIT 17
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A 
	
	GPIOA->AFR[0] |= 0x7000;    // enable usart2 on PA3
	GPIOA->MODER = 0xABFFF7AF;  // PA2 and PA3 in alternate mode, PA5 output
	
	USART2->BRR = 0x8B;         // 16MHz/115200 = 8B hex,  pg 1356/1395 of refernce manual
	USART2->CR1 |=  (1<<2);      // enable RX mode of USART RE (BIT 2)  
	USART2->CR1 |= 1<<0;        // enable USART UE BIT 0 (do after all USART config.)
}

void LED_play(int value)
{
	value %=16;
	for(; value>0;value--) // blink led the number of times as value
	{
		GPIOA->BSRR  = (1<<5);    // or 0x20;   set PA5 High
		systickDelayMS(300);
		GPIOA->BSRR  = 1<<(5+16);//or use 0x00200000;   // reset PA5 low
		systickDelayMS(300);
	}
	systickDelayMS(300);
}

void USART2_IRQHandler(void)
{
	int c;
	if(USART2->ISR & (1<<5))
	{
	    c = USART2->RDR;
		  LED_play(c);
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
