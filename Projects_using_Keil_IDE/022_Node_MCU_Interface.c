/*
 *  The below code reads temperature  and humidity from DHT11 sensor and transmit temperature  value to nodeMCU through UART for every 5 seconds.
 *
 *  The out pin of DHT11 is Connected to PD0 pin
 *  UART module 2 is used
 *  PA2 -> USART2_TX
 *	PA3 -> USART2_RX
 *	Alternate function AF7 for PA2 & PA3
 */

#include "stm32f4xx.h"
#include <stdio.h>

#define BIT(n) (1U << (n))

//----------DHT11---------START--------------------
void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read(void);
void Delay_MicroSecond(uint32_t time);
void Delay_MilliSecond(uint32_t time);

void DHT11_init(void);

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
volatile int16_t SUM, RH, TEMP;

uint8_t Presence = 0;
//----------DHT11-----------END--------------------

//----------UART---------START--------------------
void UART_init(void);
void UART2_SendChar(char c);
void UART2_SendString(char *string);
char UART2_GetChar(void);
void delay(int n);

volatile char data;
//----------UART-----------END--------------------

char buffer[10];

int main()
{
    DHT11_init();
    UART_init();
    while (1)
    {
        DHT11_Start();
        Presence = DHT11_Check_Response();

        if (Presence)
        {
            Rh_byte1 = DHT11_Read();
            Rh_byte2 = DHT11_Read();
            Temp_byte1 = DHT11_Read();
            Temp_byte2 = DHT11_Read();
            SUM = DHT11_Read();

            if (SUM == (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2))
            {
                TEMP = Temp_byte1;
                RH = Rh_byte1;
            }
            else
            {
                TEMP = -1;
                RH = -1;
            }
        }
        else
        {
            TEMP = -2;
            RH = -2;
        }
        /* Transmit prompt message */
        sprintf(buffer, "%d\n", TEMP);
        UART2_SendString(buffer);
        // UART2_SendChar(TEMP);
        // UART2_SendString("\n\r");
        // delayMs(500);
        Delay_MilliSecond(5000); // VERY IMPORTANT
    }
    return 0;
}
void UART_init(void)
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
}

void UART2_SendString(char *string)
{
    while (*string)
        UART2_SendChar(*string++);
}

void UART2_SendChar(char c)
{
    // USART2->DR = c; // load the data into DR register
    // while (!(USART2->SR & BIT(6)))
    //     ; // Wait for TC to SET.. This indicates that the data has been transmitted
    while (!(USART2->SR & BIT(7)))
        ; // TXE
    USART2->DR = c;
}

char UART2_GetChar(void)
{
    char temp;

    while (!(USART2->SR & BIT(5)))
        ;              // wait for RXNE bit to set
    temp = USART2->DR; // Read the data. This clears the RXNE also
    return temp;
}

void DHT11_init(void)
{
    //  Clock Configuration
    RCC->AHB1ENR |= BIT(3); //  Enable Clock to port D

    GPIOD->PUPDR &= ~(BIT(1) | BIT(0));
    GPIOD->PUPDR |= BIT(0); // Pull-up

    GPIOD->OSPEEDR |= BIT(1) | BIT(0); // High speed
}

uint8_t DHT11_Read(void)
{
    uint8_t i = 0, j;
    for (j = 0; j < 8; j++)
    {
        while (!(GPIOD->IDR & BIT(0)))
            ; // wait for pin to go high
        Delay_MicroSecond(30);

        if (!(GPIOD->IDR & BIT(0)))
            i &= ~(1 << (7 - j)); // write 0
        else
            i |= (1 << (7 - j)); // write 1

        while (GPIOD->IDR & BIT(0))
            ; // wait for pin to go low
    }
    return i;
}

uint8_t DHT11_Check_Response(void)
{
    uint8_t Response = 0;
    Delay_MicroSecond(40);

    if (!(GPIOD->IDR & BIT(0)))
    {
        Delay_MicroSecond(80);
        if (GPIOD->IDR & BIT(0))
            Response = 1;
        else
            Response = 0;
    }
    while (GPIOD->IDR & BIT(0))
        ; // wait until pin goes low again

    return Response;
}

void DHT11_Start(void)
{
    // Output mode
    GPIOD->MODER &= ~(BIT(1) | BIT(0));
    GPIOD->MODER |= BIT(0);

    GPIOD->ODR &= ~(BIT(0)); // LOW
    Delay_MilliSecond(18);

    GPIOD->ODR |= BIT(0); // HIGH
    Delay_MicroSecond(30);

    // Input mode
    GPIOD->MODER &= ~(BIT(1) | BIT(0));
}

void Delay_MilliSecond(uint32_t time)
{
    if (time == 0)
        return; //  handle edge case

    RCC->APB1ENR |= BIT(3); // Enable TIM5 clock

    TIM5->CR1 = 0;         // Stop timer
    TIM5->PSC = 16000 - 1; // 1 KHz → 1 ms tick
    TIM5->ARR = time - 1;  // total delay = time ms

    TIM5->CNT = 0;

    TIM5->EGR |= BIT(0); // Force update (loads PSC/ARR)
    TIM5->SR = 0;        // NOW clear UIF (VERY IMPORTANT)

    TIM5->CR1 |= BIT(0); // Start timer

    while (!(TIM5->SR & BIT(0)))
        ; // Wait for overflow

    TIM5->CR1 &= ~BIT(0); // Stop timer
}

void Delay_MicroSecond(uint32_t time)
{
    if (time == 0)
        return; //  handle edge case

    RCC->APB1ENR |= BIT(3); // Enable TIM5 clock

    TIM5->CR1 = 0;      // Stop timer
    TIM5->PSC = 16 - 1; // 1 MHz → 1 µs tick
    TIM5->ARR = time - 1;

    TIM5->CNT = 0;

    TIM5->EGR |= BIT(0); // Force update (loads PSC/ARR)
    TIM5->SR = 0;        // NOW clear UIF (VERY IMPORTANT)

    TIM5->CR1 |= BIT(0); // Start timer

    while (!(TIM5->SR & BIT(0)))
        ; // Wait for overflow

    TIM5->CR1 &= ~BIT(0); // Stop timer
}