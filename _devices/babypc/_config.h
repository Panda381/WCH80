
// ----------------------------------------------------------------------------
//                            Clock Setup
// ----------------------------------------------------------------------------

// Frequency of HSE external oscillator
#ifndef HSE_VALUE
#define HSE_VALUE	25000000
#endif

// frequency of HSI internal oscillator 24MHz
#ifndef HSI_VALUE
#define HSI_VALUE	25000000
#endif

// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
#ifndef SYSCLK_SRC
#define SYSCLK_SRC	5
#endif

// PLL multiplier
#ifndef PLLCLK_MUL
#define PLLCLK_MUL	2		// only *2 supported; 24 MHz * 2 = 48 MHz
#endif

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
#ifndef SYSCLK_DIV
#define SYSCLK_DIV	1
#endif

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
#ifndef ADCCLK_DIV
#define ADCCLK_DIV	2		// CH32V0: max. 24 MHz (48 / 2 = 24 MHz)
#endif

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#ifndef SYSTICK_MS
#define SYSTICK_MS	0
#endif

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#ifndef HCLK_PER_US
#define HCLK_PER_US	50
#endif

// ----------------------------------------------------------------------------
//                             Device setup
// ----------------------------------------------------------------------------

// Videomodes (B&W mono):
//	0 ... graphics mode 128x64 pixels, required memory 1024 B (driver size 738 B in RAM)
//	1 ... graphics mode 160x120 pixels, required memory 2400 B (driver size 738 B in RAM)
//	2 ... text mode 32x24 characters of 8x8 pixels (resolution 256x192 pixels, pseudographics 64x48 pixels), required memory 768 bytes, font 8x8 pixels 2048 bytes in Flash (driver size 768 B in RAM)
//	3 ... text mode 40x30 characters of 8x8 pixels (resolution 320x240 pixels, pseudographics 80x60 pixels), required memory 1200 bytes, font 8x8 pixels 2048 bytes in Flash (driver size 768 B in RAM)
//	4 ... text mode 80x30 characters of 8x8 pixels (resolution 640x240 pixels, pseudographics 160x60 pixels), required memory 2400 bytes, font 8x8 pixels 2048 bytes in Flash (driver size 762 B in RAM)
//	5 ... text mode 32x24 characters in ZX-80 format (NEWLINE=0x76 is new line) (driver size 828 B or 728 B in RAM)
//	6 ... text mode 40x30 characters of 8x8 pixels with ZX Spectrum font of 128 characters
//	7 ... text mode 32x24 characters of 8x8 pixels with ZX-80/ZX-81 font of 64 characters
#ifndef VMODE
#define VMODE	1
#endif

// USART communication divider (baudrate = HCLK/div, HCLK=50000000, div=min. 16; 50 at 50MHz -> 1MBaud, 1 byte = 10us)
#ifndef CPU_UART_DIV
#define CPU_UART_DIV	HCLK_PER_US // USART CPU baudrate divider (baudrate = HCLK/div, HCLK=50000000, div=min. 16)
#endif

// default device setup
#ifndef USE_DRAW
#define USE_DRAW	1	// 1=use graphics drawing functions
#endif

#ifndef USE_PRINT
#define USE_PRINT	1	// 1=use text printing functions
#endif

#ifndef USE_KEY
#define USE_KEY		1	// 1=use keyboard support
#endif

#ifndef USE_SOUND
#define USE_SOUND	1	// 1=use sound support
#endif

#ifndef USE_DISP
#define USE_DISP	1	// 1=use display support
#endif

#ifndef KEYCNT_REL
#define KEYCNT_REL	4	// keyboard counter - release interval in 1/60 sec
#endif

#ifndef KEYCNT_PRESS
#define KEYCNT_PRESS	20	// keyboard counter - first repeat in 1/60 sec
#endif

#ifndef KEYCNT_REPEAT
#define KEYCNT_REPEAT	6	// keyboard counter - next repeat in 1/60 sec
#endif

// ----------------------------------------------------------------------------
//                          Peripheral clock enable
// ----------------------------------------------------------------------------

// Periferals
#ifndef ENABLE_SRAM
#define ENABLE_SRAM	1		// SRAM enable
#endif

#ifndef ENABLE_FLASH
#define ENABLE_FLASH	1		// FLASH enable (needed to rewrite RESET function)
#endif

#ifndef ENABLE_WWDG
#define ENABLE_WWDG	0		// Window watchdog enable
#endif

#ifndef ENABLE_PWR
#define ENABLE_PWR	1		// Power module enable
#endif

#ifndef ENABLE_AFI
#define ENABLE_AFI	1		// I/O auxiliary function enable
#endif

#ifndef ENABLE_PA
#define ENABLE_PA	1		// PA port enable
#endif

#ifndef ENABLE_PB
#define ENABLE_PB	0		// PB port enable
#endif

#ifndef ENABLE_PC
#define ENABLE_PC	1		// PC port enable
#endif

#ifndef ENABLE_PD
#define ENABLE_PD	1		// PD port enable
#endif

#ifndef ENABLE_TIM1
#define ENABLE_TIM1	1		// TIM1 module enable
#endif

#ifndef ENABLE_TIM2
#define ENABLE_TIM2	1		// TIM2 module enable
#endif

#ifndef ENABLE_SPI1
#define ENABLE_SPI1	1		// SPI1 module enable
#endif

#ifndef ENABLE_USART1
#define ENABLE_USART1	1		// USART1 module enable
#endif

#ifndef ENABLE_I2C1
#define ENABLE_I2C1	1		// I2C1 module enable
#endif
