#include "stm32l4xx.h"                  // Device header

void USART2_Init(void);
void delayMS(int);
char USART2_read(void);
void USART2_write(char data);
void LED_play(int);

int main(void)
{
	USART2_Init();
	
	char ch; 
	while(1)
	{
	  ch = USART2_read();
		LED_play(ch);
		//USART2_write(ch);
	}
}

void USART2_Init(void)
{
	//NB: SYSCLK is default at MSI which is 4MHz
	//PA2 TX
	//PA3 Rx
	RCC->APB1ENR1 |= 1<<17;     // enable USART2 clk source BIT 17
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A 
	
	GPIOA->AFR[0] = 0x0700;     // PA2 Tx
	GPIOA->AFR[0] |= 0x7000;     // AF register low[0], AFSEL3 = AF7 for PA3(rx), enable usart2 on PA3
	//GPIOA->MODER = 0xABFFFFBF;  //PA3 in alternate mode
	GPIOA->MODER = 0xABFFF7AF;  //PA2 and PA3 in alternate mode, PA5 output
	
	USART2->BRR = 0x23;         // 4MHz/115200 = 23 hex,  pg 1356/1395 of refernce manual
	USART2->CR1 |=  1<<2;       // enable RX mode of USART RE (BIT 2)
	USART2->CR1 |=  1<<3;       // enable TX mode of USART TE (BIT 3)
	USART2->CR1 |= 1<<0;        // enable USART UE BIT 0 (do after all USART config.)
}

char USART2_read(void)
{
    while(!(USART2->ISR & (1<<5)));     // wait for add to be available on RX buffer
    return (char)USART2->RDR;
}

void USART2_write(char data)
{
    while(data)
    {
        while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
        USART2->TDR  = data;             // Transmit element i of data array.
                               // Increment variable for array address.
    }
}

void LED_play(int value)
{
	value %=16;
	for(; value>0;value--) // blink led the number of times as value
	{
		GPIOA->BSRR  = (1<<5);    // or 0x20;   set PA5 High
		delayMS(100);
		GPIOA->BSRR  = 1<<(5+16);//or use 0x00200000;   // reset PA5 low
		delayMS(100);
	}
	delayMS(400);
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