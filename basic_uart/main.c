//APB1 : USART2
//RCC_APB1ENR1 : USART2 clk enable at BIT 17 
//USART_BRR
//USART_CR1
#include "stm32l4xx.h"                  // Device header

void USART2_Init(void);
void USART2_write(char *data);
void delayMS(int);
	
int main(void)
{
	USART2_Init();
	while(1)
	{
	
		USART2_write("Hello World!\r\n");
		delayMS(10);
	}
}

void USART2_Init(void)
{
	//NB: SYSCLK is default at MSI which is 4MHz
	//PA2 TX
	//PA3 Rx
	RCC->APB1ENR1 |= 1<<17;     // enable USART2 clk source
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A for PA2 for usart2
	
	GPIOA->AFR[0] = 0x0700;     // AF register low[0], AFSEL2 = AF7 for PA2(tx), enable usart2 on PA2
	GPIOA->MODER = 0xABFFFFEF;  //PA2 in alternate mode
	
	USART2->BRR = 0x1A1;        // 4MHz/9600 = 1A1 hex,  pg 1356/1395 of refernce manual
	USART2->CR1 |=  1<<3;       // enable TX mode of USART (BIT3)
	USART2->CR1 |= 1<<0;        // enable USART (do after all USART config.)
}

void USART2_write(char *data)
{
	 unsigned int i = 0;
    while(data[i])
    {
        while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
        USART2->TDR  = data[i];             // Transmit element i of data array.
        i++;                                // Increment variable for array address.
    }
}


void delayMS(int delay)
{
	unsigned ticks = (delay * (SystemCoreClock/ 8)) / 1000 ;
 
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
 
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}