
// ****************************************************************************
//
//                                Variables
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_VAR_ASM			// 1=use ASM in Var module

// ----------------------------------------------------------------------------
// CLEAR variables
// ----------------------------------------------------------------------------

.global CmdClear
CmdClear:
	li	a1,1
	sh	a1,RAM_VARSNUM_OFF(gp)	// clear size of variables
	li	a1,VAR_END
	sb	a1,RAM_VARS_OFF(gp)	// set stop mark
	ret

// ----------------------------------------------------------------------------
// move to next variable
// input: A0 ... pointer to variable
// output: A0 ... next variable (VAR_END = end)
// ----------------------------------------------------------------------------
//  00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
//  01aaaaaa name valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR, variable 'name'
//  10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
//  11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
//  0xC0 ... end of variables

.global NextVar
NextVar:
	// get variable type and length
	lbu	a5,0(a0)		// A5 <- get variable type and length
	andi	a4,a5,VAR_MASK		// A4 <- length of the name
	srli	a5,a5,VAR_SHIFT		// A5 <- type of variable
	add	a0,a0,a4		// skip name, but not type (on end: length is 0)

	// simple integer variable
#if VARTYPE_NUM == 0
	bnez	a5,2f			// skip if not integer
#else
	li	a4,VARTYPE_NUM
	bne	a5,a4,2f		// skip if not integer
#endif

#if VALSIZE == 32
	addi	a0,a0,5
#else
	addi	a0,a0,3
#endif
	ret

	// FOR loop
2:	li	a4,VARTYPE_FOR
	bne	a5,a4,3f		// skip if not FOR
#if VALSIZE == 32
	addi	a0,a0,11
#else
	addi	a0,a0,7
#endif
	ret

	// array
3:	li	a4,VARTYPE_ARRAY
	bne	a5,a4,4f		// skip if not array
	lbu	a5,1(a0)		// A5 <- max. index
#if VALSIZE == 32
	slli	a5,a5,2			// A5 <- max * 4
	addi	a5,a5,6
#else
	slli	a5,a5,1			// A5 <- max * 2
	addi	a5,a5,4
#endif
	add	a0,a0,a5
	ret

	// string
4:	li	a4,VARTYPE_STR
	bne	a5,a4,8f		// skip if not string
	addi	a0,a0,1			// skip type
	li	a5,CH_QUOT		// A5 <- stop character
5:	lbu	a4,0(a0)		// load next character
	addi	a0,a0,1			// increase pointer
	bne	a4,a5,5b		// search CH_QUOT terminator

	// otherwise VAR_END = end of variables
8:	ret

// ----------------------------------------------------------------------------
// DIM command - create integer array; does not check if array exists
// input: LastResult = max. subscript index
//        VarDest points to array name
// ----------------------------------------------------------------------------
//  10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'

.global CmdDim
CmdDim:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// parse length of the name -> A3
	lw	a0,RAM_VARDEST_OFF(gp)	// A0 <- pointer to variable name VarDest
	call	ParseVarName		// parse name
	andi	a3,a0,VAR_MASK		// mask length of the name -> A3

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// get array max. index -> A1 'max'
	lw	a1,RAM_LASTRESULT_OFF(gp) // A1 <- get LastResult - max. subscript index 'max'

	// check range 0..255, subscript error
	li	a5,255			// A5 <- max. index
	bleu	a1,a5,2f		// skip if index is OK
	j	SubsError		// subscript error

// Here is: A1 = max. index 'max', A3 = length of the name 'len'

	// prepare required size -> A2 'size'
#if VALSIZE == 32
2:	slli	a2,a1,2			// A2 <- max*4
	addi	a2,a2,6			// A2 <- max*4 + 6
#else
2:	slli	a2,a1,1			// A2 <- max*2
	addi	a2,a2,4			// A2 <- max*2 + 4
#endif
	add	a2,a2,a3		// A2 <- add length of the name

// Here is: A1 = max. index 'max', A2 = array size 'size', A3 = length of the name 'len'

	// get old size of variables -> A5 'num', new size -> A4 'num2' = num + size
	lh	a5,RAM_VARSNUM_OFF(gp)	// A5 <- old size of variables 'num' VarsNum
	add	a4,a5,a2		// A4 <- new size of variables 'num2' = num + size

	// check free memory
	li	a0,VARSNUM		// A0 <- variable buffer size
	ble	a4,a0,3f		// skip if new size is OK
	j	MemError		// memory error

	// set new size of variables
3:	sh	a4,RAM_VARSNUM_OFF(gp)	// set new size of variables

// Here is: A1 = max. index 'max', A2 = array size 'size', A3 = length of the name 'len',
//          A5 = old size of variables 'num'

	// prepare pointer to variables (pointer to old terminator VAR_END) -> A4
	add	a4,gp,RAM_VARS_OFF-1	// A4 <- pointer to variables - 1
	add	a4,a4,a5		// A4 <- pointer to old terminator VAR_END: &Vars[num - 1] = 'd'

// Here is: A1 = max. index 'max', A2 = array size 'size', A3 = length of the name 'len',
//          A4 = pointer to variable 'd'

	// set variable type and length
	addi	a0,a3,VARTYPE_ARRAY << VAR_SHIFT // A0 <- variable type and length
	sb	a0,0(a4)		// store variable type and length
	addi	a4,a4,1			// increase destination pointer 'd'

	// prepare size of data -> A2
	addi	a2,a2,-2		// size without header
	sub	a2,a2,a3		// size without name

	// transfer name
	lw	a5,RAM_VARDEST_OFF(gp)	// A5 <- pointer to variable name 's'
1:	lbu	a0,0(a5)		// load character
	addi	a5,a5,1			// shift source pointer 's'
	sb	a0,0(a4)		// store character
	addi	a4,a4,1			// shift destination pointer 'd'
	addi	a3,a3,-1		// shift length counter
	bnez	a3,1b			// loop

	// set array max. index
	sb	a1,0(a4)		// set array max. index

	// clear array data
4:	sb	zero,1(a4)		// clear byte
	addi	a4,a4,1			// shift pointer
	addi	a2,a2,-1		// shift size counter
	bnez	a2,4b			// clear loop

	// terminator
	li	a2,VAR_END
	sb	a2,1(a4)		// set end terminator VAR_END
	ret

// ----------------------------------------------------------------------------
// Store string variable, terminated with quote; does not check if variable exists
// input: A0 ... pointer 'p' to string immediately after first quote "
//        VarDest points to string name.
// ----------------------------------------------------------------------------
// 11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'

.global SetStr
SetStr:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	a0,4(sp)		// pointer 'p' to string

	// parse length of the name -> A3
	lw	a0,RAM_VARDEST_OFF(gp)	// A0 <- pointer to variable name VarDest
	call	ParseVarName		// parse name
	andi	a3,a0,VAR_MASK		// mask length of the name -> A3

	// pop registers
	lw	a0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8

// Here is: A0 = source pointer 'p', A3 = name length 'len'

	// prepare string size (including quote character and name character) -> A1 'size', A2 'size+1+len'
	mv	a4,a0			// A4 <- pointer to the text 'p2'
	li	a5,CH_QUOT		// A5 <- terminating character
2:	lbu	a1,0(a4)		// A1 <- load character 'ch'
	addi	a4,a4,1			// increase pointer 'p2'
	bne	a1,a5,2b		// search end of the string
	sub	a1,a4,a0		// A1 <- size of the string 'size' = 'p2' - 'p'
	addi	a2,a1,1			// A2 <- size of the string including type 'size+1'
	add	a2,a2,a3		// A2 <- size of the string including type and name 'size+1+len'

// Here is: A0 = source pointer 'p', A1 = string size 'size', A2 = string size incl. type and name 'size+1+len',
//          A3 = name length 'len'

	// get old size of variables -> A5 'num', new size -> A4 'num2' = num + size+1+len
	lhu	a5,RAM_VARSNUM_OFF(gp)	// A5 <- old size of variables 'num' VarsNum
	add	a4,a5,a2		// A4 <- new size of variables 'num2' = num + size+1+len

	// check free memory
	li	t0,VARSNUM		// T0 <- variable buffer size
	ble	a4,t0,3f		// skip if new size is OK
	j	MemError		// memory error

	// set new size of variables
3:	sh	a4,RAM_VARSNUM_OFF(gp)	// set new size of variables

// Here is: A0 = source pointer 'p', A1 = string size 'size', A3 = name length 'len', A5 = old size of variables 'num'

	// prepare pointer to variables (pointer to old terminator VAR_END) -> A4 'd'
	add	a4,gp,RAM_VARS_OFF-1	// A4 <- pointer to variables - 1
	add	a4,a4,a5		// A4 <- pointer to old terminator VAR_END: &Vars[num - 1] = 'd'

// Here is: A0 = source pointer 'p', A1 = string size 'size', A3 = name length 'len', A4 = pointer to variable 'd'

	// set variable type and length
	addi	a5,a3,VARTYPE_STR << VAR_SHIFT // A5 <- variable type and length
	sb	a5,0(a4)		// store variable type and length
	addi	a4,a4,1			// increase destination pointer 'd'

	// transfer name
	lw	a2,RAM_VARDEST_OFF(gp)	// A2 <- pointer to variable name 's'
1:	lbu	a5,0(a2)		// A5 <- load character
	addi	a2,a2,1			// shift source pointer 's'
	sb	a5,0(a4)		// store character
	addi	a4,a4,1			// shift destination pointer 'd'
	addi	a3,a3,-1		// shift length counter
	bnez	a3,1b			// loop

	// copy string (including terminating CH_QUOT)
5:	lbu	a5,0(a0)		// A5 <- load byte
	addi	a0,a0,1			// increment source pointer 'p'
	sb	a5,0(a4)		// write byte
	addi	a4,a4,1			// increment destination pointer 'd'
	addi	a1,a1,-1		// decrease size counter
	bnez	a1,5b			// next byte

	// terminator
	li	a2,VAR_END
	sb	a2,0(a4)		// set end terminator VAR_END
	ret

// ----------------------------------------------------------------------------
// STR$ function - convert number to string (to Edit buffer)
//  Input 'num' and output 'str' to LastResult.
// ----------------------------------------------------------------------------

.global FncStr
FncStr:
	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get input number -> A0
	lw	a0,RAM_LASTRESULT_OFF(gp)

	// decode number to temporary buffer -> A0 = pointer to start of number 'p'
	call	DecTempNum

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// get length of the text -> A1 'len'
	la	a1,Temp + TEMPNUM	// A1 <- end of temporary buffer
	sub	a1,a1,a0		// A1 <- length of the text 'len'

	// check free space in edit buffer, A2 = old size of data 'n'
	lhu	a2,RAM_EDITNUM_OFF(gp)	// A2 <- size of data in edit buffer 'n'
	add	a3,a2,a1		// A3 <- new size of data in edit buffer 'n'+'len'
	li	a4,EDITNUM		// A4 <- size of edit buffer
	ble	a3,a4,2f		// skip if size of data is OK

// Here is: A0 = pointer to text 'p', A1 = length of text 'len', A2 = old size of data 'n', A3 = new size of data 'n'+'len'

	// memory error
	addi	a4,gp,RAM_NULSTRING_OFF	// A4 <- pointer to NulString
#if VALSIZE == 16
	sub	a4,a4,gp
#endif
	sw	a4,RAM_LASTRESULT_OFF(gp) // set LastResult to NulString
	j	MemError		// memory error

	// set new size of data in edit buffer
2:	sh	a3,RAM_EDITNUM_OFF(gp)	// set new size of data EditNum = n + len

	// save last result
	la	a3,Edit			// A3 <- edit buffer
	add	a3,a3,a2		// A3 <- pointer to end of edit buffer 'd'
#if VALSIZE == 32
	sw	a3,RAM_LASTRESULT_OFF(gp) // set LastResult
#else
	sub	a4,a3,gp
	sw	a4,RAM_LASTRESULT_OFF(gp) // set LastResult
#endif

// Here is: A0 = pointer to text 'p', A1 = length of text 'len', A3 = destination buffer 'd'

	// copy number
4:	lbu	a2,0(a0)		// read byte
	addi	a0,a0,1			// increment source pointer
	sb	a2,0(a3)		// write byte
	addi	a3,a3,1			// increment destination pointer
	addi	a1,a1,-1		// decrement length counter
	bnez	a1,4b			// next byte
	ret

// ----------------------------------------------------------------------------
// INKEY$ function - get key from keyboard (in Edit buffer, save pointer to LastResult)
// ----------------------------------------------------------------------------

.global FncInkey
FncInkey:
	// set L-mode
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- load flags
	andi	a1,a1,~FLAG_KMODE	// reset K-mode
	sb	a1,RAM_FLAGS_OFF(gp)	// set new flags

	// push registers
	addi	sp,sp,-4
	sw	ra,0(sp)

	// get key -> A0
	call	CharGet			// get key with remapping

	// pop registers
	lw	ra,0(sp)
	addi	sp,sp,4

	// no key
	li	a1,CH_NONE		// A1 <- no key
	bne	a0,a1,FncChr2		// skip if key is valid - convert key to string

	// use nul string
#if VALSIZE == 32
	addi	a4,gp,RAM_NULSTRING_OFF	// A4 <- pointer to NulString
#else
	li	a4,RAM_NULSTRING_OFF	// A4 <- pointer to NulString, relative
#endif
	sw	a4,RAM_LASTRESULT_OFF(gp) // set LastResult to NulString
	ret

// continue to FncChr

// ----------------------------------------------------------------------------
// CHR$ function - convert character code to 1-character string (in Edit buffer)
//  Input 'num' and output 'str' to LastResult.
// ----------------------------------------------------------------------------

.global FncChr
FncChr:
	// get input number -> A0
	lw	a0,RAM_LASTRESULT_OFF(gp)

FncChr2:
	// use nul string in case of error
#if VALSIZE == 32
	addi	a4,gp,RAM_NULSTRING_OFF	// A4 <- pointer to NulString
#else
	li	a4,RAM_NULSTRING_OFF	// A4 <- pointer to NulString, relative
#endif
	sw	a4,RAM_LASTRESULT_OFF(gp) // set LastResult to NulString

	// check valid range
	li	a5,255			// max. valid value
	bleu	a0,a5,2f		// skip if value is OK
	j	Overflow		// overflow error

	// check free space in edit buffer
2:	lhu	a2,RAM_EDITNUM_OFF(gp)	// A2 <- size of data in edit buffer 'n'
	addi	a3,a2,2			// A3 <- new size of data in edit buffer 'n'+2
	li	a4,EDITNUM		// A4 <- size of edit buffer
	ble	a3,a4,3f		// skip if size of data is OK
	j	MemError		// memory error

// Here is: A0 = number 'num', A2 = old size of data 'n', A3 = new size of data 'n'+2

	// set new size of data in edit buffer
3:	sh	a3,RAM_EDITNUM_OFF(gp)	// set new size of data EditNum = n + 2

	// save number as text
	la	a3,Edit			// A3 <- edit buffer
	add	a3,a3,a2		// A3 <- pointer to end of edit buffer 'p' = &Edit[n]
	sb	a0,0(a3)		// save number
	li	a4,CH_QUOT
	sb	a4,1(a3)		// save terminator

	// save last result
#if VALSIZE == 16
	sub	a3,a3,gp
#endif
	sw	a3,RAM_LASTRESULT_OFF(gp) // set LastResult
	ret

// ----------------------------------------------------------------------------
// TL$ function - returns tail of string, without 1st character
//  Input 'str' and output 'str' to LastResult.
// ----------------------------------------------------------------------------

.global FncStrTl
FncStrTl:
	// get input string -> A0
	lw	a0,RAM_LASTRESULT_OFF(gp)

	// convert address to pointer 'p'
#if VALSIZE == 16
	add	a0,a0,gp
#endif

	// shift pointer - only is string is not empty
	lbu	a1,0(a0)		// A1 <- load first character
	li	a2,CH_QUOT		// A2 <- terminator character
	beq	a1,a2,2f		// skip if string is empty
	addi	a0,a0,1			// shift pointer

	// save last result
2:
#if VALSIZE == 16
	sub	a0,a0,gp
#endif
	sw	a0,RAM_LASTRESULT_OFF(gp) // set LastResult
FncStrTl_Ret:
	ret

// ----------------------------------------------------------------------------
// LET command (VarDest = variable name, LastResult = last result)
// ----------------------------------------------------------------------------

.global CmdLet
CmdLet:
	// return if error has been encountered
	lbu	a4,RAM_ERRNR_OFF(gp)	// A4 <- get error number
	bnez	a4,FncStrTl_Ret		// exit on error

	// push registers
	addi	sp,sp,-20
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	sw	tp,12(sp)

// Locals: S0 = init value 'n', S1 = variable name type and length 'flag', TP = name of variable 'v',
//         16(sp) = variable 'var'

	// get pointer to name of variable -> TP 'v'
	lw	tp,RAM_VARDEST_OFF(gp)	// TP <- pointer to name of variable 'v'

	// load init value -> S0 'n'
	lw	s0,RAM_LASTRESULT_OFF(gp) // S0 <- init value from last result 'n'

	// parse variable name -> S1 'flag
	mv	a0,tp			// A0 <- pointer to variable name 'v'
	call	ParseVarName		// parse variable name
	mv	s1,a0			// S1 <- name type and length 'flag'

	// invalid name
	li	a5,VAR_END		// A5 <- end mark
	bne	a0,a5,1f		// skip if not end

CmdLet_SyntaxError:

	call	SyntaxError		// syntax error
	j	CmdLet_9

	// invalid type 'name$('
1:	srli	a0,s1,VAR_SHIFT		// A0 <- variable type
	li	a5,VARTYPE_FOR
	beq	a0,a5,CmdLet_SyntaxError // 'name$(' is invalid

	// try to find function - check reserved name
	mv	a0,tp			// A0 <- name of variable 'v'
	mv	a1,s1			// A1 <- flag
	call	ParseFnc		// check function
	bgez	a0,CmdLet_SyntaxError	// error, function is not valid at this place

	// find variable (NULL if not found) (can raise errors ERR_SUBSCRIPT or ERR_VARNFND)
	// returns pointer to data or NULL if not found
	mv	a0,tp			// A0 <- pointer to name of the variable 'v'
	addi	a1,sp,16		// A1 <- to get pointer to variable 'var'
	call	GetVar			// find variable -> A0 'p', 16(sp) 'var'

	// subscript error ("Variable not found" is OK)
	lbu	a4,RAM_ERRNR_OFF(gp)	// A4 <- get error number
	li	a3,ERR_SUBSCRIPT
	beq	a3,a4,CmdLet_9		// exit on subscript error

	// get name type -> A3 and name length -> A4
	andi	a4,s1,VAR_MASK		// A4 <- name length 'len'
	srli	a3,s1,VAR_SHIFT		// A3 <- variable type 'type'

// Locals: S0 = init value 'n', S1 = variable name type and length 'flag', TP = name of variable 'v'
//         A0 = pointer to variable data 'p', A3 = variable type 'type', A4 = name length 'len',
//         16(sp) = variable 'var'

	// variable not found (error was ERR_VARNFND)
	bnez	a0,CmdLet_Exist		// skip if variable has been found

// --- variable not found (error ERR_VARNFND)

	// Clear error "Variable not found"
	sb	zero,RAM_ERRNR_OFF(gp)	// clear error ErrNr
	sw	zero,RAM_ERRPTR_OFF(gp)	// clear error pointer ErrPtr

	// array 'name('
	li	a5,VARTYPE_ARRAY	// A2 <- token '('
	bne	a3,a5,2f		// skip if not array

	// error, variable not found (array must exist)
	call	VarNotFound
	j	CmdLet_9

	// string 'name$'
2:	li	a5,VARTYPE_STR
	bne	a3,a5,3f		// skip if not string

	// store new string to end of variables
	mv	a0,s0			// A0 <- init value
#if VALSIZE == 16
	add	a0,a0,gp		// convert address to pointer
#endif
	call	SetStr			// set string
	j	CmdLet_9

	// simple integer number - create new numeric variable
// 00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
3:	lhu	a2,RAM_VARSNUM_OFF(gp)	// A2 <- size of variables VarsNum 'num'
#if VALSIZE == 32
	addi	a3,a2,5			// new size of variables -> A3
#else
	addi	a3,a2,3			// new size of variables -> A3
#endif
	add	a3,a3,a4		// add name length, size = num + len + 5(3)

	// check memory
	li	a5,VARSNUM		// size of variable buffer
	ble	a3,a5,4f		// skip if size is OK
	call	MemError		// memory error
	j	CmdLet_9

	// set new size of variables
4:	sh	a3,RAM_VARSNUM_OFF(gp)	// set new size VarsNum

	// pointer to new variable -> A3 'd'
	add	a3,gp,RAM_VARS_OFF-1	// A3 <- pointer to variables - 1
	add	a3,a3,a2		// A3 <- pointer to old terminator VAR_END: &Vars[num - 1] = 'd'

	// initialize variable
	sb	s1,0(a3)		// save variable type and length 'flag'

5:	lbu	a0,0(tp)		// A0 <- load character of name 'v'
	addi	tp,tp,1			// shift source pointer 'v'
	sb	a0,1(a3)		// write to destination 'd'
	addi	a3,a3,1			// shift destination 'd'
	addi	a4,a4,-1		// decrease length counter
	bnez	a4,5b			// loop next character

	sb	s0,1(a3)		// set byte 1
	srai	a0,s0,8			// A0 <- byte 2
	sb	a0,2(a3)		// set byte 2
	li	a1,VAR_END
#if VALSIZE == 32
	srai	a0,s0,16		// A0 <- byte 3
	sb	a0,3(a3)		// set byte 3
	srai	a0,s0,24		// A0 <- byte 4
	sb	a0,4(a3)		// set byte 4
	sb	a1,5(a3)		// terminator
#else
	sb	a1,3(a3)		// terminator
#endif
	j	CmdLet_9

// --- variable already exists

CmdLet_Exist:

// Locals: S0 = init value 'n', S1 = variable name type and length 'flag', TP = name of variable 'v'
//         A0 = pointer to variable data 'p', A3 = variable type 'type', A4 = name length 'len',
//         16(sp) = variable 'var'

	// string 'name$'
	li	a2,VARTYPE_STR
	bne	a3,a2,5f		// skip if not string

	// store new string to end of variables
	mv	a0,s0			// A0 <- init value 'n'
#if VALSIZE == 16
	add	a0,a0,gp		// convert address to pointer
#endif
	call	SetStr			// set string
	lbu	a4,RAM_ERRNR_OFF(gp)	// A4 <- get error number
	bnez	a4,CmdLet_9		// exit on error

	// find next variable (to get length of this variable) -> A0 = 'var2'
	lw	a0,16(sp)		// A0 <- pointer to variable 'var'
	call	NextVar			// get next variable -> A0 = 'var2'

	// delete old string
	lhu	a4,RAM_VARSNUM_OFF(gp)	// A4 <- size of variables VarsNum
	addi	a5,gp,RAM_VARS_OFF	// A5 <- pointer to variables 'Vars'
	sub	a5,a0,a5		// A5 <- offset of next variable 'var2' - Vars
	sub	a5,a4,a5		// A5 <- remaining data after old variable 'i' = VarsNum - (var2 - Vars)
	lw	a3,16(sp)		// A3 <- pointer to variable 'var'
	sub	a2,a0,a3		// A2 <- length of old variable = 'var2' - 'var'
	sub	a4,a4,a2		// A4 <- new size of variables
	sh	a4,RAM_VARSNUM_OFF(gp)	// set new size of varaibles VarsNum

1:	lbu	a2,0(a0)		// A2 <- load byte from 'var2'
	addi	a0,a0,1			// shift source pointer 'var2'
	sb	a2,0(a3)		// save byte to 'var'
	addi	a3,a3,1			// shift destination pointer 'var'
	addi	a5,a5,-1		// decrement loop counter 'i'
	bnez	a5,1b			// next byte
	j	CmdLet_9

// Locals: S0 = init value 'n', A0 = pointer to variable data 'p'

	// numeric variable (or array) - store result
5:	sb	s0,0(a0)		// save byte 1
	srai	a1,s0,8			// A1 <- byte 2
	sb	a1,1(a0)		// save byte 2
#if VALSIZE == 32
	srai	a1,s0,16		// A1 <- byte 3
	sb	a1,2(a0)		// save byte 3
	srai	a1,s0,24		// A1 <- byte 4
	sb	a1,3(a0)		// save byte 4
#endif

CmdLet_9:

	// pop registers
9:	lw	tp,12(sp)
	lw	s1,8(sp)
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,20
	ret

// ----------------------------------------------------------------------------
// find variable and load its value
// input: A0 ... pointer to name of variable
//        A1 ... pointer to get address of variable (NULL = not required)
// output: A0 ... pointer to data or NULL if not found
// - Can raise errors ERR_SUBSCRIPT or ERR_VARNFND
// ----------------------------------------------------------------------------

.global GetVar
GetVar:
	// push registers
	addi	sp,sp,-28
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	sw	tp,12(sp)

// Locals:
//  16(sp) = pointer to name of variable 'p'
//  20(sp) = pointer to variable 'var', save current text address 'psave'
//  24(sp) = (u8) name length 'len'
//  25(sp) = (u8) name type 'type'
//  S0 = 'flag'
//  S1 = pointer to variables 'v'
//  TP = 'flag2'

	// clear address of variable
	beqz	a1,1f			// skip if pointer to address of variable is not valid (= NULL)
	sw	zero,0(a1)		// invalidate address

	// save pointers
1:	sw	a0,16(sp)		// save pointer to name 'p'
	sw	a1,20(sp)		// save pointer to address of variable 'var'

	// parse variable name -> A0,S0 'flag', A4 'len, A3 'type'
	call	ParseVarName		// parse variable name
	mv	s0,a0			// S0 <- flag
	andi	a4,s0,VAR_MASK		// A4 <- name length 'len'
	sb	a4,24(sp)		// save name length 'len'
	srli	a3,s0,VAR_SHIFT		// A3 <- variable type 'type'
	sb	a3,25(sp)		// save variable type 'type'

	// reject string function name$(), or not valid name
	li	a5,VARTYPE_FOR		// A5 <- string function
	beq	a3,a5,GetVar_NotFound	// exit in case of string function name$()
	li	a5,VAR_END		// A5 <- invalid name
	beq	s0,a5,GetVar_NotFound	// exit in case of invalid name

	// alternative flag INT -> FOR
	mv	tp,s0			// TP <- flag2
#if VARTYPE_NUM == 0
	bnez	a3,1f			// skip if not integer variable
#else
	li	a5,VARTYPE_NUM		// A5 <- integer variable
	bne	a3,a5,1f		// skip if not integer variable
#endif
	addi	tp,a4,VARTYPE_FOR << VAR_SHIFT // TP <- alternative flag FOR

// --- find variable

	// find variable
1:	addi	s1,gp,RAM_VARS_OFF	// S1 <- pointer to variables Vars 'v'

	// end of variables
2:	lbu	a3,0(s1)		// A3 <- get type of variable
	li	a5,VAR_END		// A5 <- terminator character
	beq	a3,a5,GetVar_NotFound	// variable not found

	// check variable type
	beq	a3,s0,3f		// 'flag' equal
	bne	a3,tp,4f		// skip if 'flag2' not equal

	// compare name
3:	addi	a0,s1,1			// A0 <- variable name 'v'+1
	lw	a1,16(sp)		// A1 <- pointer to name 'p'
	lbu	a2,24(sp)		// A2 <- length of name 'len'
	call	memcmp			// compare name
	beqz	a0,5f			// break if name equals

	// next variable
4:	mv	a0,s1			// A0 <- pointer to variables 'v'
	call	NextVar			// go to next variable
	mv	s1,a0			// S1 <- pointer 'v'
	j	2b			// continue searching

	// save pointer to variable
5:	lw	a1,20(sp)		// A1 <- pointer to address of variable 'var'
	beqz	a1,6f			// skip if pointer is not valid
	sw	s1,0(a1)		// save address of variable

	// skip type and name
6:	lbu	a2,24(sp)		// A2 <- length of name 'len'
	addi	a2,a2,1			// A2 <- length of name 'len' + 1
	add	s1,s1,a2		// skip name and type 'v'

// --- array
// here is A2 = 'len' + 1

	// array
// 10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
	li	a5,VARTYPE_ARRAY
	lbu	a4,25(sp)		// A4 <- variable type 'type'
	bne	a4,a5,7f		// skip if not array

	// save current text address
	lw	a3,RAM_CHARPTR_OFF(gp)	// A3 <- current character pointer
	sw	a3,20(sp)		// save character pointer 'psave'

	// evaluate expression (store result to LastResult)
	lw	a0,16(sp)		// A0 <- pointer to text 'p'
	add	a0,a0,a2		// skip name and '('
	call	EvalExp			// evaluate expression

	// restore current text address
	lw	a3,20(sp)		// A3 <- get old character pointer
	sw	a3,RAM_CHARPTR_OFF(gp)	// restore character pointer

	// error, check max. index
	lbu	a3,RAM_ERRNR_OFF(gp)	// A3 <- get error number ErrNr
	bnez	a3,GetVar_SubsErr	// error
	lw	a3,RAM_LASTRESULT_OFF(gp) // A3 <- get last result 'num'
	lbu	a2,0(s1)		// A2 <- get max index 'max'
	bleu	a3,a2,2f		// skip if index is OK
GetVar_SubsErr:
	li	a3,ERR_SUBSCRIPT	// A3 <- subscript error
	sb	a3,RAM_ERRNR_OFF(gp)	// set error code (overwrites current error)
	j	GetVar_Null		// error exit

	// rewind to the entry
#if VALSIZE == 32
2:	slli	a3,a3,2			// A3 <- offset num*4
	addi	a3,a3,1			// A3 <- offset num*4 + 1
	add	s1,s1,a3		// S1 <- pointer to data of variable
#else
2:	slli	a3,a3,1			// A3 <- offset num*2
	addi	a3,a3,1			// A3 <- offset num*2 + 1
	add	s1,s1,a3		// S1 <- pointer to data of variable
#endif
	j	GetVar_LoadInt		// load number

// --- string

	// string
// 11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
7:	li	a5,VARTYPE_STR
	bne	a4,a5,8f		// skip if not string

	// save last result
#if VALSIZE == 32
	mv	a0,s1			// A0 <- variable address
#else
	sub	a0,s1,gp
#endif
	j	GetVar_Result

// --- simple numeric variable or FOR

8:
GetVar_LoadInt:

	// get value
	lbu	a0,0(s1)		// A0 <- byte 1

	lbu	a1,1(s1)		// A1 <- byte 2
	slli	a1,a1,8			// A1 <- byte 2 << 8
	or	a0,a0,a1		// A0 <- byte 2 to result

#if VALSIZE == 32
	lbu	a1,2(s1)		// A1 <- byte 2
	slli	a1,a1,16		// A1 <- byte 2 << 16
	or	a0,a0,a1		// A0 <- byte 2 to result

	lbu	a1,3(s1)		// A1 <- byte 2
	slli	a1,a1,24		// A1 <- byte 2 << 24
	or	a0,a0,a1		// A0 <- byte 2 to result
#endif

	// store result
GetVar_Result:
	sw	a0,RAM_LASTRESULT_OFF(gp) // save last result
	j	GetVar_9

	// error - variable not found
GetVar_NotFound:
	call	VarNotFound		// error - variable not found
GetVar_Null:
	li	s1,0			// S1 <- return address NULL
GetVar_9:
	// pop registers
	mv	a0,s1			// A0 <- pointer to variable
	lw	tp,12(sp)
	lw	s1,8(sp)
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,28
	ret

// ----------------------------------------------------------------------------
// check LookVars state
// input: A0 req ... require numeric or string result - use FLAG_NUMRES or 0
//        A1 set ... set numeric or string result - use FLAG_NUMRES or 0
// output: A0 ... 1 = exit, 0 = continue
// ----------------------------------------------------------------------------

.global CheckLookVars
CheckLookVars:
	// get current flags -> A1
	lbu	a2,RAM_FLAGS_OFF(gp)	// A2 <- flags

	// check result type
	andi	a3,a2,FLAG_NUMRES	// check numeric flag
	beq	a0,a3,3f		// result type is OK

	// set syntax error
	addi	sp,sp,-4
	sw	ra,0(sp)
	call	SyntaxError		// syntax error
	lw	ra,0(sp)
	addi	sp,sp,4

	// result is True = exit
2:	li	a0,1			// A0 <- True result, exit
	ret

	// set result type
3:	andi	a2,a2,~FLAG_NUMRES	// clear numeric result
	or	a2,a2,a1		// set new flag
	sb	a2,RAM_FLAGS_OFF(gp)	// set new flags

	// exit if only checking syntax or error
	andi	a2,a2,FLAG_SYNTOFF	// check syntax flag
	beqz	a2,2b			// exit if only checking syntax
	lbu	a0,RAM_ERRNR_OFF(gp)	// get error number
	bnez	a0,2b			// exit if error
	ret				// here returns A0 = 0, continue

// ----------------------------------------------------------------------------
// scan variable or function (CharPtr = pointer to variable name;
//	returns result in LastResult) (can raise errors)
// input: A0 ... 'fncok' functions are allowed
// - Called during syntax checking from CLASS5.
// ----------------------------------------------------------------------------

.global LookVars
LookVars:
	// push registers
	addi	sp,sp,-20
	sw	ra,0(sp)
	sw	s0,4(sp)
	sw	s1,8(sp)
	sw	tp,12(sp)
	mv	tp,a0			// TP <- flag 'fncok' functions are allowed

	// pre-set numeric result
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- flags
	ori	a1,a1,FLAG_NUMRES	// set numeric result
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// parse variable name -> S0 'flag'
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get pointer to start of variable name 'p'
	sw	a0,16(sp)		// save pointer to start of variable name 'p'
	call	ParseVarName		// parse variable name, -> A0 flag
	mv	s0,a0			// S0 <- type and length of name 'flag'

	// invalid name
	li	a1,VAR_END
	beq	a0,a1,LookVars_VarNotFound  // variable not found

	// find function -> S1 'fnc'
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get pointer to start of variable name 'p'
	mv	a1,s0			// A1 <- type and length of name 'flag'
	call	ParseFnc		// find function, -> A0 'fnc'
	mv	s1,a0			// S1 <- function 'fnc'

	// functions are not allowed
	bltz	s1,1f			// skip if function is not valid
	beqz	tp,2f			// syntax error if function is not allowed

	// get name length -> TP and type -> A4
1:	andi	tp,s0,VAR_MASK		// TP <- name length 'len'
	srli	a4,s0,VAR_SHIFT		// A4 <- name type 'type'

	// variable name$( is invalid
	li	a5,VARTYPE_FOR		// A5 <- name type 'name$('
	bgez	s1,1f			// skip if function is valid, 'fnc' >= 0
	bne	a4,a5,1f		// skip if not 'name$('
2:	call	SyntaxError		// syntax error
	j	LookVars_9

	// skip name
1:	bne	a4,a5,1f		// skip if not 'name$('
	addi	tp,tp,2			// length +2 to skip '$('

1:	li	a5,VARTYPE_ARRAY	// array 'name('
	beq	a4,a5,2f		// do increase if array 'name('
	li	a5,VARTYPE_STR		// string 'name$'
	bne	a4,a5,1f		// skip if not string 'name$'
2:	addi	tp,tp,1			// length +1 to skip '(' or '$'

1:	call	CharAdd			// skip 1 character
	addi	tp,tp,-1		// decrease length counter
	bnez	tp,1b			// skip name

// Here is: S0 = name flags 'flag', S1 = function 'fnc', 16(sp)	= pointer to start of variable name 'p'

// --- process function

	// check if function is valid
	bltz	s1,LookVars_NoFnc	// skip if function is not valid

	// get input type of result of function (FLAG_NUMRES=number, 0=string, 255=no input expression)
	la	tp,FncResList		// TP <- table
	slli	a2,s1,1			// A2 <- function * 2
	add	tp,tp,a2		// TP <- pointer to table
	lbu	a1,0(tp)		// A1 <- input type 'itype'

	// evaluate argument of name() or name$()
	li	a5,255
	beq	a1,a5,2f		// skip if input type is not valid

	// evaluate expression, store result to LastResult
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get pointer to start of variable name 'p'
	call	Bracket			// evaluate expression

	// check and set type of result
	lbu	a0,0(tp)		// A0 <- input type 'itype'
	lbu	a1,1(tp)		// A1 <- output type 'otype'
	call	CheckLookVars		// check type
	bnez	a0,LookVars_9		// exit
	j	3f

	// set result type
2:	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- flags
	lbu	a0,1(tp)		// A0 <- output type 'otype'
	andi	a1,a1,~FLAG_NUMRES	// clear numeric result
	or	a1,a1,a0		// set output numeric result
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags

	// only check syntax or error
	andi	a1,a1,FLAG_SYNTOFF	// check syntax flag
	beqz	a1,LookVars_9		// exit if only checking syntax
	lbu	a1,RAM_ERRNR_OFF(gp)	// get error number
	bnez	a1,LookVars_9		// exit if error

	// call function
3:	lui	a1,%hi(FncList)		// A1 <- table high
	slli	a2,s1,2			// A2 <- function offset 'fnc'*4
	add	a1,a1,a2		// A1 <- address in table
	lw	a1,%lo(FncList)(a1)	// A1 <- get function address
	jalr	a1			// call function (and return)
	j	LookVars_9		// exit

// Here is: S0 = name flags 'flag', S1 = function 'fnc', 16(sp)	= pointer to start of variable name 'p'

LookVars_NoFnc:

	// get name type -> A4
	srli	a4,s0,VAR_SHIFT		// A4 <- name type 'type'
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- flags

// --- string variable 'name$'

	// string variable 'name$'
	li	a5,VARTYPE_STR
	bne	a4,a5,2f		// skip if not string

	// set string result
	andi	a1,a1,~FLAG_NUMRES	// set string result
	sb	a1,RAM_FLAGS_OFF(gp)	// save new flags
	j	LookVars_GetVar		// get variable value

// --- array 'name()'

	// array 'name()'
2:	li	a5,VARTYPE_ARRAY
	bne	a4,a5,4f		// skip if not array

	// evaluate bracket expression
	lw	a0,RAM_CHARPTR_OFF(gp)	// A0 <- get pointer to expression
	call	Bracket			// evaluate bracket expression
	lbu	a1,RAM_FLAGS_OFF(gp)	// A1 <- flags
//	j	LookVars_GetVar		// get variable value

// continue to LookVars_GetVar

// --- simple numeric variable 'name'

4:
LookVars_GetVar:

	// only check syntax or error
	andi	a1,a1,FLAG_SYNTOFF	// check syntax flag
	beqz	a1,LookVars_9		// exit if only checking syntax
	lbu	a1,RAM_ERRNR_OFF(gp)	// get error number
	bnez	a1,LookVars_9		// exit if error

	// find variable and load its value (p = pointer to first letter of name of variable; returns pointer to variable, or NULL if not found)
	// returns variable value in LastResult
	lw	a0,16(sp)		// A0 <- get pointer to start of variable name 'p'
	li	a1,0			// A1 <- NULL, no pointer to variable
	call	GetVar			// get variable value
	bnez	a0,LookVars_9		// skip OK if variable has been found

LookVars_VarNotFound:

	// variable not found
	call	VarNotFound		// variable not found

LookVars_9:

	// pop registers
	lw	tp,12(sp)
	lw	s1,8(sp)
	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,20
	ret

// ----------------------------------------------------------------------------
// PEEK function
// - Input and output 'num' to LastResult.
// ----------------------------------------------------------------------------

.global FncPeek
FncPeek:
	// get input value -> A0
	lw	a0,RAM_LASTRESULT_OFF(gp)

	// Frame counter (addr = -1)
	li	a4,-1
	bne	a0,a4,2f		// skip if not address -1
	lui	a5,%hi(DispFrame)	// A5 <- address HIGH
#if VALSIZE == 32
	lw	a0,%lo(DispFrame)(a5)	// load
#else
	lh	a0,%lo(DispFrame)(a5)	// load
#endif
	j	10f

	// Random seed (addr = -2)
2:	li	a4,-2
	bne	a0,a4,3f
	lui	a5,%hi(RandSeed)	// A5 <- address HIGH
#if VALSIZE == 32
	lw	a0,%lo(RandSeed)(a5)	// load
#else
	lh	a0,%lo(RandSeed)(a5)	// load
#endif
	j	10f

	// Time in 0.02 us (addr = -3)
3:	li	a4,-3
	bne	a0,a4,3f
#define SYSTICK_CNT	0xE000F008	// SysTick System counter
	lui	a5,%hi(SYSTICK_CNT)	// A5 <- systick counter HIGH
	lw	a0,%lo(SYSTICK_CNT)(a5)	// A0 <- get systick counter
	lw	a5,RAM_LASTTIME_OFF(gp)	// get init timer
	sub	a0,a0,a5		// A0 <- time difference
	j	10f

	// print position X (addr = -4)
3:	li	a4,-4
	bne	a0,a4,3f
	lbu	a0,RAM_PRINTX_OFF(gp)	// A0 <- print position PrintX
	j	10f

	// print position Y (addr = -5)
3:	li	a4,-5
	bne	a0,a4,3f
	lbu	a0,RAM_PRINTY_OFF(gp)	// A0 <- print position PrintY
	j	10f

	// buffers
3:	bltz	a0,9f			// skip if num < 0

	// get character from the screen (addr = 0..0x2FF)
	li	a1,FRAMESIZE
	bgeu	a0,a1,4f		// invalid frame buffer address
	lui	a2,%hi(FrameBuf)	// A2 <- frame buffer
	add	a2,a2,a0		// A2 <- pointer to frame buffer
	lbu	a0,%lo(FrameBuf)(a2)	// load character
	j	10f

	// get font (addr = 0xE00..0x1000)
	// Original ZX80 font is organised to 1 column of characters:
	//	B0..B2 = letter line 0..7
	//	B3..B8 = letter index 0..63
	// WCH80 font is organised by lines:
	//	B0..B5 = letter index 0..63
	//	B6..B8 = letter line 0..7
4:	li	a1,0xe00		// A1 <- minimal address
	sub	a1,a0,a1		// A1 <- address relative
	li	a2,0x1000-0xe00		// A2 <- max. offset
	bgeu	a1,a2,9f		// invalid address

	// convert address: num = ((num & 0x07) << 6) | ((num >> 3) & 0x3f);
	andi	a3,a1,7			// A3 <- low 3 bits (= line number in the character)
	slli	a3,a3,6			// A3 <- low 3 bits to position 6,7,8 (= line multiply of 64)
	srai	a2,a1,3			// A2 <- high 6 bits to position 0-5 (character index)
	or	a2,a2,a3		// A2 <- byte offset
	lui	a1,%hi(Font80)		// A1 <- font high
	add	a1,a1,a2		// A1 <- pointer to font
	lbu	a0,%lo(Font80)(a1)	// load byte
	j	10f

	// invalid address
9:	li	a0,0

	// set output value
10:	sw	a0,RAM_LASTRESULT_OFF(gp)
	ret

#endif // USE_VAR_ASM
