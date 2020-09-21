#include "stm32l4xx.h"                  // Device header
#include <stdio.h>
void USART2_Init(void);
void delayMS(int);
int USART2_read(void);
int USART2_write(int);

int fgetc(FILE *f);
int fputc(int c, FILE *f);
void SysClock_Config(void);

int main(void)
{
	SysClock_Config();
	USART2_Init();
	
	int n;
	char str[100]; 
	
	printf("Hello from the other side\r\n");
	fprintf(stdout," test for stdout\r\n");
	fprintf(stderr," test for stderr\r\n");
	
	while(1)
	{
		printf("How old are you?");
		scanf("%d",&n);
		printf("Your age is : %d\r\n",n);
		
		printf("Enter your first name: ");
		gets(str);
		printf("I like your style: ");
		//puts(str);
		//printf("\r\n");
	}
}

void USART2_Init(void)
{
	//NB: SYSCLK is default at MSI which is 4MHz
	//PA2 TX
	//PA3 Rx
	RCC->APB1ENR1 |= 1<<17;     // enable USART2 clk source BIT 17
	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A 
	
	GPIOA->AFR[0] |= 0x7700;    // enable usart2 on PA3, and PA2
	GPIOA->MODER = 0xABFFF7AF;  // PA2 and PA3 in alternate mode, PA5 output
	
	//USART2->BRR = 0x1A1;         // 4MHz/9600 = 1A1 hex,  pg 1356/1395 of refernce manual
	USART2->BRR = 0x683;         // 16MHz/9600 = 683 hex,  pg 1356/1395 of refernce manual
	USART2->CR1 =  (1<<2) |     // enable RX mode of USART RE (BIT 2)
	               (1<<3);      // enable TX mode of USART TE (BIT 3)
	USART2->CR1 |= 1<<0;        // enable USART UE BIT 0 (do after all USART config.)
}

int USART2_read(void)
{
    while(!(USART2->ISR & (1<<5)));     // wait for add to be available on RX buffer
    return USART2->RDR;
}

int USART2_write(int data)
{
        while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
        USART2->TDR  = data & 0XFF;             // Transmit element i of data array.
        return data;
}


/*
if you define your own version of __FILE, your own fputc() and ferror() functions,
and the __stdout object, you can use all of the printf() family, fwrite(), fputs(), 
puts() and the C++ object std::cout unchanged from the library.
*/

struct __File
{
	int handle;
};

FILE __stdin = {0};
FILE __stdout = {1};
FILE __stderr = {2};


//for scanf and its family
int fgetc(FILE *f)// write character to file f 
{
	int c;
	c = USART2_read();
	if(c == '\r')//carriage return ie Enter key
	{
		USART2_write(c);  // echo the inputed character(enter)
		c = '\n'; // newline
	}
	// NOT SURE ABOUT THIS PART
	USART2_write(c);   // write the read character to console
	return c;
}


//for printf and its family
int fputc(int c, FILE *f)
{
	return USART2_write(c);
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


void delayMS(int delay)
{
	unsigned ticks = (delay * (SystemCoreClock/ 8)) / 1000 ;
 
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
 
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}