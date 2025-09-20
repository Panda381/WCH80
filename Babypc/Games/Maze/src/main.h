
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define TILESIZE 1	// tile size in number of pixels
#define MAPW	40	// board width
#define MAPH	30	// board height
#define MAPSIZE	(MAPW*MAPH) // board size (=1200)

#define MAZEW	(((MAPW+1)/2*2)-1) // maze width (=39, must be odd number)
#define MAZEH	(((MAPH+1)/2*2)-1) // maze height (=29, must be odd number)

#define SPEED	1 //50	// step speed (delay in [ms])

// tile indices
enum {
	S_BLACK = 0,		// 0: black tile
	S_EMPTY_HIDE,		// 1: hidden empty field (grass)
	S_WALL_HIDE,		// 2: hidden wall
	S_DOOR_HIDE,		// 3: hidden door
	S_EMPTY,		// 4: empty field (grass)
	S_WALL,			// 5: wall
	S_DOOR,			// 6: door
	S_FACE,			// 7: face on empty field

	TILE_NUM		// number of tiles
};

// format: pseudo graphics with cell 2x2 pixels
// text width: 40 characters
// text height: 30 rows
// text pitch: 40 bytes
// image width: 80 pixels
// image height: 60 lines
// attribute width: 40 colors
// attribute height: 30 rows
// attribute pitch: 20 bytes
extern const u8 ImgIntro[1200];

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
