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

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define BIT(n)	(1U << (n))
#define DELAY 7

#define RCC_AHB1ENR (*((volatile uint32_t *)0x40023830))
#define GPIOE_MODER (*((volatile uint32_t *)0x40021000))
#define GPIOE_ODR	(*((volatile uint32_t *)0x40021014))

void delay(uint32_t n);

int main(void)
{
	// Enable Clock to port E
	RCC_AHB1ENR |= BIT(4);

	// Enable output mode to portE pin PE11, PE12, PE13, PE14
	GPIOE_MODER &= ~(BIT(23)|BIT(25)|BIT(27)|BIT(29));
	GPIOE_MODER |=  (BIT(22)|BIT(24)|BIT(26)|BIT(28));

	// initially set all the pins to low
	GPIOE_ODR &= ~(BIT(11)|BIT(12)|BIT(13)|BIT(14));
	while(1)
	{
		GPIOE_ODR = BIT(11);
		delay(DELAY);
		GPIOE_ODR = BIT(12);
		delay(DELAY);
		GPIOE_ODR = BIT(13);
		delay(DELAY);
		GPIOE_ODR = BIT(14);
		delay(DELAY);
	}

	return 0;
}

void delay(uint32_t n)
{
	for(volatile uint32_t i=0;i<n;i++)
		for(volatile uint32_t j=0;j<2200;j++);
}
