#pragma once
#ifdef SOUND_EXPORTS
	#define SOUND_API __declspec(dllexport)
#else
	#define SOUND_API __declspec(dllimport)
#endif
