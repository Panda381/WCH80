
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// format: pseudo graphics with cell 2x2 pixels
// text width: 37 characters
// text height: 25 rows
// text pitch: 37 bytes
// image width: 74 pixels
// image height: 50 lines
//extern const u8 ImgIntro[925];

// format: 1-bit pixel graphics
// image width: 2048 pixels
// image height: 8 lines
// image pitch: 256 bytes
extern const u8 FontBold8x8Custom[2048];

// text screen
#define FONTW	8		// font width
#define FONTH	8		// font height
#define TEXTW	40		// width of text screen
#define TEXTH	30		// height of text screen

// wait for a character
char WaitChar();

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
