
// ****************************************************************************
//
//                             Parse
//
// ****************************************************************************

#include "../include.h"

#if !USE_PARSE_ASM		// 1=use ASM in Parse module

// check alphabetic character 'A'..'Z'
// ZX80: L0D14
Bool IsAlpha(u8 ch)
{
	return (ch >= CH_A) && (ch <= CH_Z);
}

// check alphanumeric character '0'..'9' or 'A'..'Z'
// ZX80: L0D18
Bool IsAlphaNum(u8 ch)
{
#if CH_9+1 == CH_A
	return (ch >= CH_0) && (ch <= CH_Z);
#else
	return ((ch >= CH_0) && (ch <= CH_9)) ||
		((ch >= CH_A) && (ch <= CH_Z));
#endif
}

// check alphanumericdot character '.' or '0'..'9' or 'A'..'Z'
Bool IsAlphaNumDot(u8 ch)
{
#if (CH_9+1 == CH_A) && (CH_DOT+1 == CH_0)
	return (ch >= CH_DOT) && (ch <= CH_Z);
#else
	return (ch == CH_DOT) ||
		((ch >= CH_0) && (ch <= CH_9)) ||
		((ch >= CH_A) && (ch <= CH_Z));
#endif
}

// parse variable name
//  Returns:
//    bit 0..5: length of the name (1 to 63)
//    bit 6..7: type of variable VARTYPE_*
// VARTYPE_NUM (=0) ... integer number 'name'
// VARTYPE_FOR (=1) ... function 'name$()'
// VARTYPE_ARRAY (=2) ... array or function 'name()'
// VARTYPE_STR (=3) ... string 'name$'
// VAR_END (0xc0) ... invalid variable
u8 ParseVarName(u8* p)
{
	// save start of the name
	u8* p0 = p;

	// get first character
	u8 ch = *p;

	// first character must be letter
	if (!IsAlpha(ch)) return VAR_END;

	// parse name
	do {
		p++;
		ch = *p;
	} while (IsAlphaNumDot(ch));

	// length of the name
	int len = p - p0;

	// name is too long - we return 'int 63' to mark position of the error.
	if (len > VAR_MAXLEN) return (VARTYPE_NUM << VAR_SHIFT) + VAR_MAXLEN;

	// array
	if (ch == TOKEN_LPAR) return (VARTYPE_ARRAY << VAR_SHIFT) + len;

	// string or string function
	if (ch == CH_DOLLAR)
	{
		// string fuction
		if (p[1] == TOKEN_LPAR) return (VARTYPE_FOR << VAR_SHIFT) + len;

		// string
		return (VARTYPE_STR << VAR_SHIFT) + len;
	}

	// integer number
	return (VARTYPE_NUM << VAR_SHIFT) + len;
}

// parse function name (returns function index 0.., or -1=not function)
//  p ... pointer to name
//  flag ... type and length of the name, as returned from ParseVarName()
int ParseFnc(u8* p, u8 flag)
{
	u8 ch;
	int len = flag & VAR_MASK;	// name length
	const u8* v = FncNameList;	// list of functions
	int fnc = 0;			// function index
	while ((ch = *v) != VAR_END)
	{
		// check function
		if ((ch == flag) && (memcmp(p, v+1, len) == 0)) return fnc;

		// shift to next function
		int n = ch & VAR_MASK; // name length
		v += 1 + n; // skip type and name
		fnc++;	// increase function index
	}
	return -1; // not found
}

// increment pointer and get next character from program line or from edit line (no error)
// ZX80: L0052, L0018, RST 18H
u8 CharAdd()
{
	u8* p = CharPtr;
	p++;
	CharPtr = p;
	u8 ch = *p;

	// update cursor mode
	if (p == CurPtr)
	{
		if (IS_KUPDATE) SET_KMODE;
	}
	return ch;
}

// increment pointer and get next character from program line or from edit line, skipping spaces (no error)
// ZX80: L0020, RST 20H
u8 NextChar()
{
	u8 ch;
	do { ch = CharAdd(); } while (ch == CH_SPC);
	return ch;
}

// get current character from program line or from edit line, skipping spaces (no error)
// ZX80: L001A
u8 GetChar()
{
	u8* p = CharPtr;
	u8 ch = *p;
	if (ch != CH_SPC) return ch;
	return NextChar(); // get next character and skip spaces
}

// check end of command line (returns True = continue, False = only checking syntax, break) (can raise SyntaxError)
// ZX80: L083D
Bool CheckEnd()
{
	// running program, continue
	if (IS_SYNTOFF) return True;

	// check end of command line
	if (GetChar() != NEWLINE) SyntaxError();

	// checking syntax, break
	return False;
}

// parse signed number (saves result to LastResult) (can raise Overflow error)
// Value VAL_MIN cannot be entered.
// ZX80: L0679
val ParseNum()
{
	u8 ch;

	// clear result accumulator
	val res = 0;

	// parse
	while (True)
	{
		// load next character
		ch = *CharPtr;

		// check digit
		ch -= CH_0;
		if ((u8)ch > 9) break;

		// check overflow
		if (res > VAL_MAX/10) Overflow();

		// accumulator * 10
		res *= 10;

		// check overflow
		if (res > VAL_MAX - ch) Overflow();

		// add digit to accumulator
		res += ch;

		// shift pointer (with update cursor)
		CharAdd();
	}
	LastResult = res;
	return res;
}

// parse unsigned number (saves result to LastResult; not checking overflow) (no error)
u32 ParseUNum()
{
	u8 ch;

	// clear result accumulator
	u32 res = 0;

	// parse
	while (True)
	{
		// load next character
		ch = *CharPtr;

		// check digit
		ch -= CH_0;
		if ((u8)ch > 9) break;

		// accumulator * 10
		res *= 10;

		// add digit to accumulator
		res += ch;

		// shift pointer (with update cursor)
		CharAdd();
	}
	LastResult = res;
	return res;
}

#endif // USE_PARSE_ASM
