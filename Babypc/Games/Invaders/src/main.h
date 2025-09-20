
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: 1-bit pixel graphics
// image width: 160 pixels
// image height: 120 lines
// image pitch: 20 bytes
extern const u8 ImgIntro[2400];

// format: 1-bit pixel graphics
// image width: 48 pixels
// image height: 56 lines
// image pitch: 6 bytes
extern const u8 ImgSprites[336];
#define IMGSPRITE_WBS	6

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
