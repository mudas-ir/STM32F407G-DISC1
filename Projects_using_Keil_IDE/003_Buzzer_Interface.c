/*
 *  The below program beeps the Buzzer when user switch is pressed
 *  Buzzer is connected to PB0
 *  Buzzer is Low level Triggred
 */

#include "stm32f407xx.h"

#define BIT(n) (1U << (n))

int main()
{
    // Enable Clock for GPIOA and GPIOB
    RCC->AHB1ENR |= BIT(0) | BIT(1);

    // PA0 input (User switch)
    GPIOA->MODER &= ~(3 << 0);

    // Pull-down
    GPIOA->PUPDR &= ~(3 << 0);
    GPIOA->PUPDR |= (2 << 0);

    // PB0 output
    GPIOB->MODER &= ~(3 << 0);
    GPIOB->MODER |= (1 << 0);

    // PB0 Open Drain
    GPIOB->OTYPER |= BIT(0);

    while (1)
    {
        if (GPIOA->IDR & BIT(0))
            GPIOB->BSRR = BIT(16); // PB0 LOW -> buzzer ON
        else
            GPIOB->BSRR = BIT(0); // PB0 released -> buzzer OFF
    }
}