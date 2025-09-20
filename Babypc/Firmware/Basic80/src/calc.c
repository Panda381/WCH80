
// ****************************************************************************
//
//                        Calculate expression
//
// ****************************************************************************

#include "../include.h"

#if !USE_CALC_ASM		// 1=use ASM in calc module

// Fetch value (can raise error)
// - Called during syntax checking
// - On input, flags determine required result type.
// ZX80: L0889
void FetchVal()
{
	// save flags
	u8 oldflag = Flags;

	// evaluate expression (can raise error)
	ScanCalc();
	if (IS_ERROR) return;

	// if not checking syntax, run LET command
	if (IS_SYNTOFF)
	{
		CmdLet();
	}

	// if checking syntax, only compare result type
	else
	{
		// syntax error, if result types are different
		if (((Flags ^ oldflag) & FLAG_NUMRES) != 0) SyntaxError();

		// check end of command line
		CheckEnd();
	}
}

// ABS function, absolute value (note: VAL_MIN stays unchanged, should be error) (no error)
// - Input and output to LastResult.
void FncAbs()
{
	val a = LastResult;
	if (a < 0) LastResult = -a;
}

// negate value (note: VAL_MIN stays unchanged, should be error) (no error)
// ZX80: L0DF6
val NumNeg(val a)
{
	return -a;
}

// subtraction a - b (can raise Overflow error)
// ZX80: L0D39
val NumSub(val a, val b)
{
	return NumAdd(a, -b);
}

// addition a + b (can raise Overflow error)
// ZX80: L0D3E
val NumAdd(val a, val b)
{
	// check overflow in positive direction
	if ((b > 0) && (a > (val)(VAL_MAX - b))) Overflow();

	// check overflow in negative direction
	if ((b < 0) && (a < (val)(VAL_MIN - b))) Overflow();

	// do addition
	return a + b;
}

// multiplication a * b (can raise Overflow error)
// ZX80: L0D44
val NumMul(val a, val b)
{
	uval ua, ub;

	// zero
	if ((a == 0) || (b == 0)) return 0;

	// VAL_MIN has no positive opposite value
	if (((a == -1) && (b == VAL_MIN)) || ((b == -1) && (a == VAL_MIN)))
	{
		Overflow();
		return 0;
	}

	// prepare result sign
	Bool neg = False;
	if (a < 0)
	{
		ua = -a;
		neg = True;
	}
	else
		ua = a;

	if (b < 0)
	{
		ub = -b;
		neg = !neg;
	}
	else
		ub = b;

	// check overflow - result must not exceed max. bits
	if (Order(ua) + Order(ub) > VAL_BITS)
	{
		Overflow();
		return 0;
	}

	// multiply
	val c = (val)(ua*ub);

	// additional check
	if (c < 0) Overflow();

	// set result
	if (neg) c = -c;

	return c;
}

// division a / b (can raise Overflow error)
// ZX80: L0D90
val NumDiv(val a, val b)
{
	// checks
	if ((b == 0) || ((a == VAL_MIN) && (b == -1)))
	{
		Overflow();
		return 0;
	}

	// divide
	return a/b;
}

// power a ^ b (can raise Overflow error)
// ZX80: L0D70
val NumPwr(val a, val b)
{
	// exponent must be positive
	if (b < 0)
	{
		Overflow();
		return 0;
	}

	// special case 0^b
	if (a == 0)
	{
		// 0^0 is not defined
		if (b == 0) Overflow();
		return 0;
	}

	// special case 1^b
	else if (a == 1)
	{
		return 1;
	}

	// special case -1^b
	else if (a == -1)
	{
		return ((b & 1) != 0) ? -1 : 1;
	}

	// special case 2^b
	else if (a == 2)
	{
		if (b > VAL_BITS-2) Overflow();
		return (val)(1 << b);
	}

	// special case -2^b
	else if (a == -2)
	{
		if ((b & 1) != 0) // odd exponent
		{
			if (b > VAL_BITS-1) Overflow();
			return (val)(-(1 << b));
		}
		else // even exponent
		{
			if (b > VAL_BITS-2) Overflow();
			return (val)(1 << b);
		}
	}

	// multiply
	val res = 1;
	for (; b > 0; b--)
	{
		res = NumMul(res, a);
		if (IS_ERROR) break;
	}
	return res;
}

// bitwise AND a & b (no error)
// ZX80: L0DB5
val BitAND(val a, val b)
{
	return a & b;
}

// bitwise OR a | b (no error)
// ZX80: L0DBC
val BitOR(val a, val b)
{
	return a | b;
}

// compare a == b (no error)
// ZX80: L0DC3
val NumEqu(val a, val b)
{
	return (a == b) ? VAL_TRUE : VAL_FALSE;
}

// compare a > b (no error)
// ZX80: L0DCC
val NumGrt(val a, val b)
{
	return (a > b) ? VAL_TRUE : VAL_FALSE;
}

// compare a < b (no error)
// ZX80: L0DCD
val NumLes(val a, val b)
{
	return (a < b) ? VAL_TRUE : VAL_FALSE;
}

// string compare (strings are terminated with CH_QUOT character) (no error)
//  a,b ... string pointers
//  returns -1 if a<b, 0 if a==b, 1 if a>b
// ZX80: L0DE4
val StrCmp(uval a, uval b)
{
	const char* pa = RAMPTR(a);
	const char* pb = RAMPTR(b);

	char cha, chb;
	do {
		cha = *pa++;
		chb = *pb++;
		if (cha < chb) return -1;
		if (cha > chb) return 1;
	} while (cha != CH_QUOT);
	return 0;
}

// compare string if equal a == b (no error)
//  a,b ... string pointers
// ZX80: L0DD9
val StrEqu(uval a, uval b)
{
	return (StrCmp(a, b) == 0) ? VAL_TRUE : VAL_FALSE;
}

// compare string if greater a > b (no error)
//  a,b ... string pointers
// ZX80: L0DDF
val StrGrt(uval a, uval b)
{
	return (StrCmp(a, b) > 0) ? VAL_TRUE : VAL_FALSE;
}

// compare string if less a < b (no error)
//  a,b ... string pointers
// ZX80: L0DDE
val StrLes(uval a, uval b)
{
	return (StrCmp(a, b) < 0) ? VAL_TRUE : VAL_FALSE;
}

// RND function - random number generator (number 1..max will be generated) (no error)
// - Input and output to LastResult.
// ZX80: L0BED
void FncRnd()
{
	val n = LastResult;
	if (n <= 0)
	{
		LastResult = 1;
		return;
	}
#if VALSIZE == 32
	LastResult = RandS32MinMax(1, LastResult);
#else
	LastResult = RandS16MinMax(1, LastResult);
#endif
}

// command RANDOMISE (no error)
// ZX80: L0923
void CmdRandomise()
{
	val seed = LastResult;

	// if seed = 0, get seed from frame counter
	if (seed == 0) seed = RandSeed + DispFrame + DispLine;
	RandSeed = seed;
}

// CODE function - get numeric code of 1st character in string (no error)
// - Input 'str' and output 'num' to LastResult.
// ZX80: L0C24
void FncCode()
{
	u8* p = RAMPTR(LastResult);
	LastResult = *p;
}

// store to calculator stack (can raise error)
// - Called during syntax checking from CLASS5.
void PushCalc(val num, u8 op, u8 prior)
{
	int n = CalcNum; 		// get number of entries in calculator stack
	if (n >= CALCNUM)
	{
		MemError();		// calculator stack overflow
		return;
	}
	CalcNum = n+1;			// save new number of entries
	u8* p = &CalcStck[n*2];		// operation and priority
	p[0] = op;			// store operation
	p[1] = prior;			// store priority
	s32* v = &ValStck[n];		// value
	*v = num;			// store value
}

// load from calculator stack (returns value) (can raise error)
// - Called during syntax checking from CLASS5.
val PopCalc(u8* op, u8* prior)
{
	int n = CalcNum - 1; 		// get new number of entries in calculator stack
	if (n < 0)
	{
		ReturnError();		// RETURN without GOSUB
		return 0;
	}
	CalcNum = n;			// save new number of entries
	u8* p = &CalcStck[n*2];		// operation and priority
	*op = p[0];			// get operation
	*prior = p[1];			// get priority
	return (val)ValStck[n];		// value
}

// evaluate expression, store result to LastResult (p = pointer to program line) (can raise error)
// - Called during syntax checking from CLASS5.
// ZX80: L0025
void EvalExp(u8* p)
{
	// save pointer
	CharPtr = p - 1;

	// update cursor
	CharAdd();

	// scanning calculator expression (can raise error)
	ScanCalc();
}

// evaluate bracket expression (p = pointer to program line) (can raise error)
// - Called during syntax checking from CLASS5.
// ZX80: L0049
void Bracket(u8* p)
{
	// evaluate expression (can raise error)
	EvalExp(p);

	// check right bracket
	if (GetChar() != TOKEN_RPAR)
	{
		SyntaxError(); // syntax error
		return;
	}
	CharAdd(); // skip right bracket
}

// scanning calculator expression, store result to LastResult (can raise error)
// - Called during syntax checking from CLASS5.
// ZX80: L0028, RST 28H, L09E1, TabOps L0D1F
void ScanCalc()
{
	val num, num2;
	u8 op, op2;
	u8 prior, prior2;

	// reset priority level - set flag to terminate scanning
	PushCalc(0, 0, 0);

	// parse loop
	for (;;)
	{
		// break on error
		if (IS_ERROR) return;

// ---- scan operand (store result to LastResult)

		// get current character from program line or from edit line, skipping spaces
		u8 ch = GetChar();

		// check alphanumeric character '0'..'9' or 'A'..'Z'
		if (IsAlphaNum(ch))
		{
			// if alphabetic character, this is variable
			if (IsAlpha(ch))
			{
				// parse variable or function
				// - Called during syntax checking from CLASS5.
				LookVars(True);
			}

			// numeric character, this is number
			else
			{
				// parse number (saves result to LastResult) (can raise Overflow error)
				ParseNum();
				SET_NUMRES; // set numeric result
			}
		}

		// negate '-'
		else if (ch == TOKEN_SUB)
		{
			// push operation: val = 0, operation = "subtract", priority = 9
			PushCalc(0, TOKEN_SUB - TOKEN_SUB, 9);
			NextChar(); // skip operator
			continue; // load next operand
		}

		// NOT
		else if (ch == TOKEN_NOT)
		{
			// push operation: val = -1, operation = "subtract", priority = 4
			PushCalc(-1, TOKEN_SUB - TOKEN_SUB, 4);
			NextChar(); // skip operator
			continue; // load next operand
		}

		// opening bracket
		else if (ch == TOKEN_LPAR)
		{
			// evaluate bracket expression
			Bracket(CharPtr+1);
		}

		// quote "
		else if (ch == CH_QUOT)
		{
			// string result
			SET_STRRES;

			// save result (skip quote character)
			LastResult = PTRRAM(CharPtr + 1);

			// find end of string
			do {
				ch = CharAdd(); // skip and load next character
				if (ch == NEWLINE)
				{
					SyntaxError(); // end of program line (no quote)
					return;
				}
			} while (ch != CH_QUOT);

			// skip terminating quote "
			CharAdd();
		}

		// error
		else
		{
			SyntaxError();
			return;
		}

// ----- scan operator

		// get current character from program line or from edit line, skipping spaces
		op = GetChar() - TOKEN_SUB; // minimal operator is '-'

		// check operator -, +, *, /, AND, OR, **, =, >, <
		if ((u8)op < (u8)10)
		{
			// get priority
			prior = PriorTab[op];
		}

		// invalid operator - terminate this level
		else
		{
			op = 0;
			prior = 0;
		}

		// process priorities
		while (True)
		{
			// break on error
			if (IS_ERROR) return;

			// load previous calculator stack
			num2 = PopCalc(&op2, &prior2);

			// compare with previous priority
			if (prior <= prior2) // current priority is not higher, perform last operation
			{
				// both priorities are 0, end of expression (result is in LastResult)
				if (prior2 == 0) return;

				// check if only checking syntax
				if (IS_SYNTOFF) // not checking syntax
				{
					// get last result
					num = LastResult;

					// ZX80: L0D1F
					switch (op2)
					{
					// operators -, +, *, /, AND, OR, **, =, >, <
					case 0: num = NumSub(num2, num); break;		// '-'
					case 1: num = NumAdd(num2, num); break;		// '+'
					case 2: num = NumMul(num2, num); break;		// '*'
					case 3: num = NumDiv(num2, num); break;		// '/'
					case 4: num = BitAND(num2, num); break;		// 'AND'
					case 5: num = BitOR(num2, num); break;		// 'OR'
					case 6: num = NumPwr(num2, num); break;		// '**'
					case 7: num = NumEqu(num2, num); break;		// '='
					case 8: num = NumGrt(num2, num); break;		// '>'
					case 9: num = NumLes(num2, num); break;		// '<'
					case 10: num = StrEqu(num2, num); break;	// str '='
					case 11: num = StrGrt(num2, num); break;	// str '>'
					case 12: num = StrLes(num2, num); break;	// str '<'
					}

					// save new last result
					LastResult = num;
				}

				// only checking syntax
				else
				{
					// compare operands, if are the same type
					if (IS_NUMRES) // numeric result
					{
						if (op2 >= 10)
						{
							SyntaxError(); // previous operand was string
							return;
						}
					}
					else
					{
						if (op2 < 10)
						{
							SyntaxError(); // previous operand was number
							return;
						}
					}

					LastResult = num2;
				}

				// set numeric result
				SET_NUMRES;
			} // if (prior <= prior2)

			// current priority is higher (prior > prior2), go deeper
			else
			{
				// save previous operation back to the calculator stack
				PushCalc(num2, op2, prior2);

				// string operator
				if (IS_STRRES) // if string result
				{
					op += 3; // shif operator to string functions 10..12
					if (op < 10)
					{
						SyntaxError(); // invalid operator
						return;
					}
				}

				// get last result
				num = LastResult;

				// push next operation to the calculator stack
				PushCalc(num, op, prior);

				// discard this operator
				NextChar();

				// break this loop of lower priorities, load next operand
				break;
			} // if (prior <= prior2)
		} // while (True) // process priorities
	} // for (;;) // parse loop
}

// function READ, read data (returns result to LastResult)
void FncRead()
{
	int line;

	// save current pointer and K-flags
	u8* oldptr = CharPtr;
	u8 oldflag = Flags;

	// get data pointer
	u8* p = DataPtr;
	CharPtr = p;

	// need to find next DATA line
	while ((p == NULL) || (GetChar() == NEWLINE))
	{
		// pointer to next program line
		if (p == NULL)
			p = Prog; // reset to first row
		else
			p = CharPtr+1; // skip NEWLINE character

		// find DATA line
		while (True)
		{
			// check end of program
			if (CHECKLINE0(p))
			{
				// if this is first pass, do not continue, we have no DATA row
				if (DataPtr == NULL)
				{
					LastResult = 0;
					CharPtr = oldptr;
					Flags = oldflag;
					return;
				}

				// rewind to start
				p = Prog;
			}

			// check DATA program line
			CharPtr = p + 2;
			if (GetChar() == TOKEN_DATA)
			{
				// skip DATA token
				NextChar();
				break;
			}

			// next program line
			p = NextLine(p);
		}
	}

	// skip comma separators
	while (GetChar() == TOKEN_COMMA)
	{
		// skip comma
		NextChar();
	}

	// parse expression
	ScanCalc();

	// save current pointer
	DataPtr = CharPtr;

	// return pointers and flags
	CharPtr = oldptr;
	Flags = oldflag;
}

#endif // !USE_CALC_ASM
