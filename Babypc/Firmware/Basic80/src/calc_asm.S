
// ****************************************************************************
//
//                        Calculate expression
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_CALC_ASM		// 1=use ASM in Calc module

// ----------------------------------------------------------------------------
// Fetch value (can raise error)
// - Called during syntax checking
// - On input, flags determine required result type.
// ----------------------------------------------------------------------------

.global FetchVal
FetchVal:
	// prepare stack
	add	sp,sp,-8
	sw	ra,0(sp)	// save return address

	// save flags with required result type
	lbu	a0,RAM_FLAGS_OFF(gp) // A0 <- get current flags
	sw	a0,4(sp)	// save old flags

	// evaluate expression (can raise error)
	call	ScanCalc

	// restore stack
	lw	a0,4(sp)	// A0 <- restore old flags
	lw	ra,0(sp)	// restore return address
	add	sp,sp,8

	// check error
	lbu	a2,RAM_ERRNR_OFF(gp) // A2 <- get current error code
	bnez	a2,NumNegRet	// return on error

	// if not checking syntax, run LET command
	lbu	a1,RAM_FLAGS_OFF(gp) // A1 <- get current flags
	andi	a2,a1,FLAG_SYNTOFF // check syntax
	beqz	a2,2f		// skip if checking syntax
	j	CmdLet		// run LET command

// here is: A0 = old flags, A1 = current flags

	// if checking syntax, only compare result type
	// - syntax error, if result types are different
2:	xor	a0,a0,a1	// A0 <- old flags XOR new flags
	andi	a0,a0,FLAG_NUMRES // check if result types 
	beqz	a0,4f		// result type is OK
	j	SyntaxError	// syntax error

	// check end of command line
4:	j	CheckEnd

// ----------------------------------------------------------------------------
// absolute value (note: VAL_MIN stays unchanged, should be error)
// - Input and output to LastResult.
// ----------------------------------------------------------------------------

.global FncAbs
FncAbs:
	lw	a0,RAM_LASTRESULT_OFF(gp) // get input value
	bgez	a0,2f			// skip if not negative
	neg	a0,a0			// negate AI
	sw	a0,RAM_LASTRESULT_OFF(gp) // save new value
2:	ret

// ----------------------------------------------------------------------------
// negate value (note: VAL_MIN stays unchanged, should be error)
// input: A0 ... input number
// ----------------------------------------------------------------------------

.global NumNeg
NumNeg:
	neg	a0,a0			// negate A0
NumNegRet:
	ret

// ----------------------------------------------------------------------------
// subtraction a - b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumSub
NumSub:
	neg	a1,a1			// negate A1
//	j	NumAdd

// ---- continue to NumAdd

// ----------------------------------------------------------------------------
// addition a + b
// input: A0 ... 1st operand
//        A1 ... 2nd operand
// ----------------------------------------------------------------------------

.global NumAdd
NumAdd:
	// check if b > 0
	blez	a1,2f		// OK if b <= 0

	// check if a > VAL_MAX - b
	li	a5,VAL_MAX	// A5 <- VAL_MAX max. value of the s16
	sub	a5,a5,a1	// A5 <- VAL_MAX - b
	bge	a5,a0,3f	// OK if VAL_MAX - b >= a

	// overflow, no return
1:	j	Overflow

	// check if b < 0
2:	beqz	a1,3f		// OK if b == 0

	// check if a < VAL_MIN - b
	li	a5,VAL_MIN	// A5 <- VAL_MIN min. value of the s16
	sub	a5,a5,a1	// A5 <- VAL_MIN - b
	blt	a0,a5,1b	// overflow if a < VAL_MIN - b

	// do addition
3:	add	a0,a0,a1
	ret

// ----------------------------------------------------------------------------
// multiplication a * b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumMul
NumMul:
	// check if 'a' or 'b' is zero
	beqz	a0,9f		// a is zero
	beqz	a1,8f		// b is zero

	// save registers
	addi	sp,sp,-20
	sw	ra,0(sp)	// save RA

// SP+0: RA
// SP+4: A4 with result sign
// SP+8: A0 absolute value
// SP+12: A1 absolute value
// SP+16: 1st result of clz

	// VAL_MIN has no positive opposite value
	// check ((a == -1) && (b == VAL_MIN))
	li	a5,-1		// A5 <- -1
	bne	a0,a5,2f	// OK if 'a' is not -1
	li	a4,VAL_MIN	// A4 <- VAL_MIN
	bne	a1,a4,2f	// skip if OK

	// Error
NumMul_Overflow:
1:	j	Overflow

	// VAL_MIN has no positive opposite value
	// check ((b == -1) && (a == VAL_MIN))
2:	bne	a1,a5,3f	// OK if 'b' is not -1
	beq	a0,a4,1b	// skip if error

	// prepare result sign
3:	li	a4,0		// A4 <- 0, result is positive
	bgez	a0,4f		// skip if 'a' is not negative
	neg	a0,a0		// A0 <- absolute value of 'a'
	li	a4,1		// A4 <- 1, result will be negative

4:	bgez	a1,5f		// skip if 'b' is not negative
	neg	a1,a1		// A1 <- absolute value of 'b'
	xori	a4,a4,1		// change sign

// SP+0: RA
// SP+4: A4 with result sign
// SP+8: A0 absolute value 'a'
// SP+12: A1 absolute value 'b'
// SP+16: 1st result of clz

	// check overflow - result must not exceed 32 bits
5:	sw	a4,4(sp)	// push result sign
	sw	a0,8(sp)	// push 1st value 'a'
	sw	a1,12(sp)	// push 2nd value 'b'
	jal	__clzsi2	// get leading zeroes of 'a'
	sw	a0,16(sp)	// save 1st result of clz

	lw	a0,12(sp)	// A0 <- 2nd value 'b'
	jal	__clzsi2	// get leading zeroes of 'b'

	lw	a5,16(sp)	// A5 <- 1st result of clz(a)
	li	a3,64		// A3 <- 32+32
	sub	a3,a3,a5	// subtract bits of 'a'
	sub	a3,a3,a0	// subtract bits of 'b'
	li	a5,VAL_BITS	// max. bits
	blt	a5,a3,1b	// overflow max. bits result

	// multiply
	lw	a0,8(sp)	// 1st value 'a'
	lw	a1,12(sp)	// 2nd value 'b'
	mul	a0,a0,a1	// A0 <- multiply a*b

	// check overflow result
#if VALSIZE == 16
	slli	a5,a0,16	// A5 <- check sign bit
#endif
	bltz	a5,1b		// overflow if result is negative

	// set result sign
	lw	a4,4(sp)	// A4 <- result sign
	beqz	a4,7f		// positive
	neg	a0,a0		// negate result

	// restore registers
7:	lw	ra,0(sp)	// restore RA
	addi	sp,sp,20
9:	ret

	// result is 0
8:	li	a0,0
	ret

// ----------------------------------------------------------------------------
// division a / b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumDiv
NumDiv:
	// zero division check
	beqz	a1,NumMul_Overflow	// overflow if b == 0

	// VAL_MIN has no positive opposite value
	li	a5,VAL_MIN	// A5 <- VAL_MIN
	bne	a0,a5,3f
	li	a5,-1
	beq	a1,a5,NumMul_Overflow

	// divide number
3:	j	__divsi3

// ----------------------------------------------------------------------------
// power a ^ b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumPwr
NumPwr:
	// exponent must be positive
	bltz	a1,NumMul_Overflow // exponent is negative

	// special case 1^b
	li	a2,1		// A2 <- 1
	beq	a0,a2,3f	// special case 1^b returns 1

	// special case 0^b
	bnez	a0,4f		// skip if 'a' is not 0
	beqz	a1,NumMul_Overflow // 0^0 is not defined
3:	ret			// return 0 in case of 0^b

	// special case -1^b
4:	li	a2,-1		// A2 <- -1
	bne	a0,a2,5f	// skip if 'a' is not -1
	andi	a2,a1,1		// check if 'b' is even
	bnez	a2,3b		// return -1 (= a) if 'b' is off
	li	a0,1		// return 1
	ret

	// special case 2^b
5:	li	a2,2		// A2 <- 2
	bne	a0,a2,7f	// skip if 'a' is not 2
6:	li	a2,VAL_BITS-2	// max. shift value
	bgt	a1,a2,NumMul_Overflow // overflow if b > 14
	li	a0,1		// A0 <- 1
62:	sll	a0,a0,a1	// A0 <- 1 << b
	ret

	// special case -2^b
7:	li	a2,-2		// A2 <- -2
	bne	a0,a2,8f	// skip if 'a' is not -2
	andi	a2,a1,1		// check if 'b' is odd
	beqz	a2,6b		// 'b' is even, result will be positive
	li	a2,VAL_BITS-1	// max. shift value
	bgt	a1,a2,NumMul_Overflow // overflow if b > 15
	li	a0,-1		// A0 <- -1
	j	62b		// A0 <- -1 << b

	// multiply
8:	addi	sp,sp,-12
	sw	ra,0(sp)	// save return address
	sw	a0,4(sp)	// save 'a'
	li	a0,1		// A0 <- 1 result accumulator
	beqz	a1,10f		// 'b' = 0, result is 1

9:	sw	a1,8(sp)	// save 'b'
	lw	a1,4(sp)	// A1 <- 'a'
	jal	NumMul		// A0 <- A0 * 'a'

	lbu	a1,RAM_ERRNR_OFF(gp) // get error code
	bnez	a1,3b		// error exit on overflow

	lw	a1,8(sp)	// A1 <- b
	addi	a1,a1,-1	// b <- b-1
	bnez	a1,9b		// loop

10:	lw	ra,0(sp)
	addi	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// bitwise AND a & b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global BitAND
BitAND:
	and	a0,a0,a1
	ret

// ----------------------------------------------------------------------------
// bitwise OR a | b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global BitOR
BitOR:
	or	a0,a0,a1
	ret

// ----------------------------------------------------------------------------
// compare a == b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumEqu
NumEqu:
	sub	a0,a0,a1	// A0 <- A0 - A1
	seqz	a0,a0		// A0 <- 1 if A0 == A1
	neg	a0,a0		// A0 <- -1 if A0 == A1
	ret

// ----------------------------------------------------------------------------
// compare a > b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumGrt
NumGrt:
	slt	a0,a1,a0	// A0 <- 1 if A0 > A1
	neg	a0,a0		// A0 <- -1 if A0 > A1
	ret

// ----------------------------------------------------------------------------
// compare a < b
// input: A0 (s16) ... 1st operand
//        A1 (s16) ... 2nd operand
// ----------------------------------------------------------------------------

.global NumLes
NumLes:
	slt	a0,a0,a1	// A0 <- 1 if A0 < A1
	neg	a0,a0		// A0 <- -1 if A0 < A1
	ret

// ----------------------------------------------------------------------------
// string compare (strings are terminated with CH_QUOT character)
// input: A0 ... 1st operand string (offset in RAM)
//        A1 ... 2nd operand string (offset in RAM)
// returns -1 if a<b, 0 if a==b, 1 if a>b
// ----------------------------------------------------------------------------

.global StrCmp
StrCmp:
	// shift offset to pointers
#if VALSIZE == 16
	add	a0,a0,gp		// A0 <- pointer to 1st string
	add	a1,a1,gp		// A1 <- pointer to 2nd string
#endif

	// prepare terminator
	li	a3,CH_QUOT		// A3 <- terminating character

	// load 2 characters
2:	lbu	a5,0(a0)		// A5 <- load character from 1st string
	lbu	a4,0(a1)		// A4 <- load character from 2nd string
	addi	a0,a0,1			// shift 1st pointer
	addi	a1,a1,1			// shift 2nd pointer

	// check if str1 < str2
	bltu	a5,a4,4f		// skip if str1 < str2
	bltu	a4,a5,5f		// skip if str1 > str2

	// loop if not QUOT
	bne	a5,a3,2b

	// str1 == str2, result is 0
	li	a0,0			// result is 0
	ret

	// str1 < str2, result is -1
4:	li	a0,-1			// result is -1
	ret

	// str1 > str2, result is 1
5:	li	a0,1			// result is 1
	ret

// ----------------------------------------------------------------------------
// compare string if equal a == b
// input: A0 ... 1st operand string (offset in RAM)
//        A1 ... 2nd operand string (offset in RAM)
// ----------------------------------------------------------------------------

.global StrEqu
StrEqu:
	addi	sp,sp,-4
	sw	ra,0(sp)
	jal	StrCmp
	lw	ra,0(sp)
	addi	sp,sp,4

	seqz	a0,a0
	neg	a0,a0
	ret

// ----------------------------------------------------------------------------
// compare string if greater a > b
// input: A0 ... 1st operand string (offset in RAM)
//        A1 ... 2nd operand string (offset in RAM)
// ----------------------------------------------------------------------------

.global StrGrt
StrGrt:
	addi	sp,sp,-4
	sw	ra,0(sp)
	jal	StrCmp
	lw	ra,0(sp)
	addi	sp,sp,4

	sgtz	a0,a0
	neg	a0,a0
	ret

// ----------------------------------------------------------------------------
// compare string if less a < b
// input: A0 ... 1st operand string (offset in RAM)
//        A1 ... 2nd operand string (offset in RAM)
// ----------------------------------------------------------------------------

.global StrLes
StrLes:
	addi	sp,sp,-4
	sw	ra,0(sp)
	jal	StrCmp
	lw	ra,0(sp)
	addi	sp,sp,4

	srai	a0,a0,31
	ret

// ----------------------------------------------------------------------------
// RND function - random number generator (number 1..max will be generated)
// - Input and output to LastResult.
// ----------------------------------------------------------------------------

.global FncRnd
FncRnd:
	addi	sp,sp,-4
	sw	ra,0(sp)

	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- max. value
	li	a0,1		// A0 <- min. value
	blez	a1,2f		// returns 1 if n<=0
#if VALSIZE == 32
	call	RandS32MinMax
#else
	call	RandS16MinMax
#endif
2:	sw	a0,RAM_LASTRESULT_OFF(gp)

	lw	ra,0(sp)
	addi	sp,sp,4
	ret

// ----------------------------------------------------------------------------
// command RANDOMISE
// ----------------------------------------------------------------------------

.global CmdRandomise
CmdRandomise:
	lw	a5,RAM_LASTRESULT_OFF(gp) // A5 <- get last result LastResult with new seed

	// if seed = 0, get seed from frame counter
	lui	a4,%hi(RandSeed) // A4 <- pointer to RandSeed HIGH
	bnez	a5,2f		// skip if seed is not 0

	lw	a5,%lo(RandSeed)(a4) // A5 <- get RandSeed
	lw	a3,%lo(DispFrame)(a4) // A3 <- get DispFrame
	lw	a2,%lo(DispLine)(a4) // A2 <- pointer to DispLine
	add	a5,a5,a3	// A5 <- RandSeed + DispFrame
	add	a5,a5,a2	// A5 <- RandSeed + DispFrame + DispLine

2:	sw	a5,%lo(RandSeed)(a4) // store new RandSeed
	ret

// ----------------------------------------------------------------------------
// CODE function - get numeric code of 1st character in string
// - Input 'str' and output 'num' to LastResult.
// ----------------------------------------------------------------------------

.global FncCode
FncCode:
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- get input value
#if VALSIZE == 16
	add	a0,a0,gp	// convert address to pointer 'p'
#endif
	lbu	a0,0(a0)	// get character code
	sw	a0,RAM_LASTRESULT_OFF(gp) // save result
	ret

// ----------------------------------------------------------------------------
// store to calculator stack (can raise error)
// - Called during syntax checking from CLASS5.
// input: A0 (s32) ... value to store to the stack
//        A1 (u8) ... operation
//        A2 (u8) ... priority
// ----------------------------------------------------------------------------

.global PushCalc
PushCalc:
	// get current number of entries in calculator stack
	lbu	a5,RAM_CALCNUM_OFF(gp)	// A5 <- get number of entries in calculator stack
	li	a4,CALCNUM		// A3 <- max. number of entries in calculator stack
	bge	a5,a4,4f		// calculator stack overflow

	// set new number of entries
	addi	a4,a5,1			// A4 <- increase number of entries
	sb	a4,RAM_CALCNUM_OFF(gp)	// set new number of entries in the calculator stack

	// prepare pointer to operation and priority
	la	a3,CalcStck		// A3 <- calculator stack - operation and priority
	slli	a4,a5,1			// A4 <- old number of entries * 2
	add	a4,a4,a3		// A4 <- pointer to calculator stack CalcStck

	// store operation and priority
	sb	a1,0(a4)		// store operation
	sb	a2,1(a4)		// store priority

	// store value
	la	a3,ValStck		// A3 <- calculator stack - values
	slli	a4,a5,2			// A4 <- old number of entries * 4
	add	a4,a4,a3		// A4 <- pointer to calculator stack ValStck
	sw	a0,0(a4)		// store value
	ret

	// calculator stack overflow
4:	j	MemError		// calculator stack overflow

// ----------------------------------------------------------------------------
// load from calculator stack (returns value) (can raise error)
// - Called during syntax checking from CLASS5.
// input: A0 ... pointer to u8 variable to get operation
//        A1 ... pointer to u8 variable to get priority
// return: A0 ... value
// ----------------------------------------------------------------------------

.global PopCalc
PopCalc:
	// get current number of entries in calculator stack
	lbu	a5,RAM_CALCNUM_OFF(gp)	// A5 <- get number of entries in calculator stack
	addi	a5,a5,-1		// A5 <- number of entries - 1
	bltz	a5,4f			// stack underflow

	// set new number of entries
	sb	a5,RAM_CALCNUM_OFF(gp)	// set new number of entries in the calculator stack

	// prepare pointer to operation and priority
	la	a3,CalcStck		// A3 <- calculator stack - operation and priority
	slli	a4,a5,1			// A4 <- old number of entries * 2
	add	a4,a4,a3		// A4 <- pointer to calculator stack CalcStck

	// get operation and priority
	lbu	a3,0(a4)		// A3 <- get operation
	lbu	a2,1(a4)		// A2 <- get priority
	sb	a3,0(a0)		// store operation
	sb	a2,0(a1)		// store priority

	// get value
	la	a3,ValStck		// A3 <- calculator stack - values
	slli	a4,a5,2			// A4 <- old number of entries * 4
	add	a4,a4,a3		// A4 <- pointer to calculator stack ValStck
	lw	a0,0(a4)		// get value
	ret

	// RETURN without GOSUB
4:	j	ReturnError		// RETURN without GOSUB

// ----------------------------------------------------------------------------
// evaluate bracket expression (p = pointer to program line)
// - Called during syntax checking from CLASS5.
// input: A0 ... pointer to program line
// ----------------------------------------------------------------------------

.global Bracket
Bracket:
	// save return address
	add	sp,sp,-4
	sw	ra,0(sp)

	// evaluate expression
	call	EvalExp

	// get character
	call	GetChar

	// restore return address
	lw	ra,0(sp)
	add	sp,sp,4

	// check right bracket
	li	a5,TOKEN_RPAR
	bne	a0,a5,2f		// not right bracket - error
	j	CharAdd			// skip right bracket

2:	j	SyntaxError		// syntax error

// ----------------------------------------------------------------------------
// evaluate expression, store result to LastResult (p = pointer to program line)
// - Called during syntax checking from CLASS5.
// input: A0 ... pointer to program line
// ----------------------------------------------------------------------------

.global EvalExp
EvalExp:
	// save return address
	add	sp,sp,-4
	sw	ra,0(sp)

	// set pointer - 1
	addi	a0,a0,-1
	sw	a0,RAM_CHARPTR_OFF(gp)	// save new pointer CharPtr

	// update cursor
	call	CharAdd

	// restore return address
	lw	ra,0(sp)
	add	sp,sp,4

	// scanning calculator expression
	j	ScanCalc

// ---- continue to ScanCalc

// ----------------------------------------------------------------------------
// scanning calculator expression, store result to LastResult
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global ScanCalc
ScanCalc:
	// prepare stack
	add	sp,sp,-16
	sw	ra,0(sp)	// save return address
	sw	s0,4(sp)	// save S0
	sw	s1,8(sp)	// save S1

// 0: save RA
// 4: save S0 ... S0 = previous value 'val2'
// 8: save S1 ... S1 = current character 'ch', currect value 'val'
// 12: (1) current operator 'op' (address must be 16-bit aligned)
// 13: (1) current priority 'prior'
// 14: (1) previous operator 'op2'
// 15: (1) previous priority 'prior2'

	// reset priority level - set flag to terminate scanning
	li	a0,0		// value
	li	a1,0		// operation
	li	a2,0		// priority
	call	PushCalc	// push values

	// start of parse loop
ScanCalc_Loop:

	// break on error
	lbu	a0,RAM_ERRNR_OFF(gp)	// get error code ErrNr
	bnez	a0,ScanCalc_Exit	// exit on error

// ---- scan operand (store result to LastResult)

	// get current character from program line or from edit line, skipping spaces -> S1
	call	GetChar			// get character
	mv	s1,a0			// S1 <- save character

// variable or function

	// check alphanumeric character '0'..'9' or 'A'..'Z'
	call	IsAlphaNum		// check alphanumeric (return True=1 if OK)
	beqz	a0,3f			// skip if not alphanumeric

	// check alphabetic character 'A'..'Z' - if alphabetic character, this is variable
	mv	a0,s1			// A0 <- character
	call	IsAlpha			// check alphabetic character
	beqz	a0,2f			// skip if not alphabetic

	// parse variable or function
	// - Called during syntax checking from CLASS5.
	li	a0,1			// A0 <- True functions are allowed
	call	LookVars		// parse variable or function
	j	ScanCalc_Oper		// go scan operator

// number

	// numeric character, this is number
	// parse number (saves result to LastResult) (can raise Overflow error)
2:	call	ParseNum		// parse number

	// set numeric result
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	ori	a0,a0,FLAG_NUMRES	// set numeric result flag
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags
	j	ScanCalc_Oper		// go scan operator

// negate
// ... at this place: A0 = 0

	// negate '-'
3:	li	a5,TOKEN_SUB		// token '-'
	bne	a5,s1,4f		// skip if not '-' negate

	// push operation: val = 0, operation = "subtract", priority = 9
	// ... at this place: A0 = 0
	li	a2,9			// priority = 9
1:	li	a1,TOKEN_SUB - TOKEN_SUB // = 0, operation = "subtract"
ScanCalc_LoopPush:
	call	PushCalc		// push operation (value = 0)
	call	NextChar		// skip operator
	j	ScanCalc_Loop		// continue loop

// NOT

	// NOT
	// push operation: val = -1, operation = "subtract", priority = 4
4:	li	a0,-1			// value -1 (= 0xffffffff)
	li	a2,4			// priority = 4
	li	a5,TOKEN_NOT		// token NOT
	beq	a5,s1,1b		// push operation

// bracket

	// opening bracket
	li	a5,TOKEN_LPAR		// token '(' left bracket
	bne	a5,s1,5f		// skip if not left bracket

	// evaluate bracket expression
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get CharPtr pointer
	addi	a0,a0,1			// A0 <- CharPtr + 1
	call	Bracket			// evaluate bracket expression
	j	ScanCalc_Oper		// go scan operator

ScanCalc_SyntErr:

	// syntax error; exit, restore stack
9:	lw	ra,0(sp)	// restore return address
	lw	s0,4(sp)	// restore S0
	lw	s1,8(sp)	// restore S1
	add	sp,sp,16
	j	SyntaxError	// go to syntax error

// string

	// quote "
5:	li	a5,CH_QUOT		// character " string separator
	bne	a5,s1,9b		// skip if not quote

	// set string result
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,~FLAG_NUMRES	// set string result flag
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags

	// save result (skip quote character)
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get character pointer CharPtr
	addi	a0,a0,1			// skip quote character
	sw	a0,RAM_LASTRESULT_OFF(gp) // set last result

	// find end of string
6:	call	CharAdd			// skip and load next character
	li	a5,NEWLINE		// A5 <- end of line
	beq	a5,a0,9b		// go error, end of program line (no quote)
	li	a5,CH_QUOT		// A5 <- quote character
	bne	a5,a0,6b		// loop, search end of string

	// skip terminating quote "
	call	CharAdd

// ---- Scan perator

ScanCalc_Oper:

// 0: save RA
// 4: save S0 ... S0 = previous value 'val2'
// 8: save S1 ... S1 = current character 'ch', currect value 'val'
// 12: (1) current operator 'op' (address must be 16-bit aligned)
// 13: (1) current priority 'prior'
// 14: (1) previous operator 'op2'
// 15: (1) previous priority 'prior2'

	// get current character from program line or from edit line, skipping spaces
	call	GetChar			// A0 <- get next character

	// preset for case of invalid operator - terminate this level
	sh	zero,12(sp)		// clear current operator and current priority

	// check operator -, +, *, /, AND, OR, **, =, >, < ... minimal operator is '-'
	addi	a0,a0,-TOKEN_SUB	// subtract minimal operator '-'
	li	a5,10			// maximal operator value
	bgeu	a0,a5,2f		// invalid operator

	// get priority
	sb	a0,12(sp)		// save current operator (0..9)
	la	a5,PriorTab		// A5 <- priority table
	add	a5,a5,a0		// A5 <- pointer into priority table
	lbu	a0,0(a5)		// A0 <- priority of this operator
	sb	a0,13(sp)		// save current priority	

// process priorities loop
ScanCalc_OperLoop:

	// break on error
2:	lbu	a0,RAM_ERRNR_OFF(gp)	// get error code ErrNr
	bnez	a0,ScanCalc_Exit	// exit on error

	// load previous calculator stack
	addi	a0,sp,14		// A0 <- pointer to previous operator 'op2'
	addi	a1,sp,15		// A1 <- pointer to previous priority 'prior2'
	call	PopCalc			// load previous calculator stack

// >>>> from here A0 = previous value 'num2' <<<<

	// compare with previous priority
	lbu	a1,13(sp)		// A1 <- current priority 'prior'
	lbu	a2,15(sp)		// A2 <- previous priority 'prior2'
	bgt	a1,a2,ScanCalc_Higher	// skip if current priority is higher

// current priority is not higher, perform last operation

	// both priorities are 0, end of expression (result is in LastResult)
	beqz	a2,ScanCalc_Exit	// both priorities are 0, end of processing

	// A4 <- get previous operator 'op2'
	lbu	a4,14(sp)		// A4 <- previous operator 'op2'

	// check if only checking syntax
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags
	andi	a2,a1,FLAG_SYNTOFF	// check syntax off flag
	beqz	a2,ScanCalc_Syntax	// skip if only checking syntax

	// A1 <- get last result (here is A0 = previous result)
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- get last result

	// calculate previous operator
	// Here is:
	//  A0 = previous value 'num2'
	//  A1 = this value
	//  A4 = previous operator
1:	auipc	a3,%pcrel_hi(ScanCalc_Tab) // A3 <- PC + hi(ScanCalc_Tab)
	slli	a4,a4,2			// A4 <- operator * 4 (= offset in jump table)
	add	a3,a3,a4		// A3 <- address in jump table
	lw	a3,%pcrel_lo(1b)(a3)	// A3 <- load jump address
	jalr	a3			// call function (and return)
	j	ScanCalc_SyntaxLoop	// continue and set numeric result

	// only checking syntax
ScanCalc_Syntax:

// here is A0 = previous value 'num2', A1 = flags, A4 = previous operator 'op2'

	// compare operands, if are the same type
	li	a3,9			// A3 <- max. operator
	andi	a1,a1,FLAG_NUMRES	// check numeric result
	beqz	a1,3f			// skip if string result

	// numeric result - check if op2 is <= 9
	bleu	a4,a3,ScanCalc_SyntaxLoop // op2 <= 9, all OK
ScanCalc_SyntErr2:
	j	ScanCalc_SyntErr	// op2 > 9, syntax error

	// string result - check if op2 is > 9
3:	bleu	a4,a3,ScanCalc_SyntErr2	// op2 <= 9, syntax error

ScanCalc_SyntaxLoop:

	// save new last result
	sw	a0,RAM_LASTRESULT_OFF(gp) // save new last result

	// set numeric result
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	ori	a0,a0,FLAG_NUMRES	// set numeric result flag
	sb	a0,RAM_FLAGS_OFF(gp)	// save new flags
	j	ScanCalc_OperLoop	// process next operator

// current priority is higher than previous one (prior > prior2), go deeper
ScanCalc_Higher:

// here is A0 = previous value 'num2', A2 = previous priority 'prior2'

	// save previous operation back to the calculator stack
	lbu	a1,14(sp)		// A1 <- previous operator 'op2'
	call	PushCalc		// push calculator stack

	// get current operator and current priority
	lbu	a1,12(sp)		// A1 <- current operator 'op'
	lbu	a2,13(sp)		// A2 <- current priority 'prior'

	// check string operator
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,FLAG_NUMRES	// check numeric result
	bnez	a0,4f			// skip if numeric result

	// shif operator to string functions 10..12
	addi	a1,a1,3			// shift operator op += 3

	// check operator, must be > 9
	li	a3,9			// max. numeric operator
	bleu	a1,a3,ScanCalc_SyntErr2	// operator is <= 9, syntax error

	// get last result
4:	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- get last result as current value 'num'

	// break this loop of lower priorities, load next operand
	// - push next operation to the calculator stack
	// - discard this operator
	j	ScanCalc_LoopPush

ScanCalc_Exit:

	// exit, restore stack
	lw	ra,0(sp)	// restore return address
	lw	s0,4(sp)	// restore S0
	lw	s1,8(sp)	// restore S1
	add	sp,sp,16
	ret

	// jump table
	// operators -, +, *, /, AND, OR, **, =, >, <
	.align  2,,
ScanCalc_Tab:
	.word	NumSub		// '-'
	.word	NumAdd		// '+'
	.word	NumMul		// '*'
	.word	NumDiv		// '/'
	.word	BitAND		// 'AND'
	.word	BitOR		// 'OR'
	.word	NumPwr		// '**'
	.word	NumEqu		// '='
	.word	NumGrt		// '>'
	.word	NumLes		// '<'
	.word	StrEqu		// str '='
	.word	StrGrt		// str '>'
	.word	StrLes		// str '<'

// ----------------------------------------------------------------------------
// function READ, read data (returns result to LastResult)
// ----------------------------------------------------------------------------

.global FncRead
FncRead:
	// push registers
	add	sp,sp,-16
	sw	ra,0(sp)
	sw	s0,4(sp)

// Locals:
//  0(sp): RA
//  4(sp): S0
//  8(sp): save CharPtr
//  12(sp): save Flags
//  S0: program pointer 'p'

	// save current pointer and K-flags
	lw	a5,RAM_CHARPTR_OFF(gp)	// A5 <- current CharPtr
	lbu	a4,RAM_FLAGS_OFF(gp)	// A4 <- current Flags
	sw	a5,8(sp)		// save CharPtr
	sw	a4,12(sp)		// save Flags

	// get data pointer -> S0 'p'
	lw	s0,RAM_DATAPTR_OFF(gp)	// S0 <- current data pointer DataPtr
	sw	s0,RAM_CHARPTR_OFF(gp)	// use as text pointer CharPtr

FncRead_Loop:

	// check if need to find next DATA line ('p' is NULL of GetChar is NEWLINE)
2:	bnez	s0,3f			// skip if pointer 'p' is not NULL
	addi	s0,gp,RAM_PROG_OFF	// S0 <- reset pointer to start of program Prog
	j	4f

3:	call	GetChar			// get next character
	li	a5,NEWLINE		// A5 <- new line
	bne	a0,a5,FncRead_EndLoop	// some data remain
	lw	s0,RAM_CHARPTR_OFF(gp)	// S0 <- current text pointer
	addi	s0,s0,1			// skip NEWLINE character

// --- find DATA line

FncRead_Data:

	// check end of program (if line = 0)
4:	lbu	a4,0(s0)		// A4 <- program line LOW
	lbu	a5,1(s0)		// A5 <- program line HIGH
	or	a5,a5,a4		// check if program line is 0
	bnez	a5,6f			// skip if not end of program

	// if this is first pass, do not continue, we have no DATA row
	lw	a0,RAM_DATAPTR_OFF(gp)	// A0 <- get data pointer
	beqz	a0,FncRead_End		// first pass, return value 0

	// rewind pointer to start
	addi	s0,gp,RAM_PROG_OFF	// S0 <- reset pointer to start of program Prog

	// check DATA program line
6:	addi	a4,s0,2			// A4 <- skip line number
	sw	a4,RAM_CHARPTR_OFF(gp)	// save new text pointer CharPtr
	call	GetChar			// get next character -> A0
	li	a4,TOKEN_DATA		// DATA token
	beq	a0,a4,7f		// skip if DATA token has been found

	// next program line
	mv	a0,s0			// A0 <- pointer
	call	NextLine		// get next character
	mv	s0,a0			// S0 <- next line
	j	FncRead_Data

	// skip DATA token
7:	call	NextChar		// skip next character
	j	FncRead_Loop

// --- get next data

FncRead_EndLoop:

	// skip comma separators
	call	GetChar			// get next character -> A0
	li	a5,TOKEN_COMMA
	bne	a0,a5,8f		// not comma
	call	NextChar		// skip comma
	j	FncRead_EndLoop

	// parse expression
8:	call	ScanCalc		// parse expression

	// save current pointer
	lw	a5,RAM_CHARPTR_OFF(gp)	// get current pointer CharPtr
	sw	a5,RAM_DATAPTR_OFF(gp)	// save data pointer DataPtr
	j	9f

FncRead_End:	// Here is A0 = return value

	// set result
	sw	a0,RAM_LASTRESULT_OFF(gp) // set LastResult

	// return pointer and flags
9:	lw	a5,8(sp)		// load CharPtr
	lw	a4,12(sp)		// load Flags
	sw	a5,RAM_CHARPTR_OFF(gp)	// restore CharPtr
	sb	a4,RAM_FLAGS_OFF(gp)	// restore Flags

	// pop registers
	lw	s0,4(sp)
	lw	ra,0(sp)
	add	sp,sp,16
	ret

#endif // USE_CALC_ASM
