// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Material/FTMaterial.h"

namespace D3D11
{
	struct Light;
	class FTMeshGroup;

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
		/// @brief Uploads the current CPU data to its GPU buffer.
		/// @see FTMaterial::UpdateBuffer()
		/// @param context Context associated with this operation.
		virtual void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;

	public:
		/// @brief Relative path is used for importing material data.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		StandardMaterial(Common::FTResourceDef& resDef, D3D11Renderer* renderer);
		/// @brief Releases the resources managed by this instance during destruction.
		~StandardMaterial() override;

	protected:
		/// @brief Creates a pixel constant buffer using the StandardMatData.
		/// @param device Direct3D device used to create GPU resources.
		virtual void CreatePixelConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	private:
		/// @brief The values of the data are to be applied to pixel constant buffers.
		StandardMatData* mData;

	private:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @See FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @See FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
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
