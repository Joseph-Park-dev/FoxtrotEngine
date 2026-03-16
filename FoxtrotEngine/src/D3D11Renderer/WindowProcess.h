// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// WndProc to run the game loop based on the Windows platform.
/// </summary>

#pragma once
#include <Windows.h>

#ifndef WINDOW_PROC
#define WINDOW_PROC

#include "FTCore.h"

inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//return FTCoreEditor::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
	return FTCore::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"

inline LRESULT WINAPI WndProc_FTEditor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FTCoreEditor::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}
#endif // FOXTROT_ENGINE

#endif // !WINDOW_PROC