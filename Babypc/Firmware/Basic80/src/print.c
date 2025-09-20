
// ****************************************************************************
//
//                             Print
//
// ****************************************************************************
// Print position is limited to (PrintX = WIDTH, PrintY = PrintYMax)

#include "../include.h"

#if !USE_PRINT_ASM		// 1=use ASM in Print module

// command CLS - clear display (no error)
// ZX80: L0747
NOMEMSET void CmdCls()
{
	int i;

	// clear screen
	u8* d = FrameBuf;
	u8 b = CH_SPC;
	for (i = FRAMESIZE; i > 0; i--) *d++ = b;

	// reset pointers
	PrintX = 0;
	PrintY = 0;
	PrintYMax = HEIGHT-1;
}

// print space character (returns False on display overflow)
Bool PrintSpc()
{
	return PrintCh(CH_SPC);
}

// print RAW character to display (returns False on display overflow)
// ZX80: L0560, RST 10H, L0010
Bool PrintCh(u8 ch)
{
	// disable leading spaces if character is space
	if (ch == CH_SPC) SET_NOLEAD;

	// new line
	int x = PrintX;
	int y = PrintY;
	if (x >= WIDTH)
	{
		x = 0;
		y++;
	}

	// check valid row
	if (y > PrintYMax) return False;

	// write character
	FrameBuf[x + y*WIDTHBYTE] = ch;

	// increase print position
	x++;
	PrintX = x;
	PrintY = y;
	return True;
}

// print character and enable leading spaces, if it is not a space (returns False on display overflow) (no error)
// ZX80: L0559
Bool PrintChLead(u8 ch)
{
	if (ch != CH_SPC) SET_LEAD;	// enable leading spaces, if the character is not a space
	return PrintCh(ch);
}

// print tabulator (returns False on display overflow)
// ZX80: L0727
Bool PrintTab()
{
	return PrintSpcTo((PrintX + 8) & ~7);
}

// print spaces to position (returns False on display overflow)
// - Continues on the next line if the position is beyond the end of the line.
Bool PrintSpcTo(int pos)
{
	return PrintSpcNum(pos - PrintX);
}

// print space characters with count (returns False on display overflow)
Bool PrintSpcNum(int num)
{
	for (; num > 0; num--)
	{
		if (!PrintSpc()) return False;
	}
	return True;
}

// print new line (returns False on display overflow)
// ZX80: L071B
Bool PrintCR()
{
	// print spaces to end of line
	Bool res = PrintSpcTo(WIDTH);

	// shift to next row (if will not overflow)
	if (PrintY < PrintYMax)
	{
		PrintY++;
		PrintX = 0;
	}
	return res;
}

// clear to end of display (no error)
// ZX80: L05C2
void ClrEod()
{
	while ((PrintY < PrintYMax) || (PrintX < WIDTH)) PrintCR();
}

// print token 0xD4..0xFF (returns False on display overflow) (no error)
// ZX80: L0584
Bool PrintToken(u8 ch)
{
	// search token (ZX80: L05A8)
	const u8* t = Tokens;
	int i = ch - TOKEN_FIRST;
	for (; i >= 0; i--)
	{
		do { ch = *t++; } while (ch < 0x80);
	}

	// print leading space if first character is alphabetic
	if (IS_LEAD && IsAlpha(*t & 0x7f))
	{
		if (!PrintSpc()) return False;
	}

	// print token
	u8 ch2;
	do {
		ch = *t++;
		ch2 = ch & 0x7f;
		if (!PrintChLead(ch2)) return False;
	} while (ch < 0x80);

	// if last character is alphanumeric, print space character
	if (IsAlphaNumDot(ch2)) return PrintSpc();
	return True;
}

// print number (returns False on display overflow) (no error)
// ZX80: L06A1
Bool PrintNum(val num)
{
	// decode number into temporary buffer, with ending CH_QUOT character (returns start of the number)
	u8* d = DecTempNum(num);

	// print string or token, terminated with CH_QUOT character
	return PrintString(d);
}

// print string or token, terminated with CH_QUOT character (returns False on display overflow) (no error)
// ZX80: L070C (begin L0709)
Bool PrintString(const char* txt)
{
	u8 ch;
	for (;;)
	{
		// get next character
		ch = *txt++;

		// end of string
		if (ch == CH_QUOT) break;

		// print token
		if (ch >= 0xc0)
		{
			if (!PrintToken(ch)) return False;
		}

		// print simple character
		else
		{
			if (!PrintCh(ch)) return False;
		}
	}
	return True;
}

// decode number into temporary buffer, with ending CH_QUOT character (returns start of the number) (no error)
u8* DecTempNum(val num)
{
	// absolute value
	Bool neg = False;
	u32 n = (u32)num;
	if (num < 0)
	{
		neg = True;
		n = (u32)(-(s32)num);
	}

	// decode number
	u8* d = DecTempUNum(n);

	// sign
	if (neg)
	{
		d--;
		*d = CH_MINUS;
	}
	return d;
}

// decode unsigned number into temporary buffer, with ending CH_QUOT character (returns start of the number) (no error)
// Note: Use FASTCODE attribute to ensure that division is calculated using faster 64-bit multiplication with inverted value 1/10.
FASTCODE u8* DecTempUNum(u32 num)
{
	// store ending character CH_QUOT
	u8* d = &Temp[TEMPNUM-1];
	*d = CH_QUOT;

	// decode number
	u32 k;
	do {
		// FASTCODE: use faster division using 64-bit multiplication
		k = num / 10;
//		k = (u32)(((u64)num * 0xCCCCCCCDU) >> 35);

		num -= k*10;
		d--;
		*d = (u8)(num + CH_0);
		num = k;
	} while (num != 0);

	return d;
}

// print BASIC line without line number (p = points after line number) (returns False on display overflow) (no error)
// ZX80: L0512
Bool PrintBas(const u8* p)
{
	u8 ch;

	// disable leading spaces
	SET_NOLEAD;

	while (True)
	{
		// print error flag [S]
		if (p == ErrPtr)
		{
			if (!PrintCh(CH_S + CH_INV)) return False;
		}

		// print [K] or [L] cursor
		if (p == CurPtr)
		{
			if (!PrintCh(IS_LMODE ? (CH_L + CH_INV) : (CH_K + CH_INV))) return False;
		}

		// load one character
		ch = *p++;

		// new line
		if (ch == NEWLINE)
		{
			if (PrintX < WIDTH)
			{
				if (!PrintCR()) return False;
			}
			break;
		}

		// print token
		else if (ch >= 0xc0)
		{
			if (!PrintToken(ch)) return False;
		}

		// print single character
		else
		{
			if (!PrintChLead(ch)) return False;
		}
	}
	return True;
}

// print BASIC line with line number (p = points to line number) (returns False on display overflow) (no error)
// ZX80: L04F7
NOMEMSET Bool PrintBasNum(const u8* p)
{
	// check end of program (if line number is 0)
	int line = GETLINE(p);
	if (line == 0) return True;
	p += 2;

	// print line number
	u8* d = DecTempUNum(line); // decode number to temporary buffer, with ending CH_QUOT character (returns start of the number)
	while (d > &Temp[TEMPNUM-5]) // align with spaces to 9999"
	{
		d--;
		*d = CH_SPC;
	}
	if (!PrintString(d)) return False;

	// check if this line is current edited line
	Bool curline = (line == CurLine);

	// print space or cursor
	if (!PrintCh(curline ? (CH_GT + CH_INV) : CH_SPC)) return False;

	// print program line
	return PrintBas(p);
}

// PRINT command (can raise errors)
// - Called during syntax checking from CLASS5.
// ZX80: L0972
void CmdPrint()
{
	u8 ch;
	val num;
	while (True)
	{
		// get next character
		ch = GetChar();

		// end line
		if (ch == NEWLINE)
		{
			// if not checking syntax
			if (IS_SYNTOFF)
			{
				// print new line
				if (!PrintCR()) DispError();
			}
			return;
		}

		// separator ',', print tabulator
		else if (ch == TOKEN_COMMA)
		{
			// skip separator
			CharAdd();

			// if not checking syntax
			if (IS_SYNTOFF)
			{
				// print tabulator
				if (!PrintTab())
				{
					DispError();
					return;
				}
			}

			// if "," is last character on the line, no CR will be printed
			if (GetChar() == NEWLINE) return;
		}

		// separator ';', do nothing
		else if (ch == TOKEN_SEMI)
		{
			// skip separator
			ch = NextChar();

			// if ";" is last character on the line, no CR will be printed
			if (ch == NEWLINE) return;
		}

		// expression
		else
		{
			// scanning calculator expression, store result to LastResult (can raise errors)
			ScanCalc();

			// break on error
			if (IS_ERROR) return;

			// print item - only if not checking syntax
			// ZX80: L06F1
			if (IS_SYNTOFF)
			{
				// get last result
				num = LastResult;

				// print string terminated with CH_QUOT
				if (IS_STRRES)
				{
					if (!PrintString(RAMPTR(num)))
					{
						DispError();
						return;
					}
				}

				// print number
				else
				{
					if (!PrintNum(num))
					{
						DispError();
						return;
					}
				}
			}
		}
	}
}

#endif // USE_PRINT_ASM
