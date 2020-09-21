#include "stm32l4xx.h"                  // Device header


void SysClock_Config(void);
void systickDelayMS(int n);

void i2c_init(void);
char i2c_start(char);
char i2c_rep_start(char);
void i2c_stop(void);
char i2c_write(char);
char i2c_readAck(void);
char i2c_readNak(void);



int main(void)
{

	
}

void i2c_init(void)
{
RCC->AHB2ENR|= 1<<1; //enable gpiob clock rm pg251
RCC->APB1ENR1|= 1<<21; //enable i2c1 clock rm pg253
GPIOB->MODER|=0xFFFAFEBF; //set pb8and9 to alternative function rm pg305
GPIOB->AFR[1]|=0x44;     // AF4 12C on pb8 and 9 rm pg310, uCds pg93
GPIOB->OTYPER|=0x300;    // pb8 and 9 output open drain rm pg306
GPIOB->PUPDR = 0x50100;  // pb8 and 9 pull-up, rm pg307
I2C1->CR1 = 0x0000;  // PE cleared, SWresest rm pg1325
I2C1->TIMEOUTR = 0x00303D5B; // (standard mode) I2C freq 100KHz @ 16MHz clk from stm32CubeMX and rm pg1331
while(I2C1->CR1 & 1<<0);  // wait for at least 3 APB clock cycle b4 set rm pg 1325
I2C1->CR1|=0x01;
}
char i2c_start()
{
	while(I2C1->ISR & 1<<15); // wait until bus is not busy rm pg1333
	
	I2C1->CR2 = 1<<13;    // start generation rm pg1327
	while(!(I2C1->ISR & 1<<15));  // wait until start condition is set rm pg1333
	
}

char i2c_rep_start(char saddress)
{
    return i2c_start( saddress );

}

void i2c_stop(void)
{
	
}

char i2c_write(char data )
{
	
}

char i2c_readAck(void)
{
	
}


char i2c_readNak(void)
{
	
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