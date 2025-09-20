
// ****************************************************************************
//
//                                Program
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_PROG_ASM		// 1=use ASM in Prog module

// ----------------------------------------------------------------------------
// LIST command
// ----------------------------------------------------------------------------

.global CmdList
CmdList:
	// get current edited program line
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- get last result with line number

	// default value if line is invalid
	blez	a1,2f			// line <= 0, use default line = 1
	li	a2,0xffff		// max. value
	bltu	a1,a2,3f		// skip if value is OK
2:	li	a1,1			// A1 <- use default line 1

	// set new current line
3:	sh	a1,RAM_CURLINE_OFF(gp)	// set current line CirLine
	sh	a1,RAM_TOPLINE_OFF(gp)	// set top line TopLine

	// stop running (to stop processing program)
	sh	zero,RAM_PROGLINE_OFF(gp) // set program line ProgLine
	ret

// ----------------------------------------------------------------------------
// GOSUB command
// ----------------------------------------------------------------------------

.global CmdGosub
CmdGosub:
	// get current stack size
	lbu	a0,RAM_STCKNUM_OFF(gp)	// A0 <- current number of entries in program stack StckNum

	// check if stack is full
	li	a1,STCKNUM		// max. number of entries
	blt	a0,a1,2f		// skip if number of entries is OK (n < STCKNUM)
	j	MemError		// stack overflow

	// get pointer to the stack
2:	la	a1,Stck			// A1 <- stack buffer
	slli	a2,a0,1			// A2 <- index * 2 = offset in the stack
	add	a1,a1,a2		// A1 <- pointer to the stack

	// set new number of entries in the stack
	addi	a0,a0,1			// A0 <- number of entries + 1
	sb	a0,RAM_STCKNUM_OFF(gp)	// set new number of entries

	// store current program line + 1 to the stack
	lhu	a2,RAM_PROGLINE_OFF(gp)	// A2 <- get current program line
	addi	a2,a2,1			// increase to next program line
	sh	a2,0(a1)		// store current program line to the stack

	// jump to new line
	j	CmdGoto

// ----------------------------------------------------------------------------
// RETURN command
// ----------------------------------------------------------------------------

.global CmdReturn
CmdReturn:
	// get current stack size
	lbu	a0,RAM_STCKNUM_OFF(gp)	// A0 <- current number of entries in program stack StckNum

	// check if stack is empty
	bnez	a0,2f			// number of entries is OK
	j	ReturnError		// error RETURN without GOSUB

	// set new stack size
2:	addi	a0,a0,-1		// A0 <- decrease number of entries
	sb	a0,RAM_STCKNUM_OFF(gp)	// set new stack size

	// get pointer to the stack
	la	a1,Stck			// A1 <- stack buffer
	slli	a0,a0,1			// A0 <- index * 2 = offset in the stack
	add	a1,a1,a0		// A1 <- pointer to the stack

	// load new program line
	lhu	a0,0(a1)		// A0 <- get program line
	j	CmdGoto_Go

// ----------------------------------------------------------------------------
// GOTO program line
// ----------------------------------------------------------------------------

.global CmdGoto
CmdGoto:
	// set new current program line
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- get last result LatResult

CmdGoto_Go:
	sh	a0,RAM_PROGLINE_OFF(gp)	// set current program line

	// set GOTO flag
	lbu	a0,RAM_FLAGS_OFF(gp)	// get flags
	ori	a0,a0,FLAG_GOTO		// set GOTO flag
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags
	ret

// ----------------------------------------------------------------------------
// CONTINUE program
// ----------------------------------------------------------------------------

.global CmdContinue
CmdContinue:
	lw	a0,RAM_OLDLINE_OFF(gp)	// A0 <- old line OldLine
	j	CmdGoto_Go

// ----------------------------------------------------------------------------
// RUN program
// ----------------------------------------------------------------------------

.global CmdRun
CmdRun:
	// initialize stacks
	sb	zero,RAM_CALCNUM_OFF(gp) // clear calculator stack
	sb	zero,RAM_STCKNUM_OFF(gp) // clear program stack

	// reset data pointer
	sw	zero,RAM_DATAPTR_OFF(gp) // clear data pointer

	// clear variables
//	call	CmdClear		// do not use - it would require push stack
	li	a0,1			// A0 <- 1 (it will be used later as program line 1)
	sh	a0,RAM_VARSNUM_OFF(gp)	// size of data in variable buffer
	li	a1,VAR_END		// A1 <- end terminator
	sb	a1,RAM_VARS_OFF(gp)	// set end mark of variables

	// clear timer (get timer from SysTick->CNT; CNT = offset 0x08)
#define SYSTICK_CNT	0xE000F008	// SysTick System counter
	lui	a5,%hi(SYSTICK_CNT)	// A5 <- systick counter HIGH
	lw	a4,%lo(SYSTICK_CNT)(a5)	// A4 <- get systick counter
	sw	a4,RAM_LASTTIME_OFF(gp)	// save init timer

	// default run from line 1
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- get last result
	beqz	a1,CmdGoto_Go		// use line 1 if last result is 0
	j	CmdGoto			// otherwise use line from last result

// ----------------------------------------------------------------------------
// IF command
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global CmdIf
CmdIf:
	// if condition is FALSE, skip rest of line
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- get last result LastResult
	bnez	a0,CmdRemRet		// condition is TRUE (or check syntax), continue execution

	// if not checkinf syntax
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- flags
	andi	a0,a0,FLAG_SYNTOFF	// check if syntax is OFF
	beqz	a0,CmdRemRet		// if syntax is ON, continue with checking rest of row

	// if run-time, skip rest of the line
	//j	CmdRem

// continue to CmdRem

// ----------------------------------------------------------------------------
// REM command
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global CmdRem
CmdRem:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get first character
	call	GetChar

	// check end of line
2:	li	a1,NEWLINE
	beq	a0,a1,8f		// end of line

	// get next character
	call	NextChar
	j	2b

	// pop registers
8:	lw	ra,0(sp)
	addi	sp,sp,4
CmdRemRet:
	ret

// ----------------------------------------------------------------------------
// DATA command
// ----------------------------------------------------------------------------

.global CmdData
CmdData:
	// if not checkinf syntax - skip whole line
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- flags
	andi	a0,a0,FLAG_SYNTOFF	// check if syntax is OFF
	bnez	a0,CmdRem		// if syntax is OFF, skip whole line

	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get character, skip spaces
2:	call	GetChar

	// end of row
	li	a1,NEWLINE
	beq	a0,a1,9f		// exit on new line

	// separator ','
	li	a1,TOKEN_COMMA
	bne	a0,a1,3f		// skip if not comma
	call	NextChar		// skip comma character
	j	2b

	// expression
3:	call	ScanCalc		// evaluate expression

	// check numeric result
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- get flags
	andi	a2,a2,FLAG_NUMRES	// check numeric result flag
	beqz	a2,8f			// numeric result error

	// continue if not error
	lbu	a0,RAM_ERRNR_OFF(gp)	// A0 <- error number ErrNr
	beqz	a0,2b			// continue

	// syntax error
8:	call	SyntaxError

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,4
	ret

// ----------------------------------------------------------------------------
// move to next program line (returns next program line)
// input: A0 ... pointer to program line
// output: A0 ... pointer to next program line
// ----------------------------------------------------------------------------
//  lineL lineH text NEWLINE ... program line, 0 = end of program

.global NextLine
NextLine:
	// end of program
	lbu	a1,0(a0)		// A1 <- line number LOW
	lbu	a2,1(a0)		// A2 <- line number HIGH
	or	a1,a1,a2		// check line number
	beqz	a1,9f			// line number is 0, end of program, stop searching

	// skip line number
	addi	a0,a0,2

	// search end of program line
	li	a1,NEWLINE		// A1 <- end line character
2:	lbu	a2,0(a0)		// A2 <- load next byte
	addi	a0,a0,1			// A0 <- increase pointer
	bne	a1,a2,2b		// loop if not end of line
9:	ret

// ----------------------------------------------------------------------------
// find currect edited program line
// output: A0 ... pointer to program line
// ----------------------------------------------------------------------------

.global FindCurLine
FindCurLine:
	lhu	a0,RAM_CURLINE_OFF(gp)	// A0 <- current edited line CurLine
	li	a1,0			// A1 <- clear pointer to previous line
	//j	FindLine

// continue to FindLine

// ----------------------------------------------------------------------------
// find program line by line number
// input: A0 ... line number to find
//        A1 ... pointer to u8* pointer to get previous line, or NULL if not required, returns NULL if no previous line
// output: A0 ... pointer to program line (or higher)
// ----------------------------------------------------------------------------

.global FindLine
FindLine:
	// clear previous line to NULL
	beqz	a1,2f			// skip if previous line is not required (pointer = NULL)
	sw	zero,0(a1)		// set previous line to NULL

	// prepare pointer to start of program
2:	addi	a2,gp,RAM_PROG_OFF	// A2 <- pointer to start of program buffer

// A0 = line number to find 'line'
// A1 = pointer to previous line 'prevline'
// A2 = pointer to program 'p'

	// get line number 'line2'
3:	lbu	a3,1(a2)		// A3 <- program line HIGH
	lbu	a4,0(a2)		// A4 <- program line LOW
	slli	a3,a3,8			// A3 <- program line HIGH * 256
	or	a3,a3,a4		// A3 <- line number 'line2'

	// line has been found, or end of program reached
	beqz	a3,9f			// end of program reached
	bge	a3,a0,9f		// line has been found

	// save previous line
	beqz	a1,4f			// skip if previous line is not required (pointer = NULL)
	sw	a2,0(a1)		// save previous line

	// move to next program line
4:	addi	a2,a2,2			// skip program line
	li	a3,NEWLINE		// A3 <- end line character
5:	lbu	a4,0(a2)		// A4 <- load next byte
	addi	a2,a2,1			// A0 <- increase pointer
	bne	a4,a3,5b		// loop if not end of line
	j	3b			// continue loop

	// return pointer to program line
9:	mv	a0,a2			// A0 <- pointer to program line
	ret

// ----------------------------------------------------------------------------
// WAIT command
// ----------------------------------------------------------------------------

.global CmdWait
CmdWait:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// get delay in [ms] from last result
	lw	s0,RAM_LASTRESULT_OFF(gp) // S0 <- get delay from LastResult

	// end of loop
2:	blez	s0,9f			// end of loop, if 'ms' is <= 0

	// delay, max. 100ms at once
	li	a0,100			// A0 <- max. delay 100 ms
	bge	s0,a0,3f		// skip if 'ms' >= 100
	mv	a0,s0			// A0 <- limit delay to 'ms'
3:	sub	s0,s0,a0		// decrease remaining time 'ms'
	call	WaitMs			// delay

	// break program (space key is pressed)
	li	a0,KEY_SPACE
	call	KeyPressed		// check is space is pressed
	beqz	a0,2b			// continue if space is not pressed

	// break program
	call	CmdStop			// break program
	call	KeyFlush		// flush keys

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// POKE command
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global CmdPoke
CmdPoke:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)

	// save previous result - destination address
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- last result LastResult
	sw	a0,4(sp)		// save address

	// evaluate expression after comma, get value
	call	ScanCalc

	// check numeric result
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- get flags
	andi	a2,a2,FLAG_NUMRES	// check numeric result flag
	bnez	a2,2f			// skip if numeric result OK
	call	SyntaxError		// syntax error

	// break on error
2:	lbu	a2,RAM_ERRNR_OFF(gp)	// A2 <- error code ErrNr
	bnez	a2,9f			// exit on error

	// check end of command - quit if only checking syntax
	call	CheckEnd		// check end
	beqz	a0,9f			// exit if only checking syntax

	// get value to be set -> A1, address -> A0
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- last result LastResult
	lw	a0,4(sp)		// A0 <- address

	// Frame counter (addr = -1)
	li	a2,-1
	bne	a2,a0,3f		// skip if address is not -1
	lui	a2,%hi(DispFrame)	// A2 <- current frame
	sw	a1,%lo(DispFrame)(a2)	// save new current frame
	j	9f

	// Random seed (addr = -2)
3:	li	a2,-2
	bne	a2,a0,4f		// skip if address is not -2
	lui	a2,%hi(RandSeed)	// A2 <- random seed
	sw	a1,%lo(RandSeed)(a2)	// save new random seed
	j	9f

	// Time in 0.02 us (addr = -3)
4:	li	a2,-3
	bne	a2,a0,4f		// skip if address is not -3
#define SYSTICK_CNT	0xE000F008	// SysTick System counter
	lui	a5,%hi(SYSTICK_CNT)	// A5 <- systick counter HIGH
	lw	a4,%lo(SYSTICK_CNT)(a5)	// A4 <- get systick counter
	add	a4,a4,a1		// A4 <- timer + value
	sw	a4,RAM_LASTTIME_OFF(gp)	// save new timer
	j	9f

	// print position X (addr = -4)
4:	li	a2,-4
	bne	a2,a0,4f
	bgez	a1,1f			// skip if X >= 0
	li	a1,0			// minimal X is 0
1:	li	a5,WIDTH		// maximal X
	ble	a1,a5,1f		// skip if X <= WIDTH
	mv	a1,a5			// A1 <- limit X to WIDTH
1:	sb	a1,RAM_PRINTX_OFF(gp)	// set new PrintX
	j	9f

	// print position Y (addr = -5)
4:	li	a2,-5
	bne	a2,a0,4f
	bgez	a1,1f			// skip if Y >= 0
	li	a1,0			// minimal Y is 0
1:	li	a5,HEIGHT-1		// maximal Y
	ble	a1,a5,1f		// skip if Y <= HEIGHT-1
	mv	a1,a5			// A1 <- limit Y to HEIGHT-1
1:	sb	a1,RAM_PRINTY_OFF(gp)	// set new PrintY
	j	9f

	// buffers
4:	li	a2,FRAMESIZE		// max. size of frame buffer
	bgeu	a0,a2,9f		// skip if invalid screen address
	lui	a2,%hi(FrameBuf)	// A2 <- frame buffer
	add	a2,a2,a0		// A2 <- address in frame buffer
	sb	a1,%lo(FrameBuf)(a2)	// write byte to frame buffer

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// MEMORY command
// ----------------------------------------------------------------------------

.global CmdMemory
CmdMemory:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// print program size
	lhu	a0,RAM_PROGNUM_OFF(gp)	// A0 <- size of program
	call	PrintNum		// print program used
	li	a0,CH_PLUS		// A0 <- '+'
	call	PrintCh			// print '+'
	lhu	a1,RAM_PROGNUM_OFF(gp)	// A1 <- size of program
	li	a0,PROGNUM		// A0 <- size of program buffer
	sub	a0,a0,a1		// A0 <- free space
	call	PrintNum		// print program free

	li	a0,CH_COMMA		// A0 <- ','
	call	PrintCh			// print ','

	// print variable size
	lhu	a0,RAM_VARSNUM_OFF(gp)	// A0 <- size of variables
	call	PrintNum		// print variable used
	li	a0,CH_PLUS		// A0 <- '+'
	call	PrintCh			// print '+'
	lhu	a1,RAM_VARSNUM_OFF(gp)	// A1 <- size of variables
	li	a0,VARSNUM		// A0 <- size of variable buffer
	sub	a0,a0,a1		// A0 <- free space
	call	PrintNum		// print variable free

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// display NEWLINE
	j	PrintCR

// ----------------------------------------------------------------------------
// BEEP command
// ----------------------------------------------------------------------------

.global CmdBeep
CmdBeep:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)

	// save previous result - required frequency in 0.01 Hz (0 = silent)
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- last result LastResult
	sw	a0,4(sp)		// save frequency

	// evaluate expression after comma, get value
	call	ScanCalc

	// check numeric result
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- get flags
	andi	a2,a2,FLAG_NUMRES	// check numeric result flag
	bnez	a2,2f			// skip if numeric result OK
	call	SyntaxError		// syntax error

	// break on error
2:	lbu	a2,RAM_ERRNR_OFF(gp)	// A2 <- error code ErrNr
	bnez	a2,9f			// exit on error

	// check end of command - quit if only checking syntax
	call	CheckEnd		// check end
	beqz	a0,9f			// exit if only checking syntax

	// check minimal frequency 16 Hz, maximal frequency 16 kHz
	lw	a1,4(sp)		// A1 <- frequency in 0.01 Hz
	addi	a2,a1,-1600		// A2 <- frequency - 1600 (minimal frequency)
	li	a3,1600000-1600		// A3 <- maximal frequency - minimal frequency
	bgtu	a2,a3,4f		// skip if frequency is out of range

	// get divider div = (100*1000000 + f/2)/f - 1
	li	a0,100*1000000		// A0 <- 1 MHz in 0.01 Hz = 100*1000000
	srai	a2,a1,1			// A2 <- frequency / 2
	add	a0,a0,a2		// A0 <- 100*1000000 + f/2
	call	__divsi3		// divide
	addi	a0,a0,-1		// - 1

	// play tone
	call	PlayTone

	// wait
4:	call	CmdWait

	// stop sound
	call	StopSound

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// FOR command
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global CmdFor
CmdFor:
	// push registers
	addi	sp,sp,-12
	sw	ra,0(sp)

	// get INIT value
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- last result with INIT value
	sw	a0,4(sp)		// save INIT value

	// load LIMIT
	call	ScanCalc		// evaluate expression

	// check numeric result
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- flags
	andi	a0,a0,FLAG_NUMRES	// check numeric result
	bnez	a0,1f			// skip if numeric result
	call	SyntaxError		// set syntax error - numeric result is required

	// check error
1:	lbu	a0,RAM_ERRNR_OFF(gp)	// A0 <- get error number ErrNr
	bnez	a0,9f			// exit on error

	// check end of command - quit if only checking syntax
	call	CheckEnd		// check end
	beqz	a0,9f			// exit if only checking syntax

	// get LIMIT value
	lw	a0,4(sp)		// A0 <- INIT value
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- get LIMIT value from last result
	sw	a0,RAM_LASTRESULT_OFF(gp) // set INIT value as current last result
	sw	a1,4(sp)		// save LIMIT value

	// store init value into variable (or create variable)
	call	CmdLet			// set variable to INIT value
	lbu	a0,RAM_ERRNR_OFF(gp)	// A0 <- get error number ErrNr
	bnez	a0,9f			// exit on error

	// find variable (returned pointer is valid) ... returns pointer to start of variable + 1
	// returns pointer to data of integer variable, or start of text variable, or NULL if not found
	lw	a0,RAM_VARDEST_OFF(gp)	// A0 <- pointer to variable name VarDest
	addi	a1,sp,8			// A1 <- temporary in stack 'var'
	call	GetVar			// get pointer to variable, -> A0 'p'

	// check if variable is already initialized (need to change simple variable to loop variable)
// 00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
// 01aaaaaa name valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR, variable 'name'
	lw	a4,8(sp)		// A4 <- pointer to variable
	lbu	a1,0(a4)		// A1 <- get variable type and length
	andi	a3,a1,VAR_MASK		// A3 <- length of name of the variable
	ori	a2,a3,(VARTYPE_FOR << VAR_SHIFT) // set FOR type of the variable
	sb	a2,0(a4)		// store new type of the variable

// Here is: A0 = pointer to data 'p', A1 = variable type and length 'ch', 4(sp) = LIMIT value

	// move to end of simple variable
#if VALSIZE == 32
	addi	a0,a0,4
#define FORSIZE	6	// required size of new data to insert
#else
	addi	a0,a0,2
#define FORSIZE	4
#endif

	// check FOR type of the variable
	srli	a1,a1,VAR_SHIFT		// A1 <- get top 2 bits of the name
#if VARTYPE_NUM == 0
	bnez	a1,4f			// skip if FOR type of the variable
#else
	li	a5,VARTYPE_NUM
	bne	a1,a5,4f
#endif

// here is: A0 = pointer to end of simple variable 'p', 4(sp) = LIMIT value

	// check free space
	lhu	a1,RAM_VARSNUM_OFF(gp)	// A1 <- current size of variables 'num'
	li	a2,VARSNUM-FORSIZE	// A2 <- required max. size of variables
	bleu	a1,a2,2f		// skip if free space is OK
	call	MemError		// memory error
	j	9f

	// set new size of data
2:	addi	a2,a1,FORSIZE		// A2 <- new size of variables num+FORSIZE
	sh	a2,RAM_VARSNUM_OFF(gp)	// set new size of variable data

	// shift data
	addi	a3,gp,RAM_VARS_OFF	// A3 <- address of Vars buffer
	add	a3,a3,a1		// A3 <- address of old end of variables + 1 = &Vars[num] = 's'
	sub	a1,a3,a0		// A1 <- size of data to move 'i' = 's' - 'p'
3:	addi	a3,a3,-1		// decrease pointer
	lbu	a2,0(a3)		// A2 <- load byte
	sb	a2,FORSIZE(a3)		// write byte
	add	a1,a1,-1		// decrement loop counter
	bnez	a1,3b			// loop

// here is: A0 = pointer to end of simple variable 'p', 4(sp) = LIMIT value

	// store limit
4:	lw	a1,4(sp)		// A1 <- load LIMIT value
	sb	a1,0(a0)		// store byte 1
	srai	a1,a1,8			// A1 <- shift to byte 2
	sb	a1,1(a0)		// store byte 2
#if VALSIZE == 32
	srai	a1,a1,8			// A1 <- shift to byte 3
	sb	a1,2(a0)		// store byte 3
	srai	a1,a1,8			// A1 <- shift to byte 4
	sb	a1,3(a0)		// store byte 4
#endif

	// store following program line
	lhu	a1,RAM_PROGLINE_OFF(gp)	// A1 <- current program line
	addi	a1,a1,1			// A1 <- next program line
#if VALSIZE == 32
	sb	a1,4(a0)		// store byte 1
	srai	a1,a1,8			// A1 <- shift to byte 2
	sb	a1,5(a0)		// store byte 2
#else
	sb	a1,2(a0)		// store byte 1
	srai	a1,a1,8			// A1 <- shift to byte 2
	sb	a1,3(a0)		// store byte 2
#endif

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,12
CmdFor_Ret:
	ret

// ----------------------------------------------------------------------------
// NEXT command
// ----------------------------------------------------------------------------

.global CmdNext
CmdNext:
	// get address of variable name
	lw	a0,RAM_VARDEST_OFF(gp)	// A0 <- pointer to variable name VarDest 'var'

	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)

	// find variable -> A0 pointer to data 'p'
	// returns pointer to data of integer variable, or start of text variable, or NULL if not found
	addi	a1,sp,4			// A1 <- to get variable address
	call	GetVar			// find variable -> A0 pointer to data 'p'
	lw	a1,4(sp)		// A1 <- pointer to variable 'v'

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,8

	// check result
	beqz	a0,1f			// invalid pointer, variable not found
	lbu	a5,RAM_ERRNR_OFF(gp)	// A1 <- get error number ErrNr
	beqz	a5,2f			// skip if no error
1:	j	VarNotFound		// error variable not found

// This is: A0 = pointer to variable data 'p', A1 = pointer to variable 'v'

	// check correct variable type
// 001aaaaa valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR
2:	lbu	a2,0(a1)		// A2 <- variable name and type	
	li	a3,VARTYPE_FOR		// A3 <- required type VARTYPE_FOR
	srli	a2,a2,VAR_SHIFT		// A2 <- get type of the variable
	beq	a2,a3,3f		// skip if variable type is OK
	j	NextError		// error NEXT without FOR

	// increment variable value -> A2 'num'
3:	lw	a2,RAM_LASTRESULT_OFF(gp) // A2 <- last result = variable value
	addi	a2,a2,1			// A2 <- increment variable value 'num'

	// save new variable value
	sb	a2,0(a0)		// save byte 1
	srai	a3,a2,8			// A3 <- byte 2
	sb	a3,1(a0)		// save byte 2
#if VALSIZE == 32
	srai	a3,a2,16		// A3 <- byte 3
	sb	a3,2(a0)		// save byte 3
	srai	a3,a2,24		// A3 <- byte 4
	sb	a3,3(a0)		// save byte 4
#endif	

	// load limit of the loop
#if VALSIZE == 32
	lbu	a3,4(a0)		// A3 <- byte 1

	lbu	a4,5(a0)		// A4 <- byte 2
	slli	a4,a4,8			// A4 <- shift byte 2
	or	a3,a3,a4		// A3 <- byte 1,2

	lbu	a4,6(a0)		// A4 <- byte 3
	slli	a4,a4,16		// A4 <- shift byte 3
	or	a3,a3,a4		// A3 <- byte 1,2,3

	lbu	a4,7(a0)		// A4 <- byte 4
	slli	a4,a4,24		// A4 <- shift byte 4
	or	a3,a3,a4		// A3 <- 'max' value
#else
	lbu	a3,2(a0)		// A3 <- byte 1

	lbu	a4,3(a0)		// A4 <- byte 2
	slli	a4,a4,8			// A4 <- shift byte 2
	or	a3,a3,a4		// A3 <- byte 1,2
#endif

	// check if limit has been reached
	bgt	a2,a3,CmdFor_Ret	// skip if limit reached

	// get destination line 'line'
#if VALSIZE == 32
	lbu	a1,9(a0)		// A1 <- line HIGH
	lbu	a0,8(a0)		// A0 <- line LOW
#else
	lbu	a1,5(a0)		// A1 <- line HIGH
	lbu	a0,4(a0)		// A0 <- line LOW
#endif
	slli	a1,a1,8			// A1 <- shift line HIGH
	or	a0,a0,a1		// A0 <- line
	j	CmdGoto_Go		// Goto

// ----------------------------------------------------------------------------
// Auto list
// ----------------------------------------------------------------------------

.global AutoList
AutoList:
	// push registers
	addi	sp,sp,-32
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	sw	tp,12(sp)

	// save and clear ErrPtr (to not indicate an error in the list)
	lw	a0,RAM_ERRPTR_OFF(gp)	// A0 <- error pointer ErrPtr
	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer
	sw	a0,28(sp)		// save error pointer

// Locals:
//  0(sp): RA
//  4(sp): S0
//  8(sp): S1
// 12(sp): TP
// 16(sp): top line 'top'
// 20(sp): line pointer 'p'
// 24(sp): previous line 'prev'
// 28(sp): old error pointer 'ErrPtr'
//
// S0 = cursor line 'cur'
// S1 = current line 'line'
// TP = flag, cursor found OK 'curok'

AutoListRestart:

	// prepare top screen
	sb	zero,RAM_PRINTX_OFF(gp)	// reset X position
	sb	zero,RAM_PRINTY_OFF(gp)	// reset Y position
	lbu	a0,RAM_LOWSCREENH_OFF(gp) // A0 <- height of lower screen
	li	a1,HEIGHT-1		// A1 <- height of screen - 1
	sub	a0,a1,a0		// A0 <- max. Y row
	sb	a0,RAM_PRINTYMAX_OFF(gp) // set max. Y position

	// check top line underflow
	lhu	a0,RAM_TOPLINE_OFF(gp)	// A0 <- top line 'top'
	bnez	a0,1f			// skip if top line is not 0
	li	a0,1			// A0 <- minimal top line is 1

1:	lhu	s0,RAM_CURLINE_OFF(gp)	// S0 <- cursor line 'cur'
	bnez	s0,1f			// skip if cursor line is not 0
	li	s0,1			// S0 <- minimal cursor line is 1

1:	bge	s0,a0,1f		// skip if cursor is OK 'cur' >= 'top'
	mv	a0,s0			// 'top' <- 'cur'
	sh	a0,RAM_TOPLINE_OFF(gp)	// set new top line
1:	sw	a0,16(sp)		// save local top line 'top'
	li	tp,0			// reset flag cursor OK

	// find first line -> A0
	li	a1,0			// A1 <- NULL, no previous line required
	call	FindLine		// find line 'line'

// Here is A0 = pointer to line 'p', S0 = cursor 'cur', TP = cursor ok 'curok'

AutoListLoop:

	// until end of screen
	lbu	a1,RAM_PRINTY_OFF(gp)	// A1 <- current Y position PrintY
	lbu	a2,RAM_PRINTYMAX_OFF(gp) // A2 <- max. Y position PrintYMax
	blt	a1,a2,1f		// continue to loop if PrintY < PrintYMax

	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- current X position PrintX
	li	a2,WIDTH		// A2 <- max. X position
	beq	a1,a2,AutoListEndLoop	// not end of line, exit loop

	// get line number (A0 = pointer to line) -> S1
1:	lbu	a1,1(a0)		// A1 <- line HIGH
	lbu	a2,0(a0)		// A2 <- line LOW
	slli	a1,a1,8			// A1 <- line HIGH * 256
	or	s1,a1,a2		// S1 <- line
	sw	a0,20(sp)		// save pointer to line 'p'

	// this is cursor
	ble	s1,s0,2f		// skip if line <= cur
	bnez	tp,2f			// skip if cursor is OK

	mv	s0,s1			// cur <- line
	sh	s0,RAM_CURLINE_OFF(gp)	// save new cursor CurLine

	// end of program, print empty row
2:	bnez	s1,3f			// skip if line is not 0
	call	PrintCR			// print new line
	j	6f

	// if this is current line, set 'curok' flag
3:	bne	s0,s1,4f		// skip if 'line' != 'cur'
	li	tp,1			// TP <- flag True

// Here is A0 = pointer to line 'p'

	// print program line
4:	call	PrintBasNum		// print line with number
	bnez	a0,6f			// skip if no error

	// display overflow - last line was not printed whole, increase top line
	lw	a0,20(sp)		// A0 <- pointer to line 'p'
	bne	s0,s1,AutoListEndLoop	// break loop if this is not current line
	j	AutoListNextLine	// shift line

	// find next line -> A0
6:	lw	a0,20(sp)		// A0 <- pointer to line 'p'
	call	NextLine		// shift to next line -> A0
	j	AutoListLoop

// Here is A0 = pointer to line 'p', S0 = cursor 'cur', TP = cursor ok 'curok'

AutoListEndLoop:

	// check if cursor was printed
	bnez	tp,9f			// exit if cursor OK

	// get line number (A0 = pointer to line) -> S1
	lbu	a1,1(a0)		// A1 <- line HIGH
	lbu	a2,0(a0)		// A2 <- line LOW
	slli	a1,a1,8			// A1 <- line HIGH * 256
	or	s1,a1,a2		// S1 <- line

	// check if next line is line with cursor
	blt	s1,s0,7f		// skip if 'line' < 'cur'

AutoListNextLine:

	// shift to next top line -> A0
	lw	a0,16(sp)		// A0 <- top line 'top'
	li	a1,0			// A1 <- NULL, no previous line required
	call	FindLine		// find line 'line' -> A0 pointer
	call	NextLine		// find next line -> A0 pointer

	// get line number -> S1
	lbu	a1,1(a0)		// A1 <- line HIGH
	lbu	a2,0(a0)		// A2 <- line LOW
	slli	a1,a1,8			// A1 <- line HIGH * 256
	or	s1,a1,a2		// S1 <- line

	// if line is valid and line not greater cursor
	beqz	s1,7f			// skip if line is 0 (end of program)
	bgt	s1,s0,7f		// skip if line > cur

	sh	s1,RAM_TOPLINE_OFF(gp)	// set line as new top line
	j	AutoListRestart		// restart

	// move to new top line
7:	mv	a0,s0			// A0 <- cursor
	addi	a1,sp,24		// A1 <- pointer to 'prev'
	call	FindLine		// find line -> A0

	// get cursor line number -> S0
	lbu	a1,1(a0)		// A1 <- line HIGH
	lbu	a2,0(a0)		// A2 <- line LOW
	slli	a1,a1,8			// A1 <- line HIGH * 256
	or	s0,a1,a2		// S0 <- line

	// shift cursor to previous line
	bnez	s0,8f			// skip if cursor is valid
	lw	a0,24(sp)		// A0 <- previous line
	beqz	a0,8f			// skip if previous line is not valid (= NULL)

	lbu	a1,1(a0)		// A1 <- line HIGH
	lbu	a2,0(a0)		// A2 <- line LOW
	slli	a1,a1,8			// A1 <- line HIGH * 256
	or	s0,a1,a2		// S0 <- cursor

	// check if cursor is valid
8:	beqz	s0,9f			// cursor is not valid
	sh	s0,RAM_CURLINE_OFF(gp)	// set current line
	j	AutoListRestart		// restart
	
	// restore error pointer ErrPtr
9:	lw	a0,28(sp)		// A0 <- old error pointer
	sw	a0,RAM_ERRPTR_OFF(gp)	// restore old error pointer

	// pop registers
	lw	tp,12(sp)
	lw	s1,8(sp)
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,32
	ret

// ----------------------------------------------------------------------------
// Process command or check syntax in edit line
// input: A0 ... pointer to program line
// ----------------------------------------------------------------------------

.global MainGo
MainGo:
	// push registers
	addi	sp,sp,-16
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)

// Locals:
//  S0 = command 'cmd' - TOKEN_MIN
//  S1 = pointer to syntax table 's' 

	// set text pointer to start of edit/program line - 1
	addi	a0,a0,-1		// A0 <- pointer - 1
	sw	a0,RAM_CHARPTR_OFF(gp)	// save new pointer to program line

	// clear error pointer
	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer
	sb	zero,RAM_ERRNR_OFF(gp)	// clear error code (set to ERR_OK)

	// pre-set L mode
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	andi	a1,a1,~(FLAG_KMODE|FLAG_KUPDATE) // set L-mode, do not update cursor mode

	// if inputting, only fetch value
	andi	a0,a1,FLAG_INPUT	// check inputting mode
	beqz	a0,1f			// skip if not inputting mode

// --- inputting

	// save flags
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// shift cursor and update cursor mode
	call	CharAdd			// shift cursor

MainGo_FetchVal:

	// fetch value
	call	FetchVal		// fetch value
	j	MainGo9

// --- editing

	// update cursor mode
1:	ori	a1,a1,FLAG_KUPDATE	// flag - update cursor mode
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// skip spaces and update cursor mode
	call	NextChar		// skip spaces

	// parse line number -> A0 = 'line'
	call	ParseUNum		// parse line number
	li	a1,0xffff		// A1 <- max. line number
	bgeu	a0,a1,MainGo_SyntErr 	// 'line' >= 0xffff, error
 
MainGo_LineScan:

	// load next character - command code 'cmd'
	call	GetChar			// get character -> A0 = 'cmd'

	// from now, do not update cursor mode
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	andi	a1,a1,~FLAG_KUPDATE	// flag - do not update cursor mode
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// clear result
	sw	zero,RAM_LASTRESULT_OFF(gp) // clear last result

	// quit if row is empty
	li	a1,NEWLINE		// A1 <- new line character
	beq	a0,a1,MainGo9		// quit if cmd == NEWLINE

	// only tokens are allowed at this place, cmd-TOKEN_MIN -> S0
	addi	s0,a0,-TOKEN_MIN	// check minimal token required
	bltz	s0,MainGo_SyntErr	// invalid token on this place

	// skip token character
	call	NextChar		// skip this character

	// prepare pointer to syntax descriptor -> S1
1:	auipc	a1,%pcrel_hi(SyntTab)	// A1 <- PC + hi(SyntTab) pointer to syntax table
	slli	a0,s0,2			// A0 <- command * 4 (= offset in syntax table)
	add	a1,a1,a0		// A1 <- pointer to syntax table
	lw	s1,%pcrel_lo(1b)(a1)	// S1 <- load address of syntax descriptor

	// parse syntax
MainGo_Loop:

	// break on error
	lbu	a0,RAM_ERRNR_OFF(gp)	// A0 <- get error number
	bnez	a0,MainGo9		// exit on error

	// get next character (skip spaces) -> A0 = 'ch2'
	call	GetChar			// get next character

	// get next syntax descriptor -> A1 = 'ch'
	lbu	a1,0(s1)		// A1 <- next syntax descriptor
	addi	s1,s1,1			// shift pointer to syntax table 's'

// Here is: A0 = next character 'ch2', A1 = syntax descriptor 'ch', S0 = command 'cmd' - TOKEN_MIN, S1 = pointer to syntax table 's', 

// --- parse token separator

	// check token separator
	li	a2,CLASS_MAX		// A2 <- max. class code
	ble	a1,a2,4f		// skip if class code is OK

	// if token 'THEN', set update cursor mode
	li	a2,TOKEN_THEN		// token 'THEN'
	bne	a0,a2,3f		// skip if not token 'THEN'
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- load flags
	ori	a2,a2,FLAG_KUPDATE	// flag - update cursor mode
	sb	a2,RAM_FLAGS_OFF(gp)	// save new flags

	// check token separator
3:	bne	a1,a0,MainGo_SyntErr	// exit with error on incorrect separator

	// skip this separator
	call	NextChar
	j	MainGo_Loop		// continue loop

// Here is: A0 = next character 'ch2', A1 = syntax descriptor 'ch', S0 = command 'cmd' - TOKEN_MIN, S1 = pointer to syntax table 's', 

// --- CLASS2
// an expression (of type integer or string) must follow ... CLASS2 terminates sequence
// Used in: CmdLet

	// check CLASS2
4:	li	a2,CLASS2		// A2 <- CLASS2 code
	beq	a1,a2,MainGo_FetchVal	// only fetch value

// --- CLASS3
// a numeric expression may follow, otherwise zero will be used ... CLASS3 terminates sequence
// Used in: CmdList, CmdSave, CmdRandomise, CmdRun, CmdLoad

	// check CLASS3
	li	a2,CLASS3		// A2 <- CLASS3 code
	bne	a1,a2,5f		// skip if not CLASS3

	// if not end of line, load value
	li	a2,NEWLINE		// A2 <- new line character
	beq	a0,a2,6f		// skip if end of row

	// evaluate expression
	call	ScanCalc		// evaluate expression

	// check numeric expression
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	andi	a1,a1,FLAG_NUMRES	// check numeric result
	beqz	a1,MainGo_SyntErr	// error exit if not numeric expression
	j	6f			// continue processing CLASS0

// --- CLASS0
// no further operands ... CLASS0 terminates sequence
// Used in: CmdReturn, CmdCls, CmdDim, CmdGoto, CmdInput, CmdNext, CmdMemory,
//          CmdNew, CmdStop, CmdContinue, CmdGosub, CmdClear, CmdBeep

	// check CLASS0
5:	li	a2,CLASS0		// A2 <- CLASS0 code
	bne	a1,a2,7f		// skip if not CLASS0

	// check end of command - quit if only checking syntax
6:	call	CheckEnd		// check end
	beqz	a0,MainGo9		// exit if only checking syntax

	// run command S0
1:	auipc	a1,%pcrel_hi(TokenFncList) // A1 <- PC + hi(TokenFncList)
	slli	a0,s0,2			// A0 <- command * 4 (= offset in jump table)
	add	a1,a1,a0		// A1 <- addres in jump table
	lw	a1,%pcrel_lo(1b)(a1)	// A1 <- load jump address
	jalr	a1			// call function (and return)
	j	MainGo9			// exit

// --- CLASS1
// destination variable name is required
// Used in: CmdInput, CmdLet

	// check CLASS1
7:	li	a2,CLASS1		// A2 <- CLASS1 code
	bne	a1,a2,9f		// skip if not CLASS1

	// check alpha character A0
	call	IsAlpha			// check if next character is alphabetic
	beqz	a0,MainGo_SyntErr	// error exit if not alphabetic
	
	// if checking syntax, go to look vars
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- load flags
	andi	a1,a2,FLAG_SYNTOFF	// check syntax
	bnez	a1,8f			// skip if not checking syntax
	li	a0,0			// A0 <- False, functions are not allowed
	call	LookVars		// if checking syntax, scan variable (not functions)
	j	MainGo_Loop		// continue loop

	// if runtime - temporary switch ON syntax checking, to only get variable name
8:	lw	a3,RAM_CHARPTR_OFF(gp)	// A3 <- text pointer
	andi	a1,a2,~FLAG_SYNTOFF	// set syntax mode
	sw	a3,RAM_VARDEST_OFF(gp)	// set pointer to variable name
	sb	a1,RAM_FLAGS_OFF(gp)	// store flags

	li	a0,0			// A0 <- False, functions are not allowed
	call	LookVars		// scan variable (not functions)

	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	ori	a1,a1,FLAG_SYNTOFF	// set syntax off mode
	sb	a1,RAM_FLAGS_OFF(gp)	// store flags
	j	MainGo_Loop		// continue loop

// --- CLASS4
// integer variable must follow
// Used in: CmdDim, CmdFor, CmdNext

	// check CLASS4
9:	li	a2,CLASS4		// A2 <- CLASS4 code
	bne	a1,a2,10f		// skip if not CLASS4

	// store pointer to variable name
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- text pointer
	sw	a0,RAM_VARDEST_OFF(gp)	// set pointer to variable name

	// parse variable name
	call	ParseVarName		// parse variable name

	// check variable type 'name' or 'name()' (error includes invalid name VAR_END)
	srli	a1,a0,VAR_SHIFT		// A1 <- type of name
#if VARTYPE_NUM == 0
	beqz	a1,1f			// 'name' OK
#else
	li	a5,VARTYPE_NUM		// number 'name'
	beq	a1,a5,1f
#endif
	li	a5,VARTYPE_ARRAY	// array 'name()'
	bne	a1,a5,MainGo_SyntErr	// syntax error

	// skip name of the variable
1:	andi	a0,a0,VAR_MASK		// A0 <- name length
1:	sb	a0,12(sp)		// save length counter
	call	CharAdd			// shift pointer
	lbu	a0,12(sp)		// A0 <- length counter
	addi	a0,a0,-1		// decrease length counter
	bnez	a0,1b			// loop
	j	MainGo_Loop		// continue loop

// --- CLASS5
// variable syntax checked entirely by routine ... CLASS5 terminates sequence
// Used in: CmdFor, CmdPoke, CmdPrint, CmdIf, CmdRem, CmdBeep, CmdData

	// check CLASS5
10:	li	a2,CLASS5		// A2 <- CLASS5 code
	bne	a1,a2,11f		// skip if not CLASS5

	// jump to the command S0
1:	auipc	a1,%pcrel_hi(TokenFncList) // A1 <- PC + hi(TokenFncList)
	slli	a0,s0,2			// A0 <- command * 4 (= offset in jump table)
	add	a1,a1,a0		// A1 <- addres in jump table
	lw	a1,%pcrel_lo(1b)(a1)	// A1 <- load jump address
	jalr	a1			// call function (and return)

	// break on error
	lbu	a0,RAM_ERRNR_OFF(gp)	// A0 <- get error number
	bnez	a0,MainGo9		// exit on error

	// on IF command - continue scanning (on FALSE, rest of line is already skipped)
	li	a2,TOKEN_IF - TOKEN_MIN	// A2 <- token IF
	beq	s0,a2,MainGo_LineScan	// on IF continue scanning
	j	MainGo9			// exit

// --- CLASS6
// a numeric expression must follow
// Used in: CmdDim, CmdFor, CmdGoto, CmdPoke, CmdIf, CmdGosub, CmdBeep, CmdWait

// - At this place we do not need check command code, this is CLASS6

	// evaluate expression
11:	call	ScanCalc

	// check numeric expression
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	andi	a1,a1,FLAG_NUMRES	// check numeric result
	bnez	a1,MainGo_Loop		// continue loop

	// syntax error
MainGo_SyntErr:
	call	SyntaxError		// syntax error

MainGo9:
	// pop registers
	lw	s1,8(sp)
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,16
	ret

#endif // USE_PROG_ASM
