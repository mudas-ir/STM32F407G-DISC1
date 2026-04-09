#if 0
#include "stm32f407xx.h"

#define LCD_ADDR 0x27
#define LCD_BACKLIGHT 0x08
void I2C1_Init(void);
void I2C1_Write(uint8_t addr, uint8_t data);

void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_Print(char *str);
void delay_ms(int n);

// Control bits
#define EN 0x04
#define RW 0x02
#define RS 0x01

int main(void)
{
    I2C1_Init();
    LCD_Init();

    LCD_Print("Hello STM32");

    while (1)
        ;
}

void I2C1_Init(void)
{
    RCC->AHB1ENR |= (1 << 1);
    RCC->APB1ENR |= (1 << 21);

    // Reset AF
    GPIOB->AFR[1] &= ~0x000000FF;
    GPIOB->AFR[1] |= 0x00000044;

    // Set AF mode
    GPIOB->MODER &= ~0x000F0000;
    GPIOB->MODER |= 0x000A0000;

    // Open drain
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);

    // Pull-up
    GPIOB->PUPDR &= ~0x000F0000;
    GPIOB->PUPDR |= 0x00050000;

    // High speed (recommended)
    GPIOB->OSPEEDR |= 0x000F0000;

    // I2C reset
    I2C1->CR1 |= (1 << 15);
    I2C1->CR1 &= ~(1 << 15);

    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;

    I2C1->CR1 |= (1 << 0);
}

void I2C1_Write(uint8_t addr, uint8_t data)
{
    I2C1->CR1 |= (1 << 8);
    while (!(I2C1->SR1 & (1 << 0)))
        ;

    I2C1->DR = addr << 1;
    while (!(I2C1->SR1 & (1 << 1)))
        ;

    (void)I2C1->SR2;

    I2C1->DR = data | LCD_BACKLIGHT; // ⭐ KEEP BACKLIGHT ON
    while (!(I2C1->SR1 & (1 << 7)))
        ;

    I2C1->CR1 |= (1 << 9);
}

void LCD_SendNibble(uint8_t data)
{
    I2C1_Write(LCD_ADDR, data | EN | LCD_BACKLIGHT);
    delay_ms(1);

    I2C1_Write(LCD_ADDR, (data & ~EN) | LCD_BACKLIGHT);
    delay_ms(1);
}

void LCD_SendCommand(uint8_t cmd)
{
    LCD_SendNibble((cmd & 0xF0));
    LCD_SendNibble((cmd << 4));
}

void LCD_SendData(uint8_t data)
{
    LCD_SendNibble((data & 0xF0) | RS);
    LCD_SendNibble((data << 4) | RS);
}

void LCD_Init(void)
{
    delay_ms(50);

    LCD_SendNibble(0x30);
    delay_ms(5);
    LCD_SendNibble(0x30);
    delay_ms(5);
    LCD_SendNibble(0x30);
    delay_ms(5);
    LCD_SendNibble(0x20); // 4-bit mode

    LCD_SendCommand(0x28); // 4-bit, 2-line
    LCD_SendCommand(0x0E); // Display ON
    LCD_SendCommand(0x01); // Clear
    delay_ms(5);
}

void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
}

void delay_ms(int n)
{
    for (int i = 0; i < n * 4000; i++)
        ;
}
#endif
