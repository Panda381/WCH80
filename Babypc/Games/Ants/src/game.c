
// ****************************************************************************
//
//                              Game engine
//
// ****************************************************************************

#include "../include.h"

#define CARDW		10			// card width in characters (must be even number)
#define CARDH		7			// card height in characters
#define CASTLEMAX	100			// castle and fence max. height
#define CARDNUM		8			// number of cards of one player
#define CARDTYPES	30			// number of card types
#define MIDX0		(TEXTWIDTH/2-CARDW/2)	// middle card
#define MIDX1		(TEXTWIDTH/2-CARDW)	// middle card 1
#define MIDX2		(TEXTWIDTH/2)		// middle card 2

// player's parameters (order must be preserved - used to display status)
enum {
	PAR_BUILDERS = 0,
	PAR_BRICKS,
	PAR_SOLDIERS,
	PAR_WEAPONS,
	PAR_WIZARDS,
	PAR_CRYSTALS,
	PAR_CASTLE,
	PAR_FENCE,

	PAR_NUM
};

const char* ParamName[] = {
	"Builders",
	"Bricks  ",
	"Soldiers",
	"Weapons ",
	"Wizards ",
	"Crystals",
	"Castle  ",
	"Fence   ",
};

// player descriptor
typedef struct {
	Bool	comp;		// player is computer
	int	par[PAR_NUM];	// parameter
	int	add[PAR_NUM];	// parameter addition
	int	cards[CARDNUM];	// cards (-1 none)
	Bool	disable[CARDNUM]; // cards disable
} sPlayer;

// card descriptor
typedef struct {
	int	type;		// card type PAR_*
	int	costs;		// card value "costs"
	int	param;		// parameter to change PAR_* (PAR_NUM = extra)
	int	value;		// change value (+ own, - enemy)
	const char* title1;	// card title, 1st row, max. 8 characters
	const char* title2;	// card title, 2nd row, max. 8 characters
	const char* text1;	// card text, 1st row, max. 8 characters
	const char* text2;	// card text, 2nd row, max. 8 characters
	const char* text3;	// card text, 3rd row, max. 8 characters
} sCard;

// castle and fence X coordinate
const int CastleX[2] = { TEXTWIDTH/2 - 20, TEXTWIDTH/2 + 20 }; // castle X middle coordinate
const int FenceX[2] = { TEXTWIDTH/2 - 10, TEXTWIDTH/2 + 9 }; // fence X middle coordinate

// player title
const char* PlayerName[] = { " Blacks ", "  Reds  " };
const int PlayerTitleX[] = { 1, TEXTWIDTH - 1 - 10 };

// save screen content under card
u8 SaveCardBuf[CARDW*CARDH];// + CARDW/2*CARDH];

// player state
sPlayer Players[2];	// player's game parameters
Bool Demo;		// demo mode
u8 Player;		// active player
int Last;		// last laid card (-1 = none)
Bool LastDiscard;	// last card was discarded
int SelCard;		// selected card (cursor)
int FreePos;		// free card position
int DispPos, DispRow, DispCol; // print text; DispCol bit 7: invert
char NumBuf[20];	// decode number

// card table
const sCard Card[CARDTYPES] = {

// bricks
	{	PAR_BRICKS,	1,	PAR_FENCE,	+3,	"  Wall",	"",		"",		"",		"fence+3" },	// 0: wall
	{	PAR_BRICKS,	1,	PAR_CASTLE,	+2,	"  Base",	"",		"",		"",		"castle+2" },	// 1: base
	{	PAR_BRICKS,	3,	PAR_FENCE,	+6,	"Defense",	"",		"",		"",		"fence+6" },	// 2: defense
	{	PAR_BRICKS,	3,	PAR_NUM,	0,	"Reserve",	"",		"",		"castle+8",	"fence-4" },	// 3: reserve (castle +8, fence -4)
	{	PAR_BRICKS,	5,	PAR_CASTLE,	+5,	" Tower",	"", 		"",		"",		"castle+5" },	// 4: tower
	{	PAR_BRICKS,	8,	PAR_BUILDERS,	+1,	" School",	"", 		"",		"builders",	"   +1" },	// 5: school
	{	PAR_BRICKS,	10,	PAR_NUM,	0,	"  Wain",	"",		"castle+8",	" enemy",	"castle-4" },	// 6: wain (castle +8, enemy castle -4)
	{	PAR_BRICKS,	12,	PAR_FENCE,	+22,	" Fence",	"",		"",		"",		"fence+22" },	// 7: fence
	{	PAR_BRICKS,	18,	PAR_CASTLE,	+20,	"  Fort",	"",		"",		" castle",	"  +20" },	// 8: fort
	{	PAR_BRICKS,	39,	PAR_CASTLE,	+32,	"Babylon",	"",		"",		" castle",	"  +32" },	// 9: Babylon

// weapons
	{	PAR_WEAPONS,	1,	PAR_FENCE,	-2,	" Archer",	"", 		"",		"",		"attack 2" },	// 10: archer
	{	PAR_WEAPONS,	2,	PAR_FENCE,	-3,	" Knight",	"",		"",		"",		"attack 3" },	// 11: knight
	{	PAR_WEAPONS,	2,	PAR_FENCE,	-4,	" Rider",	"",		"",		"",		"attack 4" },	// 12: rider
	{	PAR_WEAPONS,	4,	PAR_FENCE,	-6,	"Platoon",	"",		"",		"",		"attack 6" },	// 13: platoon
	{	PAR_WEAPONS,	8,	PAR_SOLDIERS,	+1,	"Recruit",	"",		"",		"soldiers",	"   +1" },	// 14: recruit
	{	PAR_WEAPONS,	10,	PAR_FENCE,	-12,	" Attack",	"",		"",		" attack",	"   12" },	// 15: attack
	{	PAR_WEAPONS,	12,	PAR_NUM,	0,	"Saboteur",	"", 		"",		" enemy", 	"stocks-4" },	// 16: saboteur (enemy stocks -4)
	{	PAR_WEAPONS,	15,	PAR_NUM,	0,	" Thief",	"",		"transfer",	" enemy",	"stocks 5" },	// 17: thief (transfer enemy stocks 5)
	{	PAR_WEAPONS,	18,	PAR_CASTLE,	-10,	"  SWAT",	"",		" enemy",	" castle",	"  -10" },	// 18: swat
	{	PAR_WEAPONS,	28,	PAR_FENCE,	-32,	"Banshee",	"",		"",		" attack",	"   32" },	// 19: banshee

// crystals
	{	PAR_CRYSTALS,	4,	PAR_BRICKS,	+8,	"Conjure",	" bricks",	"",		"",		"bricks+8" },	// 20: conjure bricks
	{	PAR_CRYSTALS,	4,	PAR_BRICKS,	-8,	" Crush",	" bricks",	"",		" enemy",	"bricks-8" },	// 21: crush bricks
	{	PAR_CRYSTALS,	4,	PAR_WEAPONS,	+8,	"Conjure",	"weapons",	"",		"",		"weapon+8" },	// 22: conjure weapons
	{	PAR_CRYSTALS,	4,	PAR_WEAPONS,	-8,	" Crush",	"weapons",	"",		" enemy",	"weapon-8" },	// 23: crush weapons
	{	PAR_CRYSTALS,	4,	PAR_CRYSTALS,	+8,	"Conjure",	"crystals",	"",		"crystals",	"   +8" },	// 24: conjure crystals
	{	PAR_CRYSTALS,	4,	PAR_CRYSTALS,	-8,	" Crush",	"crystals",	"",		" enemy",	"cryst.-8" },	// 25: crush crystals
	{	PAR_CRYSTALS,	8,	PAR_WIZARDS,	+1,	"Sorcerer",	"",		"",		"",		" magi+1" },	// 26: sorcerer
	{	PAR_CRYSTALS,	21,	PAR_FENCE,	-25,	" Dragon",	"",		"",		" attack",	"   25" },	// 27: dragon
	{	PAR_CRYSTALS,	22,	PAR_CASTLE,	+22,	" Pixies",	"",		"",		" castle",	"  +22" },	// 28: pixies
	{	PAR_CRYSTALS,	25,	PAR_NUM,	0,	" Curse",	"",		"",		" all+1",	"enemy-1" },	// 29: curse (all +1, enemie's all -1)
};

// Fence characters
const u8 FenceChar[13*3] = {
	' ', ' ', ' ',		// 0
	136, ' ', ' ',		// 1
	140, ' ', ' ',		// 2
	140, 136, ' ',		// 3
	140, 140, ' ',		// 4
	140, 140, 136,		// 5
	140, 140, 140,		// 6
	142, 140, 140,		// 7
	143, 140, 140,		// 8
	143, 142, 140,		// 9
	143, 143, 140,		// 10
	143, 143, 142,		// 11
	143, 143, 143,		// 12
};

// card sound
const sMelodyNote CardSound[] = {
	{ 1, NOTE_C2 },
	{ 0, 0 },
};

// increase power sound
const sMelodyNote IncPwrSound[] = {
	{ NOTE_LEN8, NOTE_C4 },
	{ NOTE_LEN8, NOTE_E4 },
	{ NOTE_LEN8, NOTE_G4 },
	{ NOTE_LEN8, NOTE_C5 },
	{ 0, 0 },
};

// increase stock sound
const sMelodyNote IncStockSound[] = {
	{ NOTE_LEN16, NOTE_E4 },
	{ NOTE_LEN16, NOTE_G4 },
	{ NOTE_LEN8, NOTE_E4 },
	{ 0, 0 },
};

// decrease stock sound
const sMelodyNote DecStockSound[] = {
	{ NOTE_LEN16, NOTE_D3 },
	{ NOTE_LEN16, NOTE_C3 },
	{ NOTE_LEN8, NOTE_D3 },
	{ 0, 0 },
};

// increase building
const sMelodyNote IncBuildSound[] = {
	{ 1, NOTE_C3 },
	{ 1, NOTE_D3 },
	{ 1, NOTE_E3 },
	{ 1, NOTE_F3 },
	{ 1, NOTE_G3 },
	{ 1, NOTE_A3 },
	{ 1, NOTE_B3 },
	{ 1, NOTE_C4 },
	{ 0, 0 },
};

// increase building
const sMelodyNote DecBuildSound[] = {
	{ 1, NOTE_C3 },
	{ 1, NOTE_B2 },
	{ 1, NOTE_AS2 },
	{ 1, NOTE_A2 },
	{ 1, NOTE_GS2 },
	{ 1, NOTE_G2 },
	{ 1, NOTE_FS2 },
	{ 1, NOTE_F2 },
	{ 1, NOTE_E2 },
	{ 1, NOTE_DS2 },
	{ 1, NOTE_D2 },
	{ 1, NOTE_CS2 },
	{ 1, NOTE_C2 },
	{ 0, 0 },
};

// curse sound
const sMelodyNote CurseSound[] = {
	{ 1, NOTE_C4 },
	{ 1, NOTE_B4 },
	{ 1, NOTE_CS4 },
	{ 1, NOTE_AS4 },
	{ 1, NOTE_D4 },
	{ 1, NOTE_A4 },
	{ 1, NOTE_DS4 },
	{ 1, NOTE_GS4 },
	{ 1, NOTE_E4 },
	{ 1, NOTE_G4 },
	{ 1, NOTE_F4 },
	{ 1, NOTE_FS4 },
	{ 1, NOTE_F4 },
	{ 0, 0 },
};

// fanfare sound
const sMelodyNote FanfareSound[] = {
	{ NOTE_LEN8, NOTE_C4 },
	{ NOTE_LEN8, NOTE_E4 },
	{ NOTE_LEN8, NOTE_G4 },
	{ NOTE_LEN8, NOTE_C5 },
	{ NOTE_LEN8, NOTE_R },
	{ NOTE_LEN8, NOTE_G4 },
	{ NOTE_LEN4, NOTE_C5 },
	{ 0, 0 },
};

// print character
void DispChar(u8 ch)
{
	PrintCharAt(ch+(DispCol & B7), DispPos, DispRow);
	DispPos++;
}

// print character repeated
void DispCharRep(u8 ch, int num)
{
	for (; num > 0; num--) DispChar(ch);
}

// print text
void DispText(const char* txt)
{
	u8 ch;
	while ((ch = *txt++) != 0) DispChar(ch);
}

// display one card
void DispCard(int type, int x, int y, Bool disable, Bool back, Bool discard)
{
	int i;

	DispCol = COL_WHITE;

	// invalid card
	if (type < 0)
	{
		DispRow = y;
		for (i = CARDH; i > 0; i--)
		{
			DispPos = x;
			DispCharRep(' ', CARDW);
			DispRow++;
		}
	}

	// back
	else if (back)
	{
		// prepare coordinates
		DispPos = x;
		DispRow = y;

		// top row
		DispChar(132);
		DispCharRep(140, CARDW-2);
		DispChar(136);

		// inner rows
		for (i = CARDH-2; i > 0; i--)
		{
			DispRow++;
			DispPos = x;
			DispChar(133);
			DispCharRep(1, CARDW-2);
			DispChar(138);
		}

		// bottom row
		DispRow++;
		DispPos = x;
		DispChar(129);
		DispCharRep(131, CARDW-2);
		DispChar(130);
	}

	// front
	else
	{
		const sCard* card = &Card[type];

		// prepare coordinates
		DispPos = x;
		DispRow = y;

		// top row
		if (disable)
		{
			DispChar(28);
			DispCharRep(21, CARDW-2);
			DispChar(25);
		}
		else
		{
			DispChar(132);
			DispCharRep(140, CARDW-2);
			DispChar(136);
		}

		// 1st row - card type
		DispRow++;
		DispPos = x;
		DispChar(disable ? 26 : 133);
		switch (card->type)
		{
		case PAR_BRICKS:
			DispText("Brick");
			break;

		case PAR_WEAPONS:
			DispText("Sword");
			break;

		case PAR_CRYSTALS:
			DispText("Cryst");
			break;
		}
		if (card->costs < 10)
			DispText("s:");
		else
		{
			DispChar(':');
			DispChar(card->costs/10+'0');
		}
		DispChar(card->costs%10 + '0');
		DispChar(disable ? 26 : 138);

		// 2nd row - title 1
		DispRow++;
		DispPos = x;
		DispChar(disable ? 26 : 133);
		DispText(card->title1);
		DispCharRep(' ', x + CARDW-1 - DispPos);
		DispChar(disable ? 26 : 138);

		// 3rd row - title 2, text 1
		DispRow++;
		DispPos = x;
		DispChar(disable ? 26 : 133);
		if (discard)
		{
			DispText("DIS-CARD");
		}
		else
		{
			DispText(card->title2);
			DispText(card->text1);
			DispCharRep(' ', x + CARDW-1 - DispPos);
		}
		DispChar(disable ? 26 : 138);

		// 4th row - text 2
		DispRow++;
		DispPos = x;
		DispChar(disable ? 26 : 133);
		DispText(card->text2);
		DispCharRep(' ', x + CARDW-1 - DispPos);
		DispChar(disable ? 26 : 138);

		// 5th row - text 3
		DispRow++;
		DispPos = x;
		DispChar(disable ? 26 : 133);
		DispText(card->text3);
		DispCharRep(' ', x + CARDW-1 - DispPos);
		DispChar(disable ? 26 : 138);

		// bottom row
		DispRow++;
		DispPos = x;

		if (disable)
		{
			DispChar(22);
			DispCharRep(21, CARDW-2);
			DispChar(19);
		}
		else
		{
			DispChar(129);
			DispCharRep(131, CARDW-2);
			DispChar(130);
		}
	}
}

// display cards of active player
void DispCards()
{
	int x = 0;
	sPlayer* p = &Players[Player];
	int i = 0;
	for (i = 0; i < CARDNUM; i++)
	{
		DispCard(p->cards[i], x, TEXTHEIGHT - CARDH, p->disable[i], p->comp && !Demo, False);
		x += CARDW;
	}
}

// generate new card
int NewCard()
{
	// unlinear randomness - cards with low number will be most plentiful
	int card = RandU16Max(1023);
	card *= card;
	card *= CARDTYPES;
	card >>= 20;
	
	// change organization of card order - 3 groups from weakest to strongest cards
	card = (card/3) + (card % 3)*10;
	return card;
}

// test cards permission
void Permission()
{
	// initialize players
	int i, j, card, costs, par;
	sPlayer* p = Players;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < CARDNUM; j++)
		{
			// check card cost
			card = p->cards[j];
			if (card < 0) // invalid card
				p->disable[j] = True;
			else
			{
				costs = Card[card].costs;
				par = Card[card].type;
				p->disable[j] = costs > p->par[par];
			}
		}

		// next player
		p++;
	}
}

// display state of one player
void DispState1(int player)
{
	int i, n;
	sPlayer* p;
	p = &Players[player];

	DispRow = 6;
	int x = (player == 0) ? 1 : (TEXTWIDTH-9);

	for (i = 0; i < PAR_NUM; i++)
	{
		n = i/2;
		DispCol = ((n == 0) || (n == 2)) ? (COL_WHITE | B7) : COL_WHITE;

		// display title
		DispPos = x;
		DispText(ParamName[i]);
		DispRow++;

		// display value
		DispPos = x;
		n = p->par[i];
		if (n > 999) n = 999;
		n = DecNum(NumBuf, n, 0);
		DispCharRep(' ', 6 - n);
		DispText(NumBuf);
		DispCharRep(' ', 2);
		DispRow++;
	}

	DispCol = COL_WHITE;
}

// display states of players
void DispState()
{
	DispState1(0);
	DispState1(1);
}

// display grass
void DispGrass(int player)
{
	DispPos = CastleX[player] - 8;
	if (player == 1) DispPos -= 5;
	DispRow = TEXTHEIGHT - 9;
	DispCol = COL_WHITE;
	DispCharRep('#', 21);
}

// display castle of one player
//  max. 15 rows, width 12 characters xxOOxxxxOOxx, 6 states
void DispCastle(int player)
{
	int i;
	u8 ch;

	// prepare castle height
	int h = Players[player].par[PAR_CASTLE]; // h is in range 0..100...

	DispCol = COL_WHITE;

	// display castle
	DispRow = TEXTHEIGHT - 10;
	int x = CastleX[player] - 6;
	for (i = 0; i < 16; i++)
	{
		DispPos = x;

#define CHAR_FULL	1

		ch = (h > 0) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		ch = (h > 1) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		ch = (h > 2) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		ch = (h > 3) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		ch = (h > 4) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		ch = (h > 5) ? CHAR_FULL : ' ';
		DispCharRep(ch, 2);

		h -= 6;
		DispRow--;
	}

	DispPos = x;
	ch = (h > 0) ? 140 : ' ';
	DispCharRep(ch, 4);
	ch = (h > 1) ? 140 : ' ';
	DispCharRep(ch, 2);
	ch = (h > 2) ? 140 : ' ';
	DispCharRep(ch, 2);
	ch = (h > 3) ? 140 : ' ';
	DispCharRep(ch, 4);
}

// dispay fence of one player
void DispFence(int player)
{
	int i, k;
	u8 ch;

	// prepare fence height
	int h = Players[player].par[PAR_FENCE]; // h is in range 0..100

	// display fence
	DispRow = TEXTHEIGHT - 10;
	int x = FenceX[player] - 1;
	DispCol = COL_WHITE;
	for (i = 0; i < 10; i++)
	{
		DispPos = x;
		k = h;
		if (k < 0) k = 0;
		if (k > 12) k = 12;
		const u8* t = &FenceChar[k*3];
		DispChar(*t++);
		DispChar(*t++);
		DispChar(*t++);
		h -= 12;
		DispRow--;
	}
}

// display titles - player selection
void DispTitle()
{
	DispCol = COL_WHITE;
	int player;
	for (player = 0; player < 2; player++)
	{
		// player color
		int x = PlayerTitleX[player];

#define TIT_W	10

		// frame 1
		DispPos = x;
		DispRow = 0;
		DispChar(132);
		DispCharRep(140, TIT_W-2);
		DispChar(136);

		// frame 2
		DispPos = x;
		DispRow++;
		DispChar(133);
		DispCharRep(' ', TIT_W-2);
		DispChar(138);

		// frame 3
		DispPos = x;
		DispRow++;
		DispChar(133);
		DispText(PlayerName[player]);
		DispChar(138);

		// frame 4
		DispPos = x;
		DispRow++;
		DispChar(133);
		DispCharRep(' ', TIT_W-2);
		DispChar(138);

		// frame 5
		DispPos = x;
		DispRow++;
		DispChar(129);
		DispCharRep(131, TIT_W-2);
		DispChar(130);
	}
}

// save screen under card (x must be even number, CARDW must be even number)
void SaveCard(int x, int y)
{
	int i;

	// save text
	u8* dst = SaveCardBuf;
	const char* src = &FrameBuf[x + y*WIDTHBYTE];
	for (i = CARDH; i > 0; i--)
	{
		memcpy(dst, src, CARDW);
		dst += CARDW;
		src += WIDTHBYTE;
	}
}

// load screen under card (x must be even number, CARDW must be even number)
void LoadCard(int x, int y)
{
	int i;

	// load text
	const u8* src = SaveCardBuf;
	char* dst = &FrameBuf[x + y*WIDTHBYTE];
	for (i = CARDH; i > 0; i--)
	{
		memcpy(dst, src, CARDW);
		src += CARDW;
		dst += WIDTHBYTE;
	}
}

// card animation
void AnimCard(int type, int x1, int y1, int x2, int y2, Bool back, Bool discard)
{
	// play sound
	PlayMelody(CardSound);

#define ANIMSTEPS 16 // number of steps of animation

	int i, x, y, xs, ys, s;
	for (i = 0; i < ANIMSTEPS; i++)
	{
		// current card position
		x = (x2 - x1)*i/ANIMSTEPS + x1;
		y = (y2 - y1)*i/ANIMSTEPS + y1;

		// x must be even number
		x &= ~1;

		// save card screen content
		SaveCard(x, y);

		// draw card
		DispCard(type, x, y, False, back, discard);

		// wait
		WaitMs(20);

		// restore screen content
		LoadCard(x, y);
	}
}

// cursor ON
void CursorOn()
{
	if (!Players[Player].comp)
	{
		DispPos = SelCard*CARDW + CARDW/2 - 1;
		DispRow = TEXTHEIGHT - CARDH - 1;
		DispCol = COL_WHITE;
		DispCharRep(15, 2);
	}
}

// cursor OFF
void CursorOff()
{
	DispPos = SelCard*CARDW + CARDW/2 - 1;
	DispRow = TEXTHEIGHT - CARDH - 1;
	DispCol = COL_WHITE;
	DispCharRep(' ', 2);
}

// take new card
void TakeCard()
{
	// generate new card
	int card = NewCard();

	// card animation
	sPlayer* p = &Players[Player];
	AnimCard(card, MIDX1, 0, FreePos*CARDW, TEXTHEIGHT-CARDH, p->comp && !Demo, False);

	// display new card
	p->cards[FreePos] = card;
	p->disable[FreePos] = False;

	// test cards permission
	Permission();

	// redraw all cards
	DispCards();

	// delay
	WaitMs(250);
}

// change values animation
void ChangeAnim()
{
	int i, player, x, y, n, par, h;
	DispCol = COL_WHITE;

	// display change
	for (player = 0; player < 2; player++)
	{
		// prepare coordinates
		DispRow = 7;
		sPlayer* p = &Players[player];

		// loop parameters
		for (par = 0; par < PAR_NUM; par++)
		{
			// parameter changed
			if (p->add[par] != 0)
			{
				// X position
				DispPos = (player == 0) ? 1 : (TEXTWIDTH-9);

				// draw value
				if (p->add[par] >= 0)
				{
					NumBuf[0] = '+';
					DecNum(NumBuf+1, p->add[par], 0);
				}
				else
				{
					DecNum(NumBuf, p->add[par], 0);
				}
				DispText(NumBuf);
			}
			DispRow += 2;
		}
	}

	// delay
	WaitMs(700);

	// restore screen
	DispState();
}

// display whole game area
void DispAll()
{
	// display titles - player selection
	DispTitle();

	// display castles and fences
	DispGrass(0);
	DispCastle(0);
	DispFence(0);

	DispGrass(1);
	DispCastle(1);
	DispFence(1);

	// display middle cards
	DispCard(0, MIDX1, 0, False, True, False);
	DispCard(Last, MIDX2, 0, False, False, LastDiscard);

	// display state
	DispState();

	// display cards
	DispCards();
}

// start new game
void NewGame(Bool comp1, Bool comp2)
{
	// computer state
	Players[0].comp = comp1;
	Players[1].comp = comp2;
	Demo = (comp1 && comp2);

	// starting player is Black
	Player = 0;

	// none last laid card
	Last = -1;
	LastDiscard = False; // last card was discarded

	// initialize players
	int i, j;
	sPlayer* p = Players;
	for (i = 0; i < 2; i++)
	{
		// set player's parameters
		p->par[PAR_BUILDERS] = 2;
		p->par[PAR_BRICKS] = 5;
		p->par[PAR_SOLDIERS] = 2;
		p->par[PAR_WEAPONS] = 5;
		p->par[PAR_WIZARDS] = 2;
		p->par[PAR_CRYSTALS] = 5;
		p->par[PAR_CASTLE] = 30;
		p->par[PAR_FENCE] = 10;

		// generate cards
		for (j = 0; j < CARDNUM; j++) p->cards[j] = NewCard();

		// next player
		p++;
	}

	// selected card (cursor)
	SelCard = 3;

	// test cards permission
	Permission();

	// display whole game area
	PrintClear();
	DispAll();

	// display cursor
	CursorOn();
}

// display win game
void WinGame(int player)
{
	PlayMelody(FanfareSound);
	WaitMs(300);
	KeyFlush();
	while (JoyGet() == NOKEY)
	{
		Player = -1;
		DispTitle();
		WaitMs(100);
		Player = player;
		DispTitle();
		WaitMs(100);
	}
}

// put card (pos = card position; returns True = end game)
Bool PutCard(int pos, Bool discard)
{
	int i, j;

	// card is disabled
	sPlayer* p = &Players[Player];
	if (p->disable[pos]) discard = True;

	// free card position
	FreePos = pos;

	// card type
	int card = p->cards[pos];
	const sCard* c = &Card[card];

	// subtract card value (if not discarded)
	if (!discard)
	{
		// cost card
		p->par[c->type] -= c->costs;

		// display state
		DispState1(Player);

		// test cards permission
		Permission();
	}

	// delete card
	p->cards[pos] = -1; // no card

	// redraw all cards
	DispCards();

	// card animation
	AnimCard(card, pos*CARDW, HEIGHT-CARDH, MIDX2, 0, False, discard);
	Last = card;
	LastDiscard = discard;

	// dispay last card
	DispCard(Last, MIDX2, 0, False, False, LastDiscard);

	// execute card
	if (!discard)
	{
		// save old state
		int castle1 = Players[0].par[PAR_CASTLE];
		int castle2 = Players[1].par[PAR_CASTLE];
		int fence1 = Players[0].par[PAR_FENCE];
		int fence2 = Players[1].par[PAR_FENCE];
		const sMelodyNote* snd = NULL;

		// clear additions of both players
		for (i = 0; i < PAR_NUM; i++)
		{
			Players[0].add[i] = 0;
			Players[1].add[i] = 0;
		}

		// parameter change
		if (c->param != PAR_NUM) // not extra
		{
			// change to enemy
			if (c->value < 0) p = &Players[Player ^ 1];

			// play sound
			switch (c->param)
			{
			// increase power
			case PAR_BUILDERS:
			case PAR_SOLDIERS:
			case PAR_WIZARDS:
				snd = IncPwrSound;
				break;

			// castle, fence
			case PAR_CASTLE:
			case PAR_FENCE:
				if (c->value >= 0)
					// build castle
					snd = IncBuildSound;
				else
					// ruin castle
					snd = DecBuildSound;
				break;

			// stocks
			// PAR_BRICKS
			// PAR_WEAPONS
			// PAR_CRYSTALS
			default:
				if (c->value >= 0)
					// increase stock
					snd = IncStockSound;
				else
					// decrease stock
					snd = DecStockSound;
				break;
			}

			// set parameter addition
			p->add[c->param] = c->value;
		}

		// extra service
		else
		{
			switch (card)
			{
			// reserve (castle +8, fence -4)
			case 3:
				p->add[PAR_CASTLE] = 8; // castle +8
				if (p->par[PAR_FENCE] >= 4) // fence -4
					p->add[PAR_FENCE] = -4;
				else
					p->add[PAR_FENCE] = - p->par[PAR_FENCE];

				// build castle
				snd = IncBuildSound;
				break;

			// wain (castle +8, enemy castle -4)
			case 6:
				p->add[PAR_CASTLE] = 8; // castle +8
				Players[Player ^ 1].add[PAR_CASTLE] = -4; // enemy castle -4

				// build castle
				snd = IncBuildSound;
				break;
				
			// saboteur (enemy stocks -4)
			case 16:
				p = &Players[Player ^ 1];
				p->add[PAR_BRICKS] = -4;
				p->add[PAR_WEAPONS] = -4;
				p->add[PAR_CRYSTALS] = -4;

				// decrease stock
				snd = DecStockSound;
				break;

			// thief (transfer enemy stocks 5)
			case 17:
				{
					p = &Players[Player ^ 1];

					int bricks = p->par[PAR_BRICKS];
					if (bricks > 5) bricks = 5;
					p->add[PAR_BRICKS] = -bricks;

					int weapons = p->par[PAR_WEAPONS];
					if (weapons > 5) weapons = 5;
					p->add[PAR_WEAPONS] = -weapons;

					int crystals = p->par[PAR_CRYSTALS];
					if (crystals > 5) crystals = 5;
					p->add[PAR_CRYSTALS] = -crystals;

					p = &Players[Player];
					p->add[PAR_BRICKS] = bricks;
					p->add[PAR_WEAPONS] = weapons;
					p->add[PAR_CRYSTALS] = crystals;

					// increase stock
					snd = IncStockSound;
				}
				break;

			// curse (all +1, enemie's all -1)
			case 29:
				for (i = 0; i < PAR_NUM; i++) p->add[i] = 1;

				p = &Players[Player ^ 1];
				for (i = 0; i < PAR_NUM; i++)
				{
					if (p->par[i] > 0) p->add[i] = -1;
				}

				// curse sound
				snd = CurseSound;
				break;
			}
		}

		// display state
		DispState();

		// play sound
		if (snd != NULL) PlayMelody(snd);

		// change values animation
		ChangeAnim();

		// update parameters
		for (i = 0; i < 2; i++)
		{
			p = &Players[i];
			for (j = 0; j < PAR_NUM; j++)
			{
				p->par[j] += p->add[j];

				// check underflow
				switch (j)
				{
				case PAR_FENCE:
					if (p->par[j] < 0)
					{
						p->par[PAR_CASTLE] += p->par[j];
						if (p->par[PAR_CASTLE] < 0) p->par[PAR_CASTLE] = 0;
						p->par[j] = 0;
					}
					break;

				case PAR_BUILDERS:
				case PAR_SOLDIERS:
				case PAR_WIZARDS:
					if (p->par[j] < 1) p->par[j] = 1;
					break;

				default:
					if (p->par[j] < 0) p->par[j] = 0;
					break;
				}
			}
		}

		// test cards permission
		Permission();

		// redraw screen
		DispAll();

		// delay
		WaitMs(200);
	}

	// check player's 0 victory
	if ((Players[0].par[PAR_CASTLE] >= 100) ||
		(Players[1].par[PAR_CASTLE] <= 0))
	{
		WinGame(0);
		return True;
	}

	// check player's 1 victory
	if ((Players[1].par[PAR_CASTLE] >= 100) ||
		(Players[0].par[PAR_CASTLE] <= 0))
	{
		WinGame(1);
		return True;
	}

	// take new card
	TakeCard();

	// change active player
	Player ^= 1;

	// increase stocks
	p = &Players[Player];
	p->par[PAR_BRICKS] += p->par[PAR_BUILDERS];
	p->par[PAR_WEAPONS] += p->par[PAR_SOLDIERS];
	p->par[PAR_CRYSTALS] += p->par[PAR_WIZARDS];

	// display state
	DispState1(Player);

	// test cards permission
	Permission();

	// redraw all cards
	DispCards();

	// clear player's selection frame
	DispTitle();
	return False;
}

// find best card
int FindCard()
{
	int i;
	sPlayer* p = &Players[Player];

	// find best enabled card (expensive cards are better)
	int foundinx = -1; // found card position
	int foundval = 0; // found card value
	int card, testval;
	const sCard* c;
	for (i = 0; i < CARDNUM; i++)
	{
		if (!p->disable[i])
		{
			// get card
			card = p->cards[i];
			c = &Card[card];
			testval = c->costs; // card value

			// workers have preference
			if ((c->param == PAR_BUILDERS) ||
				(c->param == PAR_SOLDIERS) ||
				(c->param == PAR_WIZARDS))
			{
				testval = 100;
			}

			// found card with higher value
			if ((testval > foundval) || ((testval == foundval) && (RandU8() < 0x60)))
			{
				foundinx = i;
				foundval = testval;
			}
		}
	}

	// enabled card not found - find card for discard 
	//    (discard expensive cards, they cannot be use for a long time)
	if (foundinx < 0)
	{
		foundinx = 0;

		for (i = 0; i < CARDNUM; i++)
		{
			if (p->disable[i])
			{
				// get card
				card = p->cards[i];
				c = &Card[card];
				testval = c->costs; // card value

				// found card with higher value
				if (testval > foundval)
				{
					foundinx = i;
					foundval = testval;
				}
			}
		}
	}

	return foundinx;
}

// auto play (return True = end game)
Bool AutoPlay()
{
	// small delay
	WaitMs(400);

	// find next card to play
	int foundinx = FindCard();

	// put card
	return PutCard(foundinx, Players[Player].disable[foundinx]);
}

// play game
void Game(Bool black_comp, Bool red_comp)
{
	u8 ch;

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	// start new game
	NewGame(black_comp, red_comp);

	// main loop
	while (True)
	{
		// active player is computer
		if (Players[Player].comp)
		{
			// auto play
			if (AutoPlay()) return;

			// display cursor
			CursorOn();

			// flush keyboard
			if (!Demo)
				KeyFlush();
			else
			{
				if (JoyGet() != NOKEY) return;
			}
		}
		else
		{
			// input from keyboard
			ch = JoyGet();

			// serve keyboard
			switch (ch)
			{
			// left
			case KEY_LEFT:
				CursorOff();
				SelCard--;
				if (SelCard < 0) SelCard = CARDNUM-1;
				CursorOn();
				KeyFlush();
				break;

			// right
			case KEY_RIGHT:
				CursorOff();
				SelCard++;
				if (SelCard >= CARDNUM) SelCard = 0;
				CursorOn();
				KeyFlush();
				break;

			// discard
			case KEY_UP:
			case KEY_B:
				if (PutCard(SelCard, True)) return;
				CursorOn();
				KeyFlush();
				break;

			// select
			case KEY_DOWN:
			case KEY_A:
				if (PutCard(SelCard, Players[Player].disable[SelCard])) return;
				CursorOn();
				KeyFlush();
				break;

			// help
			case KEY_X:
				CursorOff();
				SelCard = FindCard();
				CursorOn();
				KeyFlush();
				break;

			// exit
			case KEY_Y:
				return;

			// no key
			case NOKEY:
				break;

			// invalid key
			default:
				KeyFlush();
				break;
			}
		}
	}
}
