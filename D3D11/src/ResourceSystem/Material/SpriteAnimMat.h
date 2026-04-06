#pragma once
#include "ResourceSystem/Material/FTMaterial.h"

namespace D3D11
{
	/// @brief The value of the material data are to be applied to Pixel Constant Buffers.
	struct SpriteAnimMatData
	{
		Math::FTVector4 Color	   = Math::FTVector4(1.f, 1.f, 1.f, 1.f);
		bool			UseTexture = true;

		Math::FTVector3 dummy = Math::FTVector3::Zero;
	};

	class SpriteAnimMat :
		public FTMaterial
	{
	public:
		static D3D11::ResType Type;

	public:
		virtual void CreatePixelConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;
		virtual void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;

		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		SpriteAnimMat(Core::FTResourceDef& resDef, D3D11Renderer* renderer);
		~SpriteAnimMat();

	private:
		SpriteAnimMatData* mData;

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI() override;

#endif
	};

	namespace ChunkKey
	{
		namespace SpriteAnimMat
		{
			constexpr const char* SPRITE_ANIM_MAT = "SpriteAnimMat";
			constexpr const char* COLOR			  = "Color";
			constexpr const char* USE_TEXTURE	  = "Use Texture";
			constexpr const char* ROT_ANGLE		  = "Rot Angle";
			constexpr const char* ROT_PIVOT		  = "Rot Pivot";
		} // namespace SpriteAnimMat
	} // namespace ChunkKey
} // namespace D3D11