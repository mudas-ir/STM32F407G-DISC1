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
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define BIT(n)		(1U << (n))
#define RCC_AHB1ENR	(*((volatile uint32_t *)0x40023830))
#define GPIOD_MODER (*((volatile uint32_t *)0x40020C00))
#define GPIOD_ODR	(*((volatile uint32_t *)0x40020C14))

void delay(uint32_t n);

int main(void)
{
	// Enable Clock to GPIOD
	RCC_AHB1ENR |= BIT(3);
	//printf("Enable Clock\n");

	// Enable Output function to pin13 of port D
	GPIOD_MODER &= ~(BIT(27)|BIT(26));
	GPIOD_MODER |= BIT(26);
	//printf("Enable Output\n");
	GPIOD_ODR &= ~BIT(13);
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



