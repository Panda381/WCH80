
// ****************************************************************************
//
//                              TV Tennis game
//
// ****************************************************************************

#include "../include.h"

#define COORD_SHIFT	5	// coordinates shift
#define COORD_MUL	(1<<COORD_SHIFT) // coordinate multiply

#define WIDTHM	(WIDTH*COORD_MUL)
#define HEIGHTM	(HEIGHT*COORD_MUL)

#define DIGW	(8*COORD_MUL)	// digit width in pixels
#define DIGH	(16*COORD_MUL)	// digit height in pixels, rounded to 8x8 boxes
#define DIGXL	(7*8*COORD_MUL)	// left score X
#define DIGXR	(11*8*COORD_MUL) // right score X
#define MAXSCORE 99		// max. score
#define NETW	(4*COORD_MUL)	// width of middle net
#define DIGNUM	2		// number of digits of one score
#define PADDLEW	(4*COORD_MUL)	// paddle width
#define PADDLEH	(24*COORD_MUL)	// paddle height
#define BALLSIZE (4*COORD_MUL)	// ball size
#define PADDLELX (2*COORD_MUL)	// left paddle X coordinate
#define PADDLERX ((WIDTH-2)*COORD_MUL-PADDLEW) // right paddle X coordinate

#define BALL_IN		0	// ball is in game
#define BALL_L		1	// ball is served by left player
#define BALL_R		2	// ball is served by right player

#define PADDLESPEED 	(2*COORD_MUL-COORD_MUL/2) // paddle speed (number of pixels per frame of 60fps)
#define BALLSPEED	(2*COORD_MUL)	// ball speed (number of pixels per frame of 60fps)

// time delta in number of frames
u32 OldTime;
int TimeDelta;

// computer
Bool Comp[2];	// computer flags
int CompX[2];	// X coordinate for delayed computer response
int CompY[2];	// Y coordinate of computer inaccuracy
u32 LastOut;	// last out time (to start new serve)

// current score
u8 Score[2];
Bool Tied;	// tied paddles

// paddle Y coordinate (middle)
int PaddleY[2];
int OldPaddleY[2];

// ball state BALL_*
u8 BallState;
int BallX, BallY, BallDX, BallDY;
int OldBallX, OldBallY;

// bound sound
const sMelodyNote BoundSound[] = {
	{ 1, NOTE_C7 },
	{ 0, 0 },
};

// paddle sound
const sMelodyNote PaddleSound[] = {
	{ 1, NOTE_C6 },
	{ 0, 0 },
};

// serve sound
const sMelodyNote ServeSound[] = {
	{ 1, NOTE_C5 },
	{ 0, 0 },
};

// out sound
const sMelodyNote OutSound[] = {
	{ 1, NOTE_C5 },
	{ 1, NOTE_CS5 },
	{ 1, NOTE_D5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_F5 },
	{ 1, NOTE_FS5 },
	{ 1, NOTE_G5 },
	{ 1, NOTE_GS5 },
	{ 1, NOTE_A5 },
	{ 1, NOTE_AS5 },
	{ 1, NOTE_B5 },
	{ 1, NOTE_C6 },
	{ 0, 0 },
};

// prepare auto play
void NewAuto()
{
	CompX[0] = WIDTHM/2 + RandU16Max(WIDTHM/4);
	CompY[0] = RandS16MinMax(-(PADDLEH+BALLSIZE/2)/2, (PADDLEH+BALLSIZE/2)/2);

	CompX[1] = RandU16Max(WIDTHM/4);
	CompY[1] = RandS16MinMax(-(PADDLEH+BALLSIZE/2)/2, (PADDLEH+BALLSIZE/2)/2);
}

// new game
void NewGame()
{
	int i;
	for (i = 0; i < 2; i++)
	{
		Score[i] = 0;
		PaddleY[i] = HEIGHTM/2;
	}
	BallState = BALL_L;
	LastOut = DispFrame;
	BallY = HEIGHTM/2;
	NewAuto();
}

// display middle net
void DispNet(int height)
{
	DrawRect(((WIDTHM-NETW)/2)>>COORD_SHIFT, 0, NETW>>COORD_SHIFT, height>>COORD_SHIFT, COL_WHITE);
}

// display one score
void DispScore(int inx)
{
	int j, row, col, x, y;
	u8 dig[2], k;
	u16 c;
	const u8* s;

	// decode number (2 digits)
	dig[0] = Score[inx]/10;
	dig[1] = Score[inx] - dig[0]*10;

	// start X coordinate
	x = (inx == 0) ? (DIGXL>>COORD_SHIFT) : (DIGXR>>COORD_SHIFT);

	// decode digits
	DrawImgBg(ImgDigits + dig[0], x, 0, 8, 10, 10, COL_WHITE, COL_BLACK);
	DrawImgBg(ImgDigits + dig[1], x+8, 0, 8, 10, 10, COL_WHITE, COL_BLACK);
}

// display both scores
void DispScores()
{
	DispScore(0);
	DispScore(1);
}

// display one paddle (clear old position)
#define OVERDRAW	3
void DispPaddle(int inx)
{
	int x = (inx == 0) ? (PADDLELX>>COORD_SHIFT) : (PADDLERX>>COORD_SHIFT);
	int y = PaddleY[inx];
	int oldy = OldPaddleY[inx];
	int h = y - oldy;
	if (h > 0)
	{
		if (h > PADDLEH) h = PADDLEH;
		DrawRectClr(x, ((oldy - PADDLEH/2)>>COORD_SHIFT) - OVERDRAW, PADDLEW>>COORD_SHIFT, (h>>COORD_SHIFT) + OVERDRAW);
	}
	else if (h < 0)
	{
		h = -h;
		if (h > PADDLEH) h = PADDLEH;
		DrawRectClr(x, (oldy - PADDLEH/2 + PADDLEH - h)>>COORD_SHIFT, PADDLEW>>COORD_SHIFT, (h>>COORD_SHIFT) + OVERDRAW);
	}
	DrawRect(x, (y - PADDLEH/2)>>COORD_SHIFT, PADDLEW>>COORD_SHIFT, PADDLEH>>COORD_SHIFT, COL_WHITE);
	OldPaddleY[inx] = y;
}

// display both paddles
void DispPaddles()
{
	DispPaddle(0);
	DispPaddle(1);
}

// display ball
#define OVERDRAW2	7
void DispBall()
{
	if (BallState == BALL_IN)
	{
		int x = BallX;
		int y = BallY;
		int oldx = OldBallX;
		int oldy = OldBallY;

		int w = x - oldx;
		if (w > 0)
		{
			if (w > BALLSIZE) w = BALLSIZE;
			DrawRectClr(((oldx - BALLSIZE/2)>>COORD_SHIFT) - OVERDRAW2, (oldy - BALLSIZE/2)>>COORD_SHIFT,
				(w>>COORD_SHIFT) + OVERDRAW2, BALLSIZE>>COORD_SHIFT);
		}
		else if (w < 0)
		{
			w = -w;
			if (w > BALLSIZE) w = BALLSIZE;
			DrawRectClr((oldx - BALLSIZE/2 + BALLSIZE - w)>>COORD_SHIFT, (oldy - BALLSIZE/2)>>COORD_SHIFT,
				(w>>COORD_SHIFT) + OVERDRAW2, BALLSIZE>>COORD_SHIFT);
		}

		int h = y - oldy;
		if (h > 0)
		{
			if (h > BALLSIZE) h = BALLSIZE;
			DrawRectClr((oldx - BALLSIZE/2)>>COORD_SHIFT, ((oldy - BALLSIZE/2)>>COORD_SHIFT) - OVERDRAW2,
				BALLSIZE>>COORD_SHIFT, (h>>COORD_SHIFT) + OVERDRAW2);
		}
		else if (h < 0)
		{
			h = -h;
			if (h > BALLSIZE) h = BALLSIZE;
			DrawRectClr((oldx - BALLSIZE/2)>>COORD_SHIFT, (oldy - BALLSIZE/2 + BALLSIZE - h)>>COORD_SHIFT,
				BALLSIZE>>COORD_SHIFT, (h>>COORD_SHIFT) + OVERDRAW2);
		}

		DrawRect((x - BALLSIZE/2)>>COORD_SHIFT, (y - BALLSIZE/2)>>COORD_SHIFT,
			BALLSIZE>>COORD_SHIFT, BALLSIZE>>COORD_SHIFT, COL_WHITE);
		OldBallX = x;
		OldBallY = y;
	}
}

// display all
void DispAll()
{
	// clear screen
	DrawClear();

	// display middle net
	DispNet(HEIGHTM);

	// display both scores
	DispScores();

	// display both paddles
	DispPaddles();

	// display ball
	DispBall();
}

// game control
void GameControl()
{
	u8 key0 = NOKEY;	// auto-play of left player
	u8 key1 = NOKEY;	// auto-play of right player
	int autoy;

	// auto play of left player
	if (Comp[0] && (BallX <= CompX[0]))
	{
		autoy = BallY + CompY[0];
		if (PaddleY[0] < autoy-(2<<COORD_SHIFT)) key0 = KEY_DOWN;
		if (PaddleY[0] > autoy+(2<<COORD_SHIFT)) key0 = KEY_UP;
	}

	// auto play of right player
	if (Comp[1] && (BallX >= CompX[1]))
	{
		autoy = BallY + CompY[1];
		if (PaddleY[1] < autoy-(2<<COORD_SHIFT)) key1 = KEY_DOWN;
		if (PaddleY[1] > autoy+(2<<COORD_SHIFT)) key1 = KEY_UP;
	}

	// left player up
	if (	(JoyPressed(KEY_UP) && !Comp[0]) ||	// left player UP
		((Tied || Comp[1]) && JoyPressed(KEY_B) && !Comp[0]) || // use also key B, if not used for right player
		(key0 == KEY_UP)) // auto-play of left player
	{
		PaddleY[0] -= TimeDelta*PADDLESPEED;
		if (PaddleY[0] < PADDLEH/2) PaddleY[0] = PADDLEH/2;
	}

	// left player down
	if (	(JoyPressed(KEY_DOWN) && !Comp[0]) ||	// left player down
		((Tied || Comp[1]) && JoyPressed(KEY_A) && !Comp[0]) || // use also key A, if not used for right player
		(key0 == KEY_DOWN)) // auto-play of left player
	{
		PaddleY[0] += TimeDelta*PADDLESPEED;
		if (PaddleY[0] > HEIGHTM - PADDLEH/2) PaddleY[0] = HEIGHTM - PADDLEH/2;
	}

	// right player up
	if (	(JoyPressed(KEY_B) && !Comp[1]) ||	// right player up
		((Tied || Comp[0]) && JoyPressed(KEY_UP) && !Comp[1]) || // use also key UP, if not used for left player
		(key1 == KEY_UP)) // auto-play of right player
	{
		PaddleY[1] -= TimeDelta*PADDLESPEED;
		if (PaddleY[1] < PADDLEH/2) PaddleY[1] = PADDLEH/2;
	}

	// right player down
	if (	(JoyPressed(KEY_A) && !Comp[1]) ||	// right player down
		((Tied || Comp[0]) && JoyPressed(KEY_DOWN) && !Comp[1]) || // use also DOWN, if not used for left player
		(key1 == KEY_DOWN)) // auto-play of right player
	{
		PaddleY[1] += TimeDelta*PADDLESPEED;
		if (PaddleY[1] > HEIGHTM - PADDLEH/2) PaddleY[1] = HEIGHTM - PADDLEH/2;
	}
}

// move ball
void GameBall()
{
	// move ball
	if (BallState == BALL_IN)
	{
		// ball new coordinate
		int ballx = BallX + TimeDelta*BallDX;
		int bally = BallY + TimeDelta*BallDY;

		// bounce off the top edge
		if (bally < BALLSIZE/2)
		{
			bally = BALLSIZE/2;
			BallDY = -BallDY;
			PlayMelody(BoundSound);
		}

		// bounce off the bottom edge
		if (bally > HEIGHTM - BALLSIZE/2)
		{
			bally = HEIGHTM - BALLSIZE/2;
			BallDY = -BallDY;
			PlayMelody(BoundSound);
		}

		// bounce off the left paddle
		//  Note: Using old "BallX" coordinate for case the animation step is too large.
		if (	(ballx - BALLSIZE/2 <= PADDLELX + PADDLEW) && // new X coordinate
			(BallX + BALLSIZE/2 >= PADDLELX) && // old X coordinate
			(bally - BALLSIZE/2 <= PaddleY[0] + PADDLEH/2) &&
			(bally + BALLSIZE/2 >= PaddleY[0] - PADDLEH/2))
		{
			ballx = PADDLELX + PADDLEW + BALLSIZE/2;
			BallDX = BALLSPEED;
			BallDY = RandS16MinMax(-BALLSPEED, +BALLSPEED);
			PlayMelody(PaddleSound);

			// prepare auto play
			NewAuto();
		}

		// bounce off the right paddle
		//  Note: Using old "BallX" coordinate for case the animation step is too large.
		if (	(ballx + BALLSIZE/2 >= PADDLERX) && // new X coordinate
			(BallX - BALLSIZE/2 <= PADDLERX + PADDLEW) && // old X coordinate
			(bally - BALLSIZE/2 <= PaddleY[1] + PADDLEH/2) &&
			(bally + BALLSIZE/2 >= PaddleY[1] - PADDLEH/2))
		{
			ballx = PADDLERX - BALLSIZE/2;
			BallDX = -BALLSPEED;
			BallDY = RandS16MinMax(-BALLSPEED, +BALLSPEED);
			PlayMelody(PaddleSound);

			// prepare auto play
			NewAuto();
		}

		// ball is out left
		if (ballx < -BALLSIZE)
		{
			if (Score[1] < MAXSCORE) Score[1]++;
			BallState = BALL_L;
			PlayMelody(OutSound);
			LastOut = DispFrame;
			BallY = HEIGHTM/2;
			NewAuto();
		}

		// ball is out right
		if (ballx > WIDTHM + BALLSIZE)
		{
			if (Score[0] < MAXSCORE) Score[0]++;
			BallState = BALL_R;
			PlayMelody(OutSound);
			LastOut = DispFrame;
			BallY = HEIGHTM/2;
			NewAuto();
		}

		// new position
		BallX = ballx;
		BallY = bally;
	}
}

// game display update
void GameUpdate()
{
	// display middle net
	DispNet(HEIGHTM);

	// display score
	DispScores();

	// display ball
	DispBall();

	// display both paddles
	DispPaddles();
}

// play on game
void Game(Bool comp1, Bool comp2, Bool tied)
{
	u8 ch;

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	// store computer flag
	Comp[0] = comp1;
	Comp[1] = comp2;
	Tied = tied;

	// start new game
	NewGame();
	u32 t;

	// wait for key release
	DispAll();
	while (JoyPressed(KEY_LEFT) || JoyPressed(KEY_RIGHT) ||
		JoyPressed(KEY_UP) || JoyPressed(KEY_DOWN) ||
		JoyPressed(KEY_A) || JoyPressed(KEY_B)) {}
	KeyFlush();

	// game loop
	OldTime = DispFrame;
	while (True)
	{
		// get key
		ch = JoyGet();

		// auto serve of computer after 1 second after drop-out
		if ((ch == NOKEY) && ((DispFrame - LastOut) >= 60))
		{
			if (	((BallState == BALL_L) && Comp[0]) ||
				((BallState == BALL_R) && Comp[1]))
				ch = KEY_LEFT;
		}

		// key
		switch (ch)
		{
		// serve ball
		case KEY_LEFT:
		case KEY_RIGHT:
			if (BallState != BALL_IN)
			{
				// left player serves
				if (BallState == BALL_L)
				{
					BallX = PADDLELX + PADDLEW + BALLSIZE/2;
					BallY = PaddleY[0];
					BallDX = BALLSPEED;
					BallDY = RandS16MinMax(-BALLSPEED, +BALLSPEED);
				}
				else
				{
					BallX = PADDLERX - BALLSIZE/2;
					BallY = PaddleY[1];
					BallDX = -BALLSPEED;
					BallDY = RandS16MinMax(-BALLSPEED, +BALLSPEED);
				}
				BallState = BALL_IN;
				PlayMelody(ServeSound);
			}
			KeyFlush();
			break;

		case KEY_X:
			return;

		case NOKEY:
			break;

		default:
			KeyFlush();
			break;
		}

		// time delta
		t = DispFrame;
		TimeDelta = t - OldTime;
		OldTime = t;

		// game control
		GameControl();

		// move ball
		GameBall();

		// game display update
		GameUpdate();
	}
}

// draw open screen
void OpenDraw()
{
	// clear screen
	DrawClear();

	// title
	PrintTextAt("TV Tennis", (TEXTWIDTH-9)/2, 2);

	// draw menu
	PrintTextAt("Press key to start:", 0, 5);

	PrintTextAt("LEFT", 0, 7);
	PrintTextAt("Left paddle", 6, 7);

	PrintTextAt("RIGHT", 0, 8);
	PrintTextAt("Right paddle", 6, 8);

	PrintTextAt("DOWN", 0, 9);
	PrintTextAt("2 players", 6, 9);

	PrintTextAt("UP", 0, 10);
	PrintTextAt("Demo", 6, 10);

	PrintTextAt("A", 0, 11);
	PrintTextAt("Tied paddles", 6, 11);
}

// Open screen
void Open()
{
	// draw open screen
	OpenDraw();

	// select game
	while (True)
	{
		// get key
		switch (JoyGet())
		{
		case KEY_LEFT:
			// Player + Comp + not-tied
			Game(False, True, False);
			return;

		case KEY_RIGHT:
			// Comp + Player + not-tied
			Game(True, False, False);
			return;

		case KEY_DOWN:
			// Player + Player + not-tied
			Game(False, False, False);
			return;

		case KEY_UP:
			// Comp + Comp + not-tied
			Game(True, True, False);
			return;

		case KEY_A:
			// Player + Player + tied
			Game(False, False, True);
			return;
		}
	}
}

int main(void)
{
	// display splash screen
	KeyWaitNoPressed();
	DrawImgBg(ImgIntro, 0, 0, WIDTH, HEIGHT, WIDTHBYTE, COL_WHITE, COL_BLACK);
	while (JoyGet() == NOKEY) {}
	DrawClear();

	while (True)
	{
		// Open screen
		Open();
	}
}
