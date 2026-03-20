/*
 *	Configuring UART 4
 *	PA0 -> Tx
 *	PA1 -> Rx
 *	Alternate Function 8 (AF8) for PA0 and PA1
 *	This pins are connected to an External HC-05 Bluetooth module
 */
#include "stm32f4xx.h"
#define BIT(n) (1U << (n))

void UART_TX(uint8_t data);
void UART_TX_str(uint8_t *str);
uint8_t UART_RX(void);

uint8_t rx_data;

int main()
{
	//	Clock Configuration
	RCC->AHB1ENR |= BIT(0);	 //	Enable Clock to port A
	RCC->APB1ENR |= BIT(19); //	Enable Clock to UART4

	//	GPIO Configuration
	GPIOA->MODER &= ~(0xF);
	GPIOA->MODER |= (BIT(3) | BIT(1)); //	Enable Alternate Function to PA0 and PA1
	GPIOA->AFR[0] &= ~(0xFF);
	GPIOA->AFR[0] |= (BIT(7) | BIT(3)); //	Enable AF8 to PA0 & PA1 for UART4

	//	UART Configuration
	UART4->CR2 &= ~(BIT(13) | BIT(12));	  //	1 Stop Bit
	UART4->CR1 &= ~BIT(12);				  //	1 Start bit, 8 Data bits, n Stop bit
	UART4->CR1 &= ~BIT(15);				  //	oversampling by 16
	UART4->BRR = ((104 << 4) | (3 << 0)); //	Set Baud rate to 9600
	UART4->CR1 |= (BIT(3) | BIT(2));	  //	Enable Transmitter and Reciever
	UART4->CR1 |= BIT(13);				  //	Enable UART4

	while (1)
	{
		// UART_TX('M');
		// UART_TX('M');
		// UART_TX('K');
		// UART_TX('\n');
		UART_TX_str((uint8_t *)"\nEnter a text : ");
		rx_data = UART_RX();
		UART_TX_str((uint8_t *)"\nRx text is : ");
		UART_TX(rx_data);
	}
	return 0;
}

void UART_TX(uint8_t data)
{
	while (!(UART4->SR & BIT(7)))
		; //	1:Transmit data register empty
	UART4->DR = data;
	while (!(UART4->SR & BIT(6)))
		; //	1:Transmission is complete
}

void UART_TX_str(uint8_t *str)
{
	while (*str)
		UART_TX(*str++);
}

uint8_t UART_RX(void)
{
	while (!(UART4->SR & BIT(5)))
		; //	Wait for data to be recieved
	return UART4->DR;
}