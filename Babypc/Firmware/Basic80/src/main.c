
// ****************************************************************************
//
//                               Main
//
// ****************************************************************************

#include "../include.h"

// Remap keys to ZX-80 characters
const u8 CharRemap[KEY_NUM+1] = {
	CH_NONE,	// #define NOKEY		0	// no key
	CH_1,		// #define KEY_1		1	// 1 NOT
	CH_2,		// #define KEY_2		2	// 2 AND
	CH_3,		// #define KEY_3		3	// 3 THEN
	CH_4,		// #define KEY_4		4	// 4 TO
	CH_5,		// #define KEY_5		5	// 5 LEFT
	CH_6,		// #define KEY_6		6	// 6 DOWN
	CH_7,		// #define KEY_7		7	// 7 UP
	CH_8,		// #define KEY_8		8	// 8 RIGHT
	CH_9,		// #define KEY_9		9	// 9 HOME
	CH_0,		// #define KEY_0		10	// 0 RUBOUT

	CH_Q,		// #define KEY_Q		11	// Q
	CH_W,		// #define KEY_W		12	// W
	CH_E,		// #define KEY_E		13	// E
	CH_R,		// #define KEY_R		14	// R
	CH_T,		// #define KEY_T		15	// T
	CH_Y,		// #define KEY_Y		16	// Y "
	CH_U,		// #define KEY_U		17	// U $
	CH_I,		// #define KEY_I		18	// I (
	CH_O,		// #define KEY_O		19	// O )
	CH_P,		// #define KEY_P		20	// P *

	CH_A,		// #define KEY_A		21	// A
	CH_S,		// #define KEY_S		22	// S
	CH_D,		// #define KEY_D		23	// D
	CH_F,		// #define KEY_F		24	// F
	CH_G,		// #define KEY_G		25	// G
	CH_H,		// #define KEY_H		26	// H **
	CH_J,		// #define KEY_J		27	// J -
	CH_K,		// #define KEY_K		28	// K +
	CH_L,		// #define KEY_L		29	// L =
	NEWLINE,	// #define KEY_NL		30	// NEW LINE, EDIT

	CH_NONE,	// #define KEY_SHIFT		31	// SHIFT
	CH_Z,		// #define KEY_Z		32	// Z :
	CH_X,		// #define KEY_X		33	// X ;
	CH_C,		// #define KEY_C		34	// C ?
	CH_V,		// #define KEY_V		35	// V /
	CH_B,		// #define KEY_B		36	// B OR
	CH_N,		// #define KEY_N		37	// N <
	CH_M,		// #define KEY_M		38	// M >
	CH_DOT,		// #define KEY_DOT		39	// . ,
	CH_SPC,		// #define KEY_SPACE		40	// SPACE BREAK
};

const u8 CharRemapShift[KEY_NUM+1] = {
	CH_NONE,	// #define NOKEY		0	// no key
	TOKEN_NOT,	// #define KEY_1		1	// 1 NOT
	TOKEN_AND,	// #define KEY_2		2	// 2 AND
	TOKEN_THEN,	// #define KEY_3		3	// 3 THEN
	TOKEN_TO,	// #define KEY_4		4	// 4 TO
	CH_LEFT,	// #define KEY_5		5	// 5 LEFT
	CH_DOWN,	// #define KEY_6		6	// 6 DOWN
	CH_UP,		// #define KEY_7		7	// 7 UP
	CH_RIGHT,	// #define KEY_8		8	// 8 RIGHT
	CH_HOME,	// #define KEY_9		9	// 9 HOME
	CH_DELETE,	// #define KEY_0		10	// 0 RUBOUT

	CH_1010,	// #define KEY_Q		11	// Q
	CH_1100,	// #define KEY_W		12	// W
	CH_0010,	// #define KEY_E		13	// E
	CH_0001,	// #define KEY_R		14	// R
	CH_CHESS1,	// #define KEY_T		15	// T
	CH_QUOT,	// #define KEY_Y		16	// Y "
	CH_DOLLAR,	// #define KEY_U		17	// U $
	TOKEN_LPAR,	// #define KEY_I		18	// I (
	TOKEN_RPAR,	// #define KEY_O		19	// O )
	TOKEN_MUL,	// #define KEY_P		20	// P *

	CH_CHESS,	// #define KEY_A		21	// A
	CH_1001,	// #define KEY_S		22	// S
	CH_1000,	// #define KEY_D		23	// D
	CH_0100,	// #define KEY_F		24	// F
	CH_CHESS2,	// #define KEY_G		25	// G
	TOKEN_PWR,	// #define KEY_H		26	// H **
	TOKEN_SUB,	// #define KEY_J		27	// J -
	TOKEN_ADD,	// #define KEY_K		28	// K +
	TOKEN_EQU,	// #define KEY_L		29	// L =
	CH_EDIT,	// #define KEY_NL		30	// NEW LINE, EDIT

	CH_NONE,	// #define KEY_SHIFT		31	// SHIFT
	CH_COLON,	// #define KEY_Z		32	// Z :
	TOKEN_SEMI,	// #define KEY_X		33	// X ;
	CH_QUERY,	// #define KEY_C		34	// C ?
	TOKEN_DIV,	// #define KEY_V		35	// V /
	TOKEN_OR,	// #define KEY_B		36	// B OR
	TOKEN_LT,	// #define KEY_N		37	// N <
	TOKEN_GR,	// #define KEY_M		38	// M >
	TOKEN_COMMA,	// #define KEY_DOT		39	// . ,
	CH_PERC,	// #define KEY_SPACE		40	// SPACE BREAK %
};

// Get character (Remap keys to ZX-80 characters; returns CH_NONE of no character)
u8 CharGet()
{
	u8 key;

	// get key, destroy SHIFT
	do {
		key = KeyGet();
		if (key == NOKEY) return CH_NONE;
	} while (key == KEY_SHIFT);

	// remap key
	if (KeyPressed(KEY_SHIFT))
		key = CharRemapShift[key];
	else
		key = CharRemap[key];

	// token
	if (IS_KMODE)
	{
		if ((key >= CH_A) && (key <= CH_Z)) key += 0xc0;
	}
	return key;
}	

#if DODEBUG
int DebugX = 0;
void DebCh(u8 ch) { FrameBuf[DebugX + 5*WIDTHBYTE] = ch; DebugX++; WaitMs(100); }
void Deb(int k)
{
	u8 buf[15];
	DecNum(buf, k, 0);
	u8* s = buf;
	while (*s != 0)
	{
		DebCh(*s - '0' + CH_0);
		s++;
	}
	DebCh(CH_SLASH);
	WaitMs(500);
}

extern u8 _edata;
extern u8 _sflashslot;
#endif

int main(void)
{
	int i, line, err;
	u8 ch;

	// initialize nul string
	NulString = CH_QUOT;

	// Initialize new program
	CmdNew();

	// clear flags
	Flags = 0;

#if !USE_ASM || DODEBUG || (SLOT0NUM < SLOTNUM)
	{
		u8 t[5] = { CH_D,CH_E,CH_B,CH_U,CH_G };
		memcpy(FrameBuf, t, 5);
		WaitMs(2000);
	}
#endif

	// Load default program from slot 0
	LastResult = 0;
	CmdLoad();

	// 0 = program not running
	ProgLine = 0;

	// start main execution loop
	// ZX80: L0283
	while (True)
	{
		// Auto list
		AutoList();

		// not executing
		ProgLine = 0;

		// edit (flag FLAG_INPUT determines if inputting data or editing code)
		SET_EDIT;
		DoEdit();

		// The program was started with the RUN command, or direct execution
		line = ProgLine;
		if (line != 0)
		{
			// program is running, until break
			while (True)
			{
				line = ProgLine;
				if (line == 0) break; // stop program execution
				if (line == 0xffff) break; // direct execution
				if (IS_ERROR) break; // error

				// continue on next program line
				u8* p;
				if (IS_NEXT)
				{
					// skip NEWLINE on end of current program line
					p = CharPtr;
					while (*p++ != NEWLINE) {}
				}
				else
				{
					// set flag to continue with next program line
					SET_NEXT;

					// find address of the line
					p = FindLine(line, NULL);
					if (p == NULL) break; // program is empty
				}

				// program end
				if (p >= &Prog[ProgNum-2]) break;

				// get new line index
				line = GETLINE(p);

				// end of program
				if (line == 0) break;

				// execute next program line
				ProgLine = line;

				// break program (space key is pressed)
				if (KeyPressed(KEY_SPACE))
				{
					CmdStop(); // stop program
					KeyFlush();
					break;
				}

				// execute command
				SET_SYNTOFF;	// syntax off
				MainGo(p+2);

				// clear edit line (edit line can contain temporary strings)
				ClrEdit();
			} // while (True)

			// clear to end of display
			ClrEod();

			// continue with line
			line = ProgLine;
			err = ErrNr;
			OldLine = (err == ERR_STOP) ? (line+1) : line; // on STOP, continue with next program line

			// prepare to print
			PrintX = 0;
			PrintY = HEIGHT-2;
			PrintYMax = HEIGHT-1;
			LowScreenH = 2;		// height of lower screen (number of rows)
			PrintCR(); // 1 empty row

			// print error code and line number
			PrintNum(err); // print error code
			PrintCh(CH_SLASH); // print '/'
			PrintNum((line == 0xffff) ? 0 : line);	 // print line number
			ClrEod();  // clear to end of display

			// wait a key
			do { ch = CharGet(); } while (ch == CH_NONE);
		} // if (line != 0)
	} // while (True)
}
