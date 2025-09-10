// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <Static/FTString.h>

class FTCore;
class FoxtrotRenderer;

/// @brief Initialization struct for FTResources
struct FTResourceDef
{
	const char* FileName;
	const char* RelativePath;

	FTResourceDef()
		: FileName()
		, RelativePath()
	{
	}

	FTResourceDef(const char* fileName, const char* relPath)
		: FileName(fileName)
		, RelativePath(relPath)
	{
	}

	FTResourceDef(const FTDS::String& fileName, const FTDS::String& relPath)
		: FileName(fileName.C_Str())
		, RelativePath(relPath.C_Str())
	{
	}
};

/// @brief Base class that wraps the resources used in the game.
class FTResource
{
public:
	/// @brief Saves resource properties into a file.
	/// @param ofs This should either be a stream to a .chunk file, or to a dedicated resource file
	/// like FTSpriteAnimation, FTPremade, etc.
	virtual void SaveProperties(std::ofstream& ofs);

	/// @brief Loads resource properties into an instance.
	/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
	/// like FTSpriteAnimation, FTPremade, etc.
	virtual void LoadProperties(std::ifstream& ifs);

public:
	const FTDS::String& GetFileName() const;
	const FTDS::String& GetRelativePath() const;

	/// @brief Is this resource processed and can be used during runtime?
	const bool IsProcessed() const;

public:
	FTResource(FTResourceDef& resDef);
	virtual ~FTResource();

protected:
	/// @brief A resource must be processed before used during runtime.
	/// Example of the process includes initializing meshes, creating textures, etc.
	virtual void Process();

	/// @brief A graphics resource must be processed with renderer before used during runtime.
	/// Example of the process includes initializing meshes, creating textures, etc.
	/// @param renderer Renderer object used for processing graphics resources.
	/// @todo Is this member function necessary?
	virtual void Process(FoxtrotRenderer* renderer);

private:
	/// @brief Name of the resource.
	FTDS::String* mFileName;

	/// @brief Resource path relative to the directory containing .exe.
	FTDS::String* mRelativePath;

	/// @brief If 0 < mRefCount, the resource is used somewhere in the .chunk, so it will be saved to the file.
	/// This cannot be smaller than zero.
	int mRefCount;

	/// @brief The resource is processed and ready to be used.
	bool mIsProcessed;

#ifdef FOXTROT_EDITOR
public:
	/// @brief Displays GUI to modify the data on Foxtrot Editor.
	virtual void UpdateUI() {};

public:
	/// @brief Is this resource referenced by any of the Components in the .chunk?
	/// @return if 0 < mRefCount, true.
	bool IsReferenced() const;

	/// @brief Adds mRefCount if the resource is referenced by any of the Components in a .chunk.
	virtual void AddRefCount();

	/// @brief Subtracts mRefCount if the resource is taken away from a Component in a .chunk.
	virtual void SubtractRefCount();
#endif
};

/// @brief String literal keys used for saving resource properties.
namespace ChunkKey
{
	namespace FTResource
	{
		constexpr const char* FILE_NAME		= "FileName";
		constexpr const char* RELATIVE_PATH = "RelativePath";
	} // namespace FTResource
} // namespace ChunkKey