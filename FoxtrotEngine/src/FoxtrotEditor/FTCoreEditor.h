#pragma once
/// <FTCore_Editor>
/// Editor fields & functions for development -> Active in Debug build; 
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>

#include "FoxtrotEngine/Core/FTCore.h"

class FTCoreEditor :
    public FTCore
{
public:
	virtual bool Initialize() override;
	virtual void ShutDown()	  override;

public:
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	void InitSingletonManagers() override;

private:
	void ProcessInput()   override;
	void UpdateGame()	  override;
	void GenerateOutput() override;
	void ProcessEvent()	  override;

public:
	bool GetIsUpdatingGame() const { return mIsUpdatingGame; }
	void SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

public:
	FTCoreEditor();

public:
	// FTCore instance runs as a singleton.
	static FTCoreEditor* GetInstance();
	static void			 Release();
private:
	// Singleton field to hold the FTCoreEditor instance.
	static FTCoreEditor* gPInst;

private:
	bool			 mIsUpdatingGame;
	bool			 mIsResizingWindow;
	std::string		 mEditorDataFileName;
	// std::string		 mEditorResolutionKey;

private:
	// These functions are to be included in Initialze()
	bool InitGUI();
};