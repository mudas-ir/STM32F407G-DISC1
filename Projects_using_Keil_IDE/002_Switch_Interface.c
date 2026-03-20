/*
 *   The below program turns on the Green LED(PD12) when the user switch(PA0) is pressed
 */
#include "stm32f407xx.h"
#define BIT(n) (1U << (n))
int main()
{
    //  Clock Configuration
    RCC->AHB1ENR |= (BIT(3) | BIT(0)); //  Enable Clock to port A & D

    //  GPIO Configuration
    GPIOA->MODER &= ~0x3; //  Enable Input mode for PA0 user switch
    GPIOA->PUPDR &= ~0x3;
    GPIOA->PUPDR |= 0x2; //  Enable Pull-down for PA0 user switch
    GPIOD->MODER &= ~(BIT(24) | BIT(25));
    GPIOD->MODER |= BIT(24); //  Enable PD12 Green LED as Output

    while (1)
    {
        if (GPIOA->IDR & BIT(0))
            GPIOD->BSRR |= BIT(12);
        else
            GPIOD->BSRR |= BIT(28);
    }
    return 0;
}