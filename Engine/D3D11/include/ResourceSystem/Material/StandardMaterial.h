// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Material/FTMaterial.h"

struct Light;
class FTMeshGroup;

namespace D3D11
{
	/// @brief The value of the material data are to be applied to Pixel Constant Buffers.
	struct BlinnPhongData
	{
		Math::FTVector3 Ambient	  = Math::FTVector3::Zero;
		float			Shininess = 0.0f;
		Math::FTVector3 Diffuse	  = Math::FTVector3(0.5f);
		float			dummy1;
		Math::FTVector3 Specular = Math::FTVector3(0.5f);
		float			dummy2;
	};

	/// @brief The value of the material data are to be applied to Pixel Constant Buffers.
	struct StandardMatData
	{
		uint32_t		UseTexture = 1;
		float			AlphaTrim  = 0.9f;
		Math::FTVector2 Dummy	   = Math::FTVector2(0.f, 0.f);
		Math::FTVector4 Color	   = Math::FTVector4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	/// @brief The very basic material used for mesh rendering, provided by Foxtrot Engine.
	class StandardMaterial :
		public FTMaterial
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @see FTMaterial::UpdateBuffer()
		virtual void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;

	public:
		/// @brief Relative path is used for importing material data.
		StandardMaterial(Common::FTResourceDef& resDef, D3D11Renderer* renderer);
		~StandardMaterial() override;

	protected:
		/// @brief Creates a pixel constant buffer using the StandardMatData.
		virtual void CreatePixelConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	private:
		/// @brief The values of the data are to be applied to pixel constant buffers.
		StandardMatData* mData;

	private:
		/// @See FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @See FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief GUI update function for modifying the material data.
		virtual void UpdateUI() override;

#endif
	};

	namespace ChunkKey
	{
		namespace StandardMat
		{
			constexpr const char* STANDARD_MAT = "StandardMaterial";
			constexpr const char* USE_TEXTURE  = "Use Texture";
			constexpr const char* ALPHA_TRIM   = "Alpha Trim";
			constexpr const char* COLOR		   = "Color";
		} // namespace StandardMat

		namespace BlinnPhong
		{
			constexpr const char* AMBIENT	= "Ambient";
			constexpr const char* SHININESS = "Shininess";
			constexpr const char* DIFFUSE	= "Diffuse";
			constexpr const char* SPECULAR	= "Specular";
		} // namespace BlinnPhong

	} // namespace ChunkKey

	static_assert((sizeof(StandardMatData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");
} // namespace D3D11