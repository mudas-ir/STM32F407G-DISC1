/*
 *  PC1 -> DO of LDR module
 *  PC2 -> AO of LDR module
 *
 */
#include "stm32f4xx.h"

volatile uint16_t AO_result;
volatile uint8_t DO_result;

#define BIT(n) (1U << (n))

int main(void)
{
    RCC->AHB1ENR |= BIT(2); //  Enable Clock to port C

    GPIOC->MODER &= ~(BIT(2) | BIT(3)); //  PA1 Input
    GPIOC->MODER |= (BIT(4) | BIT(5));  //  PA2 Analog mode

    // ADC Configuration
    RCC->APB2ENR |= BIT(9);            //  Enable Clock to ADC module 2
    ADC2->CR1 &= ~(BIT(24) | BIT(25)); //  set 12-bit Resolution
    ADC2->CR2 |= BIT(1);               //  Continuous conversion mode
    ADC2->SMPR1 |= (7 << 6);           // 480 cycles for channel 12
    ADC2->SQR1 &= ~(0x00F00000);       //  channel sequence length: 1
    ADC2->SQR3 = 12;                   // Channel 12 (PC2)
    ADC2->CR2 |= BIT(0);               //    Enable ADC
    ADC2->CR2 |= BIT(30);              //    Star Conversion
    while (1)
    {
        while (!(ADC2->SR & BIT(1)))
            ;                 //    Wait for the Conversion to Complete
        AO_result = ADC2->DR; //    Read Conversion result
        DO_result = (GPIOC->IDR & BIT(1));
    }
}