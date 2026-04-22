#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include <stdint.h>
#include "stm32f407mmk.h"
#define BIT(n)		(1U << (n))

int main(void)
{
    RCC->AHB1ENR |=  BIT(3);	//	Enable Clock to GPIOD

    //	Set PD13 pin to output mode
    GPIOD->MODER &=  ~(BIT(31)|BIT(30));
    GPIOD->MODER |=  BIT(30);

    SysTick->LOAD = 16000000-1;	//	Load Counter Value 16M/16M=1sec
	SysTick->CTRL = (1<<2)|(1<<0);	//	Enable counter and select system clock as clock source
	SysTick->VAL  = 0;	//	Initialize counter by clearing Current value register

    while(1)
    {
    	if(SysTick->CTRL & BIT(16))
    		GPIOD->ODR  ^= BIT(15);	// Toggle BLUE LED
    }
}
