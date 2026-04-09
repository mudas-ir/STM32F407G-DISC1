/*
 * lcd_i2c.h
 *
 * LCD1602 via PCF8574T I2C backpack
 * Bare-metal driver — no HAL, no CubeMX
 * Target : STM32F407G-DISC1
 * Toolchain: Keil MDK  |  Header: stm32f407xx.h
 *
 * Wiring (I2C1):
 *   PB6 → SCL   (with 4.7 kΩ pull-up to 3.3 V)
 *   PB7 → SDA   (with 4.7 kΩ pull-up to 3.3 V)
 *
 * PCF8574T pin mapping:
 *   P0 → RS        P4 → D4
 *   P1 → RW        P5 → D5
 *   P2 → EN        P6 → D6
 *   P3 → Backlight P7 → D7
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f407xx.h"
#include <stdint.h>

/* ── I2C address ──────────────────────────────────────────────────────────────
 * PCF8574T  with A2=A1=A0 tied low → 0x27 (7-bit)
 * PCF8574AT with A2=A1=A0 tied low → 0x3F (7-bit)
 * Change this if your solder-bridges are different.                         */
#define LCD_ADDR        0x27

/* ── PCF8574 bit masks ────────────────────────────────────────────────────── */
#define LCD_RS          (1 << 0)
#define LCD_RW          (1 << 1)
#define LCD_EN          (1 << 2)
#define LCD_BL          (1 << 3)   /* backlight */

/* ── HD44780 command bytes ────────────────────────────────────────────────── */
#define LCD_CLEAR       0x01
#define LCD_RETURNHOME  0x02
#define LCD_ENTRY       0x04
#define LCD_DISPCTRL    0x08
#define LCD_FUNCSET     0x20
#define LCD_CGRAM       0x40
#define LCD_DDRAM       0x80

/* ── Public API ───────────────────────────────────────────────────────────── */
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(const char *str);
void LCD_WriteChar(char c);
void LCD_BacklightOn(void);
void LCD_BacklightOff(void);
void LCD_CreateChar(uint8_t slot, uint8_t map[8]);

#endif /* LCD_I2C_H */
