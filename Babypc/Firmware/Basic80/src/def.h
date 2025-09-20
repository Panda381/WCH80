
// ****************************************************************************
//
//                              Definitions
//
// ****************************************************************************
// On Flash memory error decrease SLOTDATANUM.
// On RAM memory error decrease STACKSIZE or EDITNUM.

#ifndef _DEF_H
#define _DEF_H

// Config - use assembler
// This switch should be set to 1 by default. Set to 0 for debugging purposes 
// only - in that case, C code will be used and Slot 0 in CPU1 will be reduced to 256 bytes.
#define USE_ASM		1		// 1=use assembler, 0=use C code
#define DODEBUG		0		// 1=debug, use functions Deb(int k) and DebCh(u8 ch)

// global setup (duplicate set to babypc_vga.h)
#define WIDTH		32		// width in characters
#define HEIGHT		24		// height in text rows
#define WIDTHBYTE	WIDTH		// width in bytes (= 32)
#define FRAMESIZE	(WIDTHBYTE*HEIGHT) // size of frame buffer in bytes

#define	FLASH_BASE	0x08000000	// FLASH base address in the alias region

#define	B0 (1<<0)
#define	B1 (1<<1)
#define	B2 (1<<2)
#define	B3 (1<<3)
#define	B4 (1<<4)
#define	B5 (1<<5)
#define	B6 (1<<6)
#define	B7 (1<<7)

#define KEY_SPACE	40	// SPACE BREAK

// Project setup (size of buffers must be multiply of 4)
#define SLOTNUM		0x0400		// save slot size as multiply of 256 (hardcoded to 1KB)
#define PROGNUM		SLOTNUM		// size of program buffer as multiply of 256 (must be equal to SLOTNUM)
#define VARSNUM		0x0400 		// size of variable buffer (must be multiply of 4)
#define EDITNUM		0x0100		// size of edit line (must be multiply of 4)
#define STCKNUM		0x14		// number of entries in program stack (must be multiply of 4)
#define CALCNUM		0x14		// number of entries in calculator stack (must be multiply of 4)
#define TEMPNUM		0x10		// size of print temporary buffer (must be multiply of 4)

#define SLOTMAX		13		// max. slot index (last slot 13 has reduced size of 512 B)

#define VALSIZE		32		// use value size 32 or 16 bits (16-bit mode is maybe not fully functional)

// variables
// - name - max. 63 characters A..Z,0..9,. (starting with A..Z)
// ..aaaaaa = length of the name (1 to 63, 0 is avoided)
// tt...... = type of variable VARTYPE_*
// 00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
// 01aaaaaa name valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR, variable 'name'
// 10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
// 11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
// 0xC0 ... end of variables
#define VARTYPE_NUM	0		// type of variable - integer number 'name'
#define VARTYPE_FOR	1		// type of variable - FOR integer number 'name' ... or string function 'name$()'
#define VARTYPE_ARRAY	2		// type of variable - array 'name()' ... or integer function 'name()'
#define VARTYPE_STR	3		// type of variable - string 'name$'

#define VAR_MAXLEN	63		// max. lenght of the name
#define VAR_END		0xc0		// terminator of variables ... or invalid variable
#define VAR_MASK	0x3f		// mask of length
#define VAR_SHIFT	6		// shifts of type

#if USE_ASM		// 1=use assembler, 0=use C code
// ASM version (200 B free, with Slot0 and SlotData 1736 B) ... saves 1208 B
#define SLOT0NUM	0x0400		// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
#define SLOT0ALIGN	0x0400		// align slot 0 in memory as multiply of 256
#define SLOTDATANUM	0x0200		// size of data slot, multiply of 256, 0=do not use

#define USE_CALC_ASM	1		// 1=use ASM in Calc module (saves 280 B)
#define USE_DATA_ASM	1		// 1=use ASM in Data module (saves 32 B)
#define USE_EDIT_ASM	1		// 1=use ASM in Edit module (saves 172 B)
#define USE_PARSE_ASM	1		// 1=use ASM in Parse module (saves 100 B)
#define USE_PRINT_ASM	1		// 1=use ASM in Print module (saves 108 B)
#define USE_PROG_ASM	1		// 1=use ASM in Prog module (saves 256 B)
#define USE_SAVE_ASM	1		// 1=use ASM in Save module (saves 4 B)
#define USE_VAR_ASM	1		// 1=use ASM in Var module (saves 140 B)

#else // USE_ASM
// C version (16 B free, with Slot0 and SlotData 528 B)
#define SLOT0ALIGN	0x0100		// align slot 0 in memory as multiply of 256
#define SLOT0NUM	0x0100		// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
#define SLOTDATANUM	0x0100		// size of data slot, multiply of 256, 0=do not use

#define USE_CALC_ASM	0		// 1=use ASM in Calc module
#define USE_DATA_ASM	0		// 1=use ASM in Data module
#define USE_EDIT_ASM	0		// 1=use ASM in Edit module
#define USE_PARSE_ASM	0		// 1=use ASM in Parse module
#define USE_PRINT_ASM	0		// 1=use ASM in Print module
#define USE_PROG_ASM	0		// 1=use ASM in Prog module
#define USE_SAVE_ASM	0		// 1=use ASM in Save module
#define USE_VAR_ASM	0		// 1=use ASM in Var module

#endif // USE_ASM

// Number
#if VALSIZE == 32
#define VAL_BITS	32		// number of bits of numeric type
#define VAL_MAX		0x7fffffffL	// max. value of the number
#define VAL_MIN		-0x80000000L	// min. value of the number
#define VAL_TRUE	-1L		// boolean TRUE value
#define VAL_FALSE	0L		// boolean FALSE value
#else
#define VAL_BITS	16		// number of bits of numeric type
#define VAL_MAX		0x7fff		// max. value of the number
#define VAL_MIN		-0x8000		// min. value of the number
#define VAL_TRUE	-1		// boolean TRUE value
#define VAL_FALSE	0		// boolean FALSE value
#endif

// Error codes
#define ERR_OK		0	// OK
#define ERR_NEXTFOR	1	// NEXT without FOR
#define ERR_VARNFND	2	// Variable not found
#define ERR_SUBSCRIPT	3	// subscript error (invalid range od DIM)
#define ERR_MEMORY	4	// memory error, or stack overflow, or calculator stack overflow
#define ERR_SCREEN	5	// screen is full
#define ERR_OVERFLOW	6	// overflow or another numeric error
#define ERR_RETURN	7	// RETURN error, no GOSUB
#define ERR_INPUT	8	// INPUT can only be used in a program
#define ERR_STOP	9	// STOP command or BREAK key
#define ERR_SYNTAX	10	// SYNTAX error (Only when checking the syntax of the entered line - it will not be displayed as an error code, but using the [S] character)

// Flags
#define FLAG_NOLEAD	B0	// 1=no leading space, 0=leading space
#define FLAG_KMODE	B1	// 1=K mode, 0=L mode
#define FLAG_KUPDATE	B2	// 1=update cursor mode, 0=do not update cursor mode
#define FLAG_NUMRES	B3	// 1=numeric result, 0=string result
#define FLAG_SYNTOFF	B4	// 1=syntax off and executing program, 0=syntax on and checking edit line
#define FLAG_INPUT	B5	// 1=inputting, 0=editing
#define FLAG_GOTO	B6	// 1=GOTO to program line, 0=continue with next program line

// communication commands
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

// ===== Characters

// ZX-80 Character set

// Normal characters 0x00..0x3F (0..63)
//   Mosaic pixels 4321:
//	2 1
//	4 3
#define CH_SPC		0x00	// space
#define CH_QUOT		0x01	// " (string separator) ... to print " character using PRINT command, use CHR$(65)
#define CH_1010		0x02	// mosaic on left
#define CH_1100		0x03	// mosaic on bottom
#define CH_0010		0x04	// mosaic on left top
#define CH_0001		0x05	// mosaic on right top
#define CH_1000		0x06	// mosaic on left bottom
#define CH_0100		0x07	// mosaic on right bottom
#define CH_1001		0x08	// mosaic on left bottom, right top
#define CH_CHESS	0x09	// chess pattern whole
#define CH_CHESS1	0x0A	// chess pattern bottom
#define CH_CHESS2	0x0B	// chess pattern top
#define CH_PERC		0x0C	// percentage %
#define CH_DOLLAR	0x0D	// dollar $
#define CH_COLON	0x0E	// : colon
#define CH_QUERY	0x0F	// ? query
#define CH_LPAR		0x10	// ( left parenthesis
#define CH_RPAR		0x11	// ) right parenthesis
#define CH_MINUS	0x12	// - minus
#define CH_PLUS		0x13	// + plus
#define CH_ASTER	0x14	// * asterisk
#define CH_SLASH	0x15	// / slash
#define CH_EQU		0x16	// = equal
#define CH_GT		0x17	// > greater
#define CH_LT		0x18	// < less
#define CH_SEMI		0x19	// ; semicolon
#define CH_COMMA	0x1A	// , comma
#define CH_DOT		0x1B	// . dot (27)
#define CH_0		0x1C	// 0 (number 0) (28)
#define CH_1		0x1D	// 1 (number 1) (29)
#define CH_2		0x1E	// 2 (number 2) (30)
#define CH_3		0x1F	// 3 (number 3) (31)
#define CH_4		0x20	// 4 (number 4) (32)
#define CH_5		0x21	// 5 (number 5) (33)
#define CH_6		0x22	// 6 (number 6) (34)
#define CH_7		0x23	// 7 (number 7) (35)
#define CH_8		0x24	// 8 (number 8) (36)
#define CH_9		0x25	// 9 (number 9) (37)
#define CH_A		0x26	// A (38)
#define CH_B		0x27	// B (39)
#define CH_C		0x28	// C (40)
#define CH_D		0x29	// D (41)
#define CH_E		0x2A	// E (42)
#define CH_F		0x2B	// F (43)
#define CH_G		0x2C	// G (44)
#define CH_H		0x2D	// H (45)
#define CH_I		0x2E	// I (46)
#define CH_J		0x2F	// J (47)
#define CH_K		0x30	// K (48)
#define CH_L		0x31	// L (49)
#define CH_M		0x32	// M (50)
#define CH_N		0x33	// N (51)
#define CH_O		0x34	// O (52)
#define CH_P		0x35	// P (53)
#define CH_Q		0x36	// Q (54)
#define CH_R		0x37	// R (55)
#define CH_S		0x38	// S (56)
#define CH_T		0x39	// T (57)
#define CH_U		0x3A	// U (58)
#define CH_V		0x3B	// V (59)
#define CH_W		0x3C	// W (60)
#define CH_X		0x3D	// X (61)
#define CH_Y		0x3E	// Y (62)
#define CH_Z		0x3F	// Z (63)

#define CH_INV		0x80	// inversion flag
#define CURSOR		(CH_K + CH_INV)	// cursor character - inverted 'K'

// Keys
#define CH_UP		0x70	// up
#define CH_DOWN		0x71	// down
#define CH_LEFT		0x72	// left
#define CH_RIGHT	0x73	// right
#define CH_HOME		0x74	// home
#define CH_EDIT		0x75	// edit
#define NEWLINE		0x76	// NEWLINE
#define CH_DELETE	0x77	// delete (rubout)

#define CH_NONE		0xc0	// invalid character

// 0x80..0xBF (128..191) = inverted characters

// Tokens 0xD4..0xFF (212..255)
#define TOKEN_FIRST	0xD4

#define TOKEN_QUOT	0xD4 	// 212 "
#define TOKEN_THEN	0xD5	// 213 THEN
#define TOKEN_TO	0xD6	// 214 TO
#define TOKEN_SEMI	0xD7	// 215 ;
#define TOKEN_COMMA	0xD8	// 216 ,
#define TOKEN_RPAR	0xD9	// 217 )
#define TOKEN_LPAR	0xDA	// 218 (
#define TOKEN_NOT	0xDB	// 219 NOT
#define TOKEN_SUB	0xDC	// 220 -
#define TOKEN_ADD	0xDD	// 221 +
#define TOKEN_MUL	0xDE	// 222 *
#define TOKEN_DIV	0xDF	// 223 /
#define TOKEN_AND	0xE0	// 224 AND
#define TOKEN_OR	0xE1	// 225 OR
#define TOKEN_PWR	0xE2	// 226 ** power
#define TOKEN_EQU	0xE3	// 227 =
#define TOKEN_GR	0xE4	// 228 >
#define TOKEN_LT	0xE5	// 229 <
// --- tokens at start of program line (commands):
#define TOKEN_LIST	0xE6	// 230 (A) LIST
#define TOKEN_BEEP	0xE7	// 231 (B) BEEP
#define TOKEN_CLS	0xE8	// 232 (C) CLS
#define TOKEN_DIM	0xE9	// 233 (D) DIM
#define TOKEN_SAVE	0xEA	// 234 (E) SAVE
#define TOKEN_FOR	0xEB	// 235 (F) FOR
#define TOKEN_GOTO	0xEC	// 236 (G) GOTO
#define TOKEN_POKE	0xED	// 237 (H) POKE
#define TOKEN_INPUT	0xEE	// 238 (I) INPUT
#define TOKEN_RANDOM	0xEF	// 239 (J) RANDOMISE
#define TOKEN_RETURN	0xF0	// 240 (K) RETURN
#define TOKEN_LET	0xF1	// 241 (L) LET
#define TOKEN_MEMORY	0xF2	// 242 (M) MEMORY
#define TOKEN_NEXT	0xF3	// 243 (N) NEXT
#define TOKEN_LOAD	0xF4	// 244 (O) LOAD
#define TOKEN_PRINT	0xF5	// 245 (P) PRINT
#define TOKEN_NEW	0xF6	// 246 (Q) NEW
#define TOKEN_RUN	0xF7	// 247 (R) RUN
#define TOKEN_STOP	0xF8	// 248 (S) STOP
#define TOKEN_CONT	0xF9	// 249 (T) CONTINUE
#define TOKEN_IF	0xFA	// 250 (U) IF
#define TOKEN_GOSUB	0xFB	// 251 (V) GOSUB
#define TOKEN_WAIT	0xFC	// 252 (W) WAIT
#define TOKEN_CLEAR	0xFD	// 253 (X) CLEAR
#define TOKEN_REM	0xFE	// 254 (Y) REM
#define TOKEN_DATA	0xFF	// 255 (Z) DATA

#define TOKEN_MIN	TOKEN_LIST // minimal token code on start of the row (minimal command)

// Syntax class
#define CLASS0		0	// no further operands
#define CLASS1		1	// a variable is required
#define CLASS2		2	// an expression, of type integer or string, must follow
#define CLASS3		3	// a numeric expression may follow, otherwise zero will be used
#define CLASS4		4	// integer variable must follow
#define CLASS5		5	// variable syntax checked entirely by routine
#define CLASS6		6	// a numeric expression must follow

#define CLASS_MAX	6	// max. class index

// RAM data offsets (Synchronize it with the file data_asm.S)

// Editing:
#define RAM_CURPTR_OFF		0x00	// 0 CurPtr (u8*): pointer to character in edit line with cursor [K] or [L] (ZX80: 16388 $4004 IY+$04 P_PTR.; ATX80: DATA_CURPTR)
#define RAM_CURLINE_OFF		0x04	// 4 CurLine (u16): current edited program line (ZX80: 16390 $4006 IY+$06 E_PPC; ATX80: DATA_EDITLINE)
// Parsing:
#define RAM_ERRNR_OFF		0x06	// 6 ErrNr (u8): error code ERR_*, ERR_OK=no error (ZX80: 16384 $4000 IY+$00 ERR_NR; ATX80: DATA_ERRCODE)
#define RAM_FLAGS_OFF		0x07	// 7 Flags (u8): flags FLAG_* (ZX80: 16385 $4001 IY+$01 FLAGS, 16409 $4019 IY+$19 FLAGX.; ATX80: GPIOR0, DATA_FLAGX)
#define RAM_CHARPTR_OFF		0x08	// 8 CharPtr (u8*): pointer to current character to parse, in program line or in edit line (ZX80: 16422 $4026 IY+$26 CH_ADD.; ATX80: DATA_CHARPTR)
#define RAM_ERRPTR_OFF		0x0C	// 12 ErrPtr (u8*): pointer to error [S] in parsed text, NULL=no error (ZX80: 16405 $4015 IY+$15 X_PTR; ATX80: DATA_ERRPTR)
// Printing:
#define RAM_PRINTX_OFF		0x10	// 16 PrintX (u8): current print column (ZX80: 16420 $4024 IY+$24 S_POSN_X; ATX80: DATA_PRINTCOLS)
#define RAM_PRINTY_OFF		0x11	// 17 PrintY (u8): current print row (ZX80: 16421 $4025 IY+$25 S_POSN_Y; ATX80: DATA_PRINTROWS)
#define RAM_PRINTYMAX_OFF	0x12	// 18 PrintYMax (u8): max. available print row
#define RAM_LOWSCREENH_OFF	0x13	// 19 LowScreenH (u8): height of lower screen (number of rows) (ZX80: 16402 $4012 IY+$12 DF_SZ; ATX80: DATA_DISP2LINES)
#define RAM_TOPLINE_OFF		0x14	// 20 TopLine (u16): top displayed program line (ZX80: 16403 $4013 IY+$13 S_TOP.; ATX80: DATA_DISPTOP)
// Executing:
#define RAM_PROGLINE_OFF	0x16	// 22 ProgLine (u16): current executed program line (0 = program edited or syntax check, 0xffff = direct execution, other = program is running) (ZX80: 16386 $4002 IY+$02 PPC; ATX80: DATA_EXECLINE)
#define RAM_LASTRESULT_OFF	0x18	// 24 LastResult (s32): last result (ZX80: 16418 $4022 IY+$22 RESULT.; ATX80: DATA_RESULT)
#define RAM_OLDLINE_OFF		0x1C	// 28 OldLine (u16): old program line to continue (ZX80: 16407 $4017 IY+$17 OLDPPC; ATX80: DATA_OLDLINE)
#define RAM_NULSTRING_OFF	0x1E	// 30 NulString (u8): empty string (contains character CH_QUOT)
#define RAM_VARDEST_OFF		0x20	// 32 VarDest (u8*): pointer to variable name in current program line or in edit line (ZX80: 16416 $4020 IY+$20 DEST; ATX80: DATA_VARDEST)
#define RAM_DATAPTR_OFF		0x24	// 36 DataPtr (u8*): pointer to program line to read data, NULL = not initialized
#define RAM_LASTTIME_OFF	0x28	// 40 LastTime (u32): last timer Time() in 0.02 us ... cleared on start of program
// Buffer size:
#define RAM_PROGNUM_OFF		0x2C	// 44 ProgNum (u16): size of data in program buffer (including terminating line 0)
#define RAM_VARSNUM_OFF		0x2E	// 46 VarsNum (u16): size of data in variables buffer (including terminating byte 0xFF)
#define RAM_EDITNUM_OFF		0x30	// 48 EditNum (u16): size of data in edit line (including terminating byte NEWLINE)
#define RAM_STCKNUM_OFF		0x32	// 50 StckNum (u8): number of entries in program stack
#define RAM_CALCNUM_OFF		0x33	// 51 CalcNum (u8): number of entries in calculator stack
// Buffers:
#define RAM_PROG_OFF		0x34				// 52 Prog (u8[PROGNUM]): program buffer (end with line = 0) (ZX80: 16424 $4028 IY+$28; ATX80: Program)
#define RAM_VARS_OFF		(RAM_PROG_OFF+PROGNUM)		// Vars (u8[VARSNUM]): program variables (end with 0xFF) (ZX80: 16392 $4008 IY+$08 VARS; ATX80: DATA_VARSPTR)
#define RAM_EDIT_OFF		(RAM_VARS_OFF+VARSNUM)		// Edit (u8[EDITNUM]): edit line (end with NEWLINE) (ZX80: 16394 $400A IY+$0A E_LINE; ATX80: DATA_EDITPTR)
#define RAM_STCK_OFF		(RAM_EDIT_OFF+EDITNUM)		// Stck (u16[STCKNUM]): program stack
#define RAM_CALCSTCK_OFF	(RAM_STCK_OFF+2*STCKNUM)	// CalcStck (u8[2*CALCNUM]): calculator stack - operation and priority
#define RAM_VALSTCK_OFF		(RAM_CALCSTCK_OFF+2*CALCNUM)	// ValStck (s32[CALCNUM]): calculator stack - value
#define RAM_TEMP_OFF		(RAM_VALSTCK_OFF+4*CALCNUM)	// Temp (u8[TEMPNUM]): print temporary buffer

#define RAM_END_OFF		(RAM_TEMP_OFF+TEMPNUM)

#endif // _DEF_H
