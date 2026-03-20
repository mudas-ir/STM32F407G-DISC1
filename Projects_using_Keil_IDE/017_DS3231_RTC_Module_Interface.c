
/* P9_1.c - I2C byte write to a DS1337
 *
 * This program initializes the I2C and sends a
 * command to the DS1337 to turn on the 1 Hz output.
 * 1 Hz output is an open-drain output. It needs a pull-up
 * to observe the signal.
 * No errors or acknowledgement are checked.
 *
 * The connections
 * I2C1_SCL -> PB08
 * I2C1_SDA -> PB09
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0
 */
#if 1
#include "stm32f4xx.h"
#define SLAVE_ADDR 0x68    /* 1101 000.    DS1337 */

void delayMs(int n);

void I2C1_init(void);
int I2C1_byteWrite(char saddr, char maddr, char data);

int main(void)
{
    I2C1_init();

    I2C1_byteWrite(SLAVE_ADDR, 0x00, 0x40);  /* Seconds */
	  I2C1_byteWrite(SLAVE_ADDR, 0x01, 0x25);  /* min		  */
	  I2C1_byteWrite(SLAVE_ADDR, 0x02, 0x10);  /* hours   */
  
}

void I2C1_init(void) 
{
    RCC->AHB1ENR |=  2;                     /* Enable GPIOB clock */
    RCC->APB1ENR |=  0x00200000;	        /* Enable I2C1 clock */

    /* configure PB8, PB9 pins for I2C1 */
    GPIOB->AFR[1]   &= ~0x000000FF;         /* PB8, PB9 I2C1 SCL, SDA */
    GPIOB->AFR[1]   |=  0x00000044;
    GPIOB->MODER    &= ~0x000F0000;         /* PB8, PB9 use alternate function */
    GPIOB->MODER    |=  0x000A0000;
    GPIOB->OTYPER   |=  0x00000300;         /* make PB8(SCL) & PB9(SDA) output open-drain */
    GPIOB->PUPDR    &= ~0x000F0000;        
    GPIOB->PUPDR    |=  0x00050000;					/* with pull-ups */

    I2C1->CR1       =   0x8000;             /* software reset I2C1 */
    I2C1->CR1       &= ~0x8000;             /* out of reset */
    I2C1->CR2       =   0x0010;             /* peripheral clock is 16 MHz */
    I2C1->CCR       =   80;                 /* standard mode, 100kHz clock */
    I2C1->TRISE     =   17;                 /* maximum rise time */
    I2C1->CR1       |=  0x0001;             /* enable I2C1 module */
}

/* this funtion writes a byte of data to the memory location maddr of
 * a device with I2C slave device address saddr.
 * For simplicity, no error checking or error report is done.
 */
int I2C1_byteWrite(char saddr, char maddr, char data) 
{
    volatile int tmp;
    
    while (I2C1->SR2 & 2);                  /* wait until bus not busy */
    
    I2C1->CR1 |=  0x100;                    /* generate start */
    while (!(I2C1->SR1 & 1));               /* wait until start flag is set */
    
    I2C1->DR = saddr << 1;                  /* transmit slave address */
    while (!(I2C1->SR1 & 2));               /* wait until addr flag is set */
    tmp = I2C1->SR2;                        /* clear addr flag */
    
    while (!(I2C1->SR1 & 0x80));            /* wait until data register empty */
    I2C1->DR = maddr;                       /* send memory address */
    
    while (!(I2C1->SR1 & 0x80));            /* wait until data register empty */
    I2C1->DR = data;                        /* transmit data */
    
    while (!(I2C1->SR1 & 4));               /* wait until transfer finished */
    I2C1->CR1 |= 0x200;                     /* generate stop */

    return 0;
}
#endif

#if 1
/* P9_2.c - I2C byte read from a DS1337
 *
 * This program initializes the I2C and continuously reads
 * the register 0 of the DS1337.
 * Register 0 contains the second count. The clock is powered-up
 * enabled and the second count is incrementing. The bit 0 is
 * used to turn on and off the LED of the -N-u-c-l-e-o- Discovery board. The green LED
 * should blink every second.
 * No errors or acknowledgement are checked.
 *
 * The connections
 * I2C1_SCL - PB08
 * I2C1_SDA - PB09
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0
 */

#include "stm32f4xx.h"
#define SLAVE_ADDR 0x68 /* 1101 000.    DS1337 */
#define SEC 0x00
#define MIN 0x01
#define HUR 0x02

void I2C1_init(void);
int I2C1_byteRead(char saddr, char maddr, char *data);
void delayMs(int n);

int main(void)
{
    char data;

    I2C1_init();

    /* configure PD12 for the green LED */
    RCC->AHB1ENR |= 8;           /* enable GPIOD clock */
    GPIOD->MODER &= ~0x03000000; /* clear pin mode */
    GPIOD->MODER |= 0x01000000;  /* set pin to output mode */

    while (1)
    {
        I2C1_byteRead(SLAVE_ADDR, MIN, &data);
        if (data & 1)
            GPIOD->ODR |= 0x00001000; /* turn on LED */
        else
            GPIOD->ODR &= ~0x00001000; /* turn off LED */

        delayMs(10);
    }
}

void I2C1_init(void)
{
    RCC->AHB1ENR |= 2;          /* Enable GPIOB clock */
    RCC->APB1ENR |= 0x00200000; /* Enable I2C1 clock */

    /* configure PB8, PB9 pins for I2C1 */
    GPIOB->AFR[1] &= ~0x000000FF; /* PB8, PB9 I2C1 SCL, SDA */
    GPIOB->AFR[1] |= 0x00000044;
    GPIOB->MODER &= ~0x000F0000; 
    GPIOB->MODER |= 0x000A0000;/* PB8, PB9 use alternate function */
    GPIOB->OTYPER |= 0x00000300; /* output open-drain */
    GPIOB->PUPDR &= ~0x000F0000; 
    GPIOB->PUPDR |= 0x00050000;/* with pull-ups */

    I2C1->CR1 = 0x8000;   /* software reset I2C1 */
    I2C1->CR1 &= ~0x8000; /* out of reset */
    I2C1->CR2 = 0x0010;   /* peripheral clock is 16 MHz */
    I2C1->CCR = 80;       /* standard mode, 100kHz clock */
    I2C1->TRISE = 17;     /* maximum rise time */
    I2C1->CR1 |= 0x0001;  /* enable I2C1 module */
}

/* this funtion reads a byte of data from the memory location
 * maddr of a device with I2C slave device address saddr.
 * For simplicity, no error checking or error report is done.
 */
int I2C1_byteRead(char saddr, char maddr, char *data)
{
    volatile int tmp;

    while (I2C1->SR2 & 2);	/* wait until bus not busy */

    I2C1->CR1 |= 0x100;				/* generate start */
    while (!(I2C1->SR1 & 1));	/* wait until start flag is set */

    I2C1->DR = saddr << 1;		/* transmit slave address + Write */
    while (!(I2C1->SR1 & 2));	/* wait until addr flag is set */
    tmp = I2C1->SR2;					/* clear addr flag */

    while (!(I2C1->SR1 & 0x80));	/* wait until data register empty */
    I2C1->DR = maddr;							/* send memory address */

    while (!(I2C1->SR1 & 0x80));	/* wait until data register empty */

    I2C1->CR1 |= 0x100;					/* generate restart */
    while (!(I2C1->SR1 & 1));		/* wait until start flag is set */
    I2C1->DR = saddr << 1 | 1;	/* transmit slave address + Read */

    while (!(I2C1->SR1 & 2));	/* wait until addr flag is set */
    I2C1->CR1 &= ~0x400;			/* Disable Acknowledge */
    tmp = I2C1->SR2;					/* clear addr flag */

    I2C1->CR1 |= 0x200;	/* generate stop after data received */

    while (!(I2C1->SR1 & 0x40));	/* Wait until RXNE flag is set */
    *data++ = I2C1->DR;						/* Read data from DR */

    return 0;
}

/* 16 MHz SYSCLK */
void delayMs(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++);
}
#endif