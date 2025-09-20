
// ****************************************************************************
//
//                              Atoms game
//
// ****************************************************************************

#include "../include.h"

#define TILEW	3		// tile width in characters
#define TILEH	3		// tile height in rows
#define MAPW	10		// map width in number of tiles
#define MAPH	10		// map height in number of tiles
#define MAPSIZE	(MAPW*MAPH)	// map size (= 100 tiles)
#define BOARDW	(MAPW*TILEW-1)	// board width in characters (= 29)
#define BOARDH	(MAPH*TILEH-1)	// board height in text rows (= 29)
#define INFOW	(TEXTWIDTH-BOARDW) // width of info box in characters (= 11)
#define INFOX	BOARDW		// X coordinate of info box in characters (= 29)

//#define PLAYER_OFF	0	// player is OFF
#define PLAYER_HUMAN	1	// player is human
#define PLAYER_COMP	2	// player is computer

#define PLAYER_NUM	2	// number of players

// crash sound
const sMelodyNote CrashSound[] = {
	{ 1, NOTE_G3 },
	{ 1, NOTE_E3 },
	{ 1, NOTE_C3 },
	{ 0, 0 },
};

// win sound
const sMelodyNote WinSound[] = {
	{ NOTE_LEN8, NOTE_C4 },
	{ NOTE_LEN8, NOTE_E4 },
	{ NOTE_LEN8, NOTE_G4 },
	{ NOTE_LEN4, NOTE_C5 },
	{ 0, 0 },
};

typedef struct {
	u8	type;		// player type PLAYER_*
	u8	start;		// index of start position
	u8	scorey;		// score Y coordinate
	u8	cursor;		// current cursor
	int	atoms;		// number of atoms (0 = player loses and was eliminated from the game)
} sPlayer;

// players
sPlayer	Players[PLAYER_NUM] = {	// players
	{ PLAYER_HUMAN,	3*MAPW+3, 0, 0,	0 },
	{ PLAYER_COMP,	3*MAPW+6, 0, 0,	0 },
};

int Player;	// current active players
int PlayerNum;	// number of active players
u32 CurTime;	// cursor blinking time
int PlayerHuman; // number of human players

// board
u8	Atoms[MAPSIZE];	// number of atoms
u8	Owner[MAPSIZE]; // owner of field

// player type
const char* const PlayerTypeTxt[3] = {
	"not playing",
	"HUMAN      ",
	"COMPUTER   ",
};

// open screen (returns False to quit game)
Bool Open()
{
	char ch;
	int i;

	Player = 0;
	KeyFlush();

	PrintClear();

	while (True)
	{
#define MENUX 2
#define MENUY 10

		PrintTextAt("Exploding Atoms", (TEXTWIDTH - 15)/2, MENUY - 3);

		PrintTextAt("Setup players:", MENUX, MENUY);

		PrintTextAt("LEFT ..... Player 1 is", MENUX, MENUY+2);
		PrintTextAt("RIGHT .... Player 2 is", MENUX, MENUY+3);

		PrintTextAt("A ........ Start The Game", MENUX, MENUY+7);

		for (i = 0; i < PLAYER_NUM; i++)
			PrintTextAt(PlayerTypeTxt[Players[i].type], MENUX+23, MENUY+(i+2));

		ch = JoyGet();
		switch (ch)
		{
		case KEY_LEFT:
			Players[0].type++;
			if (Players[0].type > 2) Players[0].type = 1;
			break;

		case KEY_RIGHT:
			Players[1].type++;
			if (Players[1].type > 2) Players[1].type = 1;
			break;

		case KEY_A:
			return True;
		}
	}
}

// display score of one player
void DispScore(u8 player)
{
	int len;

	// zero - player lost
	if (Players[player].atoms == 0)
	{
		PrintTextAt("-lost-", INFOX + 2, Players[player].scorey + 2);
	}

	// player is winner
	else if (PlayerNum == 1)
	{
		PrintTextAt("WINNER", INFOX + 2, Players[player].scorey + 2);
	}
	else
	{
		// decode number of atoms
		len = DecUNum(DecNumBuf, Players[player].atoms, 0);

		// display score
		PrintTextAt(DecNumBuf, INFOX + 4, Players[player].scorey + 2);
		if (len == 1) PrintCharAt(' ', INFOX + 5, Players[player].scorey + 2);
		if ((len == 1) || (len == 2)) PrintCharAt(' ', INFOX + 6, Players[player].scorey + 2);
	}
}

char PlayerText[] = "Player 1";

// display score table with selection (blinking on end of game)
void DispScoreSel()
{
	int i, j;
	for (i = 0; i < PLAYER_NUM; i++)
	{
//		if (Players[i].type != PLAYER_OFF) // player is in game
		{
			int y = Players[i].scorey;
			int x = INFOX;

			// display player's name
			PlayerText[7] = i + '1';
			PrintTextAt(PlayerText, x+1, y);

			// display score
			DispScore(i);

			// display selection
			y--;
			if (i == Player)
			{
				PrintCharAt(28, x, y);
				for (j = 1; j < 9; j++) PrintCharAt(21, x+j, y);
				PrintCharAt(25, x+9, y);

				PrintCharAt(26, x, y+1);
				PrintCharAt(26, x+9, y+1);

				PrintCharAt(26, x, y+2);
				PrintCharAt(26, x+9, y+2);

				PrintCharAt(26, x, y+3);
				PrintCharAt(26, x+9, y+3);

				PrintCharAt(22, x, y+4);
				for (j = 1; j < 9; j++) PrintCharAt(21, x+j, y+4);
				PrintCharAt(19, x+9, y+4);
			}
			else
			{
				for (j = 0; j < 10; j++) PrintCharAt(' ', x+j, y);

				PrintCharAt(' ', x, y+1);
				PrintCharAt(' ', x+9, y+1);

				PrintCharAt(' ', x, y+2);
				PrintCharAt(' ', x+9, y+2);

				PrintCharAt(' ', x, y+3);
				PrintCharAt(' ', x+9, y+3);

				for (j = 0; j < 10; j++) PrintCharAt(' ', x+j, y+4);
			}
		}
	}
}

// display board grid
void DispGrid()
{
	int x, y;
	for (y = 0; y < BOARDH-2; y += 3)
	{
		for (x = 0; x < BOARDW-2; x += 3)
		{
			PrintCharAt(26, x+2, y);
			PrintCharAt(26, x+2, y+1);
			PrintCharAt(21, x, y+2);
			PrintCharAt(21, x+1, y+2);
			PrintCharAt(31, x+2, y+2);
		}
		PrintCharAt(21, x, y+2);
		PrintCharAt(21, x+1, y+2);
	}

	for (x = 0; x < BOARDW-4; x += 3)
	{
		PrintCharAt(26, x+2, y);
		PrintCharAt(26, x+2, y+1);
	}
}

// display one tile
void DispTile(u8 inx, Bool cursor)
{
	// prepare number of atoms
	u8 n = Atoms[inx];
	if (n > 4) n = 4;

	// owner
	u8 owner = Owner[inx];

	// coordinates
	int y = inx / MAPW;
	int x = inx - y*MAPW;
	y *= TILEH;
	x *= TILEW;

	// color
	u8 shift = cursor ? 128 : 0;
	u8 stone = cursor ? (16 + 128) : ((owner == 0) ? 3 : 2);
	u8 empty = ' ' + shift;

	// display tile
	PrintCharAt((n > 0) ? stone : empty, x, y);
	PrintCharAt((n > 1) ? stone : empty, x+1, y);
	PrintCharAt((n > 2) ? stone : empty, x, y+1);
	PrintCharAt((n > 3) ? stone : empty, x+1, y+1);
}

// display board
void DispBoard()
{
	int i;
	for (i = 0; i < MAPSIZE; i++) DispTile(i, False);
}

// display cursor of active player (blinking)
void DispCur()
{
	DispTile(Players[Player].cursor, (((Time() - CurTime) >> 22) & 3) < 3);
}

// hide cursor of active player
void DispCurOff()
{
	DispTile(Players[Player].cursor, False);
}

// new game
void NewGame()
{
	int i;

	KeyFlush();
	PrintClear();

	// clear board
	for (i = 0; i < MAPSIZE; i++)
	{
		Atoms[i] = 0;
		Owner[i] = 0;
	}

	// setup start condition
	PlayerNum = 0;
	PlayerHuman = 0;
	Player = -1;
	for (i = 0; i < PLAYER_NUM; i++)
	{
		// check if player is active
//		if (Players[i].type != PLAYER_OFF)
		{
			if (Player < 0) Player = i; // first active player
			Players[i].atoms = 1; // number of atoms
			Players[i].scorey = PlayerNum*5+3; // score Y coordinate
			PlayerNum++;
			Atoms[Players[i].start] = 1; // 1 default atom
			Owner[Players[i].start] = i; // atom owner
			Players[i].cursor = Players[i].start; // cursor
			if (Players[i].type == PLAYER_HUMAN) PlayerHuman++; // count of human players
		}
//		else
//			Players[i].atoms = 0; // player is not active, no atoms
	}

	// display board grid
	DispGrid();

	// display board
	DispBoard();

	// display score table with selection
	DispScoreSel();
}

// catch neighbour (add 1 own electron)
void Catch(u8 inx)
{
	u8 n = Atoms[inx];
	u8 own = Owner[inx];

	// not empty atom of enemy player
	if ((n > 0) && (own != Player))
	{
		// subtract atoms from enemy
		Players[own].atoms -= n;
		DispScore(own);

		// add atoms to player
		Players[Player].atoms += n;
		DispScore(Player);

		// enemy lost
		if (Players[own].atoms == 0) PlayerNum--;
	}

	// increase atoms
	Atoms[inx] = n+1;
	Owner[inx] = Player;

	// display atom
	DispTile(inx, False);
}

// explosions
void Explo()
{
	int x, y, i, n;
	u8 mx;
	do {
		// count exploding atoms (note: player does not need to be tested,
		//	no other player can have an exploding atom at this time)
		n = 0;
		for (y = 0; y < MAPH; y++)
		{
			for (x = 0; x < MAPW; x++)
			{
				// field index
				i = y*MAPW+x;

				// prepare maximum of atom
				mx = 4;
				if ((x == 0) || (x == MAPW-1)) mx--;
				if ((y == 0) || (y == MAPH-1)) mx--;

				// check exploding atom
				if (Atoms[i] >= mx) n++;
			}
		}

		// stop, no explosion
		if (n == 0) break;

		// select random atom to explode
		n = RandS16Max(n - 1);

		// find exploding atom
		for (y = 0; y < MAPH; y++)
		{
			for (x = 0; x < MAPW; x++)
			{
				// field index
				i = y*MAPW+x;

				// prepare maximum of atom
				mx = 4;
				if ((x == 0) || (x == MAPW-1)) mx--;
				if ((y == 0) || (y == MAPH-1)) mx--;

				// found exploding atom
				if (Atoms[i] >= mx)
				{
					n--;
					if (n < 0)
					{
						// decrease this field
						Atoms[i] -= mx;
						DispTile(i, False);

						// catch neighbours
						if (x > 0) Catch(i-1);
						if (x < MAPW-1) Catch(i+1);
						if (y > 0) Catch(i-MAPW);
						if (y < MAPH-1) Catch(i+MAPW);

						// update display
						PlayMelody(CrashSound);
						WaitMs(80);
						break;
					}
				}
			}
			if (n < 0) break;
		}

	// if 1 player left and board is too full, the explosions might never end
	} while (PlayerNum > 1);
}

// auto-play (search cursor)
void Auto()
{
	u8 bestsize = 0;
	int bestnum = 0;
	int i;
	DispCurOff();

	// find best atom size and count
	for (i = 0; i < MAPSIZE; i++)
	{
		if (Owner[i] == Player)
		{
			if (Atoms[i] > bestsize)
			{
				bestsize= Atoms[i];
				bestnum = 1;
			}
			else if (Atoms[i] == bestsize) bestnum++;			
		}
	}

	// select field
	bestnum = RandU8Max(bestnum-1);
	for (i = 0; ; i++)
	{
		if ((Owner[i] == Player) && (Atoms[i] == bestsize))
		{
			bestnum--;
			if (bestnum < 0) break;
		}
	}

	// set cursor to this field
	Players[Player].cursor = i;

	// display cursor for a while
	DispTile(i, True);

	// display update
	WaitMs(200);
}

// game
void Game()
{
	int cur;
	u8 ch;

	// cursor time
	CurTime = Time();

	while (True)
	{
		// break demo
		if (PlayerHuman == 0)
		{
			if (JoyGet() != NOKEY) return;
		}

		// auto player
		if (Players[Player].type == PLAYER_COMP)
		{
			Auto();
			ch = KEY_A;
		}
		else
			ch = JoyGet();

		// keyboard control
		cur = Players[Player].cursor;

		switch (ch)
		{
		// cursor left
		case KEY_LEFT:
			DispCurOff();
			cur--;
			if ((cur < 0) || ((cur % MAPW) == MAPW-1)) cur += MAPW;
			CurTime = Time();
			break;

		// cursor right
		case KEY_RIGHT:
			DispCurOff();
			cur++;
			if ((cur % MAPW) == 0) cur -= MAPW;
			CurTime = Time();
			break;

		// cursor up
		case KEY_UP:
			DispCurOff();
			cur -= MAPW;
			if (cur < 0) cur += MAPSIZE;
			CurTime = Time();
			break;

		// cursor down
		case KEY_DOWN:
			DispCurOff();
			cur += MAPW;
			if (cur >= MAPSIZE) cur -= MAPSIZE;
			CurTime = Time();
			break;

		case KEY_A: // put atom
			// check color
			if ((Atoms[cur] == 0) || (Owner[cur] == Player))
			{
				Owner[cur] = Player;
				Atoms[cur]++;
				Players[Player].atoms++;
				DispTile(cur, False);
				DispScore(Player);

				// explosions
				Explo();

				// end of game
				if (PlayerNum < 2)
				{
					// win sound
					PlayMelody(WinSound);

					// hide cursor of active player
					DispCurOff();

					// report end of game
					KeyFlush();
					int play = Player;
					while (JoyGet() == NOKEY)
					{
						Player = ((((Time() - CurTime) >> 21) & 3) < 2) ? play : -1;

						// display score selection (blinking on end of game)
						DispScoreSel();
					}
					return;
				}

				// change player
				do {
					Player++;
					if (Player >= PLAYER_NUM) Player = 0;
				} while (Players[Player].atoms == 0);

				cur = Players[Player].cursor;

				DispScoreSel();
				CurTime = Time();
			}
			break;

		case KEY_X: return;

		}
		Players[Player].cursor = cur;

		// blinking cursor
		DispCur();
	}

}

int main(void)
{
	// display splash screen
	KeyWaitNoPressed();
	memcpy(FrameBuf, ImgIntro, sizeof(ImgIntro));
	while (JoyGet() == NOKEY) {}
	DrawClear();

	while (True)
	{
		// open screen
		Open();

		// start new game
		NewGame();

		// randomize
		RandSeed += Time() + DispFrame + DispLine;

		// game (check number of selected players)
		if (PlayerNum > 1) Game();
	}
}
