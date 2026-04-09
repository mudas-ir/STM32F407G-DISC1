/*
 * main.c
 *
 * LCD1602 + PCF8574T demo — bare-metal, Keil MDK
 * Target  : STM32F407G-DISC1
 * Header  : stm32f407xx.h
 *
 * Wiring:
 *   PB6 → SCL  (4.7 kΩ pull-up to 3.3 V)
 *   PB7 → SDA  (4.7 kΩ pull-up to 3.3 V)
 *   LCD VCC → 5 V        LCD GND → GND
 *
 * No CubeMX, no HAL — just registers.
 */

#include "stm32f407xx.h"
#include "lcd_i2c.h"

#define BIT(n) (1U << (n))

/* ── Custom character: heart ♥ ─────────────────────────────────────────────── */
static uint8_t heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000};

/* ── Minimal busy-loop delay (rough ms, no SysTick conflict) ────────────────
 *    LCD_Init already sets up SysTick for precise delays inside the driver.
 *    Use this only AFTER LCD_Init if you need simple waits in main().       */
static void simple_delay_ms(volatile uint32_t ms)
{
    /* ~168 cycles per µs → 168000 cycles per ms */
    while (ms--)
    {
        volatile uint32_t i = 16800UL;
        while (i--)
            ;
    }
}

int main(void)
{
    /* ── Init LCD (this also inits I2C1 and the GPIO pins PB6/PB7) ── */
    LCD_Init();

    /* ── Register a custom character in CGRAM slot 0 ── */
    LCD_CreateChar(0, heart);

    /* ── Row 0: title ── */
    LCD_SetCursor(0, 0);
    LCD_Print("  STM32F407G");
    LCD_WriteChar(0); /* print ♥ from CGRAM slot 0 */
    LCD_WriteChar(' ');

    /* ── Row 1: subtitle ── */
    LCD_SetCursor(0, 1);
    LCD_Print(" LCD I2C 4-bit");

    simple_delay_ms(3000);
    LCD_Clear();

    /* ── Counter loop ── */
    uint32_t count = 0;
    char buf[17];

    LCD_SetCursor(0, 0);
    LCD_Print("   Counter:     ");

    while (1)
    {
        /* Format count as a 10-digit zero-padded number */
        uint32_t tmp = count;
        uint8_t digits[10];
        for (int i = 9; i >= 0; i--)
        {
            digits[i] = '0' + (tmp % 10);
            tmp /= 10;
        }

        LCD_SetCursor(3, 1);
        for (int i = 0; i < 10; i++)
            LCD_WriteChar(digits[i]);

        count++;
        simple_delay_ms(500);
    }
}
