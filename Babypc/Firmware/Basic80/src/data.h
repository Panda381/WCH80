
// ****************************************************************************
//
//                              Data area
//
// ****************************************************************************

#ifndef _DATA_H
#define _DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#if VALSIZE == 32
typedef s32 val;		// base numeric type
typedef u32 uval;		// unsigned numeric type
#else
typedef s16 val;		// base numeric type
typedef u16 uval;		// unsigned numeric type
#endif

extern u8 RamData_Beg;		// start of global RAM data

// Editing:
extern u8* CurPtr;		// pointer to character in edit line with cursor [K] or [L] (ZX80: 16388 $4004 IY+$04 P_PTR.; ATX80: DATA_CURPTR)
extern u16 CurLine;		// current edited program line (ZX80: 16390 $4006 IY+$06 E_PPC; ATX80: DATA_EDITLINE)
// Parsing:
extern u8 ErrNr;		// error code ERR_*, ERR_OK=no error (ZX80: 16384 $4000 IY+$00 ERR_NR; ATX80: DATA_ERRCODE)
extern u8 Flags;		// flags FLAG_* (ZX80: 16385 $4001 IY+$01 FLAGS, 16409 $4019 IY+$19 FLAGX.; ATX80: GPIOR0, DATA_FLAGX)
extern u8* CharPtr;		// pointer to current character to parse, in program line or in edit line (ZX80: 16422 $4026 IY+$26 CH_ADD.; ATX80: DATA_CHARPTR)
extern u8* ErrPtr;		// pointer to error [S] in parsed text, NULL=no error (ZX80: 16405 $4015 IY+$15 X_PTR; ATX80: DATA_ERRPTR)
// Printing:
extern u8 PrintX;		// current print column (ZX80: 16420 $4024 IY+$24 S_POSN_X; ATX80: DATA_PRINTCOLS)
extern u8 PrintY;		// current print row (ZX80: 16421 $4025 IY+$25 S_POSN_Y; ATX80: DATA_PRINTROWS)
extern u8 PrintYMax;		// max. available print row
extern u8 LowScreenH;		// height of lower screen (number of rows) (ZX80: 16402 $4012 IY+$12 DF_SZ; ATX80: DATA_DISP2LINES)
extern u16 TopLine;		// top displayed program line (ZX80: 16403 $4013 IY+$13 S_TOP.; ATX80: DATA_DISPTOP)
// Executing:
extern u16 ProgLine;		// current executed program line (0 = program edited or syntax check, 0xffff = direct execution, other = program is running) (ZX80: 16386 $4002 IY+$02 PPC; ATX80: DATA_EXECLINE)
extern s32 LastResult;		// last result (ZX80: 16418 $4022 IY+$22 RESULT.; ATX80: DATA_RESULT)
extern u16 OldLine;		// old program line to continue (ZX80: 16407 $4017 IY+$17 OLDPPC; ATX80: DATA_OLDLINE)
extern u8 NulString;		// empty string (contains character CH_QUOT)
extern u8* VarDest;		// pointer to variable name in current program line or in edit line (ZX80: 16416 $4020 IY+$20 DEST; ATX80: DATA_VARDEST)
extern u8* DataPtr;		// pointer to program line to read data, NULL = not initialized
extern u32 LastTime;		// last timer Time() in 0.02 us ... cleared on start of program
// Buffer size:
extern u16 ProgNum;		// size of data in program buffer (including terminating line 0)
extern u16 VarsNum;		// size of data in variables buffer (including terminating byte 0xFF)
extern u16 EditNum;		// size of data in edit line (including terminating byte NEWLINE)
extern u8 StckNum;		// size of data in program stack
extern u8 CalcNum;		// number of entries in calculator stack
// Buffers:
extern u8 Prog[PROGNUM];	// program buffer (end with line = 0) (ZX80: 16424 $4028 IY+$28; ATX80: Program)
extern u8 Vars[VARSNUM];	// program variables (end with 0xFF) (ZX80: 16392 $4008 IY+$08 VARS; ATX80: DATA_VARSPTR)
extern u8 Edit[EDITNUM];	// edit line (end with NEWLINE) (ZX80: 16394 $400A IY+$0A E_LINE; ATX80: DATA_EDITPTR)
extern u16 Stck[STCKNUM];	// program stack
extern u8 CalcStck[2*CALCNUM];	// calculator stack - operation and priority
extern s32 ValStck[CALCNUM];	// calculator stack - value
extern u8 Temp[TEMPNUM];	// print temporary buffer, for numbers and STR$ function

// extern u8 FrameBuf[FRAMESIZE]; // display frame buffer (ZX80: 16396 $400C IY+$0C D_FILE; ATX80: DATA_DISPPTR)

extern u8 RamData_End;		// end of global RAM data

// set flags
#define SET_NOLEAD	Flags |= FLAG_NOLEAD		// set 'no leading space' flag
#define SET_KMODE	Flags |= FLAG_KMODE		// set 'K mode' flag
#define SET_KUPDATE	Flags |= FLAG_KUPDATE		// set 'update cursor mode' flag
#define SET_NUMRES	Flags |= FLAG_NUMRES		// set 'numeric result' flag
#define SET_SYNTOFF	Flags |= FLAG_SYNTOFF		// set 'syntax off' flag
#define SET_INPUT	Flags |= FLAG_INPUT		// set 'inputting' flag
#define SET_GOTO	Flags |= FLAG_GOTO		// set 'goto' flag

// clear flag
#define SET_LEAD	Flags &= ~FLAG_NOLEAD		// set 'leading space' flag
#define SET_LMODE	Flags &= ~FLAG_KMODE		// set 'L mode' flag
#define SET_NOUPDATE	Flags &= ~FLAG_KUPDATE		// set 'do not update cursor mode' flag
#define SET_STRRES	Flags &= ~FLAG_NUMRES		// set 'string result' flag
#define SET_SYNTON	Flags &= ~FLAG_SYNTOFF		// set 'syntax on' flag
#define SET_EDIT	Flags &= ~FLAG_INPUT		// set 'editing' flag
#define SET_NEXT	Flags &= ~FLAG_GOTO		// set 'next program line' flag

// check if flag is set
#define IS_NOLEAD	((Flags & FLAG_NOLEAD) != 0)	// execute if 'no leading space' flag
#define IS_KMODE	((Flags & FLAG_KMODE) != 0)	// execute if 'K mode' flag
#define IS_KUPDATE	((Flags & FLAG_KUPDATE) != 0)	// execute if 'update cursor mode' flag
#define IS_NUMRES	((Flags & FLAG_NUMRES) != 0)	// execute if 'numeric result' flag
#define IS_SYNTOFF	((Flags & FLAG_SYNTOFF) != 0)	// execute if 'syntax off' flag
#define IS_INPUT	((Flags & FLAG_INPUT) != 0)	// execute if 'inputting' flag
#define IS_GOTO		((Flags & FLAG_GOTO) != 0)	// execute if 'goto' flag

// check if flag is clear
#define IS_LEAD		((Flags & FLAG_NOLEAD) == 0)	// execute if 'leading space' flag
#define IS_LMODE	((Flags & FLAG_KMODE) == 0)	// execute if 'L mode' flag
#define IS_NOUPDATE	((Flags & FLAG_KUPDATE) == 0)	// execute if 'do not update cursor mode' flag
#define IS_STRRES	((Flags & FLAG_NUMRES) == 0)	// execute if 'string result' flag
#define IS_SYNTON	((Flags & FLAG_SYNTOFF) == 0)	// execute if 'syntax on' flag
#define IS_EDIT		((Flags & FLAG_INPUT) == 0)	// execute if 'editing' flag
#define IS_NEXT		((Flags & FLAG_GOTO) == 0)	// execute if 'next program line' flag

#define IS_ERROR	(ErrNr != ERR_OK)		// check if hard error is set
#define IS_SYNTERR	(ErrPtr != NULL)		// check error pointer

// convert string index in RAM to pointer
#if VALSIZE == 32
#define RAMPTR(off) ((u8*)(off))
#else
#define RAMPTR(off) ((u8*)&RamData_Beg + (val)(off))
#endif

// convert pointer to string index in RAM
#if VALSIZE == 32
#define PTRRAM(ptr) ((val)(ptr))
#else
#define PTRRAM(ptr) ((val)((u8*)(ptr) - (u8*)&RamData_Beg))
#endif

// get line number from pointer
#define GETLINE(p) ((u16)((p)[0] | ((p)[1] << 8)))

// set line number to pointer
#define SETLINE(p,line) { (p)[0] = (u8)(line); (p)[1] = (u8)((line)>>8); }

// check zero line number
#define CHECKLINE0(p) (((p)[0] | (p)[1]) == 0)

// Tokens, code 0xD4..0xFF
extern const u8 Tokens[];

// name of functions
extern const u8 FncNameList[];

// input and output type of result of function (use FLAG_NUMRES=number, 0=string, 255=no input expression)
extern const u8 FncResList[];

// functions list
typedef void (*pFnc)(void);
extern const pFnc FncList[];

// priority table
extern const u8 PriorTab[10];

// token syntax table - pointer to syntax class, TOKEN_LIST..TOKEN_DATA
extern const u8* const SyntTab[26];

// command token functions, TOKEN_LIST..TOKEN_DATA
typedef void (*pTokenFnc)(void);
extern const pTokenFnc TokenFncList[26];

// Error
void Error(u8 err);

// NEXT error - NEXT without FOR
void NextError();

// variable not found
void VarNotFound();

// Subscript error
void SubsError();

// memory error
void MemError();

// display error, screen is full
void DispError();

// overflow error
void Overflow();

// RETURN error - no GOSUB
void ReturnError();

// input error
void InputError();

// STOP program
void CmdStop();

// SYNTAX error
void SyntaxError();

// Initialize new program (no error)
void CmdNew();

// INPUT command
void CmdInput();

#ifdef __cplusplus
}
#endif

#endif // _DATA_H
