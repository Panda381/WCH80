
// ****************************************************************************
//
//                              Ants game
//
// ****************************************************************************

#include "../include.h"

// draw open screen
void DrawOpen()
{
	PrintClear();

/*
	int i, j;
	u8* dst = &FrameBuf[TEXTWIDTH-18 - 10 + 2*TEXTWIDTH];
	const u8* src = ImgIntro;
	for (i = 0; i < 12; i++)
	{
		memcpy(dst, src, 18);
		dst += TEXTWIDTH;
		src += 18;
	}
*/

//	PrintCol = COL_WHITE;

	PrintText(
/*
		"\n\nNone of the black or red ants remember\n"
		"who and when started their age-old war\n"
		"for the anthill \"At the Two Spruces\".\n"
		"In order to put an end to the eternal\n"
		"battles, they decided to build castles,\n"
		"with the help of which they would gain\n"
		"dominance over the whole territory.\n"
		"The aim of the game is to build a\n"
		"castle with a height of 100 or destroy\n"
		"the opponent's castle. Each card\n"
		"consumes certain amount of resources\n"
		"(number at the top right). The rate\n"
		"of resource generation depends on\n"
		"the amount of workers.\n\n\n"
*/
		"\n\n\n     ANTS\n"
		"\n\n\n"

		"Press key to start:\n\n"
		"LEFT .... Play for black ants\n"
		"RIGHT ... Play for red ants\n"
		"DOWN .... 2 players\n"
		"UP ...... Demo\n");
}

int main(void)
{
	KeyWaitNoPressed();

	// draw open screen
	DrawOpen();

	while (True)
	{
		switch (JoyGet())
		{
		// play for black ants
		case KEY_LEFT:
			Game(False, True);
			DrawOpen();
			break;

		// play for red ants
		case KEY_RIGHT:
			Game(True, False);
			DrawOpen();
			break;

		// play 2 players
		case KEY_DOWN:
			Game(False, False);
			DrawOpen();
			break;

		// play demo
		case KEY_UP:
			Game(True, True);
			DrawOpen();
			break;
		}
	}
}
