/* Toggle GREEN LED
 * Blue push button Switch(B1) is connected to PA0. 
 * Whenever B1 is pressed an Interrupt occurs and RED LED Toggles 3 times and return from  ISR
 * Button B1(user switch) of the Discovery board is connected to PA0. 
 * When the button is pressed, PA0 becomes High(Rising Edge).
 * The green LED (LD2) is connected to PD12.
 * A high on PA0 turns on the LED.
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0
 */
 
 #include "stm32f4xx.h"
 
void delayMs(int n);

int main(void) 
{
    RCC->AHB1ENR |=  9;                 /* enable Cloock to GPIOA & GPIOD */
    RCC->APB2ENR |= (1<<14);			/* enable Cloock to System Configuration Controller */
	
    GPIOD->MODER |=  0x11000000;        /* set pin 12&14 to output mode Green & orange LED*/
    GPIOA->MODER &= ~(0x00000003);      /* clear pin mode to input mode */
		
	SYSCFG->EXTICR[0]&=~(0xF<<0);       /* enable EXTI0 for PA0 */
	EXTI->IMR |= (1<<0); 				/* Interrupt is not Masked on LIne0 */
	EXTI->RTSR |= (1<<0);				/* Rising Edge trigger enabled (for Event and Interrupt) for input line0 */
	EXTI->FTSR &= ~(1<<0);				/* Falling Edge trigger disabled (for Event and Interrupt) for input line0 */
    	
	/* Nested Vector Interrupt Controller(NVIC) Interrupt set-enable register(ISER)
	 * NVIC_ISER0 bits 0 to 31 are to set interrupt 0 to 31, respectively
	 * NVIC_ISER0 bits 0 to 31 are to set interrupt 32 to 63, respectively
	 * ........
	 * NVIC_ISER7 bits 0 to 15 are for interrupt 224 to 239, respectively  //*/
	  
	  NVIC->ISER[0] |= (1<<6);						//Alternately function NVIC_EnableIRQ(EXTI0_IRQn) can be used;*/
	
	  while(1) 
			{
          GPIOD->BSRR = 0x00001000;   /* turn ON green LED */
				  delayMs(300);
          GPIOD->BSRR = 0x10000000;   /* turn OFF green LED */
				  delayMs(300);
      }
}

void EXTI0_IRQHandler(void)
{ int i;
	//EXTI->PR |=(1<<0);
	for(i=0;i<3;i++)
	{
		GPIOD->BSRR = 0x00004000;   /* turn ON RED LED & TURNOFF GREEN LED  */
				  delayMs(300);
          GPIOD->BSRR = 0x40000000;   /* turn OFF RED LED */
				  delayMs(300);
	}
	EXTI->PR |=(1<<0);
}

/* 16 MHz SYSCLK */
void delayMs(int n) 
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++) ;
}