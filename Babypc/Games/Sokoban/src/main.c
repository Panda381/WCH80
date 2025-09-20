
// ****************************************************************************
//
//                              Sokoban game
//
// ****************************************************************************

#include "../include.h"

// wait for a character
char WaitChar()
{
	char c;
	while ((c = JoyGet()) == NOKEY) {}
	return c;
}

int main(void)
{
	// use custom font
	DrawFont = FontBold8x8Custom; // custom font

	// display splash screen
	KeyWaitNoPressed();
	PrintClear();
/*
	const u8* src = ImgIntro;
	u8* dst = &FrameBuf[1 + 2*WIDTHBYTE];
	int i;
	for (i = 25; i > 0; i--)
	{
		memcpy(dst, src, 37);
		dst += WIDTHBYTE;
		src += 37;
	}
	while (JoyGet() == NOKEY) {}
*/
	// randomize
//	RandSeed += Time() + DispFrame + DispLine;

	while (True)
	{
		// select level
		LevSelect();
		RandSeed += Time() + DispFrame + DispLine;

		// game loop
		GameLoop();
	}
}
