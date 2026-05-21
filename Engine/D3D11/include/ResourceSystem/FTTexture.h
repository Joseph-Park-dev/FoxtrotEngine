// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/D3D11Resource.h"

#include <wrl.h>
#include <d3d11.h>

namespace Core
{
	class IRenderer;
}

namespace D3D11
{
	/// @brief A class that holds the image data.
	/// This can be used as a single sprite, a texture pack for a tilemap, and
	/// a spritesheet for an animation, etc.
	class FTTexture :
		public D3D11Resource
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Get original pixel-width of the image. This shall not be edited after the FTTexture is created.
		const UINT GetWidth() const;

		/// @brief Get original pixel-height of the image. This shall not be edited after the FTTexture is created.
		const UINT GetHeight() const;

		/// @brief Get ShaderResourceView for the texture. This shall not be edited after the FTTexture is created.
		const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV() const;

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
		FTTexture(Common::FTResourceDef& resDef, Core::IRenderer* renderer);
		~FTTexture();

	protected:
		virtual void Process(Common::FTResourceDef& resDef, Core::IRenderer* renderer);

	private:
		UINT											 mWidth;
		UINT											 mHeight;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;

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
} // namespace D3D11