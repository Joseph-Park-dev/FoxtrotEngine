#pragma once
/// <FTCore_Editor>
/// Editor fields & functions for development -> Active in Debug build; 
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>

#include "Core/FTCore.h"

class FTCoreEditor :
    public FTCore
{
	SINGLETON(FTCoreEditor);

public:
	virtual bool Initialize() override;
	virtual void ShutDown()	  override;

public:
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

public:
	bool GetIsUpdatingGame() const { return mIsUpdatingGame; }
	void SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

private:
	bool			 mIsUpdatingGame;
	bool			 mIsResizingWindow;
	std::string		 mEditorDataFileName;
	// std::string		 mEditorResolutionKey;

private:
// These functions are to be included in Initialze()
	void InitSingletonManagers() override;
	bool InitGUI();

private:
	void ProcessInput()   override;
	void UpdateGame()	  override;
	void GenerateOutput() override;
	void ProcessEvent()	  override;
};