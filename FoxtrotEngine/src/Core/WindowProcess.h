#pragma once
#include <Windows.h>

#include "FTCoreEditor.h"
#ifndef WINDOW_PROC
#define WINDOW_PROC
inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FTCoreEditor::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}
#endif // !WINDOW_PROC

//#else
//#include "FTCore.h"
//LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	// g_appBase를 이용해서 간접적으로 멤버 함수 호출
//	return FTCore::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
//}
//#endif // _DEBUG

