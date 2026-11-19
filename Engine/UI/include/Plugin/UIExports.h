#pragma once
#ifdef UI_EXPORTS
	#define UI_API __declspec(dllexport)
#else
	#define UI_API __declspec(dllimport)
#endif
