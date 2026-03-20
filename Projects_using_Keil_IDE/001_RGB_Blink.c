/*
 *   The below program blinks the onboard
 *   Green LED   PD12
 *   Orange LED  PD13
 *   Red LED     PD14
 *   Blue LED    PD15
 *   for every one second(not exact)
 */
#include "stm32f4xx.h"
#define BIT(n) (1U << (n))
void delay(uint32_t n);
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
        delay(1000);
        GPIOD->BSRR |= 0xF0000000;
        delay(1000);
    }
    return 0;
}
void delay(uint32_t n)
{
    for (volatile uint32_t i = 0; i < n; i++)
        for (volatile uint32_t j = 0; j < 750; j++)
            ;
}