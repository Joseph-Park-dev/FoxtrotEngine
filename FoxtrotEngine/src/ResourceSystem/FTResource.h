// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class that wraps the resources used in the game.
/// </summary>

#pragma once
#include <Windows.h>

#include <Static/FTString.h>

class FTCore;

class FTResource
{
public:
	UINT GetRefCount() { return mRefCount; }
	bool GetIsProcessed() { return mIsProcessed; }

	void SetFileName(FTDS::String& name) { mFileName.Assign(name); }
	void SetFileName(const char* name) { mFileName.Assign(name); }
	void SetRelativePath(FTDS::String& _strPath) { mRelativePath.Assign(_strPath); }
	void SetRelativePath(const char* _strPath) { mRelativePath.Assign(_strPath); }
	void SetIsProcessed(bool val) { mIsProcessed = val; }

	FTDS::String& FileName() { return mFileName; }
	FTDS::String& RelativePath() { return mRelativePath; }

public:
	virtual void Process(FTCore* coreInst) = 0;

public:
	FTResource();
	virtual ~FTResource() { mRefCount = 0; }

private:
	FTDS::String mFileName;
	FTDS::String mRelativePath;

	// The resource is used in somewhere.
	UINT mRefCount;
	bool mIsProcessed;

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() {};
	void		 UpdateNameAndPath(FTDS::String fileExtension);

public:
	bool		 IsReferenced();
	virtual void AddRefCount();
	virtual void SubtractRefCount();
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* FILE_NAME		= "FileName";
	constexpr const char* RELATIVE_PATH = "RelativePath";
} // namespace ChunkKey