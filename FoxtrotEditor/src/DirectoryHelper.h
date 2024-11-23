#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <Windows.h>
#include <commdlg.h>
#include <shlobj_core.h>

#include "FTCoreEditor.h"
#include "Managers/ResourceManager.h"

#define PATH_PROJECT DirectoryHelper::GetInstance()->GetProjectPath()
#define PATH_CHUNK DirectoryHelper::GetInstance()->GetChunkPath()
#define CHUNK_IS_SAVED DirectoryHelper::GetInstance()->GetCurrChunkSaved()
#define SET_CHUNK_IS_SAVED(saved) DirectoryHelper::GetInstance()->SetCurrChunkSaved(saved);

class DirectoryHelper
{
	SINGLETON(DirectoryHelper)

public:
	template <class UnaryOperation>
	static void IterateForFileRecurse(
		std::string& dir,
		UnaryOperation&& unaryOp
	)
	{
		for (const std::filesystem::directory_entry& dirEntry :
			std::filesystem::recursive_directory_iterator(dir))
		{
			if(dirEntry.is_regular_file())
				unaryOp(std::move(dirEntry.path().string()));
		}
	}

public:
	std::string& GetProjectPath();
	std::string& GetChunkPath();
	bool GetCurrChunkSaved() const;

	void SetProjPath(std::string&& path);
	void SetChunkPath(std::string&& path);
	void SetCurrChunkSaved(bool val);

private:
	std::string mCurrProjectPath;
	std::string mCurrChunkPath;
	bool		mCurrChunkSaved;
};