#pragma once
#ifdef D3D11_EXPORTS
	#define D3D11_API __declspec(dllexport)
#else
	#define D3D11_API __declspec(dllimport)
#endif