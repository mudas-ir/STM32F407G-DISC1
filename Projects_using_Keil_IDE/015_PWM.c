/*
 *	Configuring PA0 as PWM (using Timer 2)
 *	50% duty cycle
 *	frequency 16000Hz
 * */

#if 0
#include "stm32f4xx.h"

#define BIT(n) (1U << (n))

void delay(uint32_t n);

int main(void)
{
    // Enable clocks
    RCC->AHB1ENR |= BIT(0); // GPIOA clock
    RCC->APB1ENR |= BIT(0); // TIM2 clock

    // Set PA0 as Alternate Function
    GPIOA->MODER &= ~(BIT(0) | BIT(1));
    GPIOA->MODER |= BIT(1);

    // AF1 for TIM2
    GPIOA->AFR[0] &= ~(0xF);
    GPIOA->AFR[0] |= BIT(0); // AF1

    // Timer configuration
    TIM2->CR1 = 0;   // Disable Counter
    TIM2->PSC = 0;   // Prescaler
    TIM2->ARR = 999; // 16kHz

    // PWM Mode 1
    TIM2->CCMR1 &= ~(7 << 4);
    TIM2->CCMR1 |= (6 << 4);

    TIM2->CCMR1 |= BIT(3); // Enable preload

    TIM2->CCR1 = 500; // 50% duty

    TIM2->CCER |= BIT(0); // Enable CH1 output

    TIM2->CR1 |= BIT(0); // Enable counter

    while (1)
    {
        for (uint32_t i = 10; i < 999; i = i + 10)
        {
            TIM2->CCR1 = i;
            delay(3000);
        }
    }
}

void delay(uint32_t n)
{
    volatile uint32_t i, j;
    for (i = 0; i < n; i++)
    {
        for (j; j < 3129; j++)
        {
        }
    }
}
#endif

#if 1
/**
 *	TIM_2_CH2->AF1->PA1
 * */
#include "stm32f407xx.h"

#define BIT(n) (1U << (n))

void delay(uint32_t n);

int main()
{
    //	Clock Configuration
    RCC->AHB1ENR |= BIT(0); //	Enable Clock to port A
    RCC->APB1ENR |= BIT(0); //	Enable Clock to Timer 2

    //	GPIOA Configuration
    GPIOA->MODER &= ~(BIT(3) | BIT(2));
    GPIOA->MODER |= BIT(3); //	Enable Alternate Function mode for PA1

    GPIOA->AFR[0] &= ~(BIT(7) | BIT(6) | BIT(5) | BIT(4));
    GPIOA->AFR[0] |= BIT(4); //	map PA1 to TIM2_CH2

    //	Timer 2 Channel 2 Configuration
    TIM2->CR1 &= ~BIT(0); //	Disable Counter
    TIM2->PSC = 0;        //	Set Prescaler value to 0
    TIM2->ARR = 999;      //	load 999 in auto reload register for 16KHz frequency

    TIM2->CCMR1 &= ~(BIT(14) | BIT(13) | BIT(12));
    TIM2->CCMR1 |= (BIT(14) | BIT(13)); //	select PWM mode 1

    TIM2->CCMR1 |= BIT(11); //	Enable Preload

    TIM2->CCR2 = 500; //	load 500 in TIM2 capture/compare register 2 for 50% duty cycle

    TIM2->CCER |= BIT(4); //	Enable Capture/Compare 2 output

    TIM2->CR1 |= BIT(0); //	Enable Counter

    while (1)
    {
        for (uint32_t i = 10; i < 999; i = i + 10)
        {
            TIM2->CCR2 = i;
            delay(30000);
        }
    }
}

void delay(uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        for (uint32_t j; j <= 3129; j++)
            ;
}
#endif
