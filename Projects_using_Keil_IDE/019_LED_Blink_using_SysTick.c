/* Toggle ALL(GREEN,ORANGE,RED,BLUE) LED's
 *This program was tested with Keil uVision v5.24a with DFP v2.11.0
 */
 
 #include "stm32f4xx.h"

void delay(int n);

int main(void) {
    RCC->AHB1ENR |=  8;                 /* enable Clock to GPIOD */
    
    GPIOD->MODER |=  0x55000000;        /* set pin to output mode */
    SysTick->LOAD = 8000000-1;					/* Load Counter Vaue 16M/16M=1sec */
	  SysTick->CTRL = (1<<2)|(1<<0);      /* enable counter and select system clock as clock source */
		SysTick->VAL  = 0;									/* initialize counter by clearing Current value register */ 
	
    while(1) 
			{   
				  if(SysTick->CTRL & 0x10000)
          GPIOD->ODR  ^= 0x0000F000;   /* turn ON ALL LED's */
          //delay(300);
	    }
}

/* 16 MHz SYSCLK */
void delay(int n) {
    int i,j;
    for (j=0; j<n ; j++)
        for (i = 0; i < 3195; i++) ;
}