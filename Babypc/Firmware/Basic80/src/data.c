
// ****************************************************************************
//
//                              Data area
//
// ****************************************************************************

#include "../include.h"

// Tokens, code 0xD4..0xFF (starting with entry TOKEN_FIRST)
const u8 Tokens[] = {
	CH_QUERY			+ 0x80,	// '?' invalid code

	CH_QUOT				+ 0x80,	// '"'		0xD4 212
	CH_T,CH_H,CH_E,CH_N		+ 0x80,	// THEN		0xD5 213
	CH_T,CH_O			+ 0x80,	// TO		0xD6 214
	CH_SEMI				+ 0x80,	// ;		0xD7 215
	CH_COMMA			+ 0x80,	// ,		0xD8 216
	CH_RPAR				+ 0x80,	// )		0xD9 217
	CH_LPAR				+ 0x80,	// (		0xDA 218
	CH_N,CH_O,CH_T			+ 0x80,	// NOT		0xDB 219
	CH_MINUS			+ 0x80,	// -		0xDC 220
	CH_PLUS				+ 0x80,	// +		0xDD 221
	CH_ASTER			+ 0x80,	// *		0xDE 222
	CH_SLASH			+ 0x80,	// /		0xDF 223
	CH_A,CH_N,CH_D			+ 0x80,	// AND		0xE0 224
	CH_O,CH_R			+ 0x80,	// OR		0xE1 225
	CH_ASTER,CH_ASTER		+ 0x80,	// **		0xE2 226
	CH_EQU				+ 0x80,	// =		0xE3 227
	CH_GT				+ 0x80,	// >		0xE4 228
	CH_LT				+ 0x80,	// <		0xE5 229
	CH_L,CH_I,CH_S,CH_T		+ 0x80,	// LIST		0xE6 230
	CH_B,CH_E,CH_E,CH_P		+ 0x80,	// BEEP		0xE7 231
	CH_C,CH_L,CH_S			+ 0x80,	// CLS		0xE8 232
	CH_D,CH_I,CH_M			+ 0x80,	// DIM		0xE9 233
	CH_S,CH_A,CH_V,CH_E		+ 0x80,	// SAVE		0xEA 234
	CH_F,CH_O,CH_R			+ 0x80,	// FOR		0xEB 235
	CH_G,CH_O,CH_T,CH_O		+ 0x80,	// GOTO		0xEC 236
	CH_P,CH_O,CH_K,CH_E		+ 0x80,	// POKE		0xED 237
	CH_I,CH_N,CH_P,CH_U,CH_T	+ 0x80,	// INPUT	0xEE 238
	CH_R,CH_A,CH_N,CH_D,CH_O,CH_M,CH_I,CH_S,CH_E + 0x80, // RANDOMISE 0xEF 239
	CH_R,CH_E,CH_T,CH_U,CH_R,CH_N 	+ 0x80,	// RETURN	0xF0 240
	CH_L,CH_E,CH_T			+ 0x80,	// LET		0xF1 241
	CH_M,CH_E,CH_M,CH_O,CH_R,CH_Y	+ 0x80,	// MEMORY	0xF2 242
	CH_N,CH_E,CH_X,CH_T		+ 0x80,	// NEXT		0xF3 243
	CH_L,CH_O,CH_A,CH_D		+ 0x80,	// LOAD		0xF4 244
	CH_P,CH_R,CH_I,CH_N,CH_T	+ 0x80,	// PRINT	0xF5 245
	CH_N,CH_E,CH_W			+ 0x80,	// NEW		0xF6 246
	CH_R,CH_U,CH_N			+ 0x80,	// RUN		0xF7 247
	CH_S,CH_T,CH_O,CH_P		+ 0x80,	// STOP		0xF8 248
	CH_C,CH_O,CH_N,CH_T,CH_I,CH_N,CH_U,CH_E + 0x80, // CONTINUE 0xF9 249
	CH_I,CH_F			+ 0x80,	// IF		0xFA 250
	CH_G,CH_O,CH_S,CH_U,CH_B	+ 0x80,	// GOSUB	0xFB 251
	CH_W,CH_A,CH_I,CH_T		+ 0x80,	// WAIT		0xFC 252
	CH_C,CH_L,CH_E,CH_A,CH_R	+ 0x80,	// CLEAR	0xFD 253
	CH_R,CH_E,CH_M			+ 0x80,	// REM		0xFE 254
	CH_D,CH_A,CH_T,CH_A		+ 0x80,	// DATA		0xFF 255
};

// name of functions
// ZX80: L0BC0
const u8 FncNameList[] = {
	4+(VARTYPE_ARRAY<<VAR_SHIFT),	CH_P, CH_E, CH_E, CH_K, 	// PEEK(, function val FncPeek(val)
	3+(VARTYPE_FOR<<VAR_SHIFT),	CH_C, CH_H, CH_R, 		// CHR$(, function str FncChr(val)
	4+(VARTYPE_ARRAY<<VAR_SHIFT),	CH_C, CH_O, CH_D, CH_E, 	// CODE(, function val FncCode(str)
	3+(VARTYPE_ARRAY<<VAR_SHIFT),	CH_R, CH_N, CH_D,		// RND(, function val FncRnd(val)
	2+(VARTYPE_FOR<<VAR_SHIFT),	CH_T, CH_L,			// TL$(, function str FncStrTl(str)
	3+(VARTYPE_FOR<<VAR_SHIFT),	CH_S, CH_T, CH_R,		// STR$(, function str FncStr(val)
	3+(VARTYPE_ARRAY<<VAR_SHIFT),	CH_A, CH_B, CH_S,		// ABS(, function val FncAbs(val)
	5+(VARTYPE_STR<<VAR_SHIFT),	CH_I, CH_N, CH_K, CH_E, CH_Y,	// INKEY$, function str FncInkey()
	4+(VARTYPE_NUM<<VAR_SHIFT),	CH_R, CH_E, CH_A, CH_D,		// READ, function val FncRead()
	VAR_END
};

// input and output type of result of function (use FLAG_NUMRES=number, 0=string, 255=no input expression)
const u8 FncResList[] = {
	FLAG_NUMRES,	FLAG_NUMRES,	// PEEK(, function val FncPeek(val)
	FLAG_NUMRES,	0,		// CHR$(, function str FncChr(val)
	0,		FLAG_NUMRES,	// CODE(, function val FncCode(str)
	FLAG_NUMRES,	FLAG_NUMRES,	// RND(, function val FncRnd(val)
	0,		0,		// TL$(, function str FncStrTl(str)
	FLAG_NUMRES,	0,		// STR$(, function str FncStr(val)
	FLAG_NUMRES,	FLAG_NUMRES,	// ABS(, function val FncAbs(val)
	255,		0,		// INKEY$, function str FncInkey()
	255,		FLAG_NUMRES,	// READ, function val FncRead()
};

// functions list
const pFnc FncList[] = {
	FncPeek,	// PEEK(, function val FncPeek(val)
	FncChr,		// CHR$(, function str FncChr(val)
	FncCode,	// CODE(, function val FncCode(str)
	FncRnd,		// RND(, function val FncRnd(val)
	FncStrTl,	// TL$(, function str FncStrTl(str)
	FncStr,		// STR$(, function str FncStr(val)
	FncAbs,		// ABS(, function val FncAbs(val)
	FncInkey,	// INKEY$, function str FncInkey()
	FncRead,	// READ, function val FncRead()
};

// priority table
// ZX80: L0AA3
const u8 PriorTab[10] = {
	6,	// 0 '-'	#define TOKEN_SUB	0xDC	// 220 -
	6,	// 1 '+'	#define TOKEN_ADD	0xDD	// 221 +
	8,	// 2 '*'	#define TOKEN_MUL	0xDE	// 222 *
	7,	// 3 '/'	#define TOKEN_DIV	0xDF	// 223 /
	3,	// 4 AND	#define TOKEN_AND	0xE0	// 224 AND
	2,	// 5 OR		#define TOKEN_OR	0xE1	// 225 OR
	10,	// 6 ** power	#define TOKEN_PWR	0xE2	// 226 ** power
	5,	// 7 =		#define TOKEN_EQU	0xE3	// 227 =
	5,	// 8 >		#define TOKEN_GR	0xE4	// 228 >
	5,	// 9 <		#define TOKEN_LT	0xE5	// 229 <
};

// token syntax
const u8 P_LIST[1] = {	// #define TOKEN_LIST	0xE6	// 230 LIST
	CLASS3,		// class 3 - a numeric expression may follow, otherwise zero will be used
}; // CmdList

const u8 P_BEEP[3] = {	// #define TOKEN_BEEP	0xE7	// 231 BEEP
	CLASS6,		// class 6 - a numeric expression must follow
	TOKEN_COMMA,	// ',' token
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdBeep

const u8 P_CLS[1] = {	// #define TOKEN_CLS	0xE8	// 232 CLS
	CLASS0,		// class 0 - no further operands
}; // CmdCls

const u8 P_DIM[5] = {	// #define TOKEN_DIM	0xE9	// 233 DIM
	CLASS4,		// class 4 - integer variable must follow
	TOKEN_LPAR,	// '(' token
	CLASS6,		// class 6 - a numeric expression must follow
	TOKEN_RPAR,	// ')' token
	CLASS0,		// class 0 - no further operands
}; // CmdDim

const u8 P_SAVE[1] = {	// #define TOKEN_SAVE	0xEA	// 234 SAVE
	CLASS3,		// class 3 - a numeric expression may follow, otherwise zero will be used
}; // CmdSave

const u8 P_FOR[5] = {	// #define TOKEN_FOR	0xEB	// 235 FOR
	CLASS4,		// class 4 - integer variable must follow
	TOKEN_EQU,	// '=' token
	CLASS6,		// class 6 - a numeric expression must follow
	TOKEN_TO,	// 'TO' token
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdFor

const u8 P_GOTO[2] = {	// #define TOKEN_GOTO	0xEC	// 236 GOTO
	CLASS6,		// class 6 - a numeric expression must follow
	CLASS0,		// class 0 - no further operands
}; // CmdGoto

const u8 P_POKE[3] = {	// #define TOKEN_POKE	0xED	// 237 POKE
	CLASS6,		// class 6 - a numeric expression must follow
	TOKEN_COMMA,	// ',' token
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdPoke

const u8 P_INPUT[2] = {	// #define TOKEN_INPUT	0xEE	// 238 INPUT
	CLASS1,		// class 1 - destination variable name is required
	CLASS0,		// class 0 - no further operands
}; // CmdInput

const u8 P_RANDOMISE[1] = { // #define TOKEN_RANDOM	0xEF	// 239 RANDOMISE
	CLASS3,		// class 3 - a numeric expression may follow, otherwise zero will be used
}; // CmdRandomise

const u8 P_RETURN[1] = { // #define TOKEN_RETURN	0xF0	// 240 RETURN
	CLASS0, 	// class 0 - no further operands
}; // CmdReturn

const u8 P_LET[3] = {	// #define TOKEN_LET	0xF1	// 241 LET
	CLASS1,		// class 1 - destination variable name is required
	TOKEN_EQU,	// '=' token
	CLASS2,		// class 2 - an expression (of type integer or string) must follow
}; // CmdLet

const u8 P_MEMORY[1] = { // #define TOKEN_MEMORY	0xF2	// 242 MEMORY
	CLASS0,		// class 0 - no further operands
}; // CmdMemory

const u8 P_NEXT[2] = {	// #define TOKEN_NEXT	0xF3	// 243 NEXT
	CLASS4,		// class 4 - integer variable must follow
	CLASS0,		// class 0 - no further operands
}; // CmdNext

const u8 P_LOAD[1] = {	// #define TOKEN_LOAD	0xF4	// 244 LOAD
	CLASS3,		// class 3 - a numeric expression may follow, otherwise zero will be used
}; // CmdLoad

const u8 P_PRINT[1] = {	// #define TOKEN_PRINT	0xF5	// 245 PRINT
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdPrint

const u8 P_NEW[1] = {	// #define TOKEN_NEW	0xF6	// 246 NEW
	CLASS0,		// class 0 - no further operands
}; // CmdNew

const u8 P_RUN[1] = {	// #define TOKEN_RUN	0xF7	// 247 RUN
	CLASS3,		// class 3 - a numeric expression may follow, otherwise zero will be used
}; // CmdRun

const u8 P_STOP[1] = {	// #define TOKEN_STOP	0xF8	// 248 STOP
	CLASS0,		// class 0 - no further operands
}; // CmdStop

const u8 P_CONTINUE[1] = { // #define TOKEN_CONT	0xF9	// 249 CONTINUE
	CLASS0,		// class 0 - no further operands
}; // CmdContinue

const u8 P_IF[3] = {	// #define TOKEN_IF	0xFA	// 250 IF
	CLASS6,		// class 6 - a numeric expression must follow
	TOKEN_THEN,	// 'THEN' token
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdIf

const u8 P_GOSUB[2] = {	// #define TOKEN_GOSUB	0xFB	// 251 GOSUB
	CLASS6,		// class 6 - a numeric expression must follow
	CLASS0,		// class 0 - no further operands
}; // CmdGosub

const u8 P_WAIT[2] = {	// #define TOKEN_WAIT	0xFC	// 252 WAIT
	CLASS6,		// class 6 - a numeric expression must follow
	CLASS0,		// class 0 - no further operands
}; // CmdWait

const u8 P_CLEAR[1] = {	// #define TOKEN_CLEAR	0xFD	// 253 CLEAR
	CLASS0,		// class 0 - no further operands
}; // CmdClear

const u8 P_REM[1] = {	// #define TOKEN_REM	0xFE	// 254 REM
	CLASS5,		// class 5 - variable syntax checked entirely by routine
}; // CmdRem

const u8 P_DATA[1] = { // #define TOKEN_DATA	0xFF	// 255 DATA
	CLASS5, 	// class 5 - variable syntax checked entirely by routine
}; // CmdData

// token syntax table - pointer na syntax class
// ZX80: L0752
const u8* const SyntTab[26] = {
	P_LIST,		// #define TOKEN_LIST	0xE6	// 230 LIST
	P_BEEP,		// #define TOKEN_BEEP	0xE7	// 231 BEEP
	P_CLS,		// #define TOKEN_CLS	0xE8	// 232 CLS
	P_DIM,		// #define TOKEN_DIM	0xE9	// 233 DIM
	P_SAVE,		// #define TOKEN_SAVE	0xEA	// 234 SAVE
	P_FOR,		// #define TOKEN_FOR	0xEB	// 235 FOR
	P_GOTO,		// #define TOKEN_GOTO	0xEC	// 236 GOTO
	P_POKE,		// #define TOKEN_POKE	0xED	// 237 POKE
	P_INPUT,	// #define TOKEN_INPUT	0xEE	// 238 INPUT
	P_RANDOMISE,	// #define TOKEN_RANDOM	0xEF	// 239 RANDOMISE
	P_RETURN,	// #define TOKEN_RETURN	0xF0	// 240 RETURN
	P_LET,		// #define TOKEN_LET	0xF1	// 241 LET
	P_MEMORY,	// #define TOKEN_MEMORY	0xF2	// 242 MEMORY
	P_NEXT,		// #define TOKEN_NEXT	0xF3	// 243 NEXT
	P_LOAD,		// #define TOKEN_LOAD	0xF4	// 244 LOAD
	P_PRINT,	// #define TOKEN_PRINT	0xF5	// 245 PRINT
	P_NEW,		// #define TOKEN_NEW	0xF6	// 246 NEW
	P_RUN,		// #define TOKEN_RUN	0xF7	// 247 RUN
	P_STOP,		// #define TOKEN_STOP	0xF8	// 248 STOP
	P_CONTINUE,	// #define TOKEN_CONT	0xF9	// 249 CONTINUE
	P_IF,		// #define TOKEN_IF	0xFA	// 250 IF
	P_GOSUB,	// #define TOKEN_GOSUB	0xFB	// 251 GOSUB
	P_WAIT,		// #define TOKEN_WAIT	0xFC	// 252 WAIT
	P_CLEAR,	// #define TOKEN_CLEAR	0xFD	// 253 CLEAR
	P_REM,		// #define TOKEN_REM	0xFE	// 254 REM
	P_DATA,		// #define TOKEN_DATA	0xFF	// 255 DATA
};

// command token functions, TOKEN_LIST..TOKEN_DATA
const pTokenFnc TokenFncList[26] = {
	CmdList,	// #define TOKEN_LIST	0xE6	// 230 LIST
	CmdBeep,	// #define TOKEN_BEEP	0xE7	// 231 BEEP
	CmdCls,		// #define TOKEN_CLS	0xE8	// 232 CLS
	CmdDim,		// #define TOKEN_DIM	0xE9	// 233 DIM
	CmdSave,	// #define TOKEN_SAVE	0xEA	// 234 SAVE
	CmdFor,		// #define TOKEN_FOR	0xEB	// 235 FOR
	CmdGoto,	// #define TOKEN_GOTO	0xEC	// 236 GOTO
	CmdPoke,	// #define TOKEN_POKE	0xED	// 237 POKE
	CmdInput,	// #define TOKEN_INPUT	0xEE	// 238 INPUT
	CmdRandomise,	// #define TOKEN_RANDOM	0xEF	// 239 RANDOMISE
	CmdReturn,	// #define TOKEN_RETURN	0xF0	// 240 RETURN
	CmdLet,		// #define TOKEN_LET	0xF1	// 241 LET
	CmdMemory,	// #define TOKEN_MEMORY	0xF2	// 242 MEMORY
	CmdNext,	// #define TOKEN_NEXT	0xF3	// 243 NEXT
	CmdLoad,	// #define TOKEN_LOAD	0xF4	// 244 LOAD
	CmdPrint,	// #define TOKEN_PRINT	0xF5	// 245 PRINT
	CmdNew,		// #define TOKEN_NEW	0xF6	// 246 NEW
	CmdRun,		// #define TOKEN_RUN	0xF7	// 247 RUN
	CmdStop,	// #define TOKEN_STOP	0xF8	// 248 STOP
	CmdContinue,	// #define TOKEN_CONT	0xF9	// 249 CONTINUE
	CmdIf,		// #define TOKEN_IF	0xFA	// 250 IF
	CmdGosub,	// #define TOKEN_GOSUB	0xFB	// 251 GOSUB
	CmdWait,	// #define TOKEN_WAIT	0xFC	// 252 WAIT
	CmdClear,	// #define TOKEN_CLEAR	0xFD	// 253 CLEAR
	CmdRem,		// #define TOKEN_REM	0xFE	// 254 REM
	CmdData,	// #define TOKEN_DATA	0xFF	// 255 DATA
};

#if !USE_DATA_ASM		// 1=use ASM in Data module

// Error
// ZX80: L0008, RST 08H
void Error(u8 err)
{
	if (!IS_ERROR)
	{
		ErrNr = err; // set error code
		ErrPtr = CharPtr; // set error pointer
	}
}

// NEXT error - NEXT without FOR
// ZX80: L0921
void NextError()
{
	Error(ERR_NEXTFOR);
}

// variable not found
// ZX80: L0CD0
void VarNotFound()
{
	Error(ERR_VARNFND);
}

// Subscript error
// ZX80: L0BBE
void SubsError()
{
	Error(ERR_SUBSCRIPT);
}

// memory error
// ZX80: L0963
void MemError()
{
	Error(ERR_MEMORY);
}

// display error, screen is full
// ZX80: L0725
void DispError()
{
	Error(ERR_SCREEN);
}

// overflow error
// ZX80: L0D42
void Overflow()
{
	Error(ERR_OVERFLOW);
}

// RETURN error - no GOSUB
// ZX80: L0970
void ReturnError()
{
	Error(ERR_RETURN);
}

// input error
// ZX80: L09CF
void InputError()
{
	Error(ERR_INPUT);
}

// STOP program
// ZX80: L092E
void CmdStop()
{
	Error(ERR_STOP);
}

// SYNTAX error
// ZX80: L08AE
void SyntaxError()
{
	Error(ERR_SYNTAX);
}

// Initialize new program (no error)
void CmdNew()
{
	// no error
	ErrNr = ERR_OK;

	// clear program memory - set terminating line 0
	ProgNum = 2;
	Prog[0] = 0;
	Prog[1] = 0;

	// clear variables
	CmdClear();

	// clear edit line
	ClrEdit();
	ErrPtr = NULL;	// no error

	// reset view lines
	CurLine = 1;	// current edit program line	
	TopLine = 1;	// first line on the screen

	// clear program and calculator stack
	StckNum = 0;
	CalcNum = 0;

	// clear screen
	CmdCls();

	// prepare lower screen
	PrintYMax = HEIGHT-3;	// max. available print row (or raise ERR_SCREEN error)
	LowScreenH = 2;		// height of lower screen (number of rows)

	// break program
	ProgLine = 0;

	// reset data pointer
	DataPtr = NULL;
}

#endif // !USE_DATA_ASM
