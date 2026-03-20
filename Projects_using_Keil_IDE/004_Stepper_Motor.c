/*
 *  Program to Drive Stepper Motor Using ULN2003 High voltage, High Current Darlington Array based Driver
 *  PORTE Pins PE1,PE2,PE3 & PE4 are inputs to Drive L1,L2,L3 & L4 Coils of Stepper Motor
 */
//  Stepper Motor Wafe Drive
#if 1
#include "stm32f4xx.h"
#define BIT(n) (1U << (n))

void delay(int n);

int main(void)
{
    //  Enable Clock to port E
    RCC->AHB1ENR |= BIT(4);

    //  Set Pin PE1, PE2, PE3 & PE4 to Output Mode
    GPIOE->MODER &= ~(0xFF << 2);
    GPIOE->MODER |= (BIT(8) | BIT(6) | BIT(4) | BIT(2));

    while (1)
    {
        GPIOE->ODR = BIT(1); //  Turn ON PE1
        delay(1);
        GPIOE->ODR = BIT(2); //  Turn ON PE2
        delay(1);
        GPIOE->ODR = BIT(3); //  Turn ON PE3
        delay(1);
        GPIOE->ODR = BIT(4); //  Turn ON PE4
        delay(1);
    }
}
/* 16 MHz SYSCLK */
void delay(int n)
{
    int i, j;
    for (j = 0; j < n; j++)
        for (i = 0; i < 2200; i++)
            ;
}
#endif

//  Stepper Motor Half Wave Drive
#if 0
#include "stm32f4xx.h"
#define BIT(n) (1U << (n))

void delay(int n);

int main(void)
{
    //  Enable Clock to port E
    RCC->AHB1ENR |= BIT(4);

    //  Set Pin PE1, PE2, PE3 & PE4 to Output Mode
    GPIOE->MODER &= ~(0xFF << 2);
    GPIOE->MODER |= (BIT(8) | BIT(6) | BIT(4) | BIT(2));

    while (1)
    {
        GPIOE->ODR = BIT(4); //  Turn ON PE4
        delay(1);
        GPIOE->ODR = (BIT(4) | BIT(3)); /* turn ON PE4 & PE3 */
        delay(1);
        GPIOE->ODR = BIT(3); /* turn ON PE3 */
        delay(1);
        GPIOE->ODR = (BIT(3) | BIT(2)); /* turn ON PE3 & PE2 */
        delay(1);
        GPIOE->ODR = BIT(2); /* turn ON PE2 */
        delay(1);
        GPIOE->ODR = (BIT(2) | BIT(1)); /* turn ON PE1 & PE2 */
        delay(1);
        GPIOE->ODR = BIT(1); /* turn ON PE1 */
        delay(1);
        GPIOE->ODR = (BIT(4) | BIT(1)); /* turn ON PE1 & PE4 */
        delay(1);
    }
}
/* 16 MHz SYSCLK */
void delay(int n)
{
    int i, j;
    for (j = 0; j < n; j++)
        for (i = 0; i < 1000; i++)
            ;
}
#endif
