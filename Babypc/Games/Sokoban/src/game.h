
// ****************************************************************************
//
//                                Game engine
//
// ****************************************************************************

#ifndef _GAME_H
#define _GAME_H

#ifdef __cplusplus
extern "C" {
#endif

// game speed
#define MOVESPEED	100	// step delay in [ms]

// Define game board, tiles 8x8
#define MAPW	  TEXTWIDTH	// width of game board
#define MAPH	  TEXTHEIGHT	// height of game board
extern u8 Board[MAPW*MAPH];	// game board

// current game state
extern u16 MarkNum;		// number of remaining marks
extern u8 LevelW, LevelH;	// level width and height
extern u8 PosX, PosY;		// player coordinates

// Tiles
enum {
	EMPTY = 0,	// 0: empty (grass)
	WALL,		// 1: wall
	FLOOR,		// 2: floor
	MARK,		// 3: mark
	CRATE,		// 4: crate
	FULL,		// 5: full crate (crate on mark)
	FACE,		// 6: face
	FACE_MARK,	// 7: face on mark

	TILE_NUM	// 8:
};

// direction
enum {
	DIR_R=0,	// right
	DIR_U,		// up
	DIR_L,		// left
	DIR_D,		// down
};

// move player in direction
void MovePlayer(u8 dir);

// clear board
void ClearBoard();

// detect board dimension
void BoardDim();

// display program info
void DispInfo();

// initialize current level
void LevelInit();

// step one level (key = move key, returns False on unsupported key)
Bool Step(char key);

// play level solve (returns True on break from keyboard)
Bool PlaySolve();

// display solved animation (returns True of solved)
Bool SolvedAnim();

// game loop
void GameLoop();

#ifdef __cplusplus
}
#endif

#endif // _GAME_H
