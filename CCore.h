#pragma once
#include <SDL2/SDL.h>
#define SDL_MAIN_HANDLED
#include <vector>
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl.h> // ComPtr

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
	SDL_Renderer* GetGameRenderer()	  const { return mGameRenderer; }
	Vector2		  GetResolution()	  const { return mResolution; }

#ifdef _DEBUG
	SDL_Window*   GetEditorWindow()   const { return mEditorWindow; }
	SDL_Renderer* GetEditorRenderer() const { return mEditorRenderer; }
	bool		  GetIsUpdatingGame() const { return mIsUpdatingGame; }
	Vector2		  GetEditorWindowSize() const;
	void		  SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

#endif // _DEBUG

private:
	std::string		mGameTitle;
	SDL_Window*		mGameviewWindow;
	Vector2			mResolution;
	SDL_Renderer*   mGameRenderer;
	Uint32			mTicksCount;
	bool			mIsRunning;

#ifdef _DEBUG
	SDL_Window*		mEditorWindow;
	SDL_Renderer*   mEditorRenderer;
	bool			mIsUpdatingGame;
	Vector2			mEditorResolution;
	std::string		mEditorDataFileName;
	std::string		mEditorResolutionKey;
#endif // _DEBUG

public:
	CCore();
	~CCore();
	static CCore* GetInstance();
	static void	  Release();

private:
	bool InitSDL();
	bool InitMainWindow();
	bool InitSDLRenderer();
	bool InitDirect3DRenderer();
	void InitSingletonManagers();
	void InitTicks();

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