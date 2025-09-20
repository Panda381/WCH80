
// ****************************************************************************
//
//                        BabyPC - VGA display driver
//
// ****************************************************************************

#include "../../includes.h"

#if USE_DISP		// 1=use display support

#if VMODE == 5
u8* volatile FrameBuf = (u8*)SRAM_BASE;	// display graphics buffer
#else
u8 FrameBuf[FRAMESIZE];		// display graphics buffer
#endif
volatile u32 DispLine;		// current display line
volatile u32 DispFrame = 0;	// current frame
volatile u8* FrameBufAddr;	// current pointer to graphics buffer
volatile u32 DispTimTest;	// test - get TIM-CNT value at start of image

// wait for VSync scanline
void WaitVSync()
{
	while (DispIsVSync()) {}
	while (!DispIsVSync()) {}
}

// Initialize videomode
void DispInit()
{
	DispLine = 0;			// current display line
	FrameBufAddr = FrameBuf;	// current pointer to graphics buffer

	// trim HSI oscillator to 25MHz
	RCC_HSITrim(31);

	// initialize GPIO pins (note: clocks are setup at config.h)
	// - PC6 video out, GPIO push-pull output, fast speed
	RCC_AFIClkEnable();
	RCC_PCClkEnable();

	// - PC4 VSYNC, GPIO push-pull output, fast speed
	GPIO_Mode(PC4, GPIO_MODE_OUT_FAST);
	// - PC7 HSYNC, TIM2 channel 2 output
	GPIO_Mode(PC7, GPIO_MODE_AF_FAST);

	// SPI setup
	RCC_SPI1ClkEnable();		// enable SPI1 clock
	RCC_SPI1Reset();		// reset SPI1 to default state
	SPI1_Baud(SPI_HDIV);		// horizontal divider
	SPI1_Data8();			// select 8-bit mode
	SPI1_MSB();			// transmit MSB (bit 7 first)
	SPI1_NSSSw();			// control NSS by software
	SPI1_NSSHigh();			// need set NSS to HIGH to enable master output
	SPI1_SSDisable();		// disable SS output
	SPI1_Bidi1();			// set 1-line bidirectional mode
	SPI1_TxBidi();			// transmit only
	SPI1_Master();			// master mode
	SPI1_Enable();			// enable SPI
	// 0 ... PC1:CS, PC5:SCK, PC7:MISO, PC6:MOSI
	GPIO_Remap_SPI1(0);		// remap MOSI to PC6
	GPIO_Mode(PC6, GPIO_MODE_AF_FAST);

	// Remap Timer 2 - mapping 3 (output channel 2 to PC7)
	// 3 ... PC1:CH1/ETR, PC7:CH2, PD6:CH3/CH1N, PD5:CH4/CH2N
	GPIO_Remap_TIM2(3);

	// Setup Timer 2 channel 2 for HSYNC signal:
	//  System frequency 50 MHz
	//  Total period 31.77756 us = 1600 clock cycles
	//  Sync 3.81331 us = 192 clock cycles
	//  Image start at 5.71996 us (= HSYNC + back porch) = 288 clock cycles (visible area is 1280 clock cycles)
// void TIM2_InitPWM(int chan, int div, int reload, int comp, Bool high);
//   chan ... channel 1..4
//   div ... divider 1..65536
//   reload ... reload value 0..65535 (timer period = reload+1)
//   comp ... compare value 0..reload
//   high ... direction HIGH->LOW (or LOW->HIGH otherwise)
	TIM2_InitPWM(2, 1, 1600-1, 192, False);

	// Setup interrupt on Timer 2 channel 1 to display image
	//  128x64: 8 clock cycles per pixel, 64 clock cycles per character, active 16 characters, total 25 characters
	//  160x120: 8 clock cycles per pixel, 64 clock cycles per character, active 20 characters, total 25 characters
	//  256x192: 4 clock cycles per pixel, 32 clock cycles per character, active 32 characters, total 50 characters
	//  320x240: 4 clock cycles per pixel, 32 clock cycles per character, active 40 characters, total 50 characters
	//  640x240: 2 clock cycles per pixel, 16 clock cycles per character, active 80 characters, total 100 characters
#if VMODE == 0
	TIM2_Comp1(288 - 60 + 16*8);	// set compare value
#elif VMODE == 1
	TIM2_Comp1(288 - 60);		// set compare value
#elif VMODE == 2
	TIM2_Comp1(288 - 74 + 4*32);	// set compare value
#elif VMODE == 3
	TIM2_Comp1(288 - 74);		// set compare value
#elif VMODE == 4
	TIM2_Comp1(288 - 74);		// set compare value
#elif VMODE == 5
	TIM2_Comp1(288 - 82 + 4*32);	// set compare value
#elif VMODE == 6
	TIM2_Comp1(288 - 76);		// set compare value
#elif VMODE == 7
	TIM2_Comp1(288 - 76 + 4*32);	// set compare value
#endif
	TIM2_OC1Mode(TIM_COMP_FREEZE);	// set compare mode
	TIM2_OC1PreEnable();		// enable preload compare register
	TIM2_CC1Enable();		// enable compare output
	TIM2_CC1IntClr();		// clear interrupt request
	TIM2_CC1IntEnable();		// enable capture compare of channel 1
	NVIC_IRQEnable(IRQ_TIM2);	// enable interrupt service
}

// Terminate videomode
void DispTerm()
{
	// disable capture compare interrupt (channel 1)
	NVIC_IRQDisable(IRQ_TIM2);	// disable interrupt service
	TIM2_CC1IntDisable();		// disable capture compare of channel 1
	TIM2_CC1Disable();		// disable compare output
	TIM2_CC1IntClr();		// enable capture compare of channel 1

	// disable timer
	TIM2_Disable();

	// Reset timer to default setup
	TIM2_Reset();

	// reset SPI
	RCC_SPI1Reset();		// reset SPI1 to default state

	// reset GPIO pins
	GPIO_PinReset(PC7);
	GPIO_PinReset(PC4);
	GPIO_PinReset(PC6);

	// reset HSI trim to default value 24 MHz
	RCC_HSITrim(16);
}

#endif // USE_DISP
