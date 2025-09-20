
// ****************************************************************************
//
//                                  Life game
//
// ****************************************************************************

#include "../include.h"

// cell
typedef struct {
	u8	cell : 2;	// current cell
	u8	save : 2;	// save cell state
	u8	neigh : 4;	// number of neighbors
} sCell;

// game board (374 B)
sCell Board[MAPSIZE];

// cursor
int CurX, CurY;

// samples (first 2 bytes = width, height)
const u8 Samples[] = {

	// 0: Life
	5,5,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
/*
	// 1: penta-decathlon
	3,12,
	1,1,1,
	0,1,0,
	0,1,0,
	1,1,1,
	0,0,0,
	1,1,1,
	1,1,1,
	0,0,0,
	1,1,1,
	0,1,0,
	0,1,0,
	1,1,1,

	// 2: R-pentomino
	3,3,
	0,1,1,
	1,1,0,
	0,1,0,

	// 3: Diehard
	8,3,
	0,0,0,0,0,0,1,0,
	1,1,0,0,0,0,0,0,
	0,1,0,0,0,1,1,1,

	// 4. Acorn
	7,3,
	0,1,0,0,0,0,0,
	0,0,0,1,0,0,0,
	1,1,0,0,1,1,1,

	// 5: Glider
	3,3,
	0,0,1,
	1,0,1,
	0,1,1,

	// 6: spaceship
	5,4,
	1,0,0,1,0,
	0,0,0,0,1,
	1,0,0,0,1,
	0,1,1,1,1,

	// 7: pulsar
	15,15,
	0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,
	0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,
	0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
	0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,
	1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,

	// 8: Gosper glider gun
	38, 9,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,1,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,1,1,0,0,0,0,0, 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,1,0,0,0, 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,0,
	0,1,1,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,1,0,0, 0,1,1,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,0,
	0,1,1,0,0,0,0,0,0,0, 0,1,0,0,0,1,0,1,1,0, 0,0,0,1,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,1,0,0, 0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,1,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

	// 9: Simkin glider gun
	33,21,
	1,1,0,0,0,0,0,1,1,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	1,1,0,0,0,0,0,1,1,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,1,1,0,1,1,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,1,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,1,0, 0,1,1,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,1,1,1,0,0,0,1,0,0, 0,1,1,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,1,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,1,1,1,0,0,0,0,0,0, 0,0,0,
	0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,0,0, 0,0,0,
*/
};

// draw cell
void DrawCell(int x, int y)
{
	int b = Board[(y+MAPY)*MAPW0 + x+MAPX].cell;
	x = x*TILESIZE;
	y = y*TILESIZE;
	switch (b)
	{
	case TILE_EMPTY:	// empty tile
		DrawImgBg(ImgTileEmpty, x, y, TILESIZE, TILESIZE, TILE_WB, COL_WHITE, COL_BLACK);
		break;

	case TILE_FULL:		// full tile
		DrawImgBg(ImgTileCell, x, y, TILESIZE, TILESIZE, TILE_WB, COL_WHITE, COL_BLACK);
		break;

	case TILE_SELEMPTY:	// selected empty tile
		DrawImgBg(ImgTileEmptySel, x, y, TILESIZE, TILESIZE, TILE_WB, COL_WHITE, COL_BLACK);
		break;

	case TILE_SELFULL:	// selected full tile
		DrawImgBg(ImgTileCellSel, x, y, TILESIZE, TILESIZE, TILE_WB, COL_WHITE, COL_BLACK);
		break;
	}
}

// draw cell with cursor
void DrawCur()
{
	DrawCell(CurX-MAPX, CurY-MAPY);
}

// draw board
void DrawBoard()
{
	int i, j, x, y, b;
	for (i = 0; i < MAPH; i++)
	{
		for (j = 0; j < MAPW; j++)
		{
			DrawCell(j, i);
		}
	}
}

// set cursor on
void CurOn()
{
	sCell* d = &Board[CurX + CurY*MAPW0];
	u8 b = d->cell;
	if (b == TILE_FULL)
		b = TILE_SELFULL;
	else if (b == TILE_EMPTY)
		b = TILE_SELEMPTY;
	d->cell = b;
	DrawCur();
}

// set cursor off
void CurOff()
{
	sCell* d = &Board[CurX + CurY*MAPW0];
	u8 b = d->cell;
	if (b == TILE_SELFULL)
		b = TILE_FULL;
	else if (b == TILE_SELEMPTY)
		b = TILE_EMPTY;
	d->cell = b;
	DrawCur();
}

// flip cursor
void CurFlip()
{
	sCell* d = &Board[CurX + CurY*MAPW0];
	u8 b = d->cell;
	if (b == TILE_SELFULL)
		b = TILE_SELEMPTY;
	else if (b == TILE_SELEMPTY)
		b = TILE_SELFULL;
	d->cell = b;
	DrawCur();
}

// get cell on coordinates (0 or 1)
int Cell(int x, int y)
{
	while (x < 0) x += MAPW0;
	while (x >= MAPW0) x -= MAPW0;
	while (y < 0) y += MAPH0;
	while (y >= MAPH0) y -= MAPH0;
	sCell* d = &Board[x + y*MAPW0];
	u8 b = d->cell;
	return (b == TILE_FULL) ? 1 : 0;
}

// save current slot
void SaveSlot()
{
	int i;
	for (i = 0; i < MAPSIZE; i++) Board[i].save = Board[i].cell;
}

// load current slot
void LoadSlot()
{
	int i;
	for (i = 0; i < MAPSIZE; i++) Board[i].cell = Board[i].save;
}

// run life
void Run()
{
	int x, y;
	u8 b;
	sCell* d;

	KeyFlush();

	// break with a key
	while (JoyGet() == NOKEY)
	{
		// calculate neighbors
		d = Board;
		for (y = 0; y < MAPH0; y++)
		{
			for (x = 0; x < MAPW0; x++)
			{
				d->neigh =
					Cell(x-1, y-1) +
					Cell(x  , y-1) +
					Cell(x+1, y-1) +

					Cell(x-1, y  ) +
					Cell(x+1, y  ) +

					Cell(x-1, y+1) +
					Cell(x  , y+1) +
					Cell(x+1, y+1);
				d++;
			}
		}

		// update cells
		d = Board;
		for (x = MAPSIZE; x > 0; x--)
		{
			b = d->neigh;

			// 3 -> new cell
			if (b == 3) 
				d->cell = TILE_FULL;

			// 2 -> unchanged
			else if (b == 2)
				;
			// else -> dead
			else
				d->cell = TILE_EMPTY;
			d++;
		}

		DrawBoard();

		// delay
		WaitMs(SPEED);
	}
}

// initialize slots by samples
void InitSlot()
{
	sCell* d;
	u8 b;
	const u8* s = Samples;
	int i, w, h;

	// sample dimension
	w = *s++;
	h = *s++;

	// center image
	d = &Board[(MAPW0-w)/2 + (MAPH0-h)/2*MAPW0];

	// copy sample
	for (; h > 0; h--)
	{
		for (i = 0; i < w; i++)
		{
			d[i].cell = s[i];
		}
		d += MAPW0;
		s += w;
	}
}

int main(void)
{
	int i;

	// display splash screen
	KeyWaitNoPressed();
	DrawImgBg(ImgIntro, 0, 0, WIDTH, HEIGHT, WIDTHBYTE, COL_WHITE, COL_BLACK);
	while (JoyGet() == NOKEY) {}
	DrawClear();

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	char ch;

	// clear buffer
	for (i = 0; i < MAPSIZE; i++) Board[i].cell = TILE_EMPTY;
	CurX = MAPW0/2;
	CurY = MAPH0/2;

	// fill slots with samples
	InitSlot();

	// draw board
	DrawBoard();

	// set cursor on
	CurOn();

	// loop with demo scene
	while (True)
	{
		// keyboard
		ch = JoyGet();
		if (ch != NOKEY)
		{
			switch (ch)
			{
			// right
			case KEY_RIGHT:
				CurOff(); // set cursor off
				CurX++;
				if (CurX >= MAPX+MAPW) CurX = MAPX;
				CurOn(); // set cursor on
				break;

			// left
			case KEY_LEFT:
				CurOff(); // set cursor off
				CurX--;
				if (CurX < MAPX) CurX = MAPX+MAPW-1;
				CurOn(); // set cursor on
				break;

			// down
			case KEY_DOWN:
				CurOff(); // set cursor off
				CurY++;
				if (CurY >= MAPY+MAPH) CurY = MAPY;
				CurOn(); // set cursor on
				break;

			// up
			case KEY_UP:
				CurOff(); // set cursor off
				CurY--;
				if (CurY < MAPY) CurY = MAPY+MAPH-1;
				CurOn(); // set cursor on
				break;
		
			// toggle
			case KEY_A:
				CurFlip(); // flip cursor
				break;

			// run
			case KEY_B:
#if USE_SCREENSHOT		// use screen shots
				ScreenShot();
#endif
				CurOff(); // set cursor off
				SaveSlot(); // save state
				Run(); // run life
				LoadSlot(); // restore state
				CurOn(); // set cursor on
				DrawBoard();
				break;
			}
		}
	}
}
