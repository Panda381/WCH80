
// ****************************************************************************
//
//                                 Save program
//
// ****************************************************************************

#include "../include.h"

#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
__attribute__((section(".flashminislot"))) __attribute__((aligned(256))) const u8 SlotData[SLOTDATANUM] = { VAR_END };
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
// default program slot 0
// Use SLOT0 address instead Slot0 for programming and reading Slot0 to avoid memory caching.
__attribute__((section(".flashslot"))) __attribute__((aligned(SLOT0ALIGN))) const u8 Slot0[SLOT0NUM] = {

// WCH80 demo (212 B)

	// 10 REM
	10,0,TOKEN_REM,NEWLINE,
	// 20 REM HELLO FROM WCH80
	20,0,TOKEN_REM,CH_H,CH_E,CH_L,CH_L,CH_O,CH_SPC,CH_F,CH_R,CH_O,CH_M,CH_SPC,
		CH_W,CH_C,CH_H,CH_8,CH_0,NEWLINE,
	// 30 REM PRESS RUN TO RUN
	30,0,TOKEN_REM,CH_P,CH_R,CH_E,CH_S,CH_S,CH_SPC,CH_R,CH_U,CH_N,CH_SPC,
		CH_T,CH_O,CH_SPC,CH_R,CH_U,CH_N,NEWLINE,
	// 40 REM
	40,0,TOKEN_REM,NEWLINE,
	// 50 PRINT ,"   WCH80 DEMO"
	50,0,TOKEN_PRINT,TOKEN_COMMA,CH_QUOT,CH_SPC,CH_SPC,CH_SPC,CH_W,CH_C,CH_H,CH_8,CH_0,CH_SPC,
		CH_D,CH_E,CH_M,CH_O,CH_QUOT,NEWLINE,
	// 60 PRINT
	60,0,TOKEN_PRINT,NEWLINE,
	// 70 LET N=0
	70,0,TOKEN_LET,CH_N,TOKEN_EQU,CH_0,NEWLINE,
	// 80 PRINT "CHARACTER SET:"
	80,0,TOKEN_PRINT,CH_QUOT,CH_C,CH_H,CH_A,CH_R,CH_A,CH_C,CH_T,CH_E,CH_R,CH_SPC,
		CH_S,CH_E,CH_T,CH_COLON,CH_QUOT,NEWLINE,
	// 90 FOR I=0 TO 255
	90,0,TOKEN_FOR,CH_I,TOKEN_EQU,CH_0,TOKEN_TO,CH_2,CH_5,CH_5,NEWLINE,
	// 100 PRINT CHR$(I);
	100,0,TOKEN_PRINT,CH_C,CH_H,CH_R,CH_DOLLAR,TOKEN_LPAR,CH_I,TOKEN_RPAR,TOKEN_SEMI,NEWLINE,
	// 110 NEXT I
	110,0,TOKEN_NEXT,CH_I,NEWLINE,
	// 120 POKE -5,17
	120,0,TOKEN_POKE,TOKEN_SUB,CH_5,TOKEN_COMMA,CH_1,CH_7,NEWLINE,
	// 130 POKE -4,0
	130,0,TOKEN_POKE,TOKEN_SUB,CH_4,TOKEN_COMMA,CH_0,NEWLINE,
	// 140 PRINT "SOUND TEST: ";N
	140,0,TOKEN_PRINT,CH_QUOT,CH_S,CH_O,CH_U,CH_N,CH_D,CH_SPC,CH_T,CH_E,CH_S,CH_T,
		CH_COLON,CH_SPC,CH_QUOT,TOKEN_SEMI,CH_N,NEWLINE,
	// 150 BEEP 100000,200
	150,0,TOKEN_BEEP,CH_1,CH_0,CH_0,CH_0,CH_0,CH_0,TOKEN_COMMA,CH_2,CH_0,CH_0,NEWLINE,
	// 160 WAIT 800
	160,0,TOKEN_WAIT,CH_8,CH_0,CH_0,NEWLINE,
	// 170 LET N=N+1
	170,0,TOKEN_LET,CH_N,TOKEN_EQU,CH_N,TOKEN_ADD,CH_1,NEWLINE,
	// 180 IF INKEY$="" THEN GOTO 120
	180,0,TOKEN_IF,CH_I,CH_N,CH_K,CH_E,CH_Y,CH_DOLLAR,TOKEN_EQU,CH_QUOT,CH_QUOT,
		TOKEN_THEN,TOKEN_GOTO,CH_1,CH_2,CH_0,NEWLINE,

	// program end (2 B)
	0, 0,
};
#endif

#if !USE_SAVE_ASM		// 1=use ASM in Save module

// clear unused space of program
NOMEMSET void ClearFreeProg()
{
	int i = PROGNUM - ProgNum;
	u8* p = &Prog[ProgNum];
	for (; i > 0; i--) *p++ = 0xff;
}

// clear unused space of variables
NOMEMSET void ClearFreeVars()
{
	int i = VARSNUM - VarsNum;
	u8* p = &Vars[VarsNum];
	for (; i > 0; i--) *p++ = VAR_END;
}

// command Save
void CmdSave()
{
	// stop running (no error report)
	ProgLine = 0;

	// program slot
	int slot = LastResult;
	if ((u32)slot > SLOTMAX) return;

	// clear unused space of program
	ClearFreeProg();

// save to CPU2

	// save to CPU2
	if (slot > 0)
	{
		// lock CPU communication
		CPU_IntLock();

		// send request
		while (!CPU_SendReady()) {}
		CPU_Send(CPU_CMD_SAVE1 + slot - 1);

		// flush receiver
		if (CPU_RecvReady()) CPU_Recv();

		// save data
		int i;
		for (i = 0; i < SLOTNUM; i++)
		{
			while (!CPU_SendReady()) {}
			CPU_Send(Prog[i]);
			WaitUs(50);
		}

		// wait for end of flashing
		while (!CPU_RecvReady()) {}

		// synchronize
		CPU_SyncInit();

		// unlock CPU communication
		CPU_IntUnlock();
	}

// save to this CPU

	else
	{
#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
		// clear unused space of variables
		ClearFreeVars();

		// clear flash slot (address and size must be multiply of 256 B)
		Flash_Erase((u32)SLOTDATA, SLOTDATANUM, 1000);

		// write data
		Flash_Program((u32)SLOTDATA, (const u32*)Vars, SLOTDATANUM, 1000);
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
		// clear flash slot (address and size must be multiply of 256 B)
		Flash_Erase((u32)SLOT0, SLOT0NUM, 1000);

		// write program
		Flash_Program((u32)SLOT0, (const u32*)Prog, SLOT0NUM, 1000);
#endif
	}
}

// command Load
void CmdLoad()
{
	// stop running (no error report)
	ProgLine = 0;

	// program slot
	int slot = LastResult;
	if ((u32)slot > SLOTMAX) return;

	// clear program
	ProgNum = 2;
	Prog[0] = 0;
	Prog[1] = 0;
	ClearFreeProg();

	// load slot from CPU2
	if (slot > 0)
	{
		// lock communication
		CPU_IntLock();

		// send request
		while (!CPU_SendReady()) {}
		CPU_Send(CPU_CMD_LOAD1 + slot - 1);

		// load data
		int i;
		for (i = 0; i < SLOTNUM; i++)
		{
			while (!CPU_RecvReady()) {}
			Prog[i] = CPU_Recv();
		}

		// unlock communication
		CPU_IntUnlock();
	}
	else
	{
#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
		// clear variables
		CmdClear();
		ClearFreeVars();

		// load variables
		memcpy(Vars, SLOTDATA, SLOTDATANUM);

		// emergency mark end of variables
		u8* pmax = &Vars[SLOTDATANUM-1];
		*pmax = CH_QUOT;

		// find end of variables
		u8* p = Vars;
		u8* p0 = p; // last valid variable
		while ((*p != VAR_END) && (p < pmax))
		{
			p0 = p;
			p = NextVar(p);
		}
		if (*p != VAR_END) p = p0;
		*p = VAR_END;
		VarsNum = (p - Vars) + 1;
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
		// load program from slot 0
		// - For the Slot0 address, we must use an alias with the FLASH_BASE base
		//   so that the cache does not apply after previous programming.
		memcpy(Prog, SLOT0, SLOT0NUM);
#else
		return;
#endif
	}

	// get program size
	u8* p = Prog;
	u8* p0 = p; // last valid line
	while (True)
	{
		// check end of program
		int line = GETLINE(p);
		if (line == 0) break;

		// invalid program line (= clear flash memory)
		if (line == 0xffff)
		{
			p = p0;
			break;
		}
		p0 = p;

		// find next line
		p += 2;
		u8 a;
		do { a = *p++; } while ((a != NEWLINE) && (p <= &Prog[PROGNUM-2]));

		// buffer overflow
		if (p > &Prog[PROGNUM-2])
		{
			p = p0;
			break;
		}
	}

	// mark end of program
	SETLINE(p, 0);

	// set program size
	ProgNum = (p - Prog) + 2;

	// reset current line
	CurLine = 1;
	TopLine = 1;
}

#endif // USE_SAVE_ASM
