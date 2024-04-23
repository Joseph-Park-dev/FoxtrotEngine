#pragma once
#pragma comment(lib,"d3d11.lib")

#define SDL_MAIN_HANDLED
#include <vector>
#include <string>
#include <windows.h>

#include "FoxtrotRenderer.h"
#include "SingletonMacro.h"
#include "Math.h"

class CCore
{
private:
	static CCore* gPInst;

public:
	bool Initialize();
	void RunLoop();
	void ShutDown();

public:
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Vector2			 GetResolution()	  const { return mResolution; }
	FoxtrotRenderer* GetEditorRenderer()  const	{ return mEditorRenderer; }

#ifdef _DEBUG
	bool		  GetIsUpdatingGame() const { return mIsUpdatingGame; }
	void		  SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

#endif // _DEBUG

private:
	std::wstring	 mGameTitle;
	//SDL_Window*		mGameviewWindow;
	Vector2			 mResolution;
	//FoxtrotRenderer*   mGameRenderer;
	bool			 mIsRunning;

#ifdef _DEBUG
	HWND			 mEditorWindow;
	FoxtrotRenderer* mEditorRenderer;
	bool			 mIsUpdatingGame;
	int				 mEditorWidth;
	int				 mEditorHeight;
	std::string		 mEditorDataFileName;
	std::string		 mEditorResolutionKey;
#endif // _DEBUG

public:
	CCore();
	~CCore();
	static CCore* GetInstance();
	static void	  Release();

private:
	bool InitMainWindow();
	bool InitFoxtrotRenderer_D3D11();
	bool InitGUI();
	void InitSingletonManagers();
	void InitTimer();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void ProcessEvent();

//#ifdef _DEBUG
//public:
//	void SaveEditorData(std::ofstream& ofs);
//#endif
};