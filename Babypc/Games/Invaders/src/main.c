
// ****************************************************************************
//
//                                  Invaders game
//
// ****************************************************************************

#include "../include.h"

// === score

#define LINEY1		6	// line 1 Y coordinate
#define LINEY2		(HEIGHT-7) // line 2 Y coordinate
#define SCOREY		0	// score Y coordinate
#define SCOREX		0	// score X coordinate
#define MAXSCOREX	(WIDTH-9*6)	// max. score X coordinate
#define LEVELY		(HEIGHT-5) // level Y coordinate
#define LEVELX		(WIDTH-8*6) // level X coordinate

// === spaceship

#define SHIPW		12	// ship width
#define SHIPH		7	// ship height
#define SHIPIMGX	(0/8)	// ship image X coordinate in sprites
#define SHIPIMGY	24	// ship image Y coordinate in sprites
#define SHIPEXPIMGX	(16/8)	// ship explosion image X coordinate
#define SHIPEXPIMGY	24	// ship explosion image Y coordinate
#define SHIPY		(HEIGHT-8-SHIPH) // ship Y coordinate
#define SHIPX		((WIDTH - SHIPW)/2) // ship initial X coordinate

#define LIVEIMGX	(24/8)	// live image X coordinate in sprites
#define LIVEIMGY	40	// live image Y coordinate in sprites
#define LIVEW		8	// live width
#define LIVEH		5	// live height
#define LIVEDX		10	// delta X of ship lives

#define SHIP_COL	COL_WHITE	// ship color

// === house

#define HOUSEW		20	// house width
#define HOUSEH		12	// house height
#define HOUSEIMGX	(0/8)	// house image X coordinate in sprites
#define HOUSEIMGY	32	// house image Y coordinate in sprites
#define HOUSEY		(SHIPY-1-HOUSEH) // house Y coordinate

#define HOUSENUM	4	// number of houses
#define HOUSEDX		32	// house delta X coordinate
#define HOUSESPACEX	(HOUSEDX - HOUSEW) // space between houses
#define HOUSETOTW	(HOUSENUM*HOUSEDX - HOUSESPACEX) // house total width
#define HOUSEX		((WIDTH-HOUSETOTW)/2) // house X coordinate

#define HOUSE_COL	COL_WHITE	// house color

// === mothership

#define MOTHERY		8	// alien mothership Y coordinate
#define MOTHERW		16	// alien mothership width
#define MOTHERH		7	// alien mothership height
#define MOTHERIMGX	(0/8)	// alien mothership image X coordinate in sprites
#define MOTHERIMGY	48	// alien mothership image Y coordinate in sprites

#define MOTHEREXPIMGX	(16/8)	// alien mothership explosion image X coordinate in sprites
#define MOTHEREXPIMGY	48	// alien mothership explosion image Y coordinate in sprites

#define NEXTMOTHER	500	// number of steps to generate new mothership
#define MOTHER_COL	COL_WHITE // mothership color

// === aliens

#define ALIENW		12	// alien width
#define ALIENH		8	// alien height

#define ALIENNUMX	8	// number of aliens in X direction
#define ALIENNUMY	5	// number of aliens in Y direction
#define ALIENNUM	(ALIENNUMX*ALIENNUMY) // number of aliens (= 55)
#define ALIENSPACEX	4	// space between aliens in X direction
#define ALIENDX		(ALIENW+ALIENSPACEX) // alien delta X coordinate (= 16)
#define ALIENTOTW	(ALIENNUMX*ALIENDX - ALIENSPACEX) // alien total initial widh (= 172)
#define ALIENTOTX	(((WIDTH - ALIENTOTW)/2) & ~3) // alien block initial X coordinate, rounded down to 4 pixels (= 40)

#define ALIENSPACEY	2	// space between aliens in Y direction
#define ALIENDY		(ALIENH+ALIENSPACEY) // alien delta Y coordinate (= 16)
#define ALIENTOTH	(ALIENNUMY*ALIENDY - ALIENSPACEY) // alien total initial height (= 72)
#define ALIENTOTY	16	// alien block initial Y coordinate
#define ALIENMAXY	40	// alien block initial max. Y coordinate
#define ALIENMINX	0	// alien min. X coordinate
#define ALIENMAXX	WIDTH	// alien max. X coordinate

#define ALIEN1_COL	COL_WHITE	// alien 1 color
#define ALIEN2_COL	COL_WHITE	// alien 2 color
#define ALIEN3_COL	COL_WHITE	// alien 3 color

// === enemy missiles

#define MISSILENUM	10	// max. number of missiles
#define MISSILEW	4	// enemy missile width
#define MISSILEH	8	// enemy missile height
#define MISSILE1IMGX	(32/8)	// enemy missile 1 image X coordinate
#define MISSILE1IMGY	48	// enemy missile 1 image Y coordinate
#define MISSILE2IMGX	(40/8)	// enemy missile 2 image X coordinate
#define MISSILE2IMGY	48	// enemy missile 2 image Y coordinate

#define MISSILEEXPW	8	// enemy missile explosion width
#define MISSILEEXPH	8	// enemy missile explosion height
#define MISSILEEXPIMGX	(32/8)	// enemy missile explosion image X coordinate
#define MISSILEEXPIMGY	24	// enemy missile explosion image Y coordinate
#define MISSILEINVIMGX	(40/8)	// enemy missile inverted explosion image X coordinate
#define MISSILEINVIMGY	24	// enemy missile inverted explosion image Y coordinate

// === spaceship shoots

#define SHOOTW		4	// shoot width
#define SHOOTH		8	// shoot height
#define SHOOTIMGX	(24/8)	// shoot image X coordinate
#define SHOOTIMGY	32	// shoot image Y coordinate

#define SHOOTEXPW	4	// shoot explosion width
#define SHOOTEXPH	8	// shoot explosion height
#define SHOOTEXPX	(32/8)	// shoot explosion image X coordinate
#define SHOOTEXPY	32	// shoot explosion image Y coordinate

#define SHOOTEXP2X	(40/8)	// shoot explosion inverted image X coordinate
#define SHOOTEXP2Y	32	// shoot explosion inverted image Y coordinate

u32 OldTime;

// current score
int Score; // current score
int MaxScore = 0; // max. score
int Level; // current level
int NextLive; // score to next live

// spaceship
int Lives; // number of remaining lives
int ShipX; // spaceship X coordinate, multiply of 4

// mothership
Bool MotherOn; // mothership is on
Bool MotherLeft; // moving left
int MotherX; // mothership X coordinate
int NextMother; // counter to next mothership

// aliens
Bool AlienLive[ALIENNUM]; // alien lives
int AlienNum; // number of aliens
int AlienX;	// X coordinate of alien block, multiply of 4
int AlienY;	// Y coordinate of alien block, multiply of 4
int AlienPhase; // alien animation phase (0 or 1)
const u8 AlienType[ALIENNUMY] = { 0, 1, 1, 2, 2 }; // types of aliens
const u8 AlienColor[ALIENNUMY] = { ALIEN1_COL, ALIEN2_COL, ALIEN2_COL, ALIEN3_COL, ALIEN3_COL };
const u8 AlienScore[ALIENNUMY] = { 30, 20, 20, 10, 10 }; // score of aliens
int AlienMinX;	// alien min. X coordinate
int AlienMaxX;	// alien max. X coordinate
Bool AlienLeft; // aliens are moving left
int AlienTimer; // alien timer to next step
int AlienTone; // current alien tone

// alien missiles
Bool MissileOn[MISSILENUM]; // missile is active
int MissileX[MISSILENUM]; // missile X coordinate
int MissileY[MISSILENUM]; // missile Y coordinate
Bool EnemyLast[ALIENNUMX]; // last enemy
int MissilePhase; // missile animation phase

// spaceship shoot
int ShootX, ShootY; // shoot coordinate
Bool ShootOn; // shoot is ON

// tones
const sMelodyNote Tone1Snd[] = { { 3, NOTE_G2 }, { 0, 0 }, };
const sMelodyNote Tone2Snd[] = { { 3, NOTE_F2 }, { 0, 0 }, };
const sMelodyNote Tone3Snd[] = { { 3, NOTE_E2 }, { 0, 0 }, };
const sMelodyNote Tone4Snd[] = { { 3, NOTE_D2 }, { 0, 0 }, };
const sMelodyNote* const Tones[4] = { Tone1Snd, Tone2Snd, Tone3Snd, Tone4Snd, };

// shoot sound
const sMelodyNote ShootSnd[] = {
	{ 1, NOTE_F6 },
	{ 1, NOTE_D6 },
	{ 1, NOTE_CS6 },
	{ 1, NOTE_E6 },
	{ 1, NOTE_DS6 },
	{ 1, NOTE_C6 },
	{ 0, 0 },
};

// mother sound
const sMelodyNote MotherSnd[] = {
	{ 1, NOTE_D5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_F5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_D5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_F5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_D5 },
	{ 1, NOTE_DS5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_F5 },
	{ 1, NOTE_E5 },
	{ 1, NOTE_DS5 },
	{ 0, 0 },
};

// hit sound
const sMelodyNote HitSnd[] = {
	{ 1, NOTE_A3 },
	{ 1, NOTE_F3 },
	{ 1, NOTE_E3 },
	{ 1, NOTE_F3 },
	{ 1, NOTE_E3 },
	{ 1, NOTE_G3 },
	{ 1, NOTE_E3 },
	{ 1, NOTE_D3 },
	{ 1, NOTE_C3 },
	{ 0, 0 },
};

// crash sound
const sMelodyNote CrashSnd[] = {
	{ 3, NOTE_GS2 },
	{ 3, NOTE_G2 },
	{ 3, NOTE_FS2 },
	{ 3, NOTE_F2 },
	{ 3, NOTE_E2 },
	{ 3, NOTE_DS2 },
	{ 3, NOTE_D2 },
	{ 3, NOTE_CS2 },
	{ 3, NOTE_C2 },
	{ 0, 0 },
};

// display sprite image
void DispSprite(int xs, int ys, int x, int y, int w, int h, u8 col)
{
	DrawImgBg(ImgSprites + xs + ys*IMGSPRITE_WBS, x, y, w, h, IMGSPRITE_WBS, col, COL_BLACK);
}

// invert sprite image
void DispSpriteInv(int xs, int ys, int x, int y, int w, int h)
{
	DrawImgInv(ImgSprites + xs + ys*IMGSPRITE_WBS, x, y, w, h, IMGSPRITE_WBS);
}

// display ship
void DispShip()
{
	DispSprite(SHIPIMGX, SHIPIMGY, ShipX, SHIPY, SHIPW, SHIPH, SHIP_COL);
}

// clear ship
void ClearShip()
{
	DrawRectClr(ShipX, SHIPY, SHIPW, SHIPH);
}

// display remaining lives
void DispLives()
{
	int i;
	for (i = 0; i < 10; i++)
	{
		if (i < Lives)
			DispSprite(LIVEIMGX, LIVEIMGY, i*LIVEDX, HEIGHT-LIVEH, LIVEW, LIVEH, SHIP_COL);
		else
			DrawRectClr(i*LIVEDX, HEIGHT-LIVEH, LIVEW, LIVEH);
	}
}

// lost live
void Lost()
{
	ClearShip();
	DispSprite(SHIPEXPIMGX, SHIPEXPIMGY, ShipX, SHIPY, SHIPW, SHIPH, SHIP_COL);
	PlayMelody(CrashSnd);
	WaitMs(2000);
	ClearShip();
	Lives--;
	ShipX = SHIPX;
	if (Lives >= 0) DispShip();
	DispLives();
}

// display score
void DispScore()
{
	char buf[6];
	int n = Score;
	int i, x;
	u8 ch;
	for (i = 4; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[5] = 0;
	DrawTextCond6Bg(buf, SCOREX+6*6, SCOREY, COL_WHITE, COL_BLACK);

	// next live
	if (Score >= NextLive)
	{
		Lives++;
		DispLives();
		NextLive *= 2;
	}
}

// display max. score
void DispMaxScore()
{
	char buf[6];
	int n = MaxScore;
	int i, x;
	u8 ch;
	for (i = 4; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[5] = 0;
	DrawTextCond6Bg(buf, MAXSCOREX+4*6, SCOREY, COL_WHITE, COL_BLACK);
}

// display level
void DispLevel()
{
	char buf[3];
	int n = Level+1;
	if (n > 99) n = 99;
	int i, x;
	u8 ch;
	for (i = 1; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[2] = 0;
	DrawTextCond6Bg(buf, LEVELX+6*6, LEVELY, COL_WHITE, COL_BLACK);
}

// mothership move
void MoveMother()
{
	// generate new mothership
	NextMother--;
	if ((NextMother <= 0) && !MotherOn)
	{
		NextMother = NEXTMOTHER;
		MotherOn = True;
		MotherLeft = !MotherLeft;
		MotherX = MotherLeft ? WIDTH : -MOTHERW;
	}

	// move mothership
	if (MotherOn)
	{
		DrawRectClr(MotherX, MOTHERY, MOTHERW, MOTHERH);
		if (MotherLeft)
		{
			MotherX -= 1;
			if (MotherX < -MOTHERW)
			{
				MotherOn = False;
				return;
			}
		}
		else
		{
			MotherX += 1;
			if (MotherX >= WIDTH)
			{
				MotherOn = False;
				return;
			}
		}
		DispSprite(MOTHERIMGX, MOTHERIMGY, MotherX, MOTHERY, MOTHERW, MOTHERH, MOTHER_COL);
	}
}

// prepare alien limits
void LimitAlien()
{
	int x, y, xmin, xmax, i;
	xmin = WIDTH;
	xmax = 0;
	for (i = 0; i < ALIENNUM; i++)
	{
		if (AlienLive[i])
		{
			y = i / ALIENNUMX;
			x = i - y*ALIENNUMX;
			y = y*ALIENDY;
			x = x*ALIENDX;
			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;	
		}
	}
	AlienMinX = ALIENMINX - xmin;
	AlienMaxX = ALIENMAXX - ALIENDX - xmax + ALIENSPACEX;
}

// clear one alien
void ClearAlien(int inx)
{
	int x, y;
	y = inx / ALIENNUMX;
	x = inx - y*ALIENNUMX;
	y = AlienY + y*ALIENDY;
	x = AlienX + x*ALIENDX;
	DrawRectClr(x, y, ALIENW, ALIENH);
}

// clear all aliens
void ClearAliens()
{
	int i;
	for (i = 0; i < ALIENNUM; i++) if (AlienLive[i]) ClearAlien(i);
}

// display one alien
void DispAlien(int inx)
{
	int x, y, ys;

	y = inx / ALIENNUMX;
	x = inx - y*ALIENNUMX;
	ys = AlienType[y]*ALIENH;
	u8 col = AlienColor[y];
	y = AlienY + y*ALIENDY;
	x = AlienX + x*ALIENDX;

	if (AlienLive[inx])
		DispSprite(AlienPhase*2, ys, x, y, ALIENW, ALIENH, col);
	else
		DispSprite(2*2, ys, x, y, ALIENW, ALIENH, col); // explosion

	// end game
	if (y+5 >= SHIPY)
	{
		// lost live
		Lost();
		ClearAliens();
		AlienY -= 4*7;
	}
}

// display all aliens
void DispAliens()
{
	int i;
	for (i = 0; i < ALIENNUM; i++) if (AlienLive[i]) DispAlien(i);
}

// display/clear all missiles
void DispClearMissile()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			if (MissilePhase)
				DispSpriteInv(MISSILE2IMGX, MISSILE2IMGY, MissileX[i], MissileY[i], MISSILEW, MISSILEH);
			else
				DispSpriteInv(MISSILE1IMGX, MISSILE1IMGY, MissileX[i], MissileY[i], MISSILEW, MISSILEH);
		}
	}
}

// display/clear shoot
void DispClearShoot()
{
	if (ShootOn) DispSpriteInv(SHOOTIMGX, SHOOTIMGY, ShootX, ShootY, SHOOTW, SHOOTH);
}

// check house collision (returns number of house pixels in area 8x4)
int CheckHouse(int x, int y)
{
	// check base Y cooordinate
	if ((y + 8 <= HOUSEY) || (y >= HOUSEY + HOUSEH)) return 0;

	// check base X coordinate
	if ((x + 4 <= HOUSEX) || (x >= HOUSEX + HOUSETOTW)) return 0;

	// check closer X coordinate
	int i = (x - HOUSEX) % HOUSEDX;
	if (i >= HOUSEW) return 0;

	// count pixels of house
	int num = 0;
	for (i = 8; i > 0; i--)
	{
		if ((y >= HOUSEY) && (y < HOUSEY + HOUSEH))
		{
			if (DrawGetPoint(x, y) == HOUSE_COL) num++;
			if (DrawGetPoint(x+1, y) == HOUSE_COL) num++;
			if (DrawGetPoint(x+2, y) == HOUSE_COL) num++;
			if (DrawGetPoint(x+3, y) == HOUSE_COL) num++;
		}
		y++;
	}
	return num;
}

// shoot hit house
void ShootHitHouse()
{
	if (ShootOn)
	{
		if (CheckHouse(ShootX, ShootY) > 16)
		{
			ShootOn = False;
			DrawRectClr(ShootX+1, HOUSEY, 2, HOUSEH);
		}
	}
}

// missile hit house
void MissileHitHouse()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			if (CheckHouse(MissileX[i], MissileY[i]) > 12)
			{
				MissileOn[i] = False;
				DrawImgClr(ImgSprites + MISSILEEXPIMGX + MISSILEEXPIMGY*IMGSPRITE_WBS, MissileX[i]-2, MissileY[i], MISSILEEXPW, MISSILEEXPH, IMGSPRITE_WBS);
			}
		}
	}
}

// generate missiles
void GenMissile()
{
	int i, j, x, y;
	for (x = 0; x < ALIENNUMX; x++) EnemyLast[x] = True;

	i = ALIENNUM-1;
	for (y = ALIENNUMY-1; y >= 0; y--)
	{
		for (x = ALIENNUMX-1; x >= 0; x--)
		{
			if (AlienLive[i] && EnemyLast[x])
			{
				EnemyLast[x] = False;
				if ((RandU16() < 300) && (AlienY + y*ALIENDY + 8 < SHIPY))
				{
					for (j = 0; j < MISSILENUM; j++)
					{
						if (!MissileOn[j])
						{
							MissileOn[j] = True;
							MissileX[j] = AlienX + x*ALIENDX + 4;
							MissileY[j] = AlienY + y*ALIENDY + 8;
							break;
						}
					}
				}
			}

			i--;
		}
	}
}

// move missiles
void MoveMissile()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			MissileY[i] += 2;
			if (MissileY[i] > SHIPY) MissileOn[i] = False;
		}
	}
}

// shoot collision service
void ShootCollision()
{
	int i, x, y, k;
	if (!ShootOn) return;

	// shoot enemy
	if ((ShootX >= AlienX) && (ShootY >= AlienY) &&
		(ShootX < AlienX + ALIENTOTW) && (ShootY < AlienY + ALIENTOTH))
	{
		x = (ShootX - AlienX)/ALIENDX;
		y = (ShootY - AlienY)/ALIENDY;
		k = y;
		i = x + y*ALIENNUMX;
		x = AlienX + x*ALIENDX - ShootX;
		y = AlienY + y*ALIENDY - ShootY;
		if (AlienLive[i] && (x < ALIENW) && (y < ALIENH))
		{
			ClearAlien(i);
			AlienLive[i] = False;
			AlienNum--;
			ShootOn = False;
			PlayMelody(HitSnd);
			DispAlien(i);
			Score += AlienScore[k];
			DispScore();
			if (Score > MaxScore)
			{
				MaxScore = Score;
				DispMaxScore();
			}			
			WaitMs(100);
			ClearAlien(i);
			LimitAlien();
			return;
		}
	}

	// missile collision
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i] && (MissileX[i] < ShootX+SHOOTW) && (MissileX[i] + MISSILEW >= ShootX) &&
			(MissileY[i] < ShootY+2*SHOOTH) && (MissileY[i] + MISSILEH >= ShootY))
		{
			MissileOn[i] = False;
			ShootOn = False;
			DispSpriteInv(SHOOTEXPX, SHOOTEXPY, ShootX, ShootY, SHOOTEXPW, SHOOTEXPH);
			WaitMs(150);
			DispSpriteInv(SHOOTEXPX, SHOOTEXPY, ShootX, ShootY, SHOOTEXPW, SHOOTEXPH);
		}
	}
	if (!ShootOn) return;

	// shoot mothership
	if (MotherOn && (ShootY < MOTHERY+MOTHERH) && (ShootX+4 >= MotherX) && (ShootX < MotherX+MOTHERW))
	{
		MotherOn = False;
		ShootOn = False;
		DispSprite(MOTHEREXPIMGX, MOTHEREXPIMGY, MotherX, MOTHERY, MOTHERW, MOTHERH, MOTHER_COL);
		StopSound();
		PlayMelody(HitSnd);
		Score += 1000;
		DispScore();
		if (Score > MaxScore)
		{
			MaxScore = Score;
			DispMaxScore();
		}			
		WaitMs(100);
		DrawRectClr(MotherX, MOTHERY, MOTHERW, MOTHERH);
		return;
	}
}

// move shoot
void MoveShoot()
{
	// move shoot
	if (ShootOn)
	{
		ShootY -= 4;
		if (ShootY < MOTHERY)
			ShootOn = False;
		else
		{
			ShootHitHouse();
			if (ShootOn)
			{
				// shoot collision service
				ShootCollision();
			}
		}
	}
}

// spaceship control
void SpaceCtrl()
{
	// move ship left
	if (JoyPressed(KEY_LEFT) && (ShipX >= 2))
	{
		ClearShip();
		ShipX -= 2;
		DispShip();
	}

	// move ship right
	if (JoyPressed(KEY_RIGHT) && (ShipX < WIDTH-ALIENW))
	{
		ClearShip();
		ShipX += 2;
		DispShip();
	}

	// shooting
	if (JoyPressed(KEY_A) && !ShootOn)
	{
		ShootOn = True;
		ShootX = ShipX + 4;
		ShootY = SHIPY - 7;
		PlayMelody(ShootSnd);
	}
}

// move aliens
void AlienMove()
{
	// move aliens
	AlienTimer--;
	if (AlienTimer <= 0)
	{
		// clear aliens
		ClearAliens();

		// shift aliens
		if (AlienLeft)
		{
			AlienX -= 4;
			if (AlienX < AlienMinX)
			{
				AlienX += 8;
				AlienLeft = False;
				AlienY += 4;
			}
		}
		else
		{
			AlienX += 4;
			if (AlienX > AlienMaxX)
			{
				AlienX -= 8;
				AlienLeft = True;
				AlienY += 4;
			}
		}

		DispAliens();	// display aliens
		AlienTimer = AlienNum/4+2;
		AlienPhase = (AlienPhase+1) & 1;

		AlienTone = (AlienTone+1) & 3;
		if (!MotherOn && !PlayingSound()) PlayMelody(Tones[AlienTone]);
	}
}

// crash collision service
void Crash()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			// spaceship collision
			if ((MissileX[i] < ShipX+SHIPW) && (MissileX[i] + MISSILEW >= ShipX) &&
				(MissileY[i] < SHIPY+SHIPH) && (MissileY[i] + MISSILEH >= SHIPY))
			{
				MissileOn[i] = False;
				DrawRectClr(MissileX[i], MissileY[i], MISSILEW, MISSILEH);

				// lost live
				Lost();
			}
		}
	}
}

// new game
void NewGame(int level)
{
	int i;

	// clear screen
	DrawClear();

	// display top and bottom line
	DrawRect(0, LINEY1, WIDTH, 1, COL_WHITE);
	DrawRect(0, LINEY2, WIDTH, 1, COL_WHITE);

	// display score
	DrawTextCond6("SCORE:", SCOREX, SCOREY, COL_WHITE);
	DispScore();

	// display max. score
	DrawTextCond6("MAX:", MAXSCOREX, SCOREY, COL_WHITE);
	DispMaxScore();

	// display level
	Level = level;	// set current level
	DrawTextCond6("LEVEL:", LEVELX, LEVELY, COL_WHITE);
	DispLevel();

	// display remaining lives
	if (level == 0)
	{
		Lives = 3;
		NextLive = 1500; // score to next live
		Score = 0;
	}
	DispLives();

	// display ship
	ShipX = SHIPX;
	ShootOn = False;
	DispShip();

	// draw houses
	for (i = 0; i < HOUSENUM; i++)
		DispSprite(HOUSEIMGX, HOUSEIMGY, HOUSEX + i*HOUSEDX, HOUSEY, HOUSEW, HOUSEH, HOUSE_COL);

	// mothership
	NextMother = NEXTMOTHER; // score of next mothership
	MotherOn = False;
	MotherLeft = True;

	// aliens
	AlienX = ALIENTOTX;
	AlienY = ALIENTOTY + level*4;
	if (AlienY > ALIENMAXY) AlienY = ALIENMAXY;
	for (i = 0; i < ALIENNUM; i++) AlienLive[i] = True;
	AlienNum = ALIENNUM;
	AlienPhase = 0;
	AlienLeft = True;
	LimitAlien();
	DispAliens();
	AlienTimer = 1;
	AlienTone = 0;

	// clear missiles
	for (i = 0; i < MISSILENUM; i++) MissileOn[i] = False;
	MissilePhase = 0;

	// next step timer
	OldTime = Time();
}

// game loop
void Game()
{
	u32 t;
	int x, y, i;

	while (True)
	{
		// clear all missiles
		DispClearMissile();
		MissilePhase = (MissilePhase + 1) & 1;

		// clear shoot
		DispClearShoot();

		// move shoot
		MoveShoot();

		// spaceship control
		SpaceCtrl();

		// move aliens
		AlienMove();

		// serve missiles
		MoveMissile();
		GenMissile();
		MissileHitHouse();

		// mothership move
		MoveMother();

		// shoot collision service
		ShootCollision();

		// crash collision service
		Crash();

		switch (JoyGet())
		{
		// reset to boot loader
		case KEY_X: return;
		}

		// new level
		if (AlienNum == 0)
		{
			DrawTextCond6Bg(" LEVEL UP ", (WIDTH-10*6)/2, (HEIGHT-6)/2, COL_WHITE, COL_WHITE);
			WaitMs(1000);
			KeyFlush();
			NewGame(Level+1);
		}

		// end game
		if (Lives < 0)
		{
			DrawTextCond6Bg(" GAME OVER ", (WIDTH-11*6)/2, (HEIGHT-6)/2, COL_WHITE, COL_WHITE);
			WaitMs(500);
			StopSound();
			KeyFlush();
			while (JoyGet() == NOKEY) {}
			return;
		}


		// display all missiles
		DispClearMissile();

		// display shoot
		DispClearShoot();

		// mother sound
		if (MotherOn && !PlayingSound()) PlayMelody(MotherSnd);

		// time step 50 ms
		do { t = Time(); } while ((u32)(t - OldTime) < (u32)(50000*HCLK_PER_US));
		OldTime = t;
	}
}

int main(void)
{
	// display splash screen
	KeyWaitNoPressed();
	DrawImgBg(ImgIntro, 0, 0, WIDTH, HEIGHT, WIDTHBYTE, COL_WHITE, COL_BLACK);
	while (JoyGet() == NOKEY) {}
	DrawClear();

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	// select game
	while (True)
	{
		// new game, 1st level
		NewGame(0);

		// startup delay
		WaitMs(2000);

		// game loop
		Game();
	}
}
