
// ****************************************************************************
//
//                           Global data area
//
// ****************************************************************************
// Global data is used for quick access to data using the GP (global pointer) register.

// In linker script *.ld:
//	.globaldata (NOLOAD) :
//	{
//		. = ALIGN(4);
//		__globaldata_start = .;
//		KEEP(*(.globaldata))
//		__globaldata_end = .;
//		. = ALIGN(4);
//	} > RAM
//	PROVIDE( __global_pointer$ = __globaldata_start );	/* pointer to global data section */

// In startup crt0.S:
//	// setup global pointer
//	.option	push			// push current linker setup
//	.option	norelax			// disable linker relaxation (disable optimizing programs at link-time)
//	la	gp,__global_pointer$	// global pointer to global data in RAM
//	.option	pop			// pop original linker setup

// RISC-V assembler:
// Function may destroy (storer=caller): x1 (ra), x5-x7 (t0-t2), x10-x15 (a0-a5)
// Function must save (storer=callee): x8 (s0/fp), x9 (s1)
// Special registers: x0 (zero), x2 (sp), x3 (gp), x4 (tp)

// TP (x4, Thread Pointer) register is not used in this system, can be used as special register in functions.

#include "def.h"

	.section	.globaldata, "aw"

RamData_Beg:			// start of global RAM data

// >>> Beware of alignment of addresses of variables! <<<
// - Synchronize it with the file def.h, entries RAM_*_OFF.

// Editing:
CurPtr:		.space	4	// 0x00 (u8*): pointer to character in edit line with cursor [K] or [L] (ZX80: 16388 $4004 IY+$04 P_PTR.; ATX80: DATA_CURPTR)
CurLine:	.space	2	// 0x04 (u16): current edited program line (ZX80: 16390 $4006 IY+$06 E_PPC; ATX80: DATA_EDITLINE)
// Parsing:
ErrNr:		.space	1	// 0x06 (u8): error code ERR_*, ERR_OK=no error (ZX80: 16384 $4000 IY+$00 ERR_NR; ATX80: DATA_ERRCODE)
Flags:		.space	1	// 0x07 (u8): flags FLAG_* (ZX80: 16385 $4001 IY+$01 FLAGS, 16409 $4019 IY+$19 FLAGX.; ATX80: GPIOR0, DATA_FLAGX)
CharPtr:	.space	4	// 0x08 (u8*): pointer to current character to parse, in program line or in edit line (ZX80: 16422 $4026 IY+$26 CH_ADD.; ATX80: DATA_CHARPTR)
ErrPtr:		.space	4	// 0x0C (u8*): pointer to error [S] in parsed text, NULL=no error (ZX80: 16405 $4015 IY+$15 X_PTR; ATX80: DATA_ERRPTR)
// Printing:
PrintX:		.space	1	// 0x10 (u8): current print column (ZX80: 16420 $4024 IY+$24 S_POSN_X; ATX80: DATA_PRINTCOLS)
PrintY:		.space	1	// 0x11 (u8): current print row (ZX80: 16421 $4025 IY+$25 S_POSN_Y; ATX80: DATA_PRINTROWS)
PrintYMax:	.space	1	// 0x12 (u8): max. available print row
LowScreenH:	.space	1	// 0x13 (u8): height of lower screen (number of rows) (ZX80: 16402 $4012 IY+$12 DF_SZ; ATX80: DATA_DISP2LINES)
TopLine:	.space	2	// 0x14 (u16): top displayed program line (ZX80: 16403 $4013 IY+$13 S_TOP.; ATX80: DATA_DISPTOP)
// Executing:
ProgLine:	.space	2	// 0x16 (u16): current executed program line (0 = program edited or syntax check, 0xffff = direct execution, other = program is running) (ZX80: 16386 $4002 IY+$02 PPC; ATX80: DATA_EXECLINE)
LastResult:	.space	4	// 0x18 (s32): last result (ZX80: 16418 $4022 IY+$22 RESULT.; ATX80: DATA_RESULT)
OldLine:	.space	2	// 0x1C (u16): old program line to continue (ZX80: 16407 $4017 IY+$17 OLDPPC; ATX80: DATA_OLDLINE)
NulString:	.space	1	// 0x1E (u8): empty string (contains character CH_QUOT)
		.space	1	// 0x1F: ... reserved (align)
VarDest:	.space	4	// 0x20 (u8*): pointer to variable name in current program line or in edit line (ZX80: 16416 $4020 IY+$20 DEST; ATX80: DATA_VARDEST)
DataPtr:	.space	4	// 0x24 (u8*): pointer to program line to read data, NULL = not initialized
LastTime:	.space	4	// 0x28 (u32): last timer Time() in 0.02 us ... cleared on start of program
// Buffer size:
ProgNum:	.space	2	// 0x2C (u16): size of data in program buffer (including terminating line 0)
VarsNum:	.space	2	// 0x2E (u16): size of data in variables buffer (including terminating byte 0xFF)
EditNum:	.space	2	// 0x30 (u16): size of data in edit line (including terminating byte NEWLINE)
StckNum:	.space	1	// 0x32 (u8): number of entries in program stack
CalcNum:	.space	1	// 0x33 (u8): number of entries in calculator stack
// Buffers (must be aligned to 32-bit):
Prog:		.space	PROGNUM		// 0x34 (PROGNUM): program buffer (end with line = 0) (ZX80: 16424 $4028 IY+$28; ATX80: Program)
Vars:		.space	VARSNUM		// program variables (end with 0xFF) (ZX80: 16392 $4008 IY+$08 VARS; ATX80: DATA_VARSPTR)
Edit:		.space	EDITNUM		// edit line (end with NEWLINE) (ZX80: 16394 $400A IY+$0A E_LINE; ATX80: DATA_EDITPTR)
Stck:		.space	2*STCKNUM	// program stack
CalcStck:	.space	2*CALCNUM	// calculator stack - operation and priority
ValStck:	.space	4*CALCNUM	// calculator stack - value
Temp:		.space	TEMPNUM		// print temporary buffer, for numbers

// FrameBuf:	.space	FRAMESIZE	// display frame buffer (ZX80: 16396 $400C IY+$0C D_FILE; ATX80: DATA_DISPPTR)

RamData_End:			// end of global RAM data

.global RamData_Beg

// Editing:
.global CurPtr
.global CurLine
// Parsing:
.global ErrNr
.global Flags
.global CharPtr
.global ErrPtr
// Printing:
.global PrintX
.global PrintY
.global PrintYMax
.global LowScreenH
.global TopLine
// Executing:
.global ProgLine
.global LastResult
.global OldLine
.global NulString
.global VarDest
.global DataPtr
.global LastTime
// Buffer size:
.global ProgNum
.global VarsNum
.global EditNum
.global StckNum
.global CalcNum
// Buffers:
.global Prog
.global Vars
.global Edit
.global Stck
.global CalcStck
.global ValStck
.global Temp

.global RamData_End


#if USE_DATA_ASM		// 1=use ASM in Data module

	.section	.text

// ----------------------------------------------------------------------------
// Overflow error
// ----------------------------------------------------------------------------

.global Overflow
Overflow:
	li	a0,ERR_OVERFLOW

// continue to Error

// ----------------------------------------------------------------------------
// Error
//  Input: A0 ... error code ERR_*
// ----------------------------------------------------------------------------

.global Error
Error:
	lbu	a1,RAM_ERRNR_OFF(gp)	// get current error vode
	bnez	a1,2f			// error is already set
	sb	a0,RAM_ERRNR_OFF(gp)	// save error code
	lw	a1,RAM_CHARPTR_OFF(gp)	// character pointer
	sw	a1,RAM_ERRPTR_OFF(gp)	// set error pointer
2:	ret

// ----------------------------------------------------------------------------
// NEXT error - NEXT without FOR
// ----------------------------------------------------------------------------

.global NextError
NextError:
	li	a0,ERR_NEXTFOR
	j	Error

// ----------------------------------------------------------------------------
// variable not found
// ----------------------------------------------------------------------------

.global VarNotFound
VarNotFound:
	li	a0,ERR_VARNFND
	j	Error

// ----------------------------------------------------------------------------
// Subscript error
// ----------------------------------------------------------------------------

.global SubsError
SubsError:
	li	a0,ERR_SUBSCRIPT
	j	Error

// ----------------------------------------------------------------------------
// memory error
// ----------------------------------------------------------------------------

.global MemError
MemError:
	li	a0,ERR_MEMORY
	j	Error

// ----------------------------------------------------------------------------
// display error, screen is full
// ----------------------------------------------------------------------------

.global DispError
DispError:
	li	a0,ERR_SCREEN
	j	Error

// ----------------------------------------------------------------------------
// RETURN error - no GOSUB
// ----------------------------------------------------------------------------

.global ReturnError
ReturnError:
	li	a0,ERR_RETURN
	j	Error

// ----------------------------------------------------------------------------
// input error
// ----------------------------------------------------------------------------

.global InputError
InputError:
	li	a0,ERR_INPUT
	j	Error

// ----------------------------------------------------------------------------
// STOP program
// ----------------------------------------------------------------------------

.global CmdStop
CmdStop:
	li	a0,ERR_STOP
	j	Error

// ----------------------------------------------------------------------------
// SYNTAX error
// ----------------------------------------------------------------------------

.global SyntaxError
SyntaxError:
	li	a0,ERR_SYNTAX
	j	Error

// ----------------------------------------------------------------------------
// Initialize new program
// ----------------------------------------------------------------------------

.global CmdNew
CmdNew:
	// save return address
	addi	sp,sp,-4
	sw	ra,0(sp)

	// no error
	sb	zero,RAM_ERRNR_OFF(gp)	// set ErrNr to 0

	// clear program memory - set terminating line 0
	li	a0,2
	sh	a0,RAM_PROGNUM_OFF(gp)	// set ProgNum to 2
	sw	zero,RAM_PROG_OFF(gp)	// set Prog[0] and Prog[1] to 0 (= line number of last row = 0; this address is aligned OK)

	// clear variables
	call	CmdClear

	// clear edit line
	call	ClrEdit
	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer ErrPtr

	// reset view lines
	li	a0,1
	sh	a0,RAM_CURLINE_OFF(gp)	// set current edit program line CurLine to 1
	sh	a0,RAM_TOPLINE_OFF(gp)	// set first line on the screen TopLine to 1

	// clear program and calculator stack
	sb	zero,RAM_STCKNUM_OFF(gp) // clear program stack StckNum
	sb	zero, RAM_CALCNUM_OFF(gp) // clear calculator stack CalcNum

	// clear scren
	call	CmdCls

	// prepare lower screen
	li	a0,HEIGHT-3		// index of last row = 21
	sb	a0,RAM_PRINTYMAX_OFF(gp) // set max. available print row PrintYMax
	li	a0,2
	sb	a0,RAM_LOWSCREENH_OFF(gp) // set height of lower screen (number of rows) LowScreenH to 2

	// break program
	sh	zero,RAM_PROGLINE_OFF(gp) // set current program line ProgLine to 0 (= not executing)

	// reset data pointer
	sw	zero,RAM_DATAPTR_OFF(gp) // reset data pointer

	// restore return address
	lw	ra,0(sp)
	addi	sp,sp,4
	ret

#endif // USE_DATA_ASM
