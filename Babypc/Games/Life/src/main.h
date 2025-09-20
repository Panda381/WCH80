
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//#define DRV	VideoVGA // timings
#define TILESIZE 8	// tile size in number of pixels
#define MAPW0	22	// hidden width
#define MAPH0	17	// hidden height
#define MAPW	(WIDTH/TILESIZE) // real board width (=20)
#define MAPH	(HEIGHT/TILESIZE) // real board height (=15)
#define MAPSIZE	(MAPW0*MAPH0) // board size (=374)
#define SLOTNUM	1	// number of slots
#define MAPX	((MAPW0-MAPW)/2) // start X
#define MAPY	((MAPH0-MAPH)/2) // start X

#define SPEED	200	// speed, sleep in [ms]

#define TILE_EMPTY	0	// empty tile
#define	TILE_FULL	1	// full tile
#define TILE_SELEMPTY 	2	// selected empty tile
#define TILE_SELFULL	3	// selected full tile

#define TILE_NUM	4	// number of tiles types

// format: 1-bit pixel graphics
// image width: 160 pixels
// image height: 120 lines
// image pitch: 20 bytes
extern const u8 ImgIntro[2400];

// format: 1-bit pixel graphics
// image width: 8 pixels
// image height: 8 lines
// image pitch: 1 bytes
extern const u8 ImgTileEmpty[8];
extern const u8 ImgTileEmptySel[8];
extern const u8 ImgTileCell[8];
extern const u8 ImgTileCellSel[8];

#define TILE_WB	1

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
