
// ****************************************************************************
//
//                             Print
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_PRINT_ASM		// 1=use ASM in Print module

// ----------------------------------------------------------------------------
// command CLS - clear display
// ----------------------------------------------------------------------------

.global CmdCls
CmdCls:
	// clear screen
	la	a1,FrameBuf		// A1 <- pointer to frame buffer FrameBuf
	addi	a2,a1,FRAMESIZE		// A2 <- end of frame buffer
2:	sb	zero,0(a1)		// save byte (CH_SPC is 0)
	addi	a1,a1,1			// shift pointer
	bne	a1,a2,2b		// next byte

	// reset pointers
	sb	zero,RAM_PRINTX_OFF(gp)	// set PrintX to 0
	sb	zero,RAM_PRINTY_OFF(gp)	// set PrintY to 0
	li	a1,HEIGHT-1		// A1 <- 23
	sb	a1,RAM_PRINTYMAX_OFF(gp) // set max. available row PrintYMax
	ret

// ----------------------------------------------------------------------------
// print space character (returns False on display overflow)
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintSpc
PrintSpc:
	li	a0,CH_SPC		// A0 <- space character
	//j	PrintCh

// continue to PrintCh

// ----------------------------------------------------------------------------
// print RAW character to display (returns False on display overflow)
// input: A0 ... character to print
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintCh
PrintCh:
	// disable leading spaces if character is space
	bnez	a0,2f			// skip if character is not space (CH_SPC has code 0)
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags
	ori	a1,a1,FLAG_NOLEAD	// set no-leading space flag
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// new line
2:	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- get current print X position PrintX
	lbu	a2,RAM_PRINTY_OFF(gp)	// A2 <- get current print Y position PrintY
	li	a3,WIDTH		// A3 <- max. X position
	blt	a1,a3,3f		// skip if PrintX position is OK
	li	a1,0			// A1 <- reset X position to 0
	addi	a2,a2,1			// A2 <- increase Y position

	// check valid row - error if y > PrintYMax
3:	lbu	a3,RAM_PRINTYMAX_OFF(gp) // A3 <- max. available print row PrintYMax
	bgt	a2,a3,9f		// error exit if y > PrintYMax

// here is: A0=character, A1=x, A2=y

	// write character
	la	a3,FrameBuf		// A3 <- frame buffer
#if WIDTHBYTE == 32
	slli	a4,a2,5			// A4 <- y*WIDTHBYTE
#else
	li	a4,WIDTHBYTE
	mul	a4,a4,a2		// A4 <- y*WIDTHBYTE
#endif
	add	a3,a3,a4		// A3 <- FrameBuf + y*WIDTHBYTE
	add	a3,a3,a1		// A3 <- FrameBuf + x + y*WIDTHBYTE
	sb	a0,0(a3)		// write character to frame buffer

	// increase print position
	addi	a1,a1,1			// increase X position
	sb	a1,RAM_PRINTX_OFF(gp)	// save new PrintX position
	sb	a2,RAM_PRINTY_OFF(gp)	// save new PrintY position

	// return True
	li	a0,1			// A0 <- True flag
	ret

	// error, display overflow
9:	li	a0,0			// A0 <- False flag
	ret

// ----------------------------------------------------------------------------
// print character and enable leading spaces, if it is not a space
// input: A0 ... character to print
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintChLead
PrintChLead:
	beqz	a0,PrintCh		// skip if character is space

	// enable leading spaces, if the character is not a space
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags
	andi	a1,a1,~FLAG_NOLEAD	// set leading space flag
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags
	j	PrintCh			// print character

// ----------------------------------------------------------------------------
// print tabulator
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintTab
PrintTab:
	lbu	a0,RAM_PRINTX_OFF(gp)	// A1 <- print position X PrintX
	addi	a0,a0,8			// x + 8
	andi	a0,a0,~7		// clear lower 3 bits
	//j	PrintSpcTo

// continue to PrintSpcTo

// ----------------------------------------------------------------------------
// print spaces to position
// - Continues on the next line if the position is beyond the end of the line.
// input: A0 ... required position
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintSpcTo
PrintSpcTo:
	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- print position X PrintX
	sub	a0,a0,a1		// A0 <- number of spaces to print
	//j	PrintSpcNum

// continue to PrintSpcNum

// ----------------------------------------------------------------------------
// print space characters with count
// input: A0 ... number of spaces (can be <= 0)
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintSpcNum
PrintSpcNum:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)

	// check number of spaces
	blez	a0,6f			// skip if number od spaces <= 0

	// print space
2:	sw	a0,4(sp)		// save counter
	call	PrintSpc		// print space
	beqz	a0,8f			// exit on error
	lw	a0,4(sp)		// A0 <- counter
	addi	a0,a0,-1		// decrement counter
	bnez	a0,2b			// print next space

	// pop registers
6:	li	a0,1			// A0 <- True, print OK
8:	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// print new line
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintCR
PrintCR:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// print spaces to end of line
	li	a0,WIDTH
	call	PrintSpcTo

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// shift to next row (if will not overflow)
	lbu	a1,RAM_PRINTY_OFF(gp)	// A1 <- current Y position
	lbu	a2,RAM_PRINTYMAX_OFF(gp) // A2 <- max. Y position
	bge	a1,a2,9f		// exit if already max. Y position
	addi	a1,a1,1			// increase Y position
	sb	a1,RAM_PRINTY_OFF(gp)	// save new Y position
	sb	zero,RAM_PRINTX_OFF(gp)	// reset X position
9:	ret

// ----------------------------------------------------------------------------
// clear to end of display
// ----------------------------------------------------------------------------

.global ClrEod
ClrEod:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// check PrintY row
2:	lbu	a1,RAM_PRINTY_OFF(gp)	// A1 <- PrintY position
	lbu	a2,RAM_PRINTYMAX_OFF(gp) // A2 <- PrintYMax max. position
	blt	a1,a2,4f		// print CR if PrintY < PrintYMax

	// check PrintX position
	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- PrintX position
	li	a2,WIDTH		// A2 <- max. X position
	bge	a1,a2,8f		// end if PrintX >= WIDTH

	// print CR
4:	call	PrintCR
	j	2b

	// pop registers
8:	lw	ra,0(sp)
	addi	sp,sp,4
	ret

// ----------------------------------------------------------------------------
// print token 0xD4..0xFF
// input: A0 ... character to print
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintToken
PrintToken:
	// push registers
	addi	sp,sp,-12
	sw	ra,0(sp)

	// search token	... for (; i >= 0; i--) do { ch = *t++; } while (ch < 0x80);
	la	a1,Tokens		// A1 <- token table

	addi	a0,a0,-TOKEN_FIRST	// A0 <- 'i' = 'ch' - TOKEN_FIRST (= TOKEN_QUOT 0xD4)
	bltz	a0,4f			// skip if character is < TOKEN_FIRST

2:	lb	a2,0(a1)		// A2 <- load character from token (load it signed!)
	addi	a1,a1,1			// shift pointer
	bgez	a2,2b			// loop while character is positive (not >= 0x80)

	addi	a0,a0,-1		// A0 <- decrement loop counter 'i'
	bgez	a0,2b			// loop

4:	sw	a1,4(sp)		// save pointer to token table

	// print leading space if first character is alphabetic
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- get flags
	andi	a2,a2,FLAG_NOLEAD	// check leading space flag
	bnez	a2,6f			// skip if no leading space

	lbu	a0,0(a1)		// A0 <- load first character of the token
	andi	a0,a0,0x7f		// clear bit 7
	call	IsAlpha			// check if first character is alphabetic
	beqz	a0,6f			// character is not alphabetic

	call	PrintSpc		// print space character
	beqz	a0,9f			// return on error (returns A0 = 0 False)

	// get next token character -> A0
6:	lw	a1,4(sp)		// A1 <- pointer to token table
	lbu	a0,0(a1)		// A0 <- get character
	addi	a1,a1,1			// A1 <- shift pointer
	sw	a1,4(sp)		// save new pointer
	sb	a0,8(sp)		// save character

	// print token character
	andi	a0,a0,0x7f		// clear bit 7
	call	PrintChLead		// print character
	beqz	a0,9f			// return on error (returns A0 = 0 False)

	lb	a0,8(sp)		// A0 <- character (load it signed!)
	bgez	a0,6b			// loop while character is positive (not >= 0x80)

	// if last character is alphanumeric, print space character
	andi	a0,a0,0x7f		// clear bit 7
	call	IsAlphaNumDot		// check if last character is alphanumeric
	beqz	a0,8f			// last character is alphanumeric
	call	PrintSpc		// print space character

	// pop registers
8:	li	a0,1			// A0 <- True, result is OK
9:	lw	ra,0(sp)
	addi	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// print number
// input: A0 ... number signed s32 (val)
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintNum
PrintNum:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// decode number into temporary buffer, with ending CH_QUOT character
	call	DecTempNum

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// print string or token, terminated with CH_QUOT character
	//j	PrintString

// continue to PrintString

// ----------------------------------------------------------------------------
// print string or token, terminated with CH_QUOT character
// input: A0 ... pointer to text
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global	PrintString
PrintString:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)
	mv	s0,a0			// S0 <- text pointer

	// get next character -> A0 'ch'
2:	lbu	a0,0(s0)		// A0 <- get character 'ch'
	addi	s0,s0,1			// increase text pointer

	// end of string - quote character
	addi	a1,a0,-CH_QUOT		// A1 <- 'ch' - CH_QUOT
	beqz	a1,9f			// end of string ... here A0 contains CH_QUOT = code 1 = True result

	// print token
	li	a5,0xc0			// A5 <- 0xc0 and more = tokens
	bltu	a0,a5,4f		// skip if 'ch' < 0xc0
	call	PrintToken		// print token
	j	5f

	// print simple character
4:	call	PrintCh			// print character
5:	bnez	a0,2b			// loop if result is OK

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// decode number into temporary buffer, with ending CH_QUOT character
// input: A0 ... number signed s32 (val)
// output: A0 ... pointer to start of number in temporary buffer
// ----------------------------------------------------------------------------

.global DecTempNum
DecTempNum:
	// decode positive number as unsigned
	bgez	a0,DecTempUNum		// decode as unsigned

	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// decode negate number
	neg	a0,a0
	call	DecTempUNum		// decode number

	// add sign character
	addi	a0,a0,-1		// shift destination pointer
	li	a1,CH_MINUS		// A1 <- minus character
	sb	a1,0(a0)		// store minus character

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4
	ret

// ----------------------------------------------------------------------------
// decode unsigned number into temporary buffer, with ending CH_QUOT character
// input: A0 ... number unsigned u32
// output: A0 ... pointer to start of number in temporary buffer
// ----------------------------------------------------------------------------

.global DecTempUNum
DecTempUNum:
	// save number to decode 'num'
	mv	a4,a0			// A4 <- number to decode 'num'

	// store ending character CH_QUOT
	la	a0,Temp + TEMPNUM - 1	// A0 <- pointer to end of temporary buffer 'd'
	li	a1,CH_QUOT		// A1 <- quote character
	sb	a1,0(a0)		// store ending quote character

	// prepare constants
	li	a1,10			// A1 <- multiplication 10
	li	a2,0xcccccccd           // A2 <- inverted value of number 10

// Here is:
//  A0 = destination pointer 'd'
//  A1 = 10
//  A2 = 1/10 = 0xcccccccd
//  A3 = ... divider k = num/10
//  A4 = number to decode 'num'
//  A5 = ... remainder num - k*10

	// divide number by 10
2:	mulhu	a3,a2,a4		// A3 <- num * 1/10, result HIGH
	srli	a3,a3,3			// A3 <- (num * 1/10) >> 35 = num/10 = 'k'

	// get remainder num - k*10
	mul	a5,a3,a1		// A5 <- k*10
	sub	a5,a4,a5		// A5 <- num - k*10, remainder after division

	// store digit
	addi	a0,a0,-1		// shift pointer d--
	addi	a5,a5,CH_0		// convert to digit
	sb	a5,0(a0)		// store digit

	// next digit
	mv	a4,a3			// num <- k
	bnez	a4,2b			// next digit
	ret

// ----------------------------------------------------------------------------
// print BASIC line without line number
// input: A0 ... points to program line after line number
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintBas
PrintBas:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)
	mv	s0,a0			// S0 <- pointer to program line

	// disable leading spaces
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags
	ori	a1,a1,FLAG_NOLEAD	// set no-leading space flag
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// print error flag [S]
2:	lw	a1,RAM_ERRPTR_OFF(gp)	// A1 <- pointer to error character [S]
	bne	a1,s0,3f		// skip if not error character
	li	a0,CH_S + CH_INV	// A0 <- character [S]
	call	PrintCh			// print character
	beqz	a0,9f			// exit on error

	// print [K] or [L] cursor
3:	lw	a1,RAM_CURPTR_OFF(gp)	// A1 <- pointer to cursor
	bne	a1,s0,4f		// skip if not cursor character
	lbu	a0,RAM_FLAGS_OFF(gp)	// A0 <- get flags
	andi	a0,a0,FLAG_KMODE	// K mode?
	seqz	a0,a0			// A0 <- 1 if L mode (flag was clear), 0 if K mode (flag was set)
	addi	a0,a0,CH_K + CH_INV	// A0 <- character [K] or [L]
	call	PrintCh			// print character
	beqz	a0,9f			// exit on error

	// load one character -> A0
4:	lbu	a0,0(s0)		// A0 <- load character
	addi	s0,s0,1			// increase pointer

	// new line - only if PrintX < WIDTH
	li	a1,NEWLINE		// A1 <- new line character
	bne	a0,a1,5f		// not new line character
	lbu	a1,RAM_PRINTX_OFF(gp)	// A1 <- print X position PrintX
	li	a2,WIDTH		// A2 <- maximal X position
	li	a0,1			// A0 <- True
	bgeu	a1,a2,9f		// exit if PrintX >= WIDTH
	call	PrintCR			// print new line
	j	9f

	// print token
5:	li	a1,0xc0			// A1 <- token if >= 0xc0
	bltu	a0,a1,6f		// skip if not token
	call	PrintToken		// print token
	beqz	a0,9f			// edit on error
	j	2b			// loop

	// print single character
6:	call	PrintChLead		// print character
	bnez	a0,2b			// loop if OK

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// print BASIC line with line number
// input: A0 ... points to program line
// output: A0 ... 0=display overflow, 1=print OK
// ----------------------------------------------------------------------------

.global PrintBasNum
PrintBasNum:
	// push registers
	addi	sp,sp,-12
	sw	ra,0(sp)

	// check end of program (if line number is 0)
	lbu	a1,0(a0)		// A1 <- line number LOW
	lbu	a2,1(a0)		// A2 <- line number HIGH
	addi	a0,a0,2			// A0 <- shift pointer
	sw	a0,4(sp)		// save pointer 'p'
	slli	a2,a2,8			// A2 <- line number HIGH * 256
	or	a1,a1,a2		// A1 <- line number
	li	a0,1			// A0 <- True
	beqz	a1,9f			// exit if line number = 0 (= end of program)
	sw	a1,8(sp)		// save line number 'line'

// Here is: 4(sp) pointer 'p', 8(sp) line number 'line'

	// decode line number (-> A1 start of text)
	mv	a0,a1			// A0 <- line
	call	DecTempUNum		// decode number to temporary buffer, with ending CH_QUOT character

	// align with spaces to 9999"
	la	a1,Temp + TEMPNUM - 5	// required minimal address to ensure 9999"
	bleu	a0,a1,3f		// skip if 'd' <= &Temp[TEMPNUM-5]
2:	addi	a0,a0,-1		// decrease pointer 'd'
	sb	zero,0(a0)		// store space character (CH_SPC has code 0)
	bgtu	a0,a1,2b		// loop while 'd' > &Temp[TEMPNUM-5]

	// print number (pointer is in A0)
3:	call	PrintString		// print string
	beqz	a0,9f			// exit on error

	// print space or cursor
	lhu	a1,RAM_CURLINE_OFF(gp)	// A1 <- current line CurLine
	lw	a2,8(sp)		// A2 <- this line 'line'
	li	a0,CH_SPC		// A0 <- space character
	bne	a1,a2,4f		// skip if this line is not current line
	li	a0,CH_GT + CH_INV	// A0 <- character '>'
4:	call	PrintCh			// print separator
	beqz	a0,9f			// exit on error

	// print program line
	lw	a0,4(sp)		// A0 <- pointer to program line
	call	PrintBas		// print program line

	// pop registers
9:	lw	ra,0(sp)
	addi	sp,sp,12
	ret

// ----------------------------------------------------------------------------
// PRINT command
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global CmdPrint
CmdPrint:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// get flag of checking syntax: S0 = 0 only checking syntax, 1 runtime
	lbu	s0,RAM_FLAGS_OFF(gp)	// S0 <- get flags Flags
	andi	s0,s0,FLAG_SYNTOFF	// check if syntax mode

	// get next character -> A0
1:	call	GetChar			// get next character

	// end line
	li	a1,NEWLINE
	bne	a0,a1,3f		// skip if not new line
	beqz	s0,9f			// exit if only checking syntax

	// print new line
	call	PrintCR			// print CR
	beqz	a0,8f			// exit on display error
	j	9f

	// separator ',', print tabulator
3:	li	a1,TOKEN_COMMA		// ','
	bne	a0,a1,4f		// skip if not comma separator
	call	CharAdd			// skip comma separator

	// print tabulator only if not checking syntax
	beqz	s0,32f			// continue loop if only checking syntax
	call	PrintTab		// print tabulator
	beqz	a0,8f			// display error

	// if "," is last character on the line, no CR will be printed
32:	call	GetChar			// get next character
	j	42f

	// separator ';', do nothing
4:	li	a1,TOKEN_SEMI		// ';'
	bne	a0,a1,5f		// skip if not semicolon separator
	call	NextChar		// skip separator

	// if ";" is last character on the line, no CR will be printed
42:	li	a1,NEWLINE
	beq	a0,a1,9f		// exit on NEWLINE
	j	1b			// continue loop

	// scanning calculator expression, store result to LastResult
5:	call	ScanCalc		// scanning expression

	// break on error
	lbu	a2,RAM_ERRNR_OFF(gp)	// A2 <- get error ErrNr
	bnez	a2,9f			// exit on error

	// print item - only if not checking syntax
	beqz	s0,1b			// continue loop if only checking syntax

	// get last result
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- last result LastResult

	// check if numeric result
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- get flags Flags
	andi	a1,a1,FLAG_NUMRES	// check numeric result
	bnez	a1,6f			// skip if numeric result

	// print string
#if VALSIZE == 16
	add	a0,a0,gp		// A0 <- pointer to the string
#endif
	call	PrintString		// print string
	j	7f

	// print number
6:	call	PrintNum
7:	bnez	a0,1b			// continue loop if result is OK

	// display error
8:	call	DispError		// raise display error

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

#endif // USE_PRINT_ASM
