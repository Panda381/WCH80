
// ****************************************************************************
//
//                                  Train game
//
// ****************************************************************************

#include "../include.h"

int main(void)
{
	int i;
	DrawClear();

	// intro
	Level = 0;
	InitLevel();
	while (JoyGet() == NOKEY) {}

	// randomize
	RandSeed += Time() + DispFrame + DispLine;

	// set first scene
	Level = LEVFIRST;
	Score = 0;

	while (True)
	{
		// demo
//		i = Level; // save current level

/*
		while (True)
		{
			// play demo scene 0
			Level = 0;
			if (HelpLevel()) break; // break

#ifdef AUTOMODE // automode - run levels to check solutions (help completes scene)
			break;
#endif

			// play random scene
			Level = RandU8MinMax(1, LEVNUM);
			if (HelpLevel()) break; // break
		}
*/

		// return current level
//		Level = i;
		InitLevel();

		// main loop
		while (True)
		{
			// game loop - called every single game step
			if (GameLoop()) break;

			// wait step
			WaitStep();
		}
	}
}
