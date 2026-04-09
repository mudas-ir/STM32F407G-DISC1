/*
 * lcd_i2c.c
 *
 * LCD1602 via PCF8574T — bare-metal 4-bit I2C driver
 * Target : STM32F407G-DISC1  (168 MHz, HSE 8 MHz PLL)
 * Toolchain : Keil MDK  |  stm32f407xx.h  (no HAL)
 *
 * ── I2C1 pins ──────────────────────────────────────────────────────────────
 *   PB6  SCL  AF4   open-drain   4.7 kΩ pull-up to 3.3 V
 *   PB7  SDA  AF4   open-drain   4.7 kΩ pull-up to 3.3 V
 */

#include "lcd_i2c.h"

/* ═══════════════════════════════════════════════════════════════════════════
 * 1.  Delay helpers
 *     SysTick is used for millisecond delays (simple polling, no IRQ).
 *     The CPU is assumed to run at 168 MHz — adjust SYS_CLOCK_HZ if needed.
 * ═══════════════════════════════════════════════════════════════════════════ */
#define SYS_CLOCK_HZ   168000000UL

static void delay_ms(uint32_t ms)
{
    /* Configure SysTick for a one-shot count-down */
    SysTick->LOAD  = (SYS_CLOCK_HZ / 1000UL) - 1UL;
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (ms--)
    {
        SysTick->VAL = 0;
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
}

static void delay_us(uint32_t us)
{
    SysTick->LOAD  = (SYS_CLOCK_HZ / 1000000UL) - 1UL;
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while (us--)
    {
        SysTick->VAL = 0;
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 2.  I2C1 bare-metal init
 *     Standard mode 100 kHz, 7-bit addressing, APB1 = 42 MHz (168/4).
 * ═══════════════════════════════════════════════════════════════════════════ */

/* APB1 = 168 MHz / 4 = 42 MHz */
#define APB1_FREQ_MHZ   42U

static void I2C1_Init(void)
{
    /* 1. Clocks */
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;   /* GPIOB */
    RCC->APB1ENR  |= RCC_APB1ENR_I2C1EN;    /* I2C1  */

    /* 2. PB6 = SCL, PB7 = SDA
     *    Alternate function 4 (I2C1), open-drain, no internal pull-up
     *    (external 4.7 kΩ pull-ups required on the PCB)               */

    /* MODER: AF mode (10) for PB6, PB7 */
    GPIOB->MODER &= ~((3U << (6*2)) | (3U << (7*2)));
    GPIOB->MODER |=  ((2U << (6*2)) | (2U << (7*2)));

    /* OTYPER: open-drain */
    GPIOB->OTYPER |= (1U << 6) | (1U << 7);

    /* OSPEEDR: high speed */
    GPIOB->OSPEEDR |= (3U << (6*2)) | (3U << (7*2));

    /* PUPDR: no pull (external resistors do the job) */
    GPIOB->PUPDR &= ~((3U << (6*2)) | (3U << (7*2)));

    /* AFR[0] = AFRL: AF4 for PB6 (bits 27:24) and PB7 (bits 31:28) */
    GPIOB->AFR[0] &= ~((0xFU << (6*4)) | (0xFU << (7*4)));
    GPIOB->AFR[0] |=  ((4U   << (6*4)) | (4U   << (7*4)));

    /* 3. Reset I2C1 peripheral, then release */
    I2C1->CR1 |=  I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /* 4. CR2: peripheral clock frequency in MHz (must match APB1) */
    I2C1->CR2 = APB1_FREQ_MHZ;   /* 42 */

    /* 5. CCR: standard mode (Sm), 100 kHz
     *    T_high = T_low = 5 µs
     *    CCR = T_high / T_APB1 = 5 µs * 42 MHz = 210             */
    I2C1->CCR = 210U;

    /* 6. TRISE: maximum rise time in Sm = 1000 ns
     *    TRISE = (1000 ns / (1/42 MHz)) + 1 = 43                  */
    I2C1->TRISE = 43U;

    /* 7. Enable */
    I2C1->CR1 |= I2C_CR1_PE;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 3.  I2C1 single-byte blocking write
 * ═══════════════════════════════════════════════════════════════════════════ */

static void I2C1_WriteByte(uint8_t addr7, uint8_t data)
{
    /* ── START ── */
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));          /* wait SB flag */

    /* ── ADDRESS (write, LSB = 0) ── */
    I2C1->DR = (uint8_t)(addr7 << 1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR));        /* wait ADDR flag */
    (void)I2C1->SR2;                             /* clear ADDR by reading SR1+SR2 */

    /* ── DATA ── */
    while (!(I2C1->SR1 & I2C_SR1_TXE));         /* wait TXE */
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));         /* wait BTF (byte transferred) */

    /* ── STOP ── */
    I2C1->CR1 |= I2C_CR1_STOP;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 4.  PCF8574T / LCD low-level helpers
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Global backlight state — starts ON */
static uint8_t g_backlight = LCD_BL;

/*
 * Write one byte to the PCF8574T.
 * This directly drives all 8 output lines of the I2C expander.
 */
static void PCF_Write(uint8_t data)
{
    I2C1_WriteByte(LCD_ADDR, data);
}

/*
 * Pulse the EN line.
 * HD44780 latches data on the FALLING edge of EN.
 *   tEH (EN high) >= 450 ns  — we use ~1 µs
 *   tEL (EN low)  >= 25  ns
 */
static void LCD_PulseEN(uint8_t data)
{
    PCF_Write(data | LCD_EN);    /* EN high */
    delay_us(1);
    PCF_Write(data & ~LCD_EN);   /* EN low  */
    delay_us(50);                /* >37 µs execution time for most commands */
}

/*
 * Send one nibble (upper 4 bits of `nibble` argument) to D7-D4.
 * `mode`: 0 = command (RS low),  LCD_RS = data (RS high).
 *
 * PCF8574 byte layout:
 *   [7]D7 [6]D6 [5]D5 [4]D4 [3]BL [2]EN [1]RW [0]RS
 */
static void LCD_WriteNibble(uint8_t nibble, uint8_t mode)
{
    uint8_t byte = (nibble & 0xF0) | g_backlight | mode;
    LCD_PulseEN(byte);
}

/*
 * Send a full byte as two nibbles, high nibble first.
 * `mode`: 0 = command,  LCD_RS = character data.
 */
static void LCD_Send(uint8_t byte, uint8_t mode)
{
    LCD_WriteNibble( byte & 0xF0,        mode);   /* high nibble */
    LCD_WriteNibble((byte << 4) & 0xF0,  mode);   /* low  nibble */
}

static void LCD_Cmd(uint8_t cmd)
{
    LCD_Send(cmd, 0);          /* RS = 0 → instruction register */
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 5.  Public API
 * ═══════════════════════════════════════════════════════════════════════════ */

void LCD_Init(void)
{
    /* ── Bring up I2C1 and GPIO ── */
    I2C1_Init();

    /* ── HD44780 power-on sequence for 4-bit mode ──────────────────────────
     *
     * After power-on the HD44780 is in 8-bit mode by default.
     * We must send the special 3-step handshake (upper nibble = 0x3)
     * before switching to 4-bit mode.  See HD44780U datasheet Fig.24.
     *
     * Step 1: wait >40 ms after Vcc rises to 2.7 V                      */
    delay_ms(50);

    /* Put expander outputs in a known low state, backlight on */
    PCF_Write(g_backlight);
    delay_ms(100);

    /* Step 2–4: send 0x30 three times as single nibbles (still 8-bit handshake) */
    LCD_WriteNibble(0x30, 0);   delay_ms(5);   /* >4.1 ms */
    LCD_WriteNibble(0x30, 0);   delay_us(200); /* >100 µs */
    LCD_WriteNibble(0x30, 0);   delay_us(200);

    /* Step 5: switch to 4-bit mode (send 0x20 as a single nibble) */
    LCD_WriteNibble(0x20, 0);   delay_us(200);

    /* ── Now fully in 4-bit mode — configure display ── */

    /* Function set: 4-bit bus, 2 lines, 5×8 dots */
    LCD_Cmd(LCD_FUNCSET | 0x08);   /* 0x28 */
    delay_ms(1);

    /* Display OFF */
    LCD_Cmd(LCD_DISPCTRL);          /* 0x08 */
    delay_ms(1);

    /* Clear display (needs >1.52 ms) */
    LCD_Cmd(LCD_CLEAR);
    delay_ms(2);

    /* Entry mode: increment cursor, no display shift */
    LCD_Cmd(LCD_ENTRY | 0x02);      /* 0x06 */
    delay_ms(1);

    /* Display ON, cursor OFF, blink OFF */
    LCD_Cmd(LCD_DISPCTRL | 0x04);   /* 0x0C */
    delay_ms(1);
}

void LCD_Clear(void)
{
    LCD_Cmd(LCD_CLEAR);
    delay_ms(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    /*
     * DDRAM address map:
     *   Row 0: 0x00–0x0F
     *   Row 1: 0x40–0x4F
     */
    uint8_t row_offset = (row == 0) ? 0x00 : 0x40;
    LCD_Cmd(LCD_DDRAM | (col + row_offset));
}

void LCD_WriteChar(char c)
{
    LCD_Send((uint8_t)c, LCD_RS);   /* RS = 1 → data register */
}

void LCD_Print(const char *str)
{
    while (*str)
        LCD_WriteChar(*str++);
}

void LCD_BacklightOn(void)
{
    g_backlight = LCD_BL;
    PCF_Write(g_backlight);
}

void LCD_BacklightOff(void)
{
    g_backlight = 0x00;
    PCF_Write(g_backlight);
}

void LCD_CreateChar(uint8_t slot, uint8_t map[8])
{
    slot &= 0x07;   /* only 8 CGRAM slots (0–7) */
    LCD_Cmd(LCD_CGRAM | (slot << 3));
    for (int i = 0; i < 8; i++)
        LCD_Send(map[i], LCD_RS);
}
