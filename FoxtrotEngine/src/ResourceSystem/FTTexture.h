// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once
#include <ResourceSystem/FTResource.h>

#include <Math/FTMath.h>

class FoxtrotRenderer;
class FTCore;

/// @brief A class that holds the image data.
/// This can be used as a single sprite, a texture pack for a tilemap, and
/// a spritesheet for an animation, etc.
class FTTexture :
	public FTResource
{
public:
	/// @brief Get original pixel-width of the image. This shall not be edited after the FTTexture is created.
	const UINT GetWidth() const;

	/// @brief Get original pixel-height of the image. This shall not be edited after the FTTexture is created.
	const UINT GetHeight() const;

	/// @brief Get ShaderResourceView for the texture. This shall not be edited after the FTTexture is created.
	const ComPtr<ID3D11ShaderResourceView>& GetSRV() const;

public:
	/// @brief Saves resource properties into a file.
	/// @param ofs A stream to a .chunk file
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @brief Loads resource properties into an instance.
	/// @param ifs A stream from a .chunk file
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief FTTexture is a graphics resource, so it needs a FTRenderer instance for initialization.
	/// @param renderer This is usually a game renderer.
	FTTexture(FTResourceDef& resDef, FoxtrotRenderer* renderer);
	~FTTexture();

protected:
	/// @brief The graphics resource must be processed with renderer before used during runtime.
	/// Example of the process includes initializing meshes, creating textures, etc.
	/// @param renderer Renderer object used for processing graphics resources.
	virtual void Process(FoxtrotRenderer* renderer) override;

private:
	UINT							 mWidth;
	UINT							 mHeight;
	ComPtr<ID3D11ShaderResourceView> mSRV;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;

#endif
};

namespace ChunkKey
{
	namespace FTTexture
	{
		constexpr const char* FT_TEXTURE = "FTTexture";
		constexpr const char* WIDTH		 = "Width";
		constexpr const char* HEIGHT	 = "Height";

	} // namespace FTTexture
} // namespace ChunkKey