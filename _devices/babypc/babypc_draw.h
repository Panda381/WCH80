
// ****************************************************************************
//
//                             BabyPC - Drawing
//
// ****************************************************************************

#if USE_DRAW || USE_PRINT || USE_DISP

#ifndef _BABYPC_DRAW_H
#define _BABYPC_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

// Colors
#define COL_BLACK	0		// black color
#define COL_WHITE	1		// white color

#ifndef FONT
#if VMODE == 7
#define FONT		Font80		// default system font
#elif VMODE == 6
#define FONT		FontZx		// default system font
#else
#define FONT		FontThin8x8	// default system font
#endif
#endif

#ifndef FONTCOND
#define FONTCOND	FontCond6x8	// default condensed font
#endif

extern const u8* volatile DrawFont; // current draw font (characters 8x8)
extern const u8* DrawFontCond;	// current draw condensed font (characters 6x8)
extern int PrintPos;		// current print position
extern int PrintRow;		// current print row
//extern u8 PrintCol;		// current print color - graphics mode
extern u8 PrintColBg;		// current background color - graphics mode (used to clear screen)
extern u8 PrintInv;		// print inversion - offset added to characters (128 = inversion)

#if USE_DRAW || USE_PRINT
// clear screen using PrintColBg color
void DrawClear();
#endif

#if USE_DRAW	// 1=use graphics drawing functions
#if VMODE == 1 // only graphics mode

// draw/clear/set/invert pixel
void DrawPoint(int x, int y, u8 col);
void DrawPointClr(int x, int y);
void DrawPointSet(int x, int y);
void DrawPointInv(int x, int y);
void DrawPointFast(int x, int y, u8 col);
void DrawPointClrFast(int x, int y);
void DrawPointSetFast(int x, int y);

// get pixel color
u8 DrawGetPoint(int x, int y);

// draw/clear/set/invert rectangle
void DrawRect(int x, int y, int w, int h, u8 col);
void DrawRectClr(int x, int y, int w, int h);
void DrawRectSet(int x, int y, int w, int h);
void DrawRectInv(int x, int y, int w, int h);

// draw/clear/set/invert horizontal line
void DrawHLine(int x, int y, int w, u8 col);
void DrawHLineClr(int x, int y, int w);
void DrawHLineSet(int x, int y, int w);
void DrawHLineInv(int x, int y, int w);

// draw/clear/set/invert vertical line
void DrawVLine(int x, int y, int h, u8 col);
void DrawVLineClr(int x, int y, int h);
void DrawVLineSet(int x, int y, int h);
void DrawVLineInv(int x, int y, int h);

// draw/clear/set/invert frame
void DrawFrame(int x, int y, int w, int h, u8 col);
void DrawFrameClr(int x, int y, int w, int h);
void DrawFrameSet(int x, int y, int w, int h);
void DrawFrameInv(int x, int y, int w, int h);

// draw/clear/set/invert line
void DrawLine(int x1, int y1, int x2, int y2, u8 col);
void DrawLineClr(int x1, int y1, int x2, int y2);
void DrawLineSet(int x1, int y1, int x2, int y2);
void DrawLineInv(int x1, int y1, int x2, int y2);

// draw/clear/set/invert round (filled circle)
void DrawRound(int x0, int y0, int r, u8 col);
void DrawRoundClr(int x0, int y0, int r);
void DrawRoundSet(int x0, int y0, int r);
void DrawRoundInv(int x0, int y0, int r);

// draw/clear/set/invert circle
void DrawCircle(int x0, int y0, int r, u8 col);
void DrawCircleClr(int x0, int y0, int r);
void DrawCircleSet(int x0, int y0, int r);
void DrawCircleInv(int x0, int y0, int r);

// draw/clear/set/invert ring
void DrawRing(int x0, int y0, int rin, int rout, u8 col);
void DrawRingClr(int x0, int y0, int rin, int rout);
void DrawRingSet(int x0, int y0, int rin, int rout);
void DrawRingInv(int x0, int y0, int rin, int rout);

// draw/clear/set/invert triangle
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, u8 col);
void DrawTriangleClr(int x1, int y1, int x2, int y2, int x3, int y3);
void DrawTriangleSet(int x1, int y1, int x2, int y2, int x3, int y3);
void DrawTriangleInv(int x1, int y1, int x2, int y2, int x3, int y3);

// Draw character normal/width/height/sized (background not changed, graphics coordinates)
void DrawChar(char ch, int x, int y, u8 col);
void DrawCharW(char ch, int x, int y, u8 col);
void DrawCharH(char ch, int x, int y, u8 col);
void DrawChar2(char ch, int x, int y, u8 col);

// Draw character condensed size 6x8 (background not changed, graphics coordinates)
void DrawCharCond(char ch, int x, int y, u8 col);

// Draw character condensed size 6x6 (background not changed, graphics coordinates)
void DrawCharCond6(char ch, int x, int y, u8 col);

// Clear character normal/width/height/sized (background not changed, graphics coordinates)
void DrawCharClr(char ch, int x, int y);
void DrawCharWClr(char ch, int x, int y);
void DrawCharHClr(char ch, int x, int y);
void DrawChar2Clr(char ch, int x, int y);

// Set character normal/width/height/sized (background not changed, graphics coordinates)
void DrawCharSet(char ch, int x, int y);
void DrawCharWSet(char ch, int x, int y);
void DrawCharHSet(char ch, int x, int y);
void DrawChar2Set(char ch, int x, int y);

// Invert character normal/width/height/sized (background not changed, graphics coordinates)
void DrawCharInv(char ch, int x, int y);
void DrawCharWInv(char ch, int x, int y);
void DrawCharHInv(char ch, int x, int y);
void DrawChar2Inv(char ch, int x, int y);

// Draw character normal/width/height/sized with background (graphics coordinates)
void DrawCharBg(char ch, int x, int y, u8 col, u8 colbg);
void DrawCharWBg(char ch, int x, int y, u8 col, u8 colbg);
void DrawCharHBg(char ch, int x, int y, u8 col, u8 colbg);
void DrawChar2Bg(char ch, int x, int y, u8 col, u8 colbg);

// Draw character condensed size 6x8 with background (graphics coordinates)
void DrawCharCondBg(char ch, int x, int y, u8 col, u8 colbg);

// Draw character condensed size 6x6 with background (graphics coordinates)
void DrawCharCond6Bg(char ch, int x, int y, u8 col, u8 colbg);

// Draw ASCIIZ text normal/width/height/sized (background not changed, graphics coordinates)
void DrawText(const char* text, int x, int y, u8 col);
void DrawTextW(const char* text, int x, int y, u8 col);
void DrawTextH(const char* text, int x, int y, u8 col);
void DrawText2(const char* text, int x, int y, u8 col);

// Draw text condensed size 6x8 (background not changed, graphics coordinates)
void DrawTextCond(const char* text, int x, int y, u8 col);

// Draw text condensed size 6x6 (background not changed, graphics coordinates)
void DrawTextCond6(const char* text, int x, int y, u8 col);

// Clear ASCIIZ text normal/width/height/sized (background not changed, graphics coordinates)
void DrawTextClr(const char* text, int x, int y);
void DrawTextWClr(const char* text, int x, int y);
void DrawTextHClr(const char* text, int x, int y);
void DrawText2Clr(const char* text, int x, int y);

// Set ASCIIZ text normal/width/height/sized (background not changed, graphics coordinates)
void DrawTextSet(const char* text, int x, int y);
void DrawTextWSet(const char* text, int x, int y);
void DrawTextHSet(const char* text, int x, int y);
void DrawText2Set(const char* text, int x, int y);

// Invert ASCIIZ text normal/width/height/sized (background not changed, graphics coordinates)
void DrawTextInv(const char* text, int x, int y);
void DrawTextWInv(const char* text, int x, int y);
void DrawTextHInv(const char* text, int x, int y);
void DrawText2Inv(const char* text, int x, int y);

// Draw ASCIIZ text normal/width/height/sized with background (graphics coordinates)
void DrawTextBg(const char* text, int x, int y, u8 col, u8 colbg);
void DrawTextWBg(const char* text, int x, int y, u8 col, u8 colbg);
void DrawTextHBg(const char* text, int x, int y, u8 col, u8 colbg);
void DrawText2Bg(const char* text, int x, int y, u8 col, u8 colbg);

// Draw text condensed size 6x8 with background (graphics coordinates)
void DrawTextCondBg(const char* text, int x, int y, u8 col, u8 colbg);

// Draw text condensed size 6x6 with background (graphics coordinates)
void DrawTextCond6Bg(const char* text, int x, int y, u8 col, u8 colbg);

// draw image fast - all coordinates and dimensions must be multiply of bytes and must be valid
void DrawImgFast(const u8* img, int x, int y, int xs, int ys, int w, int h, int wsb);
void DrawImgInvFast(const u8* img, int x, int y, int xs, int ys, int w, int h, int wsb);

// draw mono image, transparent background
void DrawImg(const u8* img, int x, int y, int w, int h, int wsb, u8 col);

// draw mono image with background
void DrawImgBg(const u8* img, int x, int y, int w, int h, int wsb, u8 col, u8 colbg);

// clear mono image
void DrawImgClr(const u8* img, int x, int y, int w, int h, int wsb);

// set mono image
void DrawImgSet(const u8* img, int x, int y, int w, int h, int wsb);

// invert mono image
void DrawImgInv(const u8* img, int x, int y, int w, int h, int wsb);

#endif // VMODE == 1 // only graphics mode
#endif // USE_DRAW

#if USE_PRINT		// 1=use text printing functions

// clear screen using PrintColBg color
INLINE void PrintClear() { DrawClear(); }

// scroll screen (using PrintColBg color)
void PrintScroll();

// print character at text position
void PrintCharAt(char ch, int x, int y);

// print ASCIIZ text at text position
void PrintTextAt(const char* text, int x, int y);

// print text with length at text position
void PrintTextLenAt(const char* text, int len, int x, int y);

// reset print position
void PrintHome(void);

// print new line
void PrintNewLine(void);

// print character RAW at print position, without control characters
void PrintCharRaw(char ch);

// print character RAW repeat, without control characters
void PrintCharRawRep(char ch, int num);

// print space character
void PrintSpc(void);

// print space character repeat
void PrintSpcRep(int num);

// print space to position
void PrintSpcTo(int pos);

// print character at print position
//   Control characters:
//     0x01 '\1' ^A ... set not-inverted text
//     0x02 '\2' ^B ... set inverted text
//     0x07 '\a' ^G ... (bell) move cursor 1 position right (no print)
//     0x08 '\b' ^H ... (back space) move cursor 1 position left (no print)
//     0x09 '\t' ^I ... (tabulator) move cursor to next 8-character position, print spaces
//     0x0A '\n' ^J ... (new line) move cursor to start of next row
//     0x0B '\v' ^K ... (vertical tabulator) move cursor to start of previous row
//     0x0C '\f' ^L ... (form feed) clear screen, reset cursor position and set default color
//     0x0D '\r' ^M ... (carriage return) move cursor to start of current row
void PrintChar(char ch);

// print character repeat
void PrintCharRep(char ch, int num);

// print ASCIIZ text
void PrintText(const char* text);

// print ASCIIZ text RAW, without control characters
void PrintTextRaw(const char* text);

// print text with length
void PrintTextLen(const char* text, int len);

// print text with length RAW, without control characters
void PrintTextLenRaw(const char* text, int len);

#endif // USE_PRINT

#ifdef __cplusplus
}
#endif

#endif // _BABYPC_DRAW_H

#endif // USE_DRAW || USE_PRINT || USE_DISP
