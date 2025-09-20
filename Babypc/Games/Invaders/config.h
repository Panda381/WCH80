
// ****************************************************************************
//                                 
//                        Project library configuration
//
// ****************************************************************************

#ifndef _CONFIG_H
#define _CONFIG_H

// Pre-set defines (use #if to check):
//	target MCU	MCU serie	MCU class	MCU subclass
//	CH32V002x4	CH32V002	CH32V0		CH32V00X
//	CH32V003x4	CH32V003	CH32V0
//	CH32V004x6	CH32V004	CH32V0		CH32V00X
//	CH32V005x6	CH32V005	CH32V0		CH32V00X
//	CH32V006x4	CH32V006	CH32V0		CH32V00X
//	CH32V006x8	CH32V006	CH32V0		CH32V00X
//	CH32V007x8	CH32V007	CH32V0		CH32V00X
//	CH32X033x8	CH32V033	CH32V0		CH32V03X
//	CH32X035x7	CH32V035	CH32V0		CH32V03X
//	CH32X035x8	CH32V035	CH32V0		CH32V03X
//	CH32V103x6	CH32V103	CH32V1
//	CH32V103x8	CH32V103	CH32V1
//	CH32L103x8	CH32L103	CH32V1

// FLASHSIZE ... Flash size in bytes
// RAMSIZE ... RAM size in bytes
// STACKSIZE ... Stack size in bytes

// default font
#define FONT		NULL		// default system font
#define FONTCOND	FontCond6x6	// default condensed font

// ----------------------------------------------------------------------------
//                            Clock Setup
// ----------------------------------------------------------------------------

// Frequency of HSE external oscillator
//#define HSE_VALUE	25000000	// CH32V0, CH32L103: 4..25 MHz

// frequency of HSI internal oscillator 24MHz
//#define HSI_VALUE	25000000

// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
//#define SYSCLK_SRC	5

// PLL multiplier
//#define PLLCLK_MUL	2		// only *2 supported; 24 MHz * 2 = 48 MHz

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
//#define SYSCLK_DIV	1

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
//#define ADCCLK_DIV	2		// CH32V0: max. 24 MHz (48 / 2 = 24 MHz)

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
//#define SYSTICK_MS	0

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
//#define HCLK_PER_US	50

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
#define VMODE	1

// USART communication divider (baudrate = HCLK/div, HCLK=50000000, div=min. 16; 50 at 50MHz -> 1MBaud, 1 byte = 10us)
//#define CPU_UART_DIV	HCLK_PER_US // USART CPU baudrate divider (baudrate = HCLK/div, HCLK=50000000, div=min. 16)
//#define USE_DRAW	1	// 1=use graphics drawing functions
//#define USE_PRINT	1	// 1=use text printing functions
//#define USE_KEY	1	// 1=use keyboard support
//#define USE_SOUND	1	// 1=use sound support
//#define USE_DISP	1	// 1=use display support
//#define KEYCNT_REL	4	// keyboard counter - release interval in 1/60 sec
//#define KEYCNT_PRESS	20	// keyboard counter - first repeat in 1/60 sec
//#define KEYCNT_REPEAT	6	// keyboard counter - next repeat in 1/60 sec

// ----------------------------------------------------------------------------
//                          Peripheral clock enable
// ----------------------------------------------------------------------------

/*
// System
#define ENABLE_SRAM	1		// SRAM enable
#define ENABLE_FLASH	1		// FLASH enable
#define ENABLE_WWDG	0		// Window watchdog enable
#define ENABLE_PWR	1		// Power module enable
// Ports
#define ENABLE_AFI	1		// I/O auxiliary function enable
#define ENABLE_PA	1		// PA port enable
#define ENABLE_PB	0		// PB port enable
#define ENABLE_PC	1		// PC port enable
#define ENABLE_PD	1		// PD port enable
// ADC
#define ENABLE_ADC1	1		// ADC1 module enable
// Timers
#define ENABLE_TIM1	1		// TIM1 module enable
#define ENABLE_TIM2	1		// TIM2 module enable
// SPI
#define ENABLE_SPI1	1		// SPI1 module enable
// USART
#define ENABLE_USART1	1		// USART1 module enable
// I2C
#define ENABLE_I2C1	1		// I2C1 module enable
// DMA
#define ENABLE_DMA1	1		// DMA1 module enable
*/

#endif // _CONFIG_H
