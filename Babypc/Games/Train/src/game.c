
// ****************************************************************************
//
//                                Game engine
//
// ****************************************************************************

#include "../include.h"

// buffers
u8 Board[MAPSIZE];	// game board
u8 Dir[MAPSIZE];	// direction map DIR_*
u8 PswBuf[PSWLEN];	// password buffer

// current game state
u8 Level;		// current level
u8 HeadX, HeadY;	// head (loco) coordinates
u8 GateX, GateY;	// gate coordinates
u16 Length;		// length of the (including loco)
u16 ItemNum;		// number of items on game board
u8 State;		// game state S_*
u8 Phase;		// current animation phase (0, 1, 2)
u8 CurDir;		// current direction DIR_*
int Score;		// current score
u32 LastTime;		// time of last step

// key buffers for 2 keys (NOKEY=no key)
char KeyBuf1, KeyBuf2;

// texts
const char LevelTxt[] = "LEVEL";
const char PswTxt[] = "password";
const char LevelHlp[] = "A:PSW";
const char Psw2Txt[] = "PSW:";
const char OkHlp[] = "A:OK";
const char EscHlp[] = "B:CANCEL";
const char CongTxt[] = "CONGRATULATIONS!";
const char Cong2Txt[] = "You became absolute";
const char Cong3Txt[] = "winner of the game!";
const char ScoreHlp[] = "SCORE";

// step sound
const sMelodyNote StepSound[] = {
	{ 1, NOTE_C3 },
	{ 0, 0 },
};

// collect sound
const sMelodyNote CollectSound[] = {
	{ 1, NOTE_G3 },
	{ 1, NOTE_A3 },
	{ 0, 0 },
};

// crash sound
const sMelodyNote CrashSound[] = {
	{ 2, NOTE_C4 },
	{ 2, NOTE_B3 },
	{ 2, NOTE_AS3 },
	{ 2, NOTE_A3 },
	{ 2, NOTE_GS3 },
	{ 2, NOTE_G3 },
	{ 2, NOTE_FS3 },
	{ 2, NOTE_F3 },
	{ 2, NOTE_E3 },
	{ 2, NOTE_DS3 },
	{ 2, NOTE_D3 },
	{ 2, NOTE_CS3 },
	{ 2, NOTE_C3 },
	{ 2, NOTE_B2 },
	{ 2, NOTE_AS2 },
	{ 2, NOTE_A2 },
	{ 2, NOTE_GS2 },
	{ 2, NOTE_G2 },
	{ 2, NOTE_FS2 },
	{ 2, NOTE_F2 },
	{ 2, NOTE_E2 },
	{ 2, NOTE_DS2 },
	{ 2, NOTE_D2 },
	{ 2, NOTE_CS2 },
	{ 2, NOTE_C2 },
	{ 0, 0 },
};

// success sound
const sMelodyNote SuccessSound[] = {
	{ NOTE_LEN8, NOTE_E4 },
	{ NOTE_LEN8, NOTE_R },
	{ NOTE_LEN8, NOTE_E4 },
	{ NOTE_LEN2, NOTE_A4 },
	{ 0, 0 },
};

// tiles source coordinates X, Y
#define WH	TILESIZE
const u16 TileXY[TILESNUM*2] = {
	0*WH,	0*WH,	// CRY	(ITEM+0)	// crystal
	1*WH,	0*WH,	// CRO	(ITEM+1)	// crown
	2*WH,	0*WH,	// TRE	(ITEM+2)	// tree
	3*WH,	0*WH,	// APL	(ITEM+3)	// apple
	4*WH,	0*WH,	// COW	(ITEM+4)	// cow
	5*WH,	0*WH,	// CHE	(ITEM+5)	// cherry
	6*WH,	0*WH,	// POL	(ITEM+6)	// pool
	7*WH,	0*WH,	// GIR	(ITEM+7)	// giraffe
	8*WH,	0*WH,	// ICE	(ITEM+8)	// ice cream
	9*WH,	0*WH,	// CAK	(ITEM+9)	// cake
	10*WH,	0*WH,	// COM	(ITEM+10)	// computer
	11*WH,	0*WH,	// CAR	(ITEM+11)	// car
	12*WH,	0*WH,	// BAL	(ITEM+12)	// balloon
	13*WH,	0*WH,	// CLO	(ITEM+13)	// alarm clock
	14*WH,	0*WH,	// ELE	(ITEM+14)	// elephant
	15*WH,	0*WH,	// GLA	(ITEM+15)	// drinking glass
	16*WH,	0*WH,	// MON	(ITEM+16)	// money
	17*WH,	0*WH,	// AIR	(ITEM+17)	// airplane
	18*WH,	0*WH,	// LEM	(ITEM+18)	// Lemmings
	19*WH,	0*WH,	// EMPTY	19		// empty tile
	0*WH,	1*WH,	// WAGON L	20		// wagons
	1*WH,	1*WH,	// WAGON U	21		// wagons
	2*WH,	1*WH,	// WAGON R	22		// wagons
	3*WH,	1*WH,	// WAGON D	23		// wagons
	4*WH,	1*WH,	// LOCO_L	24		// locomotive left
	5*WH,	1*WH,	// LOCO_U	25		// locomotive up
	6*WH,	1*WH,	// LOCO_R	26		// locomotive right
	7*WH,	1*WH,	// LOCO_D	27		// locomotive down
	8*WH,	1*WH,	// GATE	28		// gate
	9*WH,	1*WH,	// GATE	29		// gate
	10*WH,	1*WH,	// GATE	30		// gate
	11*WH,	1*WH,	// GATE	31		// gate
	12*WH,	1*WH,	// GATE	32		// gate
	13*WH,	1*WH,	// GATE	33		// gate
	14*WH,	1*WH,	// WALL	34		// wall tile
	15*WH,	1*WH,	// TITLE	35		// title
	16*WH,	1*WH,	// TITLE	36		// title
	17*WH,	1*WH,	// TITLE	37		// title
	18*WH,	1*WH,	// TITLE	38		// title
	19*WH,	1*WH,	// TITLE	39		// title
	0*WH,	2*WH,	// CRASH	40		// crash
	1*WH,	2*WH,	// CRASH	41		// crash
	2*WH,	2*WH,	// CRASH	42		// crash
	3*WH,	2*WH,	// CRASH	43		// crash
	4*WH,	2*WH,	// CRASH	44		// crash
	5*WH,	2*WH,	// CRASH	45		// crash
	6*WH,	2*WH,	// CRASH	46		// crash
	7*WH,	2*WH,	// CRASH	47		// crash
	8*WH,	2*WH,	// CRASH	48		// crash
	9*WH,	2*WH,	// CRASH	49		// crash
};

// wait step
void WaitStep()
{
	u32 t;
	for (;;)
	{
		t = Time();
		if ((u32)(t - LastTime) >= GAMESPEED*1000*HCLK_PER_US) break;
	}
	LastTime = t;
}

// clear info row
void InfoClear()
{
	DrawRectClr(0, INFOY, WIDTH, 8);
}

// display text to info row
void InfoDispText(int x, const char* txt, u16 col)
{
	DrawTextCond6Bg(txt, x, INFOY, col, COL_BLACK);
}

// display info bottom row
void InfoRow()
{
	// display score
	InfoDispText(0, ScoreHlp, COL_WHITE); // 5*6=30 pixels title "SCORE"
	DrawRectClr(30, INFOY, 2, 8); // end on X = 32
	char buf[8];
	int n = DecUNum(buf, Score, 0); // 50 levels * 240 * 10 = 120000 max, 5 digits enough = 30 pixels
	InfoDispText(32, buf, COL_WHITE); // end on X = 32+30 = 62

	// space
	DrawRectClr(32 + n*6, INFOY, 64 - 32 - n*6, 8);
	
	// display text A:PSW (30 pixels, end on X = 64+30 = 94)
	InfoDispText(64, LevelHlp, COL_WHITE);

	// space, start on X = 94, end on 116
	DrawRectClr(94, INFOY, WIDTH - 2*6 - 2 - 30 - 94, 8);

	// display level
	InfoDispText(WIDTH - 2*6 - 2 - 30, LevelTxt, COL_WHITE); // length "LEVEL" = 30 pixels, start on X=116, end on X=146
	DrawRectClr(WIDTH - 2*6 - 2, INFOY, 2, 8); // start on X=146, end on X=148
	n = DecUNum(buf, Level, 0);
	InfoDispText(WIDTH - 2*6, buf, COL_WHITE); // start on X=148
	if (n == 1) DrawRectClr(WIDTH - 6, INFOY, 6, 8);
}

// enter password
void Psw()
{
	// display help
	InfoClear();

	// display help "A:OK", "B:CANCEL"
	InfoDispText(0, OkHlp, COL_WHITE);
	InfoDispText(WIDTH - (sizeof(EscHlp)-1)*6, EscHlp, COL_WHITE);

	// load password
	char buf[PSWLEN+1];
	memcpy(buf, &LevelPsw[Level*PSWLEN], PSWLEN);
	buf[PSWLEN] = 0;

	// display password
	int x = 40;
	InfoDispText(x, Psw2Txt, COL_WHITE);
	x += (sizeof(Psw2Txt) - 1)*6;

	// edit password
	int pos = 0;
	char c;
	char buf2[2];
	buf2[1] = 0;
	for (;;)
	{
		// display password
		if (((Time() >> 23) & 1) == 0)
			InfoDispText(x, buf, COL_WHITE);
		else
		{
			buf2[0] = buf[pos];
			PrintInv = 128;
			InfoDispText(x + pos*6, buf2, COL_WHITE);
			PrintInv = 0;
		}

		// input keys
		c = JoyGet();

		// Esc break
		if ((c == KEY_B) || (c == KEY_X))
		{
			InfoRow();
			return;
		}

		// OK enter
		if (c == KEY_A)
		{
			// search password
			int i;
			for (i = 1; i <= LEVNUM; i++)
			{
				// check password
				if (memcmp(buf, &LevelPsw[i*PSWLEN], PSWLEN) == 0)
				{
					// set new level
					Level = i;
					break;
				}
			}

			// restart this scene
			InfoRow();
			OpenLevel();
			InitLevel();
			return;
		}

		// Left
		if (c == KEY_LEFT)
		{
			pos--;
			if (pos < 0) pos = PSWLEN-1;
		}

		// right
		if (c == KEY_RIGHT)
		{
			pos++;
			if (pos >= PSWLEN) pos = 0;
		}

		// up
		if (c == KEY_UP)
		{
			buf[pos]++;
			if ((u8)buf[pos] > (u8)'Z') buf[pos] = 'A';
		}

		// down
		if (c == KEY_DOWN)
		{
			buf[pos]--;
			if ((u8)buf[pos] < (u8)'A') buf[pos] = 'Z';
		}
	}
}

// display one tile of the board
void DispTile(u8 x, u8 y)
{
	u8 tile = Board[x + y*MAPW];
// draw image fast - all coordinates and dimensions must be multiply of bytes and must be valid
//void DrawImgInvFast(const u8* img, int x, int y, int xs, int ys, int w, int h, int wsb);
	DrawImgInvFast(ImgTiles, x*TILESIZE, y*TILESIZE + MAPY,
		TileXY[tile*2], TileXY[tile*2+1], TILESIZE, TILESIZE, 20);
}

// display all tiles
void DispBoard()
{
	int x, y;
	for (y = 0; y < MAPH; y++)
	{
		for (x = 0; x < MAPW; x++) DispTile(x, y);
	}
}

// put tile on the board
void PutTile(u8 x, u8 y, u8 tile)
{
	if (Board[x + y*MAPW] != tile)
	{
		Board[x + y*MAPW] = tile;
		DispTile(x, y);
	}
}

// get tile from board
u8 GetTile(u8 x, u8 y) { return Board[x + y*MAPW]; }

// clear level screen
void ClearLevel(u8 tile)
{
	u8 x, y;

	// clear screen with bricks
	for (x = 0; x < MAPW; x++)
	{
		for(y = 0; y < MAPH; y++) PutTile(x, y, tile);
		WaitMs(20);
	}
}

// congratulate
void CongLevel()
{
	// clear screen with bricks
	ClearLevel(WALL);

	// set info box ON
	DrawRectClr(BOXX-3*6, BOXY, BOXW+6*6, BOXH);

	// display frame
	DrawFrame(BOXX-1-3*6, BOXY-1, BOXW+2+6*6, BOXH+2, COL_WHITE);
	DrawFrame(BOXX-2-3*6, BOXY-2, BOXW+4+6*6, BOXH+4, COL_WHITE);

	// display text
	DrawTextCond6Bg(CongTxt, (WIDTH - (sizeof(CongTxt)-1)*6)/2, BOXY + 8, COL_WHITE, COL_BLACK);
	DrawTextCond6Bg(Cong2Txt, (WIDTH - (sizeof(Cong2Txt)-1)*6)/2, BOXY + 2*8, COL_WHITE, COL_BLACK);
	DrawTextCond6Bg(Cong3Txt, (WIDTH - (sizeof(Cong3Txt)-1)*6)/2, BOXY + 3*8, COL_WHITE, COL_BLACK);

	// display info row
	InfoRow();

	// wait
	WaitMs(250);
	KeyFlush();
	while (JoyGet() == NOKEY) {}

	// clear screen with empty
	ClearLevel(EMPTY);
}

// open level
void OpenLevel()
{
	// clear screen with bricks
	ClearLevel(WALL);

	// set info box ON
	DrawRectClr(BOXX, BOXY, BOXW, BOXH);

	// display frame
	DrawFrame(BOXX-1, BOXY-1, BOXW+2, BOXH+2, COL_WHITE);
	DrawFrame(BOXX-2, BOXY-2, BOXW+4, BOXH+4, COL_WHITE);

	// display current scene
	int x, x2;
	char buf[10];
	int n = DecUNum(buf, Level, 0);
	x = (WIDTH - (sizeof(LevelTxt)+n)*6)/2;
	DrawTextCond6Bg(LevelTxt, x, BOXY + 8, COL_WHITE, COL_BLACK);
	x += sizeof(LevelTxt)*6;
	DrawTextCond6Bg(buf, x, BOXY + 8, COL_WHITE, COL_BLACK);

	// display password
	x = (WIDTH - (sizeof(PswTxt)+PSWLEN)*6)/2;
	DrawTextCond6Bg(PswTxt, x, BOXY + 3*8, COL_WHITE, COL_BLACK);
	memcpy(buf, &LevelPsw[Level*PSWLEN], PSWLEN);
	buf[PSWLEN] = 0;
	x += sizeof(PswTxt)*6;
	DrawTextCond6Bg(buf, x, BOXY + 3*8, COL_WHITE, COL_BLACK);

	// display info row
	InfoRow();

	// wait
	WaitMs(250);
	KeyFlush();
#ifndef AUTOMODE // automode - run levels to check solutions (help completes scene)
	while (JoyGet() == NOKEY) {}
#endif

	// clear screen with empty
	ClearLevel(EMPTY);
}

// initialize current level
void InitLevel()
{
	// copy items to game board
	u8 b;
	int i, j;
	u8* dst = Board;
	const u8* src = &Levels[Level*(MAPW/2)*MAPH];
	for (i = 0; i < MAPH; i++)
	{
		for (j = 0; j < MAPW; j++)
		{
			b = *src;
			if ((j & 1) != 0)
			{
				b >>= 4;
				src++;
			}
			b &= 0x0f;
			if (b == EMP)
				b = EMPTY;
			else if (b == WAL)
				b = WALL;
			else if (b == LOC)
				b = LOCO_R;
			else if (b == GAT)
				b = GATE;
			*dst++ = b;
		}
	}

	// analyse board
	int x, y;
	ItemNum = 0;
	for (y = 0; y < MAPH; y++)
	{
		for (x = 0; x < MAPW; x++)
		{
			// get item from board
			b = Board[x+y*MAPW];

			// locomotive
			if ((b >= LOCOMIN) && (b <= LOCOMAX))
			{
				HeadX = x;
				HeadY = y;
			}

			// gate
			else if (b == GATE)
			{
				GateX = x;
				GateY = y;
			}

			// item
			else if ((b >= ITEMMIN) && (b <= ITEMMAX))
			{
				ItemNum++;
			}
		}
	}

	// reset variables
	State = S_WAIT;
	Length = 1;
	CurDir = DIR_R;
	Phase = 0;
	KeyBuf1 = NOKEY;
	KeyBuf2 = NOKEY;

	// display info row
	if (Level == 0)
		InfoClear();
	else
		InfoRow();

	// display board
	DispBoard();

	// time of last step
	LastTime = Time();
}

// train step
void StepLevel()
{
	// next position
	s8 x = HeadX;
	s8 y = HeadY;
	u8 d = CurDir;
	if (d == DIR_L) x--;
	if (d == DIR_U) y--;
	if (d == DIR_R) x++;
	if (d == DIR_D) y++;
	u8* bold = &Board[HeadX + HeadY*MAPW]; // current position
	u8* bnew = &Board[x + y*MAPW]; // new position
	u8 b = *bnew; // item on new position

	// crash
	if ((x < 0) || (x >= MAPW) || // x is out of board
		(y < 0) || (y >= MAPH) || // y is out of board
		((b != EMPTY) && // not empty field
		((b <= GATEMIN) || (b > GATEMAX)) && // not open gate
		((b < ITEMMIN) || (b > ITEMMAX)))) // not valid item
	{
		State = S_CRASH;	// crash state
		b = CRASH;		// crash image

		// crash animation
		PlayMelody(CrashSound);

		while (True)
		{
			PutTile(HeadX, HeadY, b); // set locomotive crash image
			WaitStep();
			b++;
			if (b > CRASHMAX) b = CRASHMAX - 2;
			if (JoyGet() != NOKEY) break;
		}

		// restart new level
		InitLevel();
		return;
	}

	// set loco to new position
	u8 b2;
	s8 xold = HeadX;
	s8 yold = HeadY;
	if (d == DIR_L)
		b2 = LOCO_L; // direction left
	else if (d == DIR_U)
		b2 = LOCO_U; // direction up
	else if (d == DIR_R)
		b2 = LOCO_R; // direction right
	else b2 = LOCO_D; // direction down
	PutTile(x, y, b2); // store new locomotive image
	Dir[x + y*MAPW] = d; // store new direction at this point
	HeadX = x; // new locomotive position
	HeadY = y;

	// move wagons
	int i;
	for (i = Length-1; i > 0; i--)
	{
		x = xold;
		y = yold;

		// get position of previous wagon
		d = Dir[x + y*MAPW];
		if (d == DIR_L) x++;
		if (d == DIR_U) y++;
		if (d == DIR_R) x--;
		if (d == DIR_D) y--;

		// change direction of previous wagon
		b2 = WAGON_L;
		if (d == DIR_U) b2 = WAGON_U;
		if (d == DIR_R) b2 = WAGON_R;
		if (d == DIR_D) b2 = WAGON_D;

		// put wagon on new position
		PutTile(xold, yold, b2);

		xold = x;
		yold = y;
	}

	// collect item, increase train length
	if ((b >= ITEMMIN) && (b <= ITEMMAX))
	{
		// collect sound
		PlayMelody(CollectSound);

		// increase train length
		Length++;

		// new wagon
		d = Dir[xold + yold*MAPW];
		b = WAGON_L;
		if (d == DIR_U) b = WAGON_U;
		if (d == DIR_R) b = WAGON_R;
		if (d == DIR_D) b = WAGON_D;
		PutTile(xold, yold, b);

		// decrease number of items
		ItemNum--;

		// start opening gate
		if (ItemNum == 0) PutTile(GateX, GateY, GATEMIN+1);

		// count score
		if (State == S_GO)
		{
			Score += 10;
			InfoRow();
		}
	}
	else
	{
		// step sound
		PlayMelody(StepSound);

		// empty last position
		PutTile(xold, yold, EMPTY);
	}

	// gate
	if ((HeadX == GateX) && (HeadY == GateY))
	{
		int s = State;
		State = S_FINISH;
		WaitMs(500);
		KeyFlush();

#ifndef AUTOMODE // automode - run levels to check solutions (help completes scene)
		if (s == S_HELP) return;
#endif

		// play sound
		PlayMelody(SuccessSound);
		WaitMs(1000);

		// open next level
		if (Level < LEVNUM)
		{
			Level++;
			OpenLevel();
		}
		else
			CongLevel();
		InitLevel();
	}
}

// level animation - called every single game step
void AnimLevel()
{
	// open gate
	if ((GetTile(GateX, GateY) > GATEMIN) && (GetTile(GateX, GateY) < GATEMAX))
		PutTile(GateX, GateY, GetTile(GateX, GateY)+1);

	// increase animation phase
	Phase++;
	if (Phase >= 3)
	{
		Phase = 0;

		// change direction by keyboard (and potentially start the game)
		char c = KeyBuf1;
		KeyBuf1 = KeyBuf2;
		KeyBuf2 = NOKEY;
		if (c != NOKEY)
		{
			if (State == S_WAIT) State = S_GO;
			if (c == KEY_DOWN) CurDir = DIR_D;
			if (c == KEY_RIGHT) CurDir = DIR_R;
			if (c == KEY_LEFT) CurDir = DIR_L;
			if (c == KEY_UP) CurDir = DIR_U;
		}

		// one step
		if ((State == S_GO) || (State == S_HELP)) StepLevel();
	}
}

/*
// help level (returns True on break from keyboard)
Bool HelpLevel()
{
	char b;

	// re-initialize current scene
	InitLevel();

	// help mode
	State = S_HELP;

	// prepare pointer to level solution
	const char* s = Solution[Level];

	// macro loop
	while (State == S_HELP)
	{
		// get next key from solution macro
		b = *s++;
		if (b == 0)
		{
			State = S_FINISH;

			// wait 1.6 seconds on end of scene
			for (b = 10; b > 0; b--)
			{
				if (JoyGet() != NOKEY) return True;
				AnimLevel();
				// wait step
				WaitStep();
			}
			return False;
		}
		if (b == 'R') CurDir = DIR_R;
		if (b == 'U') CurDir = DIR_U;
		if (b == 'L') CurDir = DIR_L;
		if (b == 'D') CurDir = DIR_D;

		if (JoyGet() != NOKEY) return True;
		AnimLevel();
		WaitStep();
		if (JoyGet() != NOKEY) return True;
		AnimLevel();
		WaitStep();
		if (JoyGet() != NOKEY) return True;
		AnimLevel();

		// step
		WaitStep();

		// break help
		if (JoyGet() != NOKEY) return True;
	}

	// no break from keyboard
	return False;
}
*/
// game loop - called every single game step (returns True on break Esc)
Bool GameLoop()
{
	// animation
	AnimLevel();

	// key input
	char c = JoyGet();

#ifdef AUTOMODE // automode - run levels to check solutions (help completes scene)
	c = KEY_X;
#endif

	if (c == NOKEY) return False;

	switch (c)
	{
	// enter password
	case KEY_A:
		Psw();
		break;

	// restart
	case KEY_B:
		// break
		if (State != S_GO) return True; 

		// restart scene
		InitLevel();
		break;

	// help
//	case KEY_X:
//		HelpLevel();
//		InitLevel();
//		break;

	// direction
	case KEY_RIGHT:
	case KEY_UP:
	case KEY_LEFT:
	case KEY_DOWN:
		// save key into key buffer
		if (KeyBuf1 == NOKEY)
			KeyBuf1 = c;
		else
		{
			if (KeyBuf2 == NOKEY)
				KeyBuf2 = c;
			else
			{
				KeyBuf1 = KeyBuf2;
				KeyBuf2 = c;
			}
		}
		break;

	// unknown key - display help
	default:
		KeyFlush();
	}

	// no break
	return False;
}
