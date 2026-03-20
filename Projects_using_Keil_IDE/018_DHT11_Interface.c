/*
 *  The below program Interfaces DHT11 Sensor
 *  The out pin of DHT11 is Connected to PD0 pin
 *
 *
 */

#include "stm32f4xx.h"

#define BIT(n) (1U << (n))

void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read(void);
void Delay_MicroSecond(uint32_t time);
void Delay_MilliSecond(uint32_t time);

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;

int main()
{
    //  Clock Configuration
    RCC->AHB1ENR |= BIT(3); //  Enable Clock to port D

    GPIOD->PUPDR &= ~(BIT(1) | BIT(0));
    GPIOD->PUPDR |= BIT(0); // Pull-up

    GPIOD->OSPEEDR |= BIT(1) | BIT(0); // High speed

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

                Temperature = (float)TEMP;
                Humidity = (float)RH;
            }
        }

        Delay_MilliSecond(1000); // VERY IMPORTANT
    }
    return 0;
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