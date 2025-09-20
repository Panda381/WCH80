
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

// ----------------------------------------------------------------------------
//                            Clock Setup
// ----------------------------------------------------------------------------

// Frequency of HSE external oscillator
#define HSE_VALUE	24000000	// CH32V0, CH32L103: 4..25 MHz

// System clock source: 1=HSI, 2=HSE, 3=HSE_Bypass, 4=PLL_HSI, 5=PLL_HSE, 6=PLL_HSE_Bypass, 7=PLL_HSI/2, 8=PLL_HSE/2, 9=PLL_HSE_Bypass/2
#define SYSCLK_SRC	4

// PLL multiplier
#define PLLCLK_MUL	2		// only *2 supported; 24 MHz * 2 = 48 MHz

// System clock divider: 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256 (default 1)
#define SYSCLK_DIV	1

// ADC clock divider: (1,) 2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128 (default 1 or 2)
#define ADCCLK_DIV	2		// CH32V0: max. 24 MHz (48 / 2 = 24 MHz)

// number of HCLK clock cycles per 1 us (used with Wait functions)
// - If you want to change frequency of system clock run-time, use a variable instead of constant.
#define HCLK_PER_US	48

// increment of system time in [ms] on SysTick interrupt (0=do not use SysTick interrupt)
#define SYSTICK_MS	0

// ----------------------------------------------------------------------------
//                          Peripheral clock enable
// ----------------------------------------------------------------------------

// System
#define ENABLE_SRAM	1		// SRAM enable
#define ENABLE_FLASH	1		// FLASH enable
#define ENABLE_WWDG	1		// Window watchdog enable
#define ENABLE_PWR	1		// Power module enable
#define ENABLE_CRC	1		// CRC module enable
#define ENABLE_BKP	1		// Backup module enable
#define ENABLE_FSMC	1		// FSMC module enable
#define ENABLE_RNG	1		// RNG module enable
#define ENABLE_SDIO	1		// SDIO module enable
#define ENABLE_DVP	1		// DVP module enable
#define ENABLE_BLEC	1		// BLEC module enable
#define ENABLE_BLES	1		// BLES module enable
// Ports
#define ENABLE_AFI	1		// I/O auxiliary function enable
#define ENABLE_PA	1		// PA port enable
#define ENABLE_PB	1		// PB port enable
#define ENABLE_PC	1		// PC port enable
#define ENABLE_PD	1		// PD port enable
#define ENABLE_PE	1		// PE port enable
// ADC
#define ENABLE_ADC1	1		// ADC1 module enable
#define ENABLE_ADC2	1		// ADC2 module enable
// DAC
#define ENABLE_DAC	1		// DAC module enable
// Timers
#define ENABLE_TIM1	1		// TIM1 module enable
#define ENABLE_TIM2	1		// TIM2 module enable
#define ENABLE_TIM3	1		// TIM3 module enable
#define ENABLE_TIM4	1		// TIM4 module enable
#define ENABLE_TIM5	1		// TIM5 module enable
#define ENABLE_TIM6	1		// TIM6 module enable
#define ENABLE_TIM7	1		// TIM7 module enable
#define ENABLE_TIM8	1		// TIM8 module enable
#define ENABLE_TIM9	1		// TIM9 module enable
#define ENABLE_TIM10	1		// TIM10 module enable
#define ENABLE_LPTIM	1		// LPTIM module enable
// SPI
#define ENABLE_SPI1	1		// SPI1 module enable
#define ENABLE_SPI2	1		// SPI2 module enable
// USART
#define ENABLE_USART1	1		// USART1 module enable
#define ENABLE_USART2	1		// USART2 module enable
#define ENABLE_USART3	1		// USART3 module enable
#define ENABLE_USART4	1		// USART4 module enable
#define ENABLE_USART5	1		// USART5 module enable
#define ENABLE_USART6	1		// USART6 module enable
#define ENABLE_USART7	1		// USART7 module enable
#define ENABLE_USART8	1		// USART8 module enable
// I2C
#define ENABLE_I2C1	1		// I2C1 module enable
#define ENABLE_I2C2	1		// I2C2 module enable
// CAN
#define ENABLE_CAN1	1		// CAN1 module enable
#define ENABLE_CAN2	1		// CAN2 module enable
// DMA
#define ENABLE_DMA1	1		// DMA1 module enable
#define ENABLE_DMA2	1		// DMA2 module enable
// USB
#define ENABLE_USBFS	1		// USBFS module enable
#define ENABLE_USBPD	1		// USBPD module enable
#define ENABLE_USBD	1		// USBD module enable
#define ENABLE_USBHS	1		// USBHS module enable
#define ENABLE_USBOTG	1		// USBOTG module enable
// Ethernet
#define ENABLE_ETHMAC	1		// ETHMAC module enable
#define ENABLE_ETHMACTX	1		// ETHMACTX module enable
#define ENABLE_ETHMACRX	1		// ETHMACRX module enable

#endif // _CONFIG_H
