#pragma once
#ifdef MATH_EXPORTS
	#define MATH_API __declspec(dllexport)
#else
	#define MATH_API __declspec(dllimport)
#endif