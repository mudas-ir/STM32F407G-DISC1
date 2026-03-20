/*
 *  Configure UART using USART module 2
 *	PA2 -> USART2_TX
 *	PA3 -> USART2_RX
 *	Alternate function AF7 for PA2 & PA3
 * */
#include "stm32f4xx.h"

#define BIT(n) (1U << (n))

void UART2_SendChar(char c);
void UART2_SendString(char *string);
char UART2_GetChar(void);
void delay(int n);

volatile char data;

int main(void)
{
    //	Clock Configuration
    RCC->AHB1ENR |= BIT(0);  //	Enable clock to Port A
    RCC->APB1ENR |= BIT(17); //	Enable clock to USART module 2

    //	GPIO Configuration
    GPIOA->MODER &= ~(BIT(7) | BIT(6) | BIT(5) | BIT(4));
    GPIOA->MODER |= (BIT(7) | BIT(5)); //	Enable Alternate Function for PA2 & PA3

    GPIOA->AFR[0] &= ~(0x0000FF00);
    GPIOA->AFR[0] |= (0x00007700); //	Enable AF7 for PA2 & PA3 -> USART2

    //	UART Configuration
    USART2->CR1 &= ~BIT(13); // Disable first

    USART2->BRR = (104 << 4) | 3;        //	Set baud rate to 9600
    USART2->CR1 &= ~BIT(12);             //	1 Start bit, 8 Data bits, n Stop bit
    USART2->CR2 &= ~(BIT(13) | BIT(12)); //	Select 1 Stop bit

    USART2->CR1 |= BIT(3); // TE
    USART2->CR1 |= BIT(2); // RE

    USART2->CR1 |= BIT(13); // Enable USART at end

    while (1)
    {
        UART2_SendString("\n\rEnter a Character : ");
        // delay(1000);
        data = UART2_GetChar();
        UART2_SendString("\n\rThe Entered Character is : ");
        UART2_SendChar(data);
        // delay(1000);
    }

    return 0;
}

void UART2_SendString(char *string)
{
    while (*string)
        UART2_SendChar(*string++);
}

void UART2_SendChar(char c)
{
    USART2->DR = c; // load the data into DR register
    while (!(USART2->SR & BIT(6)))
        ; // Wait for TC to SET.. This indicates that the data has been transmitted
}

char UART2_GetChar(void)
{
    char temp;

    while (!(USART2->SR & BIT(5)))
        ;              // wait for RXNE bit to set
    temp = USART2->DR; // Read the data. This clears the RXNE also
    return temp;
}

/* 16 MHz SYSCLK */
void delay(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
            ;
}
