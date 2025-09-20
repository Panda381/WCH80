
// ****************************************************************************
//
//                                  Edit
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_EDIT_ASM		// 1=use ASM in Edit module

// ----------------------------------------------------------------------------
// Delete whole edit line
// ----------------------------------------------------------------------------

.global ClrEdit
ClrEdit:
	li	a0,1			// A0 <- 1 byte in edit line
	sh	a0,RAM_EDITNUM_OFF(gp)	// set length of edit line EditNum to 1 byte
	la	a0,Edit			// A0 <- edit buffer
	li	a1,NEWLINE		// A1 <- new line character
	sb	a1,0(a0)		// mark end of line
	sw	a0,RAM_CURPTR_OFF(gp)	// set current pointer of cursor CurPtr to start of edit line
	ret

// ----------------------------------------------------------------------------
// INPUT command
// ----------------------------------------------------------------------------

.global CmdInput
CmdInput:
	// prepare stack
	add	sp,sp,-12
	sw	ra,0(sp)	// save return address

	// check program line - error if program is not running
	lh	a0,RAM_PROGLINE_OFF(gp)	// A0 <- current executed program line ... load signed(!) halfword
	beqz	a0,8f			// program line is 0 (= checking syntax), error
	addi	a0,a0,1			// program line + 1
	beqz	a0,8f			// program line is -1 (= direct execute), error

	// set input mode
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	ori	a0,a0,FLAG_INPUT	// set input mode
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags

	// clear edit line
	call	ClrEdit		// clear edit line

	// check if string mode (string result)
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,FLAG_NUMRES // check numeric result
	bnez	a0,4f		// skip if numeric result

	// setup string mode (prepare two quote characters)
	li	a0,3		// A0 <- new length of edit line is 3 bytes ("" NEWLINE)
	sh	a0,RAM_EDITNUM_OFF(gp) // set new edit length EditNum
	la	a0,Edit+1		// A0 <- edit buffer + 1
	li	a1,CH_QUOT		// A1 <- quote character
	sb	a1,-1(a0)		// mark start of text
	sb	a1,0(a0)		// mark end of text
	li	a1,NEWLINE		// A1 <- new line character
	sb	a1,1(a0)		// mark end of line
	sw	a0,RAM_CURPTR_OFF(gp)	// set current pointer of cursor CurPtr to start of edit line + 1

	// set lower screen to remaining rows
4:	lbu	a0,RAM_PRINTY_OFF(gp)	// A0 <- current print row PrintY 'y'
	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- current print position PrintX 'x'
	li	a2,HEIGHT-1		// A1 <- max. row y
	sub	a2,a2,a0		// A2 <- height of lower screen
	bgtz	a2,5f			// skip if height is big enough
	li	a2,1			// minimal height of low screen
5:	sb	a2,RAM_LOWSCREENH_OFF(gp) // save new height of low screen LowScreenH

	// save program pointer and screen
	sb	a0,8(sp)		// save PrintY
	sb	a1,9(sp)		// save PrintX
	sb	a2,10(sp)		// save LowScreenH
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- character pointer
	sw	a0,4(sp)		// save character pointer

	// edit
	call	DoEdit

	// clear edit
	li	a1,HEIGHT
	lbu	a0,10(sp)		// A0 <- restore LowScreenH
	sub	a1,a1,a0		// A1 <- old y
	sb	a1,RAM_PRINTY_OFF(gp)	// set PrintY
	sb	zero,RAM_PRINTX_OFF(gp)	// clear PrintX
	call	ClrEod			// clear to end of screen

	// restore program pointer and screen
	lbu	a0,8(sp)		// A0 <- old y
	lbu	a1,9(sp)		// A1 <- old x
	sb	a0,RAM_PRINTY_OFF(gp)	// restore PrintY
	sb	a1,RAM_PRINTX_OFF(gp)	// restore PrintX
	li	a0,HEIGHT-1
	sb	a0,RAM_PRINTYMAX_OFF(gp) // reset PrintYMax
	lw	a0,4(sp)
	sw	a0,RAM_CHARPTR_OFF(gp)	// restore CharPtr

	// set NEXT and EDIT mode
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,~(FLAG_GOTO|FLAG_INPUT) // set NEXT and EDIT mode
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags
	j	9f

	// error if program is not running
8:	call	InputError

	// restore stack
9:	lw	ra,0(sp)	// restore return address
	add	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// add BASIC line to the program (CharPtr = start of program line
// - does not raise error - ignores command on memory overflow)
// input: A0 ... line number 'line' (line must not be 0)
// ----------------------------------------------------------------------------

.global MainAdd
MainAdd:
	// prepare stack
	add	sp,sp,-12
	sw	ra,0(sp)	// save return address
	sw	s0,4(sp)	// save S0

	// set new edit line
	sh	a0,RAM_CURLINE_OFF(gp) // set new edit line CurLine

	// find program line (exact, or higher, or end of program)
	addi	a1,sp,8		// A1 <- pointer to get previous line
	call	FindLine	// find program line -> A0
	mv	s0,a0		// S0 <- save pointer to program line 'p'

	// get found line (= 0 on end of program) 'line2'
	lbu	a2,1(a0)	// A2 <- line2 HIGH
	lbu	a1,0(a0)	// A1 <- line2 LOW
	slli	a2,a2,8		// A2 <- line2 HIGH << 8
	or	a1,a1,a2	// A1 <- found line line2

	// check if old row need to be delete
	lhu	a2,RAM_CURLINE_OFF(gp) // A2 <- get current line
	bne	a2,a1,2f	// skip if not current row

// lines are equal - delete old line

	// shift to next program line, to get length of old program line
	// here is S0,A0 = pointer to program line 'p'
	call	NextLine	// find next program line -> A0 'p2'

// here is:
//  A0 = next program line 'p2'
//  S0 = current program line 'p'
//  8(sp) = previous program line 'prev'

	// delete old line
	lhu	a1,RAM_PROGNUM_OFF(gp) // A1 <- size of data in program buffer ProgNum
	sub	a3,a0,s0	// A3 <- size of deleted program line 'p2' - 'p'
	sub	a3,a1,a3	// A3 <- new size of the program ProgNum - (p2 - p)
	sh	a3,RAM_PROGNUM_OFF(gp) // set new size of the program ProgNum

	addi	a2,gp,RAM_PROG_OFF // A2 <- start of program buffer Prog
	add	a2,a2,a1	// A2 <- pointer to old end of program Prog + ProgNum
	sub	a2,a2,a0	// A2 <- length of rest of the program to be shifted Prog + ProgNum - p2 = i

	mv	a1,s0		// A1 <- pointer to current program line 'd'
1:	lbu	a4,0(a0)	// A4 <- load byte from 'p2'
	addi	a0,a0,1		// shift source pointer 'p2'
	sb	a4,0(a1)	// write byte to 'd'
	addi	a1,a1,1		// shift destination pointer 'd'
	addi	a2,a2,-1	// shift loop counter
	bnez	a2,1b		// loop

// edit on empty line
// here is S0 = pointer to program line 'p', 8(sp) = previous program line 'prev'

	// skip spaces
2:	call	GetChar		// skip spaces

	// if line is empty, only deleting old program line
	li	a3,NEWLINE	// A3 <- new line character
	bne	a0,a3,4f	// skip if not empty row

	// get new line
	lbu	a2,1(s0)	// A2 <- line HIGH
	lbu	a1,0(s0)	// A1 <- line LOW
	slli	a2,a2,8		// A2 <- line HIGH << 8
	or	a1,a1,a2	// A1 <- line
	bnez	a1,3f		// skip if line is valid

	// we are at end of program, use previous program line
	li	a1,1		// A1 <- use first program line, if program is empty
	lw	a3,8(sp)	// A3 <- previous program line
	beqz	a3,3f		// previous program line is not valid

	lbu	a2,1(a3)	// A2 <- line HIGH
	lbu	a1,0(a3)	// A1 <- line LOW
	slli	a2,a2,8		// A2 <- line HIGH << 8
	or	a1,a1,a2	// A1 <- line

	// store new line as current edited line
3:	sh	a1,RAM_CURLINE_OFF(gp) // new current edited line
	j	9f

// store new line
// here is S0 = pointer to program line 'p'

	// get size of new program line
4:	lw	a0,RAM_CHARPTR_OFF(gp) // A0 <- get current pointer to edit line CharPtr = 's'
	lhu	a3,RAM_EDITNUM_OFF(gp) // A3 <- size of data in edit buffer EditNum
	la	a2,Edit		// A2 <- pointer to edit buffer Edit
	add	a2,a2,a3	// A2 <- end of data in edit buffer Edit + EditNum
	sub	a2,a2,a0	// A2 <- size of program line 'len' = Edit + EditNum - CharPtr

	// check memory (does not rise error, only ignore operation)
	lhu	a1,RAM_PROGNUM_OFF(gp) // A1 <- current length of program ProgNum
	li	a4,PROGNUM	// A4 <- size of program buffer
	sub	a4,a4,a1	// A4 <- remaining space PROGNUM - ProgNum
	addi	a3,a2,2		// A3 <- length of program line + 2 (= size of line number)
	bgt	a3,a4,9f	// overflow of program memory

// here is:
//  S0 = pointer to program line 'p'
//  A0 = start of program line 's' = 'CharPtr'
//  A1 = length of program 'ProgNum'
//  A2 = size of program line 'len'
//  A3 = length of program line 'len' + 2

	// prepare free space
	addi	a4,gp,RAM_PROG_OFF-1 // A4 <- start of program buffer Prog - 1
	add	a4,a4,a1	// A4 <- pointer to old last byte &Prog[ProgNum-1] = 'p2'

	add	a1,a1,a3	// A1 <- ProgNum + len + 2, new size of the program
	sh	a1,RAM_PROGNUM_OFF(gp) // set new size of the program ProgNum

	sub	a5,a4,s0	// A5 <- length of data to move - 1 = 'p2' - 'p'
	add	a3,a3,a4	// A3 <- len + 2 + p2, pointer to new last byte 'd'
	addi	a5,a5,1		// A5 <- +1, length of data to move 'i'

// here is:
//  S0 = pointer to program line 'p'
//  A0 = start of program line 's' = 'CharPtr'
//  A2 = size of program line 'len'
//  A3 = pointer to new last byte 'd'
//  A4 = pointer to old last byte 'p2'
//  A5 = length of data to move 'i'

	// move data
5:	lbu	a1,0(a4)	// A1 <- load byte from 'p2'
	addi	a4,a4,-1	// decrease source pointer
	sb	a1,0(a3)	// store byte to 'd'
	addi	a3,a3,-1	// decrease destination pointer
	addi	a5,a5,-1	// decrease loop counter
	bnez	a5,5b		// next byte

	// store line number
	lhu	a3,RAM_CURLINE_OFF(gp) // A3 <- new line
	sb	a3,0(s0)	// set line number LOW
	srli	a3,a3,8		// line number >> 8
	sb	a3,1(s0)	// set line number HIGH

	// move program code
6:	lbu	a5,0(a0)	// A5 <- load byte
	addi	a0,a0,1		// increase source pointer
	sb	a5,2(s0)	// store byte
	addi	s0,s0,1		// increase destination pointer
	addi	a2,a2,-1	// decrease loop counter
	bnez	a2,6b		// next byte

	// restore stack
9:	lw	s0,4(sp)	// restore S0
	lw	ra,0(sp)	// restore return address
	add	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// edit (flag FLAG_INPUT determines if inputting data or editing code) (can raise errors)
// ----------------------------------------------------------------------------

.global DoEdit
DoEdit:
	// prepare stack (0(sp) used as temporary buffer)
	add	sp,sp,-12
	sw	ra,4(sp)		// save return address
	sw	s0,8(sp)

DoEdit_Loop:

	// start of edit buffer -> S0
	la	s0,Edit			// S0 <- address of edit buffer

	// clear error pointer
	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer
	sb	zero,RAM_ERRNR_OFF(gp)	// clear error number

	// set flags NOLEAD, SYNTON, NEXT
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,~(FLAG_SYNTOFF|FLAG_GOTO) // set syntax on, continue on next program line
	ori	a5,a5,FLAG_NOLEAD	// set no leading space flag
	sb	a5,RAM_FLAGS_OFF(gp)	// save flags

	// check syntax of current line
	sb	zero,RAM_CALCNUM_OFF(gp) // clear calculator stack
	sb	zero,RAM_STCKNUM_OFF(gp) // clear program stack
	mv	a0,s0			// A0 <- start of edit buffer
	call	MainGo			// check syntax

	// prepare printing to lower screen
	sb	zero,RAM_PRINTX_OFF(gp)	// reset PrintX
	lbu	a4,RAM_LOWSCREENH_OFF(gp) // A4 <- heigh of low screen
	li	a5,HEIGHT		// A5 <- screen height
	sub	a5,a5,a4		// A5 <- first row of lower screen
	sb	a5,RAM_PRINTY_OFF(gp)	// set PrintY
	li	a5,HEIGHT - 1		// A5 <- max Y
	sb	a5,RAM_PRINTYMAX_OFF(gp) // set max Y PrintYMax
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// skip CR if inputting
	bnez	a5,1f			// skip CR
	call	PrintCR			// clear first empty line

	// print edit line (returns False on screen overflow)
	//  printed without line number - line number is now entered as text digits, not binary number
1:	mv	a0,s0			// A0 <- start of edit buffer
	call	PrintBas		// print program line
	bnez	a0,2f			// skip if program line was printes whole

	// overflow - increase height of lower screen
	lbu	a5,RAM_LOWSCREENH_OFF(gp) // A5 <- height of low screen
	li	a4,HEIGHT-2		// A4 <- max. height of low screen
	bgeu	a5,a4,2f		// low screen is too big
	addi	a5,a5,1			// increase height
	sb	a5,RAM_LOWSCREENH_OFF(gp) // set new height of low screen
DoEdit_AutoList:
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if inputting
	bnez	a5,DoEdit_Loop		// skip if inputting
	call	AutoList		// restart auto-list
	j	DoEdit_Loop		// restart edit

	// clear to end of display
2:	call	ClrEod			// clear to end of display

	// get key from keyboard -> A0
3:	call	CharGet			// get key
	li	a4,CH_NONE		// A4 <- invalid character
	beq	a0,a4,3b		// wait for a key

	// get current pointer to cursor -> A1
	lw	a1,RAM_CURPTR_OFF(gp)	// A1 <- current cursor

// Here is: S0 = Edit, A0 = key from keyboard 'ch', A1 = cursor 'cur'

	// check check control keys (arrows,...) 0x70..0x77
	addi	a3,a0,-0x70		// minimal code is 0x70
	li	a2,7			// A2 <- max. key
	bleu	a3,a2,5f		// skip if control key OK

// --- insert character at cursor position

	// check number of characters
	lhu	a3,RAM_EDITNUM_OFF(gp)	// A3 <- size of data in edit buffer
	li	a2,EDITNUM		// A2 <- max. size of edit buffer
	bgeu	a3,a2,DoEdit_Loop	// buffer is full

	// prepare pointer to new end of edit line (= last character + 1)
	add	a2,s0,a3		// A2 <- end of edit buffer 'p'
	addi	a3,a3,1			// A3 <- new number of characters
	sh	a3,RAM_EDITNUM_OFF(gp)	// set new number of characters

// Here is: S0 = Edit, A0 = key from keyboard 'ch', A1 = cursor 'cur', A2 = end of buffer 'p'

	// move rest of text
4:	lbu	a4,-1(a2)		// A4 <- load character
	addi	a2,a2,-1		// A2 <- shift pointer 'p'
	sb	a4,1(a2)		// save character
	bne	a1,a2,4b		// loop next character until current position reached

	// store new character
	sb	a0,0(a2)		// store new character
	addi	a1,a1,1			// A1 <- cursor + 1
	sw	a1,RAM_CURPTR_OFF(gp)	// set new cursor pointer
	j	DoEdit_Loop

// --- control keys
// Here is: S0 = Edit, A0 = key from keyboard 'ch', A1 = cursor 'cur'

// --- 0x70 UP

	// check UP key
5:	li	a2,CH_UP
	bne	a0,a2,6f		// not UP key

	// only if editing
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if editing
	bnez	a5,DoEdit_Loop		// break if not editing

	// get current line	
	lhu	a0,RAM_CURLINE_OFF(gp)	// A0 <- current line
	mv	a1,sp			// A1 <- temporary variable in stack
	call	FindLine		// find line with cursor and previous line
	lw	a5,0(sp)		// A5 <- previous line
	beqz	a5,DoEdit_Loop		// break if previous line is not valid

	// get number of previous line
	lbu	a3,1(a5)		// A3 <- line HIGH
	lbu	a4,0(a5)		// A4 <- line LOW
	slli	a3,a3,8			// A3 <- line HIGH << 8
	or	a4,a4,a3		// A4 <- line number

52:	sh	a4,RAM_CURLINE_OFF(gp)	// set new current line
	j	DoEdit_AutoList

// --- 0x71 DOWN

	// check DOWN key
6:	li	a2,CH_DOWN
	bne	a0,a2,7f		// not DOWN key

	// only if editing
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if editing
	bnez	a5,DoEdit_Loop		// break if not editing

	// get next line	
	lhu	a0,RAM_CURLINE_OFF(gp)	// A0 <- current line
	addi	a0,a0,1			// A0 <- next line
	li	a1,0			// no previous line required
	call	FindLine		// find line -> A0

	// get number of next line
	lbu	a3,1(a0)		// A3 <- line HIGH
	lbu	a4,0(a0)		// A4 <- line LOW
	slli	a3,a3,8			// A3 <- line HIGH << 8
	or	a4,a4,a3		// A4 <- line number
	bnez	a4,52b			// line is valid (not 0)
	j	DoEdit_Loop

// --- 0x72 LEFT

	// check LEFT key
7:	li	a2,CH_LEFT
	bne	a0,a2,8f		// not LEFT key

	// check if cursor is on the first position
	bleu	a1,s0,DoEdit_Loop	// break if cursor is already on start of buffer
	addi	a1,a1,-1		// A1 <- decrease cursor position
72:	sw	a1,RAM_CURPTR_OFF(gp)	// save new cursor pointer
	j	DoEdit_Loop

// --- 0x73 RIGHT

	// check RIGHT key
8:	li	a2,CH_RIGHT
	bne	a0,a2,9f		// not RIGHT key

	// check if cursor is on the last position
	lhu	a4,RAM_EDITNUM_OFF(gp)	// A4 <- number of bytes in edit buffer
	add	a4,a4,s0		// A4 <- end of edit buffer
	addi	a1,a1,1			// A1 <- increase cursor position
	bltu	a1,a4,72b		// cursor is OK
	j	DoEdit_Loop

// --- 0x74 HOME

	// check HOME key
9:	li	a2,CH_HOME
	bne	a0,a2,10f		// not HOME key

	// only if editing
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if editing
	bnez	a5,DoEdit_Loop		// break if not editing

	// go to lowest line
	li	a5,1
	sh	a5,RAM_CURLINE_OFF(gp)	// set cursor line
	sh	a5,RAM_TOPLINE_OFF(gp)	// set top line
	j	DoEdit_AutoList

// --- 0x75 EDIT

	// check EDIT key
10:	li	a2,CH_EDIT
	bne	a0,a2,11f		// not EDIT key

	// only if editing
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if editing
	bnez	a5,DoEdit_Loop		// break if not editing

	// find current edited program line -> A0
	call	FindCurLine
	sw	a0,0(sp)		// save pointer to program line

	// get line number (0 = end of program) -> A0
	lbu	a3,1(a0)		// A3 <- line HIGH
	lbu	a4,0(a0)		// A4 <- line LOW
	slli	a3,a3,8			// A3 <- line HIGH << 8
	or	a0,a4,a3		// A0 <- line number
	beqz	a0,DoEdit_Loop		// line is not valid (= 0)

	// set as new current line
	sh	a0,RAM_CURLINE_OFF(gp)	// set as new current line

	// decode line number to temporary buffer -> A0 'p2' pointer to text
	call	DecTempUNum		// decode number to temporary buffer

	// transfer line number to edit buffer (without QUOT character)
	mv	a4,s0			// A4 <- start of edit buffer 'd'
	li	a1,CH_QUOT		// A1 <- terminator
1:	lbu	a2,0(a0)		// A2 <- load character from source buffer 'p2'
	addi	a0,a0,1			// shift source pointer 'p2'
	beq	a2,a1,2f		// break on QUOT character
	sb	a2,0(a4)		// store character to 'd'
	addi	a4,a4,1			// increment destination pointer 'd'
	j	1b			// continue loop

	// set new cursor pointer to 'd'
2:	sw	a4,RAM_CURPTR_OFF(gp)	// set new cursor pointer CurPtr

	// transfer program line
	lw	a0,0(sp)		// A0 <- pointer to program line
	addi	a0,a0,2			// skip line number 'p'
	addi	a1,s0,EDITNUM-1		// A1 <- max. destination address 'p2'

// Here is: S0 = Edit, A0 = source program 'p', A1 = max. destination address 'p2',
//          A3 = NEWLINE, A4 = destination buffer 'd'

	li	a3,NEWLINE		// A3 <- terminator
3:	lbu	a2,0(a0)		// A2 <- load character from 'p'
	addi	a0,a0,1			// shift source pointer 'p'
	sb	a2,0(a4)		// write character to destination 'd'
	addi	a4,a4,1			// shift destination pointer 'd'
	beq	a2,a3,4f		// break on NEWLINE
	bne	a4,a1,3b		// loop if not end of buffer, while d != p2

	// mark end of line NEWLINE
	sb	a3,0(a4)		// write NEWLINE
	addi	a4,a4,1			// shift destination pointer 'd'

	// set length of edit line
4:	sub	a4,a4,s0		// A4 <- length of edit line
	sh	a4,RAM_EDITNUM_OFF(gp)	// set number of bytes
	j	DoEdit_AutoList

// --- 0x77 DELETE
// Here is: S0 = Edit, A0 = key from keyboard 'ch', A1 = cursor 'cur'

	// check DELETE key
11:	li	a2,CH_DELETE
	bne	a0,a2,12f		// not DELETE key

	// check if cursor is on the first position
	bleu	a1,s0,DoEdit_Loop	// break if cursor is already on start of buffer

	// decrease number of characters
	lhu	a3,RAM_EDITNUM_OFF(gp)	// A3 <- number of characters
	addi	a3,a3,-1		// A3 <- decrease number of bytes
	sh	a3,RAM_EDITNUM_OFF(gp)	// set new number of characters

	// pointer to last old character
	add	a3,a3,s0		// A3 <- pointer to last old character 'p'

	// shuft cursor 1 position left
	addi	a1,a1,-1		// A1 <- decrease cursor
	sw	a1,RAM_CURPTR_OFF(gp)	// set new cursor pointer

	// move text
1:	lbu	a4,1(a1)		// A4 <- load next character
	sb	a4,0(a1)		// store character
	addi	a1,a1,1			// shift cursor
	bne	a1,a3,1b		// loop
	j	DoEdit_Loop

// --- 0x76 NEWLINE
// Here is: S0 = Edit, A0 = key from keyboard 'ch', A1 = cursor 'cur'

	// check syntax error (cannot continue on syntax error)
12:	lw	a5,RAM_ERRPTR_OFF(gp)	// A5 <- get error pointer
	bnez	a5,DoEdit_Loop		// break on error

	// prepare to parse
	sw	s0,RAM_CHARPTR_OFF(gp)	// set character pointer to Edit

	// skip spaces
	call	GetChar

	// check if editing
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	andi	a5,a5,FLAG_INPUT	// check if inputting
	bnez	a5,20f			// skip if not editing

	// parse line number (no error checking) -> A0
	call	ParseUNum
	blez	a0,14f			// skip if line number is 0

	// check line number and add code to the program
	li	a5,0xfffe		// max. line number
	bgt	a0,a5,13f		// skip if line number is not valid
	call	MainAdd			// add line to program

DoEdit_ClrList:

	// clear edit line
13:	call	ClrEdit			// clear edit buffer
	li	a1,2			// A1 <- height of lower screen (number of rows)
	sb	a1,RAM_LOWSCREENH_OFF(gp) // set new height of lower screen
	call	AutoList		// repaint autolist
	j	DoEdit_Exit

	// line is 0, direct execution of the program line
14:	li	a4,-1
	sh	a4,RAM_PROGLINE_OFF(gp)	// set ProgLine to 0xffff, flag of direct execution of the program

	// clear screen
	call	CmdCls			// clear screen

	// empty command - do listing
	call	GetChar			// get next character
	li	a4,NEWLINE		// empty row?
	bne	a0,a4,20f		// skip if not empty row
	sh	zero,RAM_PROGLINE_OFF(gp) // clear current program line
	j	DoEdit_ClrList

	// clear error flag
20:	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer
	sb	zero,RAM_ERRNR_OFF(gp)	// clear error number

	// set flags LEAD, SYNTOFF, GOTO
	lbu	a5,RAM_FLAGS_OFF(gp)	// A5 <- flags
	ori	a5,a5,FLAG_SYNTOFF|FLAG_GOTO // set syntax off, goto program line
	andi	a5,a5,~FLAG_NOLEAD	// set leading space flag
	sb	a5,RAM_FLAGS_OFF(gp)	// save flags

	// execute command
	mv	a0,s0			// A0 <- start of edit buffer
	call	MainGo			// execute command

	// clear edit line (edit line can contain temporary strings)
	call	ClrEdit			// clear edit buffer
	li	a1,2			// A1 <- height of lower screen (number of rows)
	sb	a1,RAM_LOWSCREENH_OFF(gp) // set new height of lower screen

DoEdit_Exit:

	// restore stack
	lw	s0,8(sp)
	lw	ra,4(sp)		// restore return address
	add	sp,sp,12
	ret

#endif // USE_EDIT_ASM
