
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 1-bit pixel graphics
// image width: 14 pixels
// image height: 210 lines
// image pitch: 2 bytes
extern const u8 ImgTiles[420];

// format: 1-bit pixel graphics
// image width: 144 pixels
// image height: 105 lines
// image pitch: 18 bytes
extern const u8 ImgIntro[1890];

// format: 1-bit pixel graphics
// image width: 48 pixels
// image height: 120 lines
// image pitch: 6 bytes
extern const u8 ImgFrame[720];

// format: 1-bit pixel graphics
// image width: 4 pixels
// image height: 5 lines
// image pitch: 1 bytes
extern const u8 ImgComputer[5];

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
