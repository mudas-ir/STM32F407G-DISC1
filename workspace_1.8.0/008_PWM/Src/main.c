/**
 ******************************************************************************
 * @file           : main.c
 * @author         :
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/*
 *	Configuring PA0 as PWM (using Timer 2)
 *	50% duty cycle
 *	frequency 16000Hz
 * */

#if 0
#include <stdint.h>

#define BIT(n)	(1U << (n))

#define RCC_AHB1ENR	(*((volatile uint32_t *)0x40023830))

#define GPIOA_MODER	(*((volatile uint32_t *)0x40020000))
#define GPIOA_AFRL	(*((volatile uint32_t *)0x40020020))
#define GPIOA_AFRH	(*((volatile uint32_t *)0x40020024))

#define RCC_APB1ENR  (*((volatile uint32_t *)0x40023840))

#define TIM2_CR1     (*((volatile uint32_t *)0x40000000))
#define TIM2_PSC     (*((volatile uint32_t *)0x40000028))
#define TIM2_ARR     (*((volatile uint32_t *)0x4000002C))
#define TIM2_CCR1    (*((volatile uint32_t *)0x40000034))
#define TIM2_CCMR1   (*((volatile uint32_t *)0x40000018))
#define TIM2_CCER    (*((volatile uint32_t *)0x40000020))

void delay(uint32_t n);

int main(void)
{
    // Enable clocks
    RCC_AHB1ENR |= BIT(0);   // GPIOA clock
    RCC_APB1ENR |= BIT(0);   // TIM2 clock

    // Set PA0 as Alternate Function
    GPIOA_MODER &= ~(BIT(0)|BIT(1));
    GPIOA_MODER |= BIT(1);

    // AF1 for TIM2
    GPIOA_AFRL &= ~(0xF);
    GPIOA_AFRL |= BIT(0);    // AF1

    // Timer configuration
    TIM2_CR1 = 0;			// Disable Counter
    TIM2_PSC = 0;            // Prescaler
    TIM2_ARR = 999;          // 16kHz

    // PWM Mode 1
    TIM2_CCMR1 &= ~(7 << 4);
    TIM2_CCMR1 |= (6 << 4);

    TIM2_CCMR1 |= BIT(3);    // Enable preload

    TIM2_CCR1 = 500;         // 50% duty

    TIM2_CCER |= BIT(0);     // Enable CH1 output

    TIM2_CR1 |= BIT(0);      // Enable counter

    while(1)
    {
//    	for(uint32_t i=10;i < 999;i=i+10)
//    	{
//    		TIM2_CCR1 = i;
//    		delay(30000);
//    	}
    }
}

void delay(uint32_t n)
{
	for(uint32_t i = 0;i < n;i++)
		for(uint32_t j;j < 3129;j++);
}
#endif

#if 1
/**
 *	TIM_2_CH2->AF1->PA1
 * */
#include <stdio.h>

#define BIT(n)	(1U << (n))

#define RCC_AHB1ENR	(*((volatile uint32_t *)0x40023830))
#define RCC_APB1ENR	(*((volatile uint32_t *)0x40023840))

#define GPIOA_MODER	(*((volatile uint32_t *)0x40020000))
#define GPIOA_AFRL	(*((volatile uint32_t *)0x40020020))

#define TIM2_CR1	(*((volatile uint32_t *)0x40000000))
#define TIM2_PSC	(*((volatile uint32_t *)0x40000028))
#define TIM2_ARR	(*((volatile uint32_t *)0x4000002C))
#define TIM2_CCR2	(*((volatile uint32_t *)0x40000038))
#define TIM2_CCMR1	(*((volatile uint32_t *)0x40000018))
#define TIM2_CCER	(*((volatile uint32_t *)0x40000020))

int main()
{
	//	Clock Configuration
	RCC_AHB1ENR |= BIT(0);	//	Enable Clock to port A
	RCC_APB1ENR |= BIT(0);	//	Enable Clock to Timer 2

	//	GPIOA Configuration
	GPIOA_MODER &= ~(BIT(3)|BIT(2));
	GPIOA_MODER |=	BIT(3);	//	Enable Alternate Function mode for PA1

	GPIOA_AFRL &= ~(BIT(7)|BIT(6)|BIT(5)|BIT(4));
	GPIOA_AFRL |= BIT(4);	//	map PA1 to TIM2_CH2

	//	Timer 2 Channel 2 Configuration
	TIM2_CR1 &= ~BIT(0);	//	Disable Counter
	TIM2_PSC = 0;			//	Set Prescaler value to 0
	TIM2_ARR = 999;			//	load 999 in auto reload register for 16KHz frequency

	TIM2_CCMR1 &= ~(BIT(14)|BIT(13)|BIT(12));
	TIM2_CCMR1 |= (BIT(14)|BIT(13));	//	select PWM mode 1

	TIM2_CCMR1 |= BIT(11);	//	Enable Preload

	TIM2_CCR2 = 500;		//	load 500 in TIM2 capture/compare register 2 for 50% duty cycle

	TIM2_CCER |= BIT(4);	//	Enable Capture/Compare 2 output

	TIM2_CR1 |= BIT(0);		//	Enable Counter

	while(1);

	return 0;
}
#endif
