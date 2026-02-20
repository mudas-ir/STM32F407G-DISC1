/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Mudassir
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

/*
#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define BIT(n) (1U << (n))

#define RCC_AHB1ENR (*((volatile uint32_t *)0x40023830))
#define RCC_APB2ENR (*((volatile uint32_t *)0x40023844))
#define NVIC_ISER0 	(*((volatile uint32_t *)0xE000E100))

#define GPIOD_MODER	(*((volatile uint32_t *)0x40020C00))
#define GPIOD_ODR	(*((volatile uint32_t *)0x40020C14))

#define GPIOA_MODER	(*((volatile uint32_t *)0x40020000))
#define GPIOA_ODR	(*((volatile uint32_t *)0x40020014))
#define GPIOA_IDR	(*((volatile uint32_t *)0x40020010))
#define GPIOA_PUPDR (*((volatile uint32_t *)0x4002000C))

#define SYSCFG_EXTICR1	(*((volatile uint32_t *)0x40013808))

#define EXTI_IMR	(*((volatile uint32_t *)0x40013C00))
#define EXTI_EMR	(*((volatile uint32_t *)0x40013C04))
#define EXTI_RTSR	(*((volatile uint32_t *)0x40013C08))
#define EXTI_PR 	(*((volatile uint32_t *)0x40013C14))


void delay(uint32_t n);

int main(void)
{
	// Enable Clock to port A and D
	RCC_AHB1ENR |= (BIT(0) | BIT(3));

	// Enable Clock to SYSCFG which is on APB2 bus
	RCC_APB2ENR |= BIT(14);  // Enable SYSCFG clock

	// Enable output mode for port D pin PD13 and PD15
	GPIOD_MODER &= ~(BIT(27)|BIT(31));
	GPIOD_MODER |=  (BIT(26)|BIT(30));

	// Enable Input mode for port A pin PA0 (user switch)
	GPIOA_MODER &= ~(BIT(0)|BIT(1));


	GPIOA_PUPDR |= BIT(1); // Pull-down for PA0


	// Configure PA0 as External Interrupt
	SYSCFG_EXTICR1 &= ~(BIT(0)|BIT(1)|BIT(2)|BIT(3));
	EXTI_IMR 	|= BIT(0);
	EXTI_EMR 	|= BIT(0);
	EXTI_RTSR 	|= BIT(0);

	NVIC_ISER0 |= BIT(6);  // EXTI0 IRQ number = 6

	while(1)
	{
		GPIOD_ODR ^= BIT(13);
		delay(1000);
	}
	return 0;
}

void delay(uint32_t n)
{
	for(volatile uint32_t i=0;i<n;i++)
		for(volatile uint32_t j=0;j<3196;j++);
}

void EXTI0_IRQHandler(void)
{
	for(uint8_t i=0;i<3;i++)
	{
		GPIOD_ODR |= BIT(15);
		delay(500);
		GPIOD_ODR &= ~BIT(15);
		delay(500);
	}
	EXTI_PR |= BIT(0);   // Clear pending bit

}
//*/

#include <stdint.h>

#define BIT(n)		(1U << (n))

#define RCC_AHB1ENR (*((volatile uint32_t *)0x40023830))
#define RCC_APB2ENR (*((volatile uint32_t *)0x40023844))

#define GPIOA_MODER (*((volatile uint32_t *)0x40020000))
#define GPIOA_PUPDR (*((volatile uint32_t *)0x4002000C))
#define GPIOA_IDR 	(*((volatile uint32_t *)0x40020010))

#define GPIOD_MODER (*((volatile uint32_t *)0x40020C00))
#define GPIOD_ODR	(*((volatile uint32_t *)0x40020C14))

#define SYSCFG 		(*((volatile uint32_t *)0x40013808))

#define EXTI_IMR 	(*((volatile uint32_t *)0x40013C00))
#define EXTI_RTSR 	(*((volatile uint32_t *)0x40013C08))
#define EXTI_PR 	(*((volatile uint32_t *)0x40013C14))

#define NVIC_ISER 	(*((volatile uint32_t *)0xE000E100))

void delay(uint32_t n);

int main()
{
	// Configure Clock
	RCC_AHB1ENR |=	(BIT(0)|BIT(3));	//	Enable Clock to Port A & Port D.

	// GPIO Configuration
	GPIOA_MODER &= ~(BIT(0)|BIT(1));	//	Enable Input for PA0 (user Switch)
	GPIOA_PUPDR &= ~(BIT(0)|BIT(1));
	GPIOA_PUPDR |= 	 BIT(1);			//	Enable PullDown for PA0

	GPIOD_MODER &= ~(BIT(27)|BIT(26));
	GPIOD_MODER |= 	 BIT(26);			//	Enable Output for PD13 (orange LED)

	//	External Interrupt Configuration
	RCC_APB2ENR |= BIT(14);				//	Enable clock to SYSCFG
	SYSCFG &= ~(0xF);					//	Connect PA0 to EXTI0.

	EXTI_IMR |= BIT(0);
	EXTI_RTSR |= BIT(0);

	NVIC_ISER |= BIT(6);

	while(1)
	{

	}
	return 0;
}

void EXTI0_IRQHandler(void)
{
	EXTI_PR |= BIT(0);
	for(uint8_t i = 0;i < 3;i++)
	{
		GPIOD_ODR |= BIT(13);
		delay(500);
		GPIOD_ODR &= ~BIT(13);
		delay(500);
	}
}

void delay(uint32_t n)
{
	for(uint32_t i = 0;i < n ;i++)
		for(uint32_t j = 0;j < 3129 ;j++);
}
