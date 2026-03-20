/* A to D conversion of channel 1

 * This program converts the analog input from channel 1.
 * Channel 1 is connected to input from PA1.
 * Clock prescaler is left at 0 (divided by 2) and Sampling
 * time is also left at default of 3 cycles.
 * Software trigger is used. The bit 8 of the conversion
 * result is used to turn on/off the LED0. For the full
 * scale of the reference voltage, the LED0 should be on
 * for 8 times.
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0.
 */

#include "stm32f4xx.h"
volatile int result;
int main(void)
{
    /* set up pin PA1 for analog input AN1 */
    RCC->AHB1ENR |= 1;   /* enable GPIOA clock */
    GPIOA->MODER |= 0xC; /* PA1 analog */

    /* setup ADC1 */
    RCC->APB2ENR |= 0x00000100; /* enable ADC1 clock */
    ADC1->CR2 = 0;              /* SW trigger */
    ADC1->SQR3 = 1;             /* conversion sequence starts at ch 1 */
    ADC1->SQR1 = 0;             /* conversion sequence length 1 */
    ADC1->CR2 |= 1;             /* enable ADC1 */

    while (1)
    {
        ADC1->CR2 |= 0x40000000; /* start a conversion */
        while (!(ADC1->SR & 2))
            ;              /* wait for conv complete */
        result = ADC1->DR; /* read conversion result */
    }
}