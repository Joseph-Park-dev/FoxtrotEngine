#pragma once
#include <Windows.h>


#include "FTCoreEditor.h"
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// g_appBase�� �̿��ؼ� ���������� ��� �Լ� ȣ��
	return FTCoreEditor::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}
//#else
//#include "FTCore.h"
//LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	// g_appBase�� �̿��ؼ� ���������� ��� �Լ� ȣ��
//	return FTCore::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
//}
//#endif // _DEBUG

