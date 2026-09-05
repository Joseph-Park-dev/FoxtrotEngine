#pragma once
#ifdef MATH_STATIC
	#define MATH_API
#elif defined(MATH_EXPORTS)
	#define MATH_API __declspec(dllexport)
#else
	#define MATH_API __declspec(dllimport)
#endif