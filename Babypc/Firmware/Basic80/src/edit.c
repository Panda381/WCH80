
// ****************************************************************************
//
//                                  Edit
//
// ****************************************************************************

#include "../include.h"

#if !USE_EDIT_ASM		// 1=use ASM in edit module

// Delete whole edit line (no error)
// ZX80: L0D0A
void ClrEdit()
{
	EditNum = 1;
	Edit[0] = NEWLINE;
	CurPtr = Edit;
}

// INPUT command (can raise errors)
// ZX80: L099A
void CmdInput()
{
	// error if program is not running
	if ((ProgLine == 0) || (ProgLine == 0xffff))
	{
		InputError();
		return;
	}

	// set input mode
	SET_INPUT;

	// clear edit line
	ClrEdit();

	// string mode
	Bool str = IS_STRRES;
	if (str)
	{
		EditNum = 3;
		Edit[0] = CH_QUOT;
		Edit[1] = CH_QUOT;
		Edit[2] = NEWLINE;
		CurPtr = Edit + 1;
	}

	// save program pointer and screen
	u8* p = CharPtr;
	u8 y = PrintY;
	u8 x = PrintX;

	// set lower screen to remaining rows
	int h = HEIGHT-1 - y;
	if (h <= 0) h = 1;
	LowScreenH = h;

	// edit
	DoEdit();

	// clear edit
	PrintY = HEIGHT - h;
	PrintX = 0;
	ClrEod();	// clear to end of display

	// restore program pointer and screen
	PrintY = y;
	PrintX = x;
	PrintYMax = HEIGHT-1;
	CharPtr = p;
	SET_NEXT;	// continue to next program line
	SET_EDIT;	// set edit mode
}

// add BASIC line to the program (CharPtr = start of program line;
// does not raise error - ignores command on memory overflow; line must not be 0)
// ZX80: L04BA
void MainAdd(u16 line)
{
	u8 *p, *p2, *s, *d, *prev, ch;
	int i, len;
	u16 line2;

	// set new edit line
	CurLine = line;

	// find program line (exact, or higher, or end of program)
	p = FindLine(line, &prev);

	// get line number (= 0 on end of program)
	line2 = GETLINE(p);

	// delete old line
	if (line == line2)
	{
		// shift to next program line, to get length of old program line
		p2 = NextLine(p);

		// delete old line
		i = &Prog[ProgNum] - p2; // length of rest of the program
		ProgNum -= p2 - p;	// new size of the program
		d = p;
		for (; i > 0; i--) *d++ = *p2++;
	}

	// skip spaces
	ch = GetChar();

	// line is empty, only deleting old program line
	if (ch == NEWLINE)
	{
		line = GETLINE(p);
		if (line == 0) // last row was deleted
		{
			line = 1;
			if (prev != NULL) line = GETLINE(prev);
		}
		CurLine = line; // new current edited line
		return;
	}

	// get size of new program line
	s = CharPtr; // pointer to edit line
	len = &Edit[EditNum] - s; // size of program line

	// check memory (does not rise error, only ignore operation)
	if (len + 2 > PROGNUM - ProgNum) return;

	// prepare free space
	p2 = &Prog[ProgNum-1];	// pointer to old last byte
	i = p2 - p + 1;		// length of data to move
	d = p2 + len + 2;	// pointer to new last byte
	ProgNum += len + 2;	// new size of the program
	for (; i > 0; i--) *d-- = *p2--; // move data

	// store line number
	SETLINE(p, line);

	// move program code
	p += 2;
	for (; len > 0; len--) *p++ = *s++;
}

// edit (flag FLAG_INPUT determines if inputting data or editing code) (can raise errors)
// ZX80: L02F7
void DoEdit()
{
	u8 ch;
	int i;
	u8 *cur, *p, *p2, *d;

	while (True)
	{
		// clear error pointer
		ErrPtr = NULL;
		ErrNr = ERR_OK;

		// set flags
		SET_NOLEAD;	// no leading space
		SET_SYNTON;	// syntax on
		SET_NEXT;	// next program line

		// check syntax of current line
		CalcNum = 0;
		StckNum = 0;
		MainGo(Edit);

		// prepare printing to lower screen
		PrintX = 0;
		PrintY = HEIGHT - LowScreenH;
		PrintYMax = HEIGHT - 1;
		if (IS_EDIT) PrintCR(); // clear first empty line

		// print edit line (returns False on screen overflow)
		//  printed without line number - line number is now entered as text digits, not binary number
		if (!PrintBas(Edit))
		{
			if (LowScreenH < HEIGHT-2)
			{
				// overflow - increase height of lower screen
				LowScreenH++;

				// restart auto-list
				if (IS_EDIT) AutoList();
				continue; // restart
			}
		}

		// clear to end of display
		ClrEod();

		// get key from keyboard
		do {
			ch = CharGet();
		} while (ch == CH_NONE);

		// editing keys
		cur = CurPtr; // current pointer to cursor

		// ZX80: L035E
		if ((ch >= 0x70) && (ch <= 0x77)) // control keys (arrows, ...)
		{
			// ZX80: L035E, L0372
			switch (ch)
			{
			// ZX80: L03A9
			case CH_UP: // 0x70 // up
				if (IS_EDIT) // only if editing
				{
					int line = CurLine; // current edited program line
					u8* prev;
					u8* p = FindLine(line, &prev); // find current and previous line
					if (prev != NULL) // if not start
					{
						CurLine = GETLINE(prev); // get previous line
						AutoList(); // repaint screen
					}
				}
				break;

			// ZX80: L02D5
			case CH_DOWN: // 0x71 // down
				if (IS_EDIT) // only if editing
				{
					int line = CurLine + 1; // next edited program line
					u8* p = FindLine(line, NULL);
					line = GETLINE(p); // get new line
					if (line != 0) // if not end
					{
						CurLine = line;
						AutoList(); // repaint screen
					}
				}
				break;

			// ZX80: L0382
			case CH_LEFT: // 0x72 // left
				if (cur > Edit) CurPtr = cur - 1;
				break;

			// ZX80: L0387
			case CH_RIGHT: // 0x73 // right
				if (cur < &Edit[EditNum-1]) CurPtr = cur + 1;
				break;

			// ZX80: L03B9
			case CH_HOME: // 0x74 // home
				if (IS_EDIT) // only if editing
				{
					CurLine = 1; // go to lowest line
					TopLine = 1;
					AutoList(); // repaint screen
				}
				break;

			// ZX80: L03CB
			case CH_EDIT: // 0x75 // edit
				if (IS_EDIT) // only if editing
				{
					// find current edited program line
					p = FindCurLine();

					// get line
					int line = GETLINE(p);

					// check if line is valid (0 = end of program)
					if (line != 0)
					{
						CurLine = line;

						// decode line number to temporary buffer
						p2 = DecTempUNum((u16)line);
						d = Edit;
						for (;;) // transfer line number to edit buffer
						{
							ch = *p2++;
							if (ch == CH_QUOT) break;
							*d++ = ch;
						}
						CurPtr = d;

						// transfer program line
						p += 2; // skip line number
						p2 = &Edit[EDITNUM-1]; // max. destination address
						for (;;)
						{
							ch = *p++;
							*d++ = ch;
							if (ch == NEWLINE) break;
							if (d == p2)
							{
								*d++ = NEWLINE;
								break;
							}
						}
						EditNum = d - Edit; // set length of edit line
						AutoList(); // repaint screen
					}
				}
				break;

			// ZX80: L0408
			case NEWLINE: // 0x76 // NEWLINE

				// check syntax error (cannot continue on syntax error)
				if (!IS_SYNTERR)
				{
					// prepare to parse
					CharPtr = Edit;

					// skip spaces
					GetChar();

					// check if editing 
					if (IS_EDIT)
					{
						// parse line number (no error checking)
						int line = ParseUNum();

						// check line number and add code to the program
						if (line > 0)
						{
							if (line < 0xffff) MainAdd(line);

							// clear edit line
							ClrEdit();
							LowScreenH = 2;		// height of lower screen (number of rows)
							AutoList();
							return;
						}

						// line is 0, direct execution of the program line
						else
						{
							// flag of direct execution of the program
							ProgLine = 0xffff;

							// clear screen
							CmdCls();

							// empty command - do listing
							if (GetChar() == NEWLINE)
							{
								ProgLine = 0;
								ClrEdit();
								LowScreenH = 2;		// height of lower screen (number of rows)
								AutoList();
								return;
							}

						}
					} // if (IS_EDIT)

					// clear error flag
					ErrPtr = NULL; // no syntax error pointer
					ErrNr = ERR_OK; // no error
		
					// set flags
					SET_LEAD;	// leading space
					SET_SYNTOFF;	// syntax off
					SET_GOTO;	// goto program line

					// execute command
					MainGo(Edit);

					// clear edit line (edit line can contain temporary strings)
					ClrEdit();
					LowScreenH = 2;		// height of lower screen (number of rows)
					return;
				}
				break;

			// ZX80: L0395
			case CH_DELETE: // 0x77 // delete (rubout)
				if (cur > Edit)
				{
					i = EditNum - 1; // new number of characters
					EditNum = i; // set new length of edit line
					p = &Edit[i]; // pointer to last old character
					cur--; // shift cursor 1 position left
					CurPtr = cur; // set new cursor position
// use "volatile" so that the compiler uses a program loop instead of memmove, as this is more memory efficient.
					volatile u8* p2 = cur;
					do { p2[0] = p2[1]; p2++; } while (p2 != p); // delete 1 character
				}
				break;

			default:
				break;
			}
		} // if ((ch >= 0x70) && (ch <= 0x77)) // control keys (arrows, ...)

		// insert character at cursor position
		else
		{
			// check number of characters
			i = EditNum;
			if (i < EDITNUM)
			{
// use "volatile" so that the compiler uses a program loop instead of memmove, as this is more memory efficient.
				volatile u8* p = &Edit[i]; // pointer to new end of edit line (= last character + 1)
				i++; // new number of characters
				EditNum = i; // set new number of characters
				do {
					p--; // shift to previous old character
					p[1] = p[0]; // move 1 character
				} while (p != cur); // until current position reached
				p[0] = ch; // store new character
				CurPtr = cur + 1; // shift cursor 1 position right
			}
		}
	}
}

#endif // USE_EDIT_ASM
