// -----------------Mudassir Logic-----------------
#if 1
#include "stm32f407xx.h"
#include <stdbool.h>

#define BIT(n) (1U << (n))

void IR_init(void);
void Delay_MilliSecond(uint32_t time);
bool IR_1(void);

uint32_t count = 0;

int main()
{
    IR_init();
    while (1)
    {
        if (IR_1())
        {
            count++;
            // Delay_MilliSecond(500);
        }
    }

    return 0;
}

void IR_init(void)
{
    RCC->AHB1ENR |= BIT(2);               //  Enable Clock to PORT C.
    GPIOC->MODER &= ~(BIT(31) | BIT(30)); //  make PC15 as Input pin.
    GPIOC->PUPDR &= ~(BIT(31) | BIT(30)); // clear
    GPIOC->PUPDR |= BIT(30);              // pull-up
}

bool IR_1(void)
{
    bool sense = false;

    while (!(GPIOC->IDR & BIT(15)))
    {
        Delay_MilliSecond(5);
        while (GPIOC->IDR & BIT(15))
        {
            sense = true;
            break;
        }
        break;
    }
    return sense;
}

void Delay_MilliSecond(uint32_t time)
{
    if (time == 0)
        return; //  handle edge case

    RCC->APB1ENR |= BIT(3); // Enable TIM5 clock

    TIM5->PSC = 16000 - 1; // 1 KHz → 1 ms tick
    TIM5->CR1 = 0;         // Stop timer
    TIM5->ARR = time - 1;  // total delay = time ms

    TIM5->CNT = 0;

    TIM5->EGR |= BIT(0); // Force update (loads PSC/ARR)
    TIM5->SR = 0;        // NOW clear UIF (VERY IMPORTANT)

    TIM5->CR1 |= BIT(0); // Start timer

    while (!(TIM5->SR & BIT(0)))
        ; // Wait for overflow

    TIM5->CR1 &= ~BIT(0); // Stop timer
}
#endif

// -----------------AI Logic-----------------
#if 0
#include "stm32f407xx.h"
#include <stdbool.h>

#define BIT(n) (1U << (n))
#define IR1_PIN 15

#define IR1_READ() ((GPIOC->IDR & BIT(IR1_PIN)) ? 1 : 0)

void IR_init(void);
void TIM5_Init(void);
void Delay_MilliSecond(uint32_t time);
bool IR_1(void);

uint32_t count = 0;

int main(void)
{
    IR_init();
    TIM5_Init();

    while (1)
    {
        if (IR_1())
        {
            if (count < 1000)
                count++;
        }
    }
}

void IR_init(void)
{
    RCC->AHB1ENR |= BIT(2);

    GPIOC->MODER &= ~(BIT(31) | BIT(30)); // input

    GPIOC->PUPDR &= ~(BIT(31) | BIT(30)); // clear
    GPIOC->PUPDR |= BIT(30);              // pull-up
}

bool IR_1(void)
{
    static uint8_t prev = 1;
    uint8_t curr = IR1_READ();

    if (prev == 1 && curr == 0)
    {
        Delay_MilliSecond(20);

        if (IR1_READ() == 0)
        {
            prev = curr;
            return true;
        }
    }

    prev = curr;
    return false;
}

void TIM5_Init(void)
{
    RCC->APB1ENR |= BIT(3); // Enable TIM5 clock
    TIM5->PSC = 16000 - 1;  // 1 KHz → 1 ms tick
    TIM5->CR1 = 0;          // Stop timer
}

void Delay_MilliSecond(uint32_t time)
{
    if (time == 0)
        return;           //  handle edge case
    TIM5->ARR = time - 1; // total delay = time ms
    TIM5->CNT = 0;

    TIM5->EGR |= BIT(0); // Force update (loads PSC/ARR)
    TIM5->SR = 0;        // NOW clear UIF (VERY IMPORTANT)

    TIM5->CR1 |= BIT(0); // Start timer

    while (!(TIM5->SR & BIT(0)))
        ; // Wait for overflow

    TIM5->CR1 &= ~BIT(0); // Stop timer
}
#endif
