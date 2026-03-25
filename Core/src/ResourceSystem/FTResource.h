// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <iosfwd>

#include "Renderer/FoxtrotRenderer.h"
#include "Debugging/DebugMemAlloc.h"
#include "FTDS/Static/FTString.h"

#include "Plugin/CoreExports.h"

/// @brief Initialization struct for FTResources
struct FTResourceDef
{
	const char* FileName;
	const char* Path;

	FTResourceDef()
		: FileName()
		, Path()
	{
	}
	FTResourceDef(const char* fileName, const char* path)
		: FileName(fileName)
		, Path(path)
	{
	}

	FTResourceDef(const FTDS::String& fileName, const FTDS::String& path)
		: FileName(fileName.C_Str())
		, Path(path.C_Str())
	{
	}
};

/// @brief Base class that wraps the resources used in the game.
class FTResource
{
public:
	/// @brief Every active resource must have an ID.
	/// @return If invalid, returns -1;
	static int ID() { return -1; };

public:
	/// @brief Saves resource properties into a file.
	/// @param ofs This should either be a stream to a .chunk file, or to a dedicated resource file
	/// like FTSpriteAnimation, FTPremade, etc.
	virtual void SaveProperties(std::ofstream& ofs) = 0;

	/// @brief Loads resource properties into an instance.
	/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
	/// like FTSpriteAnimation, FTPremade, etc.
	virtual void LoadProperties(std::ifstream& ifs) = 0;

public:
	virtual ~FTResource() = default;

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

extern "C" CORE_API FTResource* CreateResource(FTResourceDef& def, FoxtrotRenderer* rnd);