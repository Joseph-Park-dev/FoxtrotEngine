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
		/// @brief Uploads the current CPU data to its GPU buffer.
		/// @param context Context associated with this operation.
		virtual void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Initializes shader resources used for animated sprites.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		SpriteAnimMat(Common::FTResourceDef& resDef, D3D11Renderer* renderer);
		/// @brief Releases the resources managed by this instance during destruction.
		~SpriteAnimMat();

	protected:
		/// @brief Creates the pixel-shader constant buffer used by this shader.
		/// @param device Direct3D device used to create GPU resources.
		virtual void CreatePixelConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	private:
		SpriteAnimMatData* mData;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
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
