#include "input_ime.h"

#ifdef XASH_SDL
#include <SDL_keyboard.h>
#include <platform/sdl/events.h>
#endif

const char* IME_GetCompositionString()
{
	return "";
}

size_t IME_GetCandidateListCount()
{
	return 0;
}

size_t IME_GetCandidateListPageStart()
{
	return 0;
}

size_t IME_GetCandidateListPageSize()
{
	return 0;
}

size_t IME_GetCandidateListSelectedItem()
{
	return 0;
}

int IME_CandidateListStartsAtOne()
{
	return 0;
}

const char* IME_GetCandidate(size_t i)
{
	return "";
}

void IME_SetInputScreenPos(int x, int y)
{
#ifdef XASH_SDL
	SDL_Rect rect = { x, y };
	SDL_SetTextInputRect(&rect);
#endif
}

void IME_GetInputScreenPos(int* x, int* y)
{
	;
}

int IME_ShouldShowBuiltInCandidateWindow()
{
	return false;
}

void IME_CreateContext()
{
	;
}

void IME_SetIMEEnabled(int enable, int force)
{
#if XASH_INPUT == INPUT_SDL
	SDLash_EnableTextInput(enable, force);
#elif XASH_INPUT == INPUT_ANDROID
	Android_EnableTextInput(enable, force);
#endif
}