#pragma once
#ifdef PHYSICS2D_EXPORTS
	#define PHYSICS2D_API __declspec(dllexport)
#else
	#define PHYSICS2D_API __declspec(dllimport)
#endif