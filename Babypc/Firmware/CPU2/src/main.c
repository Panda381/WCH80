
// PC1: COL7
// PC2: COL6
// PC3: COL5
// PC4: COL4
// PC6: COL3
// PC7: COL2
// PD4: COL1
// PD5: USART1_TX mapping 0, RX1-TX2 (send from CPU2 to CPU1, this is CPU2)
// PD6: USART1_RX mapping 0, TX1-RX2 (send from CPU1 to CPU2, this is CPU2)
// PD7: ROW4 (RST)
// PA1: COL8
// PA2: COL9
// PC0: COL10

// Input from the buttons is handled during the VGA rendering service,
// during back porch vertical scanlines (33 scanlines).
// Whole line = 31.8 us. 10us to transfer 1 byte.

// start of scanline 1:
//  time 0us: CPU1 start sending CPU_CMD_ROW4x
//  time 10us: CPU2 received CPU_CMD_ROW4x
//  time 12us: CPU2 start sending response byte 1
//  time 22us: CPU1 has byte 1 ready in buffer
// start of scanline 2:
//  time 32us: CPU1 received byte 1, CPU1 processed byte 1
//  time 43us: CPU2 start sending response byte 2
//  time 53us: CPU1 has byte 2 ready in buffer
// start of scanline 3:
//  time 64us: CPU1 start sending CPU_CMD_ROW4x
//  time 64us: CPU1 received byte 2, CPU1 processed byte 2

#define CPU_SEND_WAIT	31		// wait delay in [us] after sending first column byte

// USART communication divider (baudrate = HCLK/div, HCLK=48000000, div=min. 16; 48 at 48MHz -> 1MBaud, 1 byte = 10us)
#define CPU_UART_DIV	HCLK_PER_US	// USART CPU baudrate divider (baudrate = HCLK/div, HCLK=48000000, div=min. 16)

// commands from CPU1 to CPU2
#define CPU_CMD_SYNC	0x16		// sync - echo CPU_STATE_SYNC back, and set ROW4 to 1
#define CPU_CMD_ROW41	0x17		// return columns in 2 bytes (in bits 0..4) and set ROW4 to 1
#define CPU_CMD_ROW40	0x18		// return columns in 2 bytes (in bits 0..4) and set ROW4 to 0

#define CPU_CMD_LOAD1	0x20		// load program from slot 1
#define CPU_CMD_LOAD2	0x21		// load program from slot 2
#define CPU_CMD_LOAD3	0x22		// load program from slot 3
#define CPU_CMD_LOAD4	0x23		// load program from slot 4
#define CPU_CMD_LOAD5	0x24		// load program from slot 5
#define CPU_CMD_LOAD6	0x25		// load program from slot 6
#define CPU_CMD_LOAD7	0x26		// load program from slot 7
#define CPU_CMD_LOAD8	0x27		// load program from slot 8
#define CPU_CMD_LOAD9	0x28		// load program from slot 9
#define CPU_CMD_LOAD10	0x29		// load program from slot 10
#define CPU_CMD_LOAD11	0x2A		// load program from slot 11
#define CPU_CMD_LOAD12	0x2B		// load program from slot 12
#define CPU_CMD_LOAD13	0x2C		// load program from slot 13

#define CPU_CMD_SAVE1	0x30		// save program to slot 1
#define CPU_CMD_SAVE2	0x31		// save program to slot 2
#define CPU_CMD_SAVE3	0x32		// save program to slot 3
#define CPU_CMD_SAVE4	0x33		// save program to slot 4
#define CPU_CMD_SAVE5	0x34		// save program to slot 5
#define CPU_CMD_SAVE6	0x35		// save program to slot 6
#define CPU_CMD_SAVE7	0x36		// save program to slot 7
#define CPU_CMD_SAVE8	0x37		// save program to slot 8
#define CPU_CMD_SAVE9	0x38		// save program to slot 9
#define CPU_CMD_SAVE10	0x39		// save program to slot 10
#define CPU_CMD_SAVE11	0x3A		// save program to slot 11
#define CPU_CMD_SAVE12	0x3B		// save program to slot 12
#define CPU_CMD_SAVE13	0x3C		// save program to slot 13

// state from CPU2 to CPU1
#define CPU_STATE_SYNC	0x15		// sync - echo back after CPU_CMD_SYNC

#include "../include.h"

// receive slot
sSlot Slot0;

// check free send byte to other CPU
Bool CPU_SendReady()
{
	return USART1_TxEmpty();
}

// send byte to other CPU
void CPU_Send(u8 data)
{
	USART1_SendChar(data);
}

// check received byte from other CPU
Bool CPU_RecvReady()
{
	return USART1_RxReady();
}

// receive byte from other CPU
u8 CPU_Recv()
{
	return USART1_RecvChar();
}

// remapping table of bits of PC port (table size: 512 bytes)
//  0 -> 9 (PC0 -> COL10)
//  1 -> 6 (PC1 -> COL7)
//  2 -> 5 (PC2 -> COL6)
//  3 -> 4 (PC3 -> COL5)
//  4 -> 3 (PC4 -> COL4)
//  6 -> 2 (PC6 -> COL3)
//  7 -> 1 (PC7 -> COL2)
u16 PortCRemap[256];

int main(void)
{
	u8 ch, ra, rc, rd;
	u32 r;
	u16 kk;
	int i;

	// prepare remapping table
// remapping table of bits of PC port (table size: 512 bytes)
//  0 -> 9 (PC0 -> COL10)
//  1 -> 6 (PC1 -> COL7)
//  2 -> 5 (PC2 -> COL6)
//  3 -> 4 (PC3 -> COL5)
//  4 -> 3 (PC4 -> COL4)
//  6 -> 2 (PC6 -> COL3)
//  7 -> 1 (PC7 -> COL2)
	for (i = 0; i < 256; i++)
	{
		kk = 0;
		if ((i & B0) == 0) kk |= B9;
		if ((i & B1) == 0) kk |= B6;
		if ((i & B2) == 0) kk |= B5;
		if ((i & B3) == 0) kk |= B4;
		if ((i & B4) == 0) kk |= B3;
		if ((i & B6) == 0) kk |= B2;
		if ((i & B7) == 0) kk |= B1;
		PortCRemap[i] = kk;
	}

	// initialize port clock
	RCC_AFIClkEnable();
	RCC_PAClkEnable();
	RCC_PCClkEnable();
	RCC_PDClkEnable();

	// trim HSI oscillator to 24MHz
	RCC_HSITrim(16);

	// switch PD7 to GPIO mode, if not already set
	if (((OB->USER >> 3) & 3) != 3)
	{
		OB_t ob;
		Flash_OBRead(&ob);	// read option bytes
		ob.USER |= B3|B4;	// disable RESET function
		Flash_OBWrite(&ob);	// write new option bytes
	}

	// Remap PA1 & PA2 pins to OSC_IN & OSC_OUT (default 0)
	// 0 ... pins PA1 & PA2 are used as GPIO ports
	GPIO_Remap_PA1PA2(0);

	// setup column pins to IN_PU pull-up
	GPIO_Mode(PC1, GPIO_MODE_IN_PU);	// COL7, pin 1
	GPIO_Mode(PC2, GPIO_MODE_IN_PU);	// COL6, pin 2
	GPIO_Mode(PC3, GPIO_MODE_IN_PU);	// COL5, pin 3
	GPIO_Mode(PC4, GPIO_MODE_IN_PU);	// COL4, pin 4
	GPIO_Mode(PC6, GPIO_MODE_IN_PU);	// COL3, pin 5
	GPIO_Mode(PC7, GPIO_MODE_IN_PU);	// COL2, pin 6
	GPIO_Mode(PD4, GPIO_MODE_IN_PU);	// COL1, pin 8
	GPIO_Mode(PA1, GPIO_MODE_IN_PU);	// COL8, pin 12
	GPIO_Mode(PA2, GPIO_MODE_IN_PU);	// COL9, pin 13
	GPIO_Mode(PC0, GPIO_MODE_IN_PU);	// COL10, pin 16

	// setup row4
	GPIO_Out1(PD7);		// ROW4
	GPIO_Mode(PD7, GPIO_MODE_OD);	// ROW4

	// setup USART1 mapping 0, to communicate with CPU1
	// 0 ... PD5:TX, PD6:RX, PD3:CTS, PC2:RTS
// PD5: USART1_TX mapping 0, TX1-RX2 (send from CPU1 to CPU2, this is CPU1)
// PD6: USART1_RX mapping 0, RX1-TX2 (send from CPU2 to CPU1, this is CPU1)
	GPIO_Remap_USART1(0);
	USART1_Init((RCC_GetFreq(CLK_HCLK) + CPU_UART_DIV/2)/CPU_UART_DIV,
		USART_WORDLEN_8, USART_PARITY_NONE, USART_STOP_1);
	GPIO_Mode(PD5, GPIO_MODE_AF);		// TX
	GPIO_Mode(PD6, GPIO_MODE_IN);		// RX

	while(True)
	{
		if (CPU_RecvReady() && CPU_SendReady())
		{
			ch = CPU_Recv();
			switch (ch)
			{
			// sync - echo CPU_STATE_SYNC back
			case CPU_CMD_SYNC:
				CPU_Send(CPU_STATE_SYNC);
				GPIO_Out1(PD7);		// disable ROW4
				break;

			// return columns in 2 bytes (in bits 0..4) and set ROW4 to 1
			case CPU_CMD_ROW41:
			// return columns in 2 bytes (in bits 0..4) and set ROW4 to 0
			case CPU_CMD_ROW40:

				// read columns
				ra = GPIOA_InAll();
				rc = GPIOC_InAll();
				rd = GPIOD_InAll();
				i = PortCRemap[rc];
				if ((rd & B4) == 0) i |= B0;
				i |= ((~ra) & (B1 | B2)) << 6;

				// send columns
				CPU_Send((u8)(i & 0x1f));
				WaitUs(CPU_SEND_WAIT);
				CPU_Send((u8)(i >> 5));

				// set ROW4
				if (ch == CPU_CMD_ROW41)
					GPIO_Out1(PD7);		// disable ROW4
				else
					GPIO_Out0(PD7);		// enable ROW4
				break;

			// load from slot
			case CPU_CMD_LOAD1:
			case CPU_CMD_LOAD2:
			case CPU_CMD_LOAD3:
			case CPU_CMD_LOAD4:
			case CPU_CMD_LOAD5:
			case CPU_CMD_LOAD6:
			case CPU_CMD_LOAD7:
			case CPU_CMD_LOAD8:
			case CPU_CMD_LOAD9:
			case CPU_CMD_LOAD10:
			case CPU_CMD_LOAD11:
			case CPU_CMD_LOAD12:
				{
					int inx = ch - CPU_CMD_LOAD1;
					const sSlot* s = &Slots[inx];
					for (i = 0; i < SLOTNUM; i++)
					{
						while (!CPU_SendReady()) {}
						CPU_Send(s->data[i]);
						WaitUs(CPU_SEND_WAIT*2); // CPU1 can be interrupted by HSYNC
					}
				}
				break;

			case CPU_CMD_LOAD13:
				{
					const u8* s = Slot13;
					for (i = 0; i < SLOT13NUM; i++)
					{
						while (!CPU_SendReady()) {}
						CPU_Send(s[i]);
						WaitUs(CPU_SEND_WAIT*2); // CPU1 can be interrupted by HSYNC
					}

					for (; i < SLOTNUM; i++)
					{
						while (!CPU_SendReady()) {}
						CPU_Send(0xff);
						WaitUs(CPU_SEND_WAIT*2); // CPU1 can be interrupted by HSYNC
					}
				}
				break;

			// save to slot
			case CPU_CMD_SAVE1:
			case CPU_CMD_SAVE2:
			case CPU_CMD_SAVE3:
			case CPU_CMD_SAVE4:
			case CPU_CMD_SAVE5:
			case CPU_CMD_SAVE6:
			case CPU_CMD_SAVE7:
			case CPU_CMD_SAVE8:
			case CPU_CMD_SAVE9:
			case CPU_CMD_SAVE10:
			case CPU_CMD_SAVE11:
			case CPU_CMD_SAVE12:
			case CPU_CMD_SAVE13:
				{
					// receive data
					sSlot* s0 = &Slot0;
					for (i = 0; i < SLOTNUM; i++)
					{
						while (!CPU_RecvReady()) {}
						s0->data[i] = CPU_Recv();
					}

					// prepare slot address and size
					const u8* s;
					int n;
					if (ch == CPU_CMD_SAVE13)
					{
						s = Slot13;
						n = 0x0200;
					}
					else
					{
						s = Slots[ch - CPU_CMD_SAVE1].data;
						n = SLOTNUM;
					}

					// clear flash slot (address and size must be multiply of 256 B)
					Flash_Erase((u32)s + FLASH_BASE, n, 1000);

					// write program
					Flash_Program((u32)s + FLASH_BASE, (const u32*)s0, n, 1000);

					// send OK
					while (!CPU_SendReady()) {}
					CPU_Send(CPU_STATE_SYNC);
				}
				break;
			}
		}
	}
}
