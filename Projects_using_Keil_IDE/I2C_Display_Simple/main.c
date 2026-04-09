/*
 * LCD1602 via PCF8574T I2C backpack — 4-bit mode
 * Target    : STM32F407G-DISC1
 * Toolchain : Keil MDK  |  stm32f407xx.h  (no HAL, no CubeMX)
 * Clock     : HSI 16 MHz (default, no PLL needed)
 *
 * I2C1 pins (alternate function set, same as your working code):
 *   PB8 → SCL   AF4   open-drain
 *   PB9 → SDA   AF4   open-drain
 *
 * PCF8574T bit layout (every byte sent over I2C):
 *   Bit7 Bit6 Bit5 Bit4 | Bit3 Bit2 Bit1 Bit0
 *    D7   D6   D5   D4  |  BL   EN   RW   RS
 */

#include "stm32f407xx.h"

/* ── I2C / LCD config ─────────────────────────────────────────────────────── */
#define LCD_ADDR        0x27    /* PCF8574T 7-bit address (A2=A1=A0=0)        */
                                /* Change to 0x3F for PCF8574AT variant        */
#define LCD_BACKLIGHT   0x08    /* P3 = backlight control bit                  */

/* PCF8574T control bit masks */
#define EN              0x04    /* P2 */
#define RW              0x02    /* P1 */
#define RS              0x01    /* P0 */

/* ── Function prototypes ─────────────────────────────────────────────────────*/
void delay_ms(int n);
void I2C1_Init(void);
void I2C1_Write(uint8_t addr, uint8_t data);
void LCD_SendNibble(uint8_t nibble);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(char *str);
void LCD_Clear(void);
void LCD_CreateChar(uint8_t slot, uint8_t map[8]);

/* ════════════════════════════════════════════════════════════════════════════
 * delay_ms
 * Simple busy-loop delay — matches your exact working implementation.
 * ════════════════════════════════════════════════════════════════════════════ */
void delay_ms(int n)
{
    for (int i = 0; i < n * 4000; i++);
}

/* ════════════════════════════════════════════════════════════════════════════
 * I2C1_Init
 * Matches your working code exactly:
 *   PB8=SCL, PB9=SDA, AF4, open-drain, pull-up
 *   HSI 16 MHz → CR2=16, CCR=80, TRISE=17
 * ════════════════════════════════════════════════════════════════════════════ */
void I2C1_Init(void)
{
    /* Enable clocks: GPIOB (bit1) and I2C1 (bit21) */
    RCC->AHB1ENR |= (1 << 1);
    RCC->APB1ENR |= (1 << 21);

    /* PB8, PB9 alternate function → AF4 (I2C1)
     * AFR[1] controls PB8–PB15  (AFRH register)
     * PB8 occupies bits [3:0] of AFRH, PB9 occupies bits [7:4]             */
    GPIOB->AFR[1] &= ~0x000000FF;
    GPIOB->AFR[1] |=  0x00000044;   /* AF4 for PB8 and PB9 */

    /* MODER: AF mode (10) for PB8 and PB9
     * PB8 → bits [17:16], PB9 → bits [19:18]                               */
    GPIOB->MODER &= ~0x000F0000;
    GPIOB->MODER |=  0x000A0000;

    /* OTYPER: open-drain for PB8 and PB9 */
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);

    /* PUPDR: pull-up for PB8 and PB9
     * PB8 → bits [17:16] = 01, PB9 → bits [19:18] = 01                     */
    GPIOB->PUPDR &= ~0x000F0000;
    GPIOB->PUPDR |=  0x00050000;

    /* OSPEEDR: high speed for PB8 and PB9 */
    GPIOB->OSPEEDR |= 0x000F0000;

    /* Software reset I2C1 then release */
    I2C1->CR1 |=  (1 << 15);
    I2C1->CR1 &= ~(1 << 15);

    /* HSI = 16 MHz → APB1 = 16 MHz (no PLL, no prescaler)
     * CR2   = peripheral clock in MHz = 16
     * CCR   = T_high / T_APB1 = 5µs * 16MHz = 80  (100 kHz standard mode)
     * TRISE = (1000ns / 62.5ns) + 1 = 17                                    */
    I2C1->CR2   = 16;
    I2C1->CCR   = 80;
    I2C1->TRISE = 17;

    /* Enable I2C1 peripheral */
    I2C1->CR1 |= (1 << 0);
}

/* ════════════════════════════════════════════════════════════════════════════
 * I2C1_Write
 * Sends a single byte to the PCF8574T.
 * Backlight bit is OR-ed in here so it is NEVER accidentally cleared.
 * ════════════════════════════════════════════════════════════════════════════ */
void I2C1_Write(uint8_t addr, uint8_t data)
{
    /* Generate START */
    I2C1->CR1 |= (1 << 8);
    while (!(I2C1->SR1 & (1 << 0)));   /* Wait for SB (Start Bit) flag */

    /* Send address + write bit (addr << 1, LSB=0) */
    I2C1->DR = addr << 1;
    while (!(I2C1->SR1 & (1 << 1)));   /* Wait for ADDR flag */
    (void)I2C1->SR2;                    /* Clear ADDR by reading SR1 then SR2 */

    /* Send data byte — always keep backlight ON */
    I2C1->DR = data | LCD_BACKLIGHT;
    while (!(I2C1->SR1 & (1 << 7)));   /* Wait for TXE (Tx buffer empty) */

    /* Generate STOP */
    I2C1->CR1 |= (1 << 9);
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_SendNibble
 * Writes one nibble (already positioned in upper 4 bits of `nibble`)
 * to D7-D4 by pulsing EN high then low.
 *
 * `nibble` must already have RS set if sending data (caller's job).
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_SendNibble(uint8_t nibble)
{
    I2C1_Write(LCD_ADDR, nibble | EN);         /* EN high → HD44780 reads */
    delay_ms(1);
    I2C1_Write(LCD_ADDR, nibble & ~EN);        /* EN low  → latch          */
    delay_ms(1);
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_SendCommand
 * Sends a full command byte in two nibbles, RS=0 (instruction register).
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_SendCommand(uint8_t cmd)
{
    LCD_SendNibble(cmd & 0xF0);          /* high nibble, RS=0 */
    LCD_SendNibble((cmd << 4) & 0xF0);   /* low  nibble, RS=0 */
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_SendData
 * Sends a full character byte in two nibbles, RS=1 (data register).
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_SendData(uint8_t data)
{
    LCD_SendNibble((data & 0xF0) | RS);          /* high nibble, RS=1 */
    LCD_SendNibble(((data << 4) & 0xF0) | RS);   /* low  nibble, RS=1 */
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_Init
 * HD44780 power-on sequence for 4-bit mode (datasheet Figure 24).
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_Init(void)
{
    delay_ms(50);                /* >40 ms after Vcc rises to 2.7 V */

    LCD_SendNibble(0x30);        /* 8-bit mode handshake — step 1 */
    delay_ms(5);                 /* >4.1 ms */
    LCD_SendNibble(0x30);        /* 8-bit mode handshake — step 2 */
    delay_ms(5);                 /* >100 µs (we use 5 ms to be safe) */
    LCD_SendNibble(0x30);        /* 8-bit mode handshake — step 3 */
    delay_ms(5);

    LCD_SendNibble(0x20);        /* Switch to 4-bit mode */
    delay_ms(5);

    LCD_SendCommand(0x28);       /* 4-bit bus, 2 lines, 5×8 dots */
    LCD_SendCommand(0x0C);       /* Display ON, cursor OFF, blink OFF */
    LCD_SendCommand(0x06);       /* Entry mode: increment, no shift */
    LCD_SendCommand(0x01);       /* Clear display */
    delay_ms(5);                 /* Clear needs >1.52 ms */
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_SetCursor
 * Move cursor to (col, row). col: 0–15, row: 0 or 1.
 * DDRAM addresses — Row 0: 0x00, Row 1: 0x40
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t address = (row == 0) ? (0x00 + col) : (0x40 + col);
    LCD_SendCommand(0x80 | address);
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_Print
 * Print a null-terminated string at the current cursor position.
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_Print(char *str)
{
    while (*str)
        LCD_SendData(*str++);
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_Clear
 * Clear display and return cursor to home.
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    delay_ms(5);
}

/* ════════════════════════════════════════════════════════════════════════════
 * LCD_CreateChar
 * Define a custom 5×8 glyph in CGRAM slot (0–7).
 * Use LCD_SendData('\x00') … '\x07' to print custom chars.
 * ════════════════════════════════════════════════════════════════════════════ */
void LCD_CreateChar(uint8_t slot, uint8_t map[8])
{
    slot &= 0x07;
    LCD_SendCommand(0x40 | (slot << 3));    /* Set CGRAM address */
    for (int i = 0; i < 8; i++)
        LCD_SendData(map[i]);
    LCD_SendCommand(0x80);                  /* Return to DDRAM (home) */
}

/* ════════════════════════════════════════════════════════════════════════════
 * main — demo
 * ════════════════════════════════════════════════════════════════════════════ */

/* Custom char: heart */
uint8_t heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

int main(void)
{
    I2C1_Init();
    LCD_Init();

    /* Register heart glyph in CGRAM slot 0 */
    LCD_CreateChar(0, heart);

    /* Line 0 */
    LCD_SetCursor(0, 0);
    LCD_Print("Hello STM32!");
    LCD_SendData('\x00');   /* print heart */

    /* Line 1 */
    LCD_SetCursor(0, 1);
    LCD_Print("I2C LCD 4-bit");

    while (1);
}
