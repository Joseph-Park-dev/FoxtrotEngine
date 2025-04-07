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
#include <string>
#include <Windows.h>

class FTResource
{
public:
	const std::string& GetFileName() { return mFileName; }
	const std::string& GetRelativePath() { return mRelativePath; }
	UINT			   GetRefCount() { return mRefCount; }
	bool			   GetIsProcessed() { return mIsProcessed; }

	void SetFileName(const std::string name) { mFileName.assign(name); }
	void SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }
	void SetIsReferenced(bool val) { mRefCount = val; }
	void SetIsProcessed(bool val) { mIsProcessed = val; }

	std::string& FileName() { return mFileName; }

	bool IsReferenced();
	void AddRefCount() { ++mRefCount; }
	void SubtractRefCount() { --mRefCount; }

public:
	FTResource();
	virtual ~FTResource() {}

private:
	std::string mFileName;
	std::string mRelativePath;

	// The resource is used in somewhere.
	UINT mRefCount;
	bool mIsProcessed;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key);
	virtual UINT LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() {};
	void		 UpdateNameAndPath(std::string fileExtension);
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* KEY			= "Key";
	constexpr const char* FILE_NAME		= "FileName";
	constexpr const char* RELATIVE_PATH = "RelativePath";
} // namespace ChunkKey