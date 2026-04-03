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
#include "Math/FTMath.h"

class Tile;
class Animator;
class FTTexture;
class FTJSON;
class FTPixelShader;
struct AnimationFrame;

namespace D3D11
{
#ifdef FOXTROT_EDITOR
	struct FTSpriteAnimationDef : FTResourceDef
	{
		FTJSON*	   JSON		   = nullptr;
		FTTexture* SpriteSheet = nullptr;
		bool	   IsRepeated  = true;
		int		   FPS		   = 24;
		int		   MinFrameIdx = -1;
		int		   MaxFrameIdx = -1;
	};
#endif

	/// @brief A FTResource that holds a Sprite Animation.
	/// This will be registered to a Animator Component instance.
	class FTSpriteAnimation : public D3D11::FTSprite
	{
	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns Frames per second
		const int GetFPS() const;

		/// @brief Returns the last frame index.
		const int GetMaxFrameIdx() const;

		/// @brief Returns the first frame index.
		const int GetMinFrameIdx() const;

		const size_t GetFrameCount() const;

	public:
		/// @brief Relative path is used for importing .spriteanim file.
		FTSpriteAnimation(FTResourceDef& resDef, D3D11Renderer* renderer);
		~FTSpriteAnimation();

	private:
		/// @brief A JSON file that holds the rect data on a spritesheet.
		FTJSON* mJSON;

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
		void Initialize(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

#ifdef FOXTROT_EDITOR
	public:
		FTSpriteAnimation(FTSpriteAnimationDef& resDef, FoxtrotRenderer* renderer);

	public:
		virtual void AddRefCount() override;
		virtual void SubtractRefCount() override;

	public:
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