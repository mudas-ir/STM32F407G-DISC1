/*
 *  1 second = 1000     millisecond
 *  1 second = 1000000  macrosecond
 *
 *
 */

#include "stm32f4xx.h"

#define BIT(n) (1U << (n))

void Delay_MilliSecond(uint32_t time);
void Delay_MicroSecond(uint32_t time);

int main()
{
    //  Clock Configuration
    RCC->AHB1ENR |= BIT(3); //  Enable Clock to port D

    // GPIO Configuration
    GPIOD->MODER &= ~0xFF000000;
    GPIOD->MODER |= 0x55000000; //  Enable Output Mode for PD12, PD13, PD14, PD15

    while (1)
    {
        GPIOD->BSRR |= 0x0000F000;
        //  Delay_MilliSecond(1000);
        Delay_MicroSecond(1000000);
        GPIOD->BSRR |= 0xF0000000;
        // Delay_MilliSecond(1000);
        Delay_MicroSecond(1000000);
    }
    return 0;
}

// Old my version
// void Delay_MilliSecond(uint32_t time)
// {
//     RCC->APB1ENR |= BIT(3); // Enable TIM5 clock

//     TIM5->SR = 0;          // Clear all flags before starting
//     TIM5->PSC = 0;         // 16MHz
//     TIM5->ARR = 16000 - 1; // Overflow every 1 ms

//     TIM5->CNT = 0;
//     TIM5->CR1 |= BIT(0); // Enable timer

//     for (uint32_t i = 0; i < time; i++)
//     {
//         while (!(TIM5->SR & BIT(0)))
//             ;                // Wait for UIF flag
//         TIM5->SR &= ~BIT(0); // Clear UIF
//     }

//     TIM5->CR1 &= ~BIT(0); // Stop timer
// }

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