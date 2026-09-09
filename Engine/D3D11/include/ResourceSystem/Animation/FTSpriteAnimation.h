// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Sprite/FTSprite.h"

#include "TemplateFunctions.h"
#include "FTMath.h"

namespace GenericData
{
	class FTJSON;
} // namespace GenericData

namespace D3D11
{
	class Tile;
	class Animator;
	class FTTexture;
	class FTPixelShader;
	struct AnimationFrame;

#ifdef FOXTROT_EDITOR
	struct FTSpriteAnimationDef : Common::FTResourceDef
	{
		GenericData::FTJSON* JSON		  = nullptr;
		FTTexture*	  SpriteSheet = nullptr;
		bool		  IsRepeated  = true;
		int			  FPS		  = 24;
		int			  MinFrameIdx = -1;
		int			  MaxFrameIdx = -1;
	};
#endif

	/// @brief A FTResource that holds a Sprite Animation.
	/// This will be registered to a Animator Component instance.
	class FTSpriteAnimation : public D3D11::FTSprite
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns Frames per second
		/// @return Current fps.
		const int GetFPS() const;

		/// @brief Returns the last frame index.
		/// @return Current max frame idx.
		const int GetMaxFrameIdx() const;

		/// @brief Returns the first frame index.
		/// @return Current min frame idx.
		const int GetMinFrameIdx() const;

		/// @brief Returns the frame count used by this ftsprite animation.
		/// @return Current frame count.
		const size_t GetFrameCount() const;

	public:
		/// @brief Relative path is used for importing .spriteanim file.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTSpriteAnimation(Common::FTResourceDef& resDef, Core::IRenderer* renderer);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~FTSpriteAnimation();

	private:
		/// @brief A JSON file that holds the rect data on a spritesheet.
		GenericData::FTJSON* mJSON;

		/// @brief Frames-per-second for this animation.
		int mFPS;

		/// @brief Should this animation be looped?
		bool mIsRepeated;

		/// @brief the first frame index.
		int mMinFrameIdx;

		/// @brief the last frame index.
		int mMaxFrameIdx;

	private:
		/// @brief Takes tiles array and generate sprite animation base on its data.
		/// Creates one Mesh object per every sprite frame, adding it to the MeshGroup.
		/// @param tiles Tile data consists of Rect on sprite sheet & game screen.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		/// @throws std::invalid_argument If the operation encounters the failure condition checked by this implementation.
		void Initialize(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Initializes the clip's frame collection and playback metadata.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTSpriteAnimation(FTSpriteAnimationDef& resDef, Core::IRenderer* renderer);

	public:
		/// @brief Increments the resource metadata's reference count when metadata exists.
		virtual void AddRefCount() override;
		/// @brief Decrements the resource metadata's reference count when metadata exists.
		virtual void SubtractRefCount() override;

	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI();

#endif
	};

	namespace ChunkKey
	{
		namespace FTSpriteAnimation
		{
			constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
			constexpr const char* JSON				  = "JSON";
			constexpr const char* SPRITE_SHEET		  = "Sprite Sheet";
			constexpr const char* SIZE_SCALE		  = "Size Scale";
			constexpr const char* FPS				  = "FPS";
			constexpr const char* IS_REPEATED		  = "Is Repeated";
			constexpr const char* MAX_FRAME_IDX		  = "Max Frame Index";
			constexpr const char* MIN_FRAME_IDX		  = "Min Frame Index";

		} // namespace FTSpriteAnimation
	} // namespace ChunkKey

	namespace SpriteSheetKeys
	{
		constexpr const char* BASE		  = "frames";
		constexpr const char* PROPERTIES  = "meta";
		constexpr const char* SIZE		  = "size";
		constexpr const char* FRAME		  = "frame";
		constexpr const char* SOURCE_SIZE = "spriteSourceSize";
		constexpr const char* ROTATED	  = "rotated";
		constexpr const char* TRIMMED	  = "trimmed";
		constexpr const char* PIVOT		  = "pivot";

		constexpr const char* X = "x";
		constexpr const char* Y = "y";
		constexpr const char* W = "w";
		constexpr const char* H = "h";

	} // namespace SpriteSheetKeys
} // namespace D3D11
