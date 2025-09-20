
// ****************************************************************************
//
//                             Parse
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_PARSE_ASM		// 1=use ASM in Parse module

// ----------------------------------------------------------------------------
// check alphabetic character 'A'..'Z'
// input: A0 ... input character
// output: A0 ... 1=True, 0=False
// ----------------------------------------------------------------------------

.global IsAlpha
IsAlpha:
	addi	a0,a0,-CH_A		// A0 <- character - 38
	sltiu	a0,a0,CH_Z-CH_A+1	// A0 <- set 1 if < 26
	ret

// ----------------------------------------------------------------------------
// check alphanumeric character '0'..'9' or 'A'..'Z'
// input: A0 ... input character
// output: A0 ... 1=True, 0=False
// ----------------------------------------------------------------------------

.global IsAlphaNum
IsAlphaNum:
	addi	a0,a0,-CH_0		// A0 <- character - 28
	sltiu	a0,a0,CH_Z-CH_0+1	// A0 <- set 1 if < 36
	ret

// ----------------------------------------------------------------------------
// check alphanumeric character '.' or '0'..'9' or 'A'..'Z'
// input: A0 ... input character
// output: A0 ... 1=True, 0=False
// ----------------------------------------------------------------------------

.global IsAlphaNumDot
IsAlphaNumDot:
	addi	a0,a0,-CH_DOT		// A0 <- character - 27
	sltiu	a0,a0,CH_Z-CH_DOT+1	// A0 <- set 1 if < 37
	ret

// ----------------------------------------------------------------------------
// parse variable name (skip spaces and rewinds CharPtr to start of variable name)
// input: A0 ... pointer to text
// output: A0 ... variable type and length
// ----------------------------------------------------------------------------
//  Returns:
//    bit 0..5: length of the name (1 to 63)
//    bit 6..7: type of variable VARTYPE_*
// VARTYPE_NUM (=0) ... integer number 'name'
// VARTYPE_FOR (=1) ... function 'name$()'
// VARTYPE_ARRAY (=2) ... array or function 'name()'
// VARTYPE_STR (=3) ... string 'name$'
// VAR_END (0xc0) ... invalid variable

.global ParseVarName
ParseVarName:
	// push registers
	addi	sp,sp,-12
	sw	ra,0(sp)
	sw	s0,4(sp)

	// pointer to text -> S0, save start of the name
	mv	s0,a0			// S0 <- pointer to name
	sw	a0,8(sp)		// save start of the name

	// get first character -> A0
	lbu	a0,0(s0)

	// first character must be letter
	call	IsAlpha			// check alphabetic
	mv	a5,a0
	li	a0,VAR_END		// A0 <- invalid variable name
	beqz	a5,9f			// exit if variable name is not valid

	// parse name
2:	lbu	a0,1(s0)		// A0 <- get character
	addi	s0,s0,1			// shift poniter
	call	IsAlphaNumDot		// check if this character is alphanumeric or dot
	bnez	a0,2b			// loop if alphanumeric or dot

	// length of the name -> A1
	lw	a1,8(sp)		// A1 <- start of name
	sub	a1,s0,a1		// A1 <- length of the name

	// name is too long - we return 'int 63' to mark position of the error.
	li	a0,(VARTYPE_NUM << VAR_SHIFT) + VAR_MAXLEN // A0 <- max. int name
	li	a2,VAR_MAXLEN		// A2 <- max length
	bgt	a1,a2,9f		// exit of name is too long

	// array
	lbu	a2,0(s0)		// A2 <- next character
	li	a5,TOKEN_LPAR		// A5 <- '(' character
	li	a0,VARTYPE_ARRAY << VAR_SHIFT // A0 <- array type
	add	a0,a0,a1		// add length
	beq	a2,a5,9f		// exit if array variable

	// string or string function
	li	a4,CH_DOLLAR		// A4 <- '$' character
	bne	a2,a4,3f		// skip if not '$'

	// string function
	lbu	a2,1(s0)		// A2 <- next next character
	xori	a0,a0,(VARTYPE_ARRAY << VAR_SHIFT) ^ (VARTYPE_FOR << VAR_SHIFT) // change to string function type
	beq	a2,a5,9f		// exit if string function 'name$('

	// string
	xori	a0,a0,(VARTYPE_FOR << VAR_SHIFT) ^ (VARTYPE_STR << VAR_SHIFT) // change to string type
	j	9f

	// integer number
3:	xori	a0,a0,(VARTYPE_ARRAY << VAR_SHIFT) ^ (VARTYPE_NUM << VAR_SHIFT) // change to integer type

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// parse function name
// input: A0 ... pointer to name 'p'
//        A1 ... type and length of the name, as returned from ParseVarName() 'flag'
// output: A0 ... function index 0.., or -1=not function
// ----------------------------------------------------------------------------

.global ParseFnc
ParseFnc:
	// push registers
	addi	sp,sp,-16
	sw	ra,0(sp)
	sw	a0,4(sp)
	sb	a1,8(sp)

	// pointer to list of functions -> A1 'v'
	la	a1,FncNameList

	// function index -> A5 'fnc'
	li	a5,0

// Locals:
//  4(sp) = pointer to name 'p'
//  8(sp) = (u8) type and length 'flag'
//  9(sp), A5 = (u8) function index 'fnc'
//  12(sp), A1 = pointer to list of functions 'v'

	// check end of table
2:	lbu	a2,0(a1)		// A2 <- get type 'ch'
	addi	a1,a1,1			// A1 <- skip flag in function list 'v'+1
	li	a3,VAR_END		// A3 <- end mark
	li	a0,-1			// exit code - not found
	beq	a2,a3,9f		// exit on end of table

	// check flag
	lbu	a3,8(sp)		// A3 <- type and length 'flag'
	bne	a2,a3,3f		// skip if not equal flag

	// check name
	sb	a5,9(sp)		// save function index 'fnc'
	sw	a1,12(sp)		// save pointer to list of functions 'v'+1
	lw	a0,4(sp)		// A0 <- pointer to name 'p'
	andi	a2,a3,VAR_MASK		// A2 <- name length
	call	memcmp			// compare name
	lbu	a5,9(sp)		// A5 <- restore function index
	lw	a1,12(sp)		// A1 <- restore pointer to list of functions 'v'+1
	beqz	a0,8f			// function found OK

	// shift to next function
	lbu	a2,-1(a1)		// A2 <- get function flag 'ch'
3:	andi	a2,a2,VAR_MASK		// A2 <- name length 'n'
	add	a1,a1,a2		// skip name of function
	addi	a5,a5,1			// A5 <- increase function index
	j	2b			// loop to next function

	// return function indx
8:	mv	a0,a5			// A0 <- function index

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,16
	ret

// ----------------------------------------------------------------------------
// increment pointer and get next character from program line or from edit line
// output: A0 ... character
// ----------------------------------------------------------------------------

.global CharAdd
CharAdd:
	// increase text pointer
	lw	a4,RAM_CHARPTR_OFF(gp)	// A4 <- character pointer CharPtr
	addi	a4,a4,1			// A4 <- increase character pointer
	sw	a4,RAM_CHARPTR_OFF(gp)	// save new character pointer

	// get next character
	lbu	a0,0(a4)		// A0 <- load character

	// update cursor mode
	lw	a1,RAM_CURPTR_OFF(gp)	// A1 <- cursor pointer CurPtr
	bne	a4,a1,9f		// skip if not cursor

	// check if update K mode
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags
	andi	a2,a1,FLAG_KUPDATE	// check K-update flag
	beqz	a2,9f			// skip if not K-update flag

	// set K mode
	ori	a1,a1,FLAG_KMODE	// set K-mode flag
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags
9:	ret

// ----------------------------------------------------------------------------
// get current character from program line or from edit line, skipping spaces
// output: A0 ... character
// ----------------------------------------------------------------------------

.global GetChar
GetChar:
	// get current character pointer
	lw	a4,RAM_CHARPTR_OFF(gp)	// A4 <- character pointer CharPtr

	// get next character
	lbu	a0,0(a4)		// A0 <- load character

	// skip if not space (CH_SPC has code 0)
	bnez	a0,NextChar9		// exit if character is not space
	// j	NextChar

// continue to NextChar

// ----------------------------------------------------------------------------
// increment pointer and get next character from program line or from edit line, skipping spaces
// output: A0 ... character
// ----------------------------------------------------------------------------

.global NextChar
NextChar:
	// save return address
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get next character
2:	call	CharAdd			// get next character
	beqz	a0,2b			// loop if next character is CH_SPC (with code 0)

	// restore return address
	lw	ra,0(sp)
	addi	sp,sp,4
NextChar9:
	ret

// ----------------------------------------------------------------------------
// check end of command line
// output: A0 ... 1 = continue, 0 = only checking syntax, break
// ----------------------------------------------------------------------------

.global CheckEnd
CheckEnd:
	// exit with True if not checking syntax
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,FLAG_SYNTOFF	// check syntax OFF flag
	bnez	a0,8f			// syntax OFF, program is running, exit with True

	// save return address
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get current character
	call	GetChar			// get current character, skip spaces

	// check end of command line
	li	a1,NEWLINE		// A1 <- new line character
	beq	a0,a1,6f		// skip if end of line is OK

	// not end of line, syntax error
	call	SyntaxError

	// restore return address
6:	lw	ra,0(sp)
	addi	sp,sp,4

	// exit with False
	li	a0,0			// A0 <- False, checking syntax
	ret

	// exit with True
8:	li	a0,1			// A0 <- True, continue program
	ret

// ----------------------------------------------------------------------------
// parse signed number (saves result to LastResult)
// Value VAL_MIN cannot be entered.
// ouput: A0 ... value
// ----------------------------------------------------------------------------

.global ParseNum
ParseNum:
	// save registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// clear result accumulator 'res'
	li	s0,0

	// load next character 'ch' -> A0
2:	lw	a4,RAM_CHARPTR_OFF(gp)	// A4 <- get pointer to current character CharPtr
	lbu	a0,0(a4)		// A0 <- load next character 'ch'

	// check digit
	li	a1,9			// A1 <- max. digit
	addi	a0,a0,-CH_0		// A0 <- subtract digit base CH_0
	bgtu	a0,a1,9f		// not digit, break loop

	// check overflow if 'res' > VAL_MAX/10
	li	a1,VAL_MAX/10		// max. valid value before multiplication * 10 (= 0x7fffffff/10 = 0xccccccc)
	bgt	s0,a1,4f		// go overflow if accumulator > VAL_MAX/10

	// accumulator * 10
	li	a1,10
	mul	s0,s0,a1		// S0 <- accumulator * 10

	// check overflow if 'res' + 'ch' > VAL_MAX
	li	a1,VAL_MAX		// A1 <- max. value 0x7fffffff
	sub	a1,a1,a0		// A1 <- VAL_MAX - 'ch'
	ble	s0,a1,5f		// skip if OK, if 'res' <= VAL_MAX - 'ch'

	// overflow
4:	call	Overflow

	// add digit to accumulator (on overflow A0 is invalid, but it is OK)
5:	add	s0,s0,a0		// add digit to accumulator 'res' += 'ch'

	// shift pointer (with update cursor)
	call	CharAdd
	j	2b			// next loop

	// store result
9:	mv	a0,s0			// A0 <- result
	sw	a0,RAM_LASTRESULT_OFF(gp) // save last result

	// restore registers
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// parse unsigned number (saves result to LastResult; not checking overflow)
// ouput: A0 ... value
// ----------------------------------------------------------------------------

.global ParseUNum
ParseUNum:
	// save registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// clear result accumulator 'res'
	li	s0,0

	// load next character 'ch' -> A0
2:	lw	a4,RAM_CHARPTR_OFF(gp)	// A4 <- get pointer to current character CharPtr
	lbu	a0,0(a4)		// A0 <- load next character 'ch'

	// check digit
	li	a1,9			// A1 <- max. digit
	addi	a0,a0,-CH_0		// A0 <- subtract digit base CH_0
	bgtu	a0,a1,9f		// not digit, break loop

	// accumulator * 10
	li	a1,10
	mul	s0,s0,a1		// S0 <- accumulator * 10

	// add digit to accumulator (on overflow A0 is invalid, but it is OK)
	add	s0,s0,a0		// add digit to accumulator 'res' += 'ch'

	// shift pointer (with update cursor)
	call	CharAdd
	j	2b			// next loop

	// store result
9:	mv	a0,s0			// A0 <- result
	sw	a0,RAM_LASTRESULT_OFF(gp) // save last result

	// restore registers
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

#endif // USE_PARSE_ASM
