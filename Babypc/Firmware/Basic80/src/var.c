
// ****************************************************************************
//
//                                Variables
//
// ****************************************************************************

#include "../include.h"

#if !USE_VAR_ASM		// 1=use ASM in Var module

// CLEAR variables (no error)
// ZX80: L065B
void CmdClear()
{
	VarsNum = 1;
	Vars[0] = VAR_END;
}

// move to next variable (returns next variable, or VAR_END = end) (no error)
//  00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
//  01aaaaaa name valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR, variable 'name'
//  10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
//  11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
//  0xC0 ... end of variables
// ZX80: L0624
u8* NextVar(u8* p)
{
	u8 ch;

	// get variable type and length
	ch = *p;
	int type = ch >> VAR_SHIFT;	// variable type
	int namelen = ch & VAR_MASK;	// length of the name
	p += namelen; // skip name, but not type (on end: length is 0)

	// simple integer variable
	if (type == VARTYPE_NUM)
	{
#if VALSIZE == 32
		p += 5;
#else
		p += 3;
#endif
	}

	// FOR loop
	else if (type == VARTYPE_FOR)
	{
#if VALSIZE == 32
		p += 11;
#else
		p += 7;
#endif
	}

	// array
	else if (type == VARTYPE_ARRAY)
	{
		int max = p[1];
#if VALSIZE == 32
		p += 6 + (max << 2);
#else
		p += 4 + (max << 1);
#endif
	}

	// string
	else if (type == VARTYPE_STR)
	{
		p++; // skip type
		do { ch = *p++; } while (ch != CH_QUOT);
	}
	// else VAR_END = end of variables	

	return p;
}

// DIM command - create integer array; does not check if array exists (can raise errors)
//  LastResult = max. subscript index, VarDest points to array name
//  10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
// ZX80: L0CD3
NOMEMSET void CmdDim()
{
	// parse length of the name
	int len = ParseVarName(VarDest) & VAR_MASK;

	// get array max. index
	val max = LastResult;

	// check range 0..255, subscript error
	if ((uint)max > 255)
	{
		SubsError();
		return;
	}

	// prepare required size
#if VALSIZE == 32
	int size = max*4 + 6 + len;
#else
	int size = max*2 + 4 + len;
#endif

	// check free memory
	int num = VarsNum;
	if (num + size > VARSNUM)
	{
		MemError();
		return;
	}

	// set new size of variables
	VarsNum = num + size;

	// initialize array
	u8* d = &Vars[num - 1]; // pointer to old terminator VAR_END
	*d++ = len + (VARTYPE_ARRAY << VAR_SHIFT); // set variable type and length
	size -= 2 + len; // size without header (= size of data entries)
	u8* s = VarDest; // name of the variable
	do {
		*d++ = *s++;
		len--;
	} while (len > 0);	// transfer name of the variable
	*d++ = (u8)max; // array max. index

	// clear array
	for (; size > 0; size--) *d++ = 0;

	// terminator
	*d = VAR_END;
}

// Store string variable, terminated with quote; does not check if variable exists
//	(p = pointer to string immediately after first quote ") (can raise errors)
//  VarDest points to string name.
// 11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
// ZX80: L0CA4
void SetStr(u8* p)
{
	// parse length of the name
	int len = ParseVarName(VarDest) & VAR_MASK;

	// prepare string size
	u8 ch;
	u8* p2 = p;
	do { ch = *p2++; } while (ch != CH_QUOT);
	int size = p2 - p + 1 + len; // string size (including quote character, type and name)

	// check free memory
	int num = VarsNum;
	if (num + size > VARSNUM)
	{
		MemError();
		return;
	}

	// set new size of variables
	VarsNum = num + size;

	// initialize variable
	u8* d = &Vars[num - 1]; // pointer to old terminator VAR_END
	*d++ = len + (VARTYPE_STR << VAR_SHIFT); // set variable type and length
	size -= 1 + len; // size without header (= length of string)
	u8* s = VarDest; // name of the variable
	do {
		*d++ = *s++;
		len--;
	} while (len > 0);	// transfer name of the variable

	// copy string (including terminating CH_QUOT)
	for (; size > 0; size--) *d++ = *p++;

	// terminator
	*d = VAR_END;
}

// STR$ function - convert number to string (to Edit buffer; can raise errors)
//  Input 'num' and output 'str' to LastResult.
// ZX80: L0C10
void FncStr()
{
	// get input value
	val num = LastResult;

	// decode number to temporary buffer
	u8* p = DecTempNum(num);

	// length of the text
	int len = &Temp[TEMPNUM] - p;

	// check free space in edit buffer
	int n = EditNum;
	if (n + len > EDITNUM)
	{
		LastResult = PTRRAM(&NulString);
		MemError();
		return;
	}
	EditNum = n + len;

	// save last result
	u8* d = &Edit[n];
	LastResult = PTRRAM(d);

	// copy number
	for (; len > 0; len--) *d++ = *p++;
}

// CHR$ function - convert character code to 1-character string (in Edit buffer, can raise errors)
//  Input 'num' and output 'str' to LastResult.
// ZX80: L0C28
void FncChr()
{
	// input number
	val num = LastResult;

	// use nul string in case of error
	LastResult = PTRRAM(&NulString);

	// check valid range
	if ((uval)num > (uval)255)
	{
		Overflow();
		return;
	}

	// check free space in edit buffer
	int n = EditNum;
	if (n + 2 > EDITNUM)
	{
		MemError();
		return;
	}
	EditNum = n + 2;

	// save number as text
	u8* p = &Edit[n];
	p[0] = (u8)num; // save number
	p[1] = CH_QUOT;

	// save last result
	LastResult = PTRRAM(p);
}

// INKEY$ function - get key from keyboard (in Edit buffer,
//	save pointer to result to LastResult) (can raise errors)
void FncInkey()
{
	// get key
	SET_LMODE;
	u8 ch = CharGet();

	// no key
	if (ch == CH_NONE)
	{
		LastResult = PTRRAM(&NulString);
		return;
	}

	// convert key to string
	FncChr(ch);
}

// TL$ function - returns tail of string, without 1st character (no error)
//  Input 'str' and output 'str' to LastResult.
// ZX80: L0C38
void FncStrTl()
{
	// pointer to string
	u8* p = RAMPTR(LastResult);

	// check if string is empty
	if (*p != CH_QUOT) p++;

	// save last result
	LastResult = PTRRAM(p);
}

// LET command (VarDest = variable name, LastResult = last result) (can raise errors)
// ZX80: L0C3D
void CmdLet()
{
	// return if error has been encountered
	if (IS_ERROR) return;

	// pointer to name of variable
	u8* v = VarDest;

	// save init value
	val n = LastResult;

	// parse variable name
	u8 flag = ParseVarName(v); // parse variable name

	// invalid name
	if (flag == VAR_END)
	{
		SyntaxError();
		return;
	}

	// invalid type 'name$('
	int type = flag >> VAR_SHIFT; // variable type
	if (type == VARTYPE_FOR)
	{
		SyntaxError();
		return;
	}

	// try to find function - check reserved name
	int fnc = ParseFnc(v, flag);
	if (fnc >= 0)
	{
		SyntaxError();
		return;
	}

	// find variable (NULL if not found) (can raise errors ERR_SUBSCRIPT or ERR_VARNFND)
	// returns pointer to data or NULL if not found
	u8* var;
	u8* p = GetVar(v, &var);

	// subscript error ("Variable not found" is OK)
	if (ErrNr == ERR_SUBSCRIPT) return;

	// get name type and name length
	type = flag >> VAR_SHIFT; // variable type
	int len = flag & VAR_MASK; // name length

	// variable not found (error ERR_VARNFND)
	if (p == NULL)
	{
		// Clear error "Variable not found"
		ErrNr = ERR_OK;
		ErrPtr = NULL;	// no error

		// array 'name('
		if (type == VARTYPE_ARRAY)
		{
			// error, variable not found (array must exist)
			VarNotFound();
			return;
		}

		// string 'name$'
		else if (type == VARTYPE_STR)
		{
			// store new string to end of variables
			SetStr(RAMPTR(n));
		}

		// simple integer number
		else
		{
			// create new numeric variable
// 00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
#if VALSIZE == 32
			int size = 5 + len;
#else
			int size = 3 + len;
#endif
			int num = VarsNum;
			if (size + num > VARSNUM)
			{
				MemError();
				return;
			}
			VarsNum = num + size;
			u8* d = &Vars[num - 1];
			*d++ = flag; // variable type and length
			for (; len > 0; len--) *d++ = *v++;
			*d++ = (u8)n;
			*d++ = (u8)(n >> 8);
#if VALSIZE == 32
			*d++ = (u8)(n >> 16);
			*d++ = (u8)(n >> 24);
#endif
			*d = VAR_END; // terminator
		}
	}

	// variable already exists
	else
	{
		// string 'name$'
		if (type == VARTYPE_STR)
		{
			// store new string to end of variables
			SetStr(RAMPTR(n));
			if (IS_ERROR) return;

			// find next variable (to get length of this variable)
			u8* var2 = NextVar(var);

			// delete old string
			int i = VarsNum - (var2 - Vars); // remaining data after old variable
			VarsNum -= var2 - var; // subtract length of old variable
			for (; i > 0; i--) *var++ = *var2++;
		}

		// numeric variable (or array) - store result
		else
		{
			p[0] = (u8)n;
			p[1] = (u8)(n >> 8);
#if VALSIZE == 32
			p[2] = (u8)(n >> 16);
			p[3] = (u8)(n >> 24);
#endif
		}
	}
}

// find variable and load its value (p = pointer to name of variable; returns pointer
// to data or NULL if not found) (can raise errors ERR_SUBSCRIPT or ERR_VARNFND)
//  var = to get pointer to variable, or NULL = not required
// ZX80: L0B3B
u8* GetVar(u8* p, u8** var)
{
	// clear address of variable
	if (var != NULL) *var = NULL;

	// parse variable name
	u8 flag = ParseVarName(p);
	int len = flag & VAR_MASK; // name length
	int type = flag >> VAR_SHIFT; // variable type

	// reject string function name$(), or not valid name
	if ((type == VARTYPE_FOR) || (flag == VAR_END))
	{
		VarNotFound();
		return NULL;
	}

	// alternative flag INT -> FOR
	u8 flag2 = flag;
	if (type == VARTYPE_NUM) flag2 = len + (VARTYPE_FOR << VAR_SHIFT);

	// find variable
	u8* v = Vars;
	u8 ch;
	while (True)
	{
		// end of variables
		ch = *v;
		if (ch == VAR_END)
		{
			VarNotFound();
			return NULL; // variable not found
		}

		// check variable type
		if ((ch == flag) || (ch == flag2))
		{
			// compare name
			if (memcmp(v+1, p, len) == 0) break; // variable found
		}

		// next variable
		v = NextVar(v); // go to next variable
	}

	// save pointer to variable
	if (var != NULL) *var = v;

	// skip type and name
	v += 1 + len;

	// array
// 10aaaaaa name max valL (val2 val3) valH...  ... array, max=max index 0..255, variable 'name()'
	if (type == VARTYPE_ARRAY)
	{
		// save current text address
		u8* psave = CharPtr;

		// evaluate expression (store result to LastResult)
		EvalExp(p + len + 1);

		// restore current text address
		CharPtr = psave;

		// error, check max. index
		val num = LastResult;
		if (IS_ERROR || ((uval)num > (uval)v[0]))
		{
			ErrNr = ERR_SUBSCRIPT; // subscript error (overwrites current error)
			return NULL;
		}

		// rewind to the entry
#if VALSIZE == 32
		v += num*4 + 1;
#else
		v += num*2 + 1;
#endif
		goto GetVar_LoadInt;
	}

	// string
// 11aaaaaa name text CH_QUOT ... text terminated with CH_QUOT, variable 'name$'
	else if (type == VARTYPE_STR)
	{
		// save last result
		LastResult = PTRRAM(v);
	}

	// simple numeric variable or FOR
// 00aaaaaa name valL (val2 val3) valH ... integer variable 'name'
// 01aaaaaa name valL (val2 val3) valH maxL (max2 max3) maxH lineL lineH ... FOR control, val=value, max=limit TO, line=line with FOR, variable 'name'
	else
	{
		// load result
GetVar_LoadInt:
#if VALSIZE == 32
		val num = v[0] | (v[1] << 8) | (v[2] << 16) | (v[3] << 24);
#else
		val num = v[0] | (v[1] << 8);
#endif

		// save last result
		LastResult = num;
	}

	// return pointer to data of variable (or start of string variable)
	return v;
}

// check LookVars state
//  req ... require numeric or string result - use FLAG_NUMRES or 0
//  set ... set numeric or string result - use FLAG_NUMRES or 0
// output: True = exit, False = continue
Bool CheckLookVars(int req, int set)
{
	// get current flags
	u8 f = Flags;

	// check result type
	if ((f & FLAG_NUMRES) != req)
	{
		SyntaxError(); // syntax error
		return True;
	}

	// set result type
	Flags = (f & ~FLAG_NUMRES) | set;

	// exit if only checking syntax or error
	return ((f & FLAG_SYNTOFF) == 0) || IS_ERROR;
}

// scan variable or function (CharPtr = pointer to variable name;
//	returns result in LastResult) (can raise errors)
//  fncok ... functions are allowed
// - Called during syntax checking from CLASS5.
// ZX80: L0AAD
void LookVars(Bool fncok)
{
	// pre-set numeric result
	SET_NUMRES;

	// parse variable name
	u8* p = CharPtr; // save pointer to start of variable name
	u8 flag = ParseVarName(p); // parse variable name

	// invalid name
	if (flag == VAR_END)
	{
		VarNotFound(); // variable not found
		return;
	}

	// find function
	int fnc = ParseFnc(p, flag);

	// functions are not allowed
	if ((fnc >= 0) && !fncok)
	{
		SyntaxError();
		return;
	}

	// get name type and length
	int type = flag >> VAR_SHIFT; // variable type
	int len = flag & VAR_MASK; // name length

	// variable name$( is invalid
	if ((fnc < 0) && (type == VARTYPE_FOR))
	{
		SyntaxError();
		return;
	}

	// skip name
	if (type == VARTYPE_FOR) len += 2; // skip '$('
	if ((type == VARTYPE_ARRAY) || (type == VARTYPE_STR)) len++; // skip '(' or '$'
	for (; len > 0; len--) CharAdd(); // skip name

	// function is valid
	if (fnc >= 0)
	{
		// get input and output type of result of function (FLAG_NUMRES=number, 0=string, 255=no input expression)
		u8 itype = FncResList[fnc*2];
		u8 otype = FncResList[fnc*2+1];

		// evaluate argument of name() or name$()
		if (itype != 255)
		{
			// evaluate expression, store result to LastResult
			Bracket(CharPtr);

			// check and set type of result
			if (CheckLookVars(itype, otype)) return;
		}
		else
		{
			// set result type
			Flags = (Flags & ~FLAG_NUMRES) | otype;

			// only check syntax or error
			if (IS_SYNTON || IS_ERROR) return;
		}

		// call function
		FncList[fnc]();
		return;
	}

	// string variable 'name$'
	if (type == VARTYPE_STR)
	{
		SET_STRRES; // set string result
		goto LookVars_GetVar;
	}

	// array 'name()'
	else if (type == VARTYPE_ARRAY)
	{
		// evaluate bracket expression
		Bracket(CharPtr);
		goto LookVars_GetVar;
	}

	// simple numeric variable 'name'
	else
	{
LookVars_GetVar:
		if (IS_SYNTON || IS_ERROR) return; // only check syntax or error

		// find variable and load its value (p = pointer to first letter of name of variable; returns pointer to variable, or NULL if not found)
		// returns variable value in LastResult
		if (GetVar(p, NULL) == NULL)
		{
			VarNotFound(); // variable not found
			return;
		}
	}
}

// PEEK function (no error)
// - Input and output 'num' to LastResult.
// ZX80: L0C24
void FncPeek()
{
	// get input value
	val num = LastResult;

	// Frame counter (addr = -1)
	if (num == -1)
		num = (val)(u32)DispFrame;

	// Random seed (addr = -2)
	else if (num == -2)
		num = (val)(u32)RandSeed;

	// Time in 0.02 us (addr = -3)
	else if (num == -3)
		num = (val)(Time() - LastTime);

	// print position X (addr = -4)
	else if (num == -4)
		num = (val)PrintX;

	// print position Y (addr = -5)
	else if (num == -5)
		num = (val)PrintY;

	// buffers
	else if (num >= 0)
	{
		// get character from the screen (addr = 0..0x2FF)
		if (num < FRAMESIZE)
			num = (u8)FrameBuf[num];

		// get font (addr = 0xE00..0x1000)
		// Original ZX80 font is organised to 1 column of characters:
		//	B0..B2 = letter line 0..7
		//	B3..B8 = letter index 0..63
		// WCH80 font is organised by lines:
		//	B0..B5 = letter index 0..63
		//	B6..B8 = letter line 0..7
		else if ((num >= 0xE00) && (num < 0x1000))
		{
			num = ((num & 0x07) << 6) | ((num >> 3) & 0x3f);
			num = (u8)Font80[num];
		}
		else
			num = 0;
	}
	else
		num = 0;

	// store result
	LastResult = num;
}

#endif // USE_VAR_ASM
