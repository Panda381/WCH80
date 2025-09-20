
// ****************************************************************************
//
//                                 Select
//
// ****************************************************************************

#include "../include.h"

const char SelLevel[] = "Select Level";	// 12 chars

#define LEVW	4	// width of one level
#define ROWLEV	(TEXTW/LEVW) // number of level columns per row (= 10)

// print 1 level
void LevSelect1(int inx)
{
	// prepare coordinates on screen
	int n = inx/ROWLEV;
	PrintRow = 2*n + 2;
	PrintPos = (inx - n*ROWLEV)*LEVW;

	// prepare color
	PrintChar((inx == Level) ? '\2' : '\1');

	// display number
	inx++;
	char buf[5];
	n = DecNum(buf, inx, 0);
	if (n < 3) PrintSpc();
	if (n < 2) PrintSpc();
	PrintText(buf);
	PrintSpc();
}

// select level (returns True = OK, False = Esc)
Bool LevSelect()
{
	// draw title
	PrintText("\f\2");
	PrintSpcRep(14);
	PrintText(SelLevel);
	PrintSpcRep(14);
	PrintText("\1");

	// print list of levels
	int i;
	for (i = 0; i < LevNum; i++)
	{
		LevSelect1(i);
	}

	// select
	char c;
	int j;
	for (;;)
	{
		// get character
		c = WaitChar();

		switch (c)
		{
		case KEY_UP:
			i = Level;
			Level = -1;
			LevSelect1(i);
			i -= ROWLEV;
			if (i < 0)
			{
				do i += ROWLEV; while (i < LevNum);
				i -= ROWLEV;
				if (i < 0) i += ROWLEV;
			}
			Level = i;
			LevSelect1(i);

//			LevDef = Levels[i*2]; // current level definition
//			LevSolve = Levels[i*2+1]; // current level solve
			break;

		case KEY_LEFT:
			i = Level;
			Level = -1;
			LevSelect1(i);
			i--;
			if (i < 0) i = LevNum-1;
			Level = i;
			LevSelect1(i);

//			LevDef = Levels[i*2]; // current level definition
//			LevSolve = Levels[i*2+1]; // current level solve
			break;

		case KEY_DOWN:
			i = Level;
			Level = -1;
			LevSelect1(i);
			i += ROWLEV;
			if (i >= LevNum)
			{
				do i -= ROWLEV; while (i >= 0);
				i += ROWLEV;
				if (i >= LevNum) i -= ROWLEV;
			}
			Level = i;
			LevSelect1(i);

//			LevDef = Levels[i*2]; // current level definition
//			LevSolve = Levels[i*2+1]; // current level solve
			break;

		case KEY_RIGHT:
			i = Level;
			Level = -1;
			LevSelect1(i);
			i++;
			if (i > LevNum-1) i = 0;
			Level = i;
			LevSelect1(i);

//			LevDef = Levels[i*2]; // current level definition
//			LevSolve = Levels[i*2+1]; // current level solve
			break;

		case KEY_A: // select
//			LevDef = Levels[i*2]; // current level definition
//			LevSolve = Levels[i*2+1]; // current level solve
			return True;

//		case KEY_X:
//		case KEY_Y:
//		case KEY_B: // Esc
//			return False;
		}
	}
}
