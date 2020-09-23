/*
*Author : Henry Ugochukwu Odoemelem
*Description: stm32L4 Code to read data from MPU6050 IMU using 12C1 on pins PB6(scl) and PB7(sda)
*Date: 23 Sept. 2020
*/

#include "stm32l4xx.h"                  // Device header

// MPU6050 Device adress
#define IMU_ADW 0xD0     // write  
#define IMU_ADR 0xD1     // read


void SysClock_Config(void);
void systickDelayMS(int n);

void i2c_init(void);
void i2c_resetISR(void);
void i2c_readRequest(char saddr,char maddr,int nRegs);
int16_t i2c_readByte(void);
void IMU_COnfig(char saddr,char maddr,char wdata);

int16_t accXD,accYD,accZD;
int16_t gyroXD,gyroYD,gyroZD;

int16_t temperature;

double accX,accY,accZ;
double gyroX,gyroY,gyroZ;

int16_t rdata;

int main(void)
{
	SysClock_Config();
	
	// for LED 
	RCC->AHB2ENR |= (1<<0);       // enable GPIOA clk, port A is Bit 0 on the register
	GPIOA->MODER = 0xABFFF7FF;    // ie ... 0111 1111 1111 MODE5 is PA5, 01 for output
	GPIOA->BSRR  = 1<<(5+16);     //or use 0x00200000;   // reset PA5 low
	
	
	i2c_init();
	
	// set default values of these registers in MPU6050
	// acc FS:+-2g and gryro FS:+-250 deg/s
  IMU_COnfig(IMU_ADW,0x1C,0);
	GPIOA->BSRR  = 1<<(5+16);     // LED off
	IMU_COnfig(IMU_ADW,0x6B,0);
	GPIOA->BSRR  = 1<<(5+16);
	IMU_COnfig(IMU_ADW,0x6C,0);
	GPIOA->BSRR  = 1<<(5+16);
	IMU_COnfig(IMU_ADW,0x19,0);
	GPIOA->BSRR  = 1<<(5+16);
	IMU_COnfig(IMU_ADW,0x1B,0);
	GPIOA->BSRR  = 1<<(5+16);
	
		
	while(1)
	{
	 // Request 14 registers reading starting from 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
	 i2c_readRequest(IMU_ADW,0x3B,14);  
	 accXD = i2c_readByte()<<8|i2c_readByte();  //combine the bytes MSByte<<8|LSByte
	 accYD = i2c_readByte()<<8|i2c_readByte();  
	 accZD = i2c_readByte()<<8|i2c_readByte();  
	 temperature = i2c_readByte()<<8|i2c_readByte(); 
	 gyroXD = i2c_readByte()<<8|i2c_readByte();
	 gyroYD = i2c_readByte()<<8|i2c_readByte();
	 gyroZD = i2c_readByte()<<8|i2c_readByte();
		
	 // convert to g
   accX  = (double)accXD/16384.0;
	 accY  = (double)accYD/16384.0;
	 accZ  = (double)accZD/16384.0;
	 // convert of deg/s 
	 gyroX = (double)gyroXD/131.0;
	 gyroY = (double)gyroYD/131.0;
	 gyroZ = (double)gyroZD/131.0;
	}
	
}

void i2c_init(void)
{
RCC->AHB2ENR|= 1<<1;        //enable gpiob clock rm pg251
RCC->APB1ENR1|= 1<<21;      //enable i2c1 clock rm pg253
GPIOB->MODER = 0xFFFFAEBF;  //set pb6and7 to alternative function rm pg305
GPIOB->AFR[0] =0x44000000;  // AF4 12C on pb6 and 7 rm pg310, uCds pg93
GPIOB->OTYPER =0xC0;        // pb6 and 7 output open drain rm pg306
GPIOB->PUPDR = 0x5100;      // pb6 and 7 pull-up, rm pg307
i2c_resetISR();
}

void i2c_resetISR(void)
{
	//PE set and reset
	I2C1->CR1 = 0x0000;          // PE cleared, SWresest rm pg1325
	I2C1->TIMINGR = 0x10320309;  // I2C at 400kHz from uCds fast mode, as MPU6050 uses fast mode too
  while(I2C1->CR1 & 1<<0);     // wait for at least 3 APB clock cycle b4 set rm pg 1325
  I2C1->CR1|=0x01;             // set PE
}

void i2c_readRequest(char saddr,char maddr,int nRegs)
{
  i2c_resetISR();
	
  //start and send slave address +write mode
	I2C1->CR2 = saddr;           // slave address
	I2C1->CR2 &=~(1<<10);        // write mode
	I2C1->CR2 |= 1<<16;          // NBTYES transmit 1 byte
	I2C1->CR2 &=~ (1<<25);       // software end mode ie 0
	while(I2C1->ISR & 1<<15);    // wait until bus is not busy rm pg1333
	I2C1->CR2 |= 1<<13;          // start condition rm pg1327
	while((I2C1->CR2 & 1<<13));  // wait until start bit is reset, meaning address sent
	
	//send register address
	while(!(I2C1->ISR & 1<<1));  // wait until transmit buffer is empty rm pg1336
	I2C1->TXDR = maddr;          // transmit register address
	while(!(I2C1->ISR & 1<<6));  // wait until NBTYES is transferred
	
	//Start Repeat and send slave address + read mode
	I2C1->CR2 = saddr;          // slave address
	I2C1->CR2 |= 1<<10;         // read mode
	I2C1->CR2 |= nRegs<<16;     // read n number of registers(byte)
	I2C1->CR2 |= 1<<25;         // automatic end mode ie 1 after NBTYES read
	I2C1->CR2 |= 1<<13;         // start condition rm pg1327
	while((I2C1->CR2 & 1<<13)); // wait until start bit is reset, meaning address sent
	

}
	

	
int16_t i2c_readByte(void)
{

	
	//read register data
	while(!(I2C1->ISR & 1<<2));    // wait until receive buffer has data
	rdata = (int16_t)(I2C1->RXDR); // read the rx buffer
	
	//Blink LED on PA5 at every read
	GPIOA->BSRR  = (1<<5);
	systickDelayMS(100);
	GPIOA->BSRR  = 1<<(5+16);
	systickDelayMS(100);
	
	return rdata;
}

void IMU_COnfig(char saddr,char maddr,char wdata)
{
  i2c_resetISR();
	
	//start and send slave address +write mode
	I2C1->CR2 = saddr;           // slave address
	I2C1->CR2 &=~(1<<10);        // write mode
	I2C1->CR2 |= 2<<16;          // NBTYES transmit 2 byte
	I2C1->CR2 &=~ (1<<25);       // software end mode ie 0
	while(I2C1->ISR & 1<<15);    // wait until bus is not busy rm pg1333
	I2C1->CR2 |= 1<<13;          // start condition rm pg1327
	while((I2C1->CR2 & 1<<13));  // wait until start bit is reset, meaning address sent
	
	//send/select register address
	while(!(I2C1->ISR & 1<<1));  // wait until transmit buffer is empty rm pg1336
	I2C1->TXDR = maddr;          // transmit register address
	
	//send data to register
	while(!(I2C1->ISR & 1<<1));  // wait until transmit buffer is empty rm pg1336
	I2C1->TXDR = wdata;          // send data to register
	
	//Manual stop condition
	while(!(I2C1->ISR & 1<<6));  // wait until NBTYES is transferred ie TC = 1
	I2C1->CR2 |= 1<<14;          // stop condition
	while((I2C1->ISR & 1<<6));   // TC = 0 at stop condition
	
	//LED on after each write
	GPIOA->BSRR  = (1<<5);
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