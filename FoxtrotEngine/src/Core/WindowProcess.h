#pragma once
#include <Windows.h>

#ifdef FOXTROT_EDITOR

#include "FTCoreEditor.h"
#ifndef WINDOW_PROC
#define WINDOW_PROC
inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FTCoreEditor::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}
#endif

#else
#include "FTCore.h"
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FTCore::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}
#endif // !WINDOW_PROC