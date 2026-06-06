// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Mesh/IModel.h"

#include "FTDS/Dynamic/DynamicArray.h"

namespace spine
{
	template <typename TYPE>
	class Vector;

	class Animation;
	class Skin;
	class Atlas;
	class SkeletonData;
	class Skeleton;
	class AnimationStateData;
	class AnimationState;
} // namespace spine

namespace Graphics
{
	class ICamera;
	class ITexture;
	class IPixelShader;
	struct SpineMesh;
	enum SPINE_ATTACHMENT_TYPE;

	struct SpineAnimData :
		Common::ResourceData
	{
		/// @brief JSON path, which will be use to create spine anim.
		Common::FTDS::String* mJSONPath;

		/// @brief Atlas file path, which will be use to create spine anim.
		Common::FTDS::String* mAtlasPath;

		/// @brief Timescale for the loaded clips.
		float mTimeScale;

		/// @brief Loaded spine animation clips.
		spine::Vector<spine::Animation*>* mLoadedClips;

		/// @brief Loaded spine skins.
		spine::Vector<spine::Skin*>* mSkins;

		/// @brief Defines how the skins are combined.
		unsigned char mSkinCombination;

		/// @brief Current animation index.
		int mCurrAnimIdx;

		spine::Atlas*			   mAtlas;
		spine::SkeletonData*	   mSkeletonData;
		spine::Skeleton*		   mSkeleton;
		spine::AnimationStateData* mStateData;
		spine::AnimationState*	   mState;

		/// @brief FTSpineAnimation uses SpineMesh, not Mesh!
		/// @see Meshes()
		Common::FTDS::DynamicArray<SpineMesh*>* mMeshes;
	};

	/// @brief A FTResource that holds a Spine Animation.
	/// This will be registered to a SpineAnimator Component instance.
	class ISpineAnimation :
		public Graphics::IModel
	{
		////////////////
		/// Gameloop ///
		////////////////
	public:
		virtual void Update(float deltaTime) = 0;

		/// @brief Renders the entire meshes created, as a full model.
		virtual void Render(
			Graphics::IRenderer* renderer,
			Core::Transform*	 transform,
			Graphics::ICamera*	 camInst,
			D3D11PSO*			 pso,
			FTMaterial*			 mat) override = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Gets the skin combination consists of shifted bits.
		virtual const unsigned char GetSkinCombination() const = 0;

		/// @brief Sets the skin conbination & updates the skin.
		virtual void SetSkinCombination(const unsigned char skinCombi) = 0;

		/// @brief Sets timescale for the loaded clips.
		/// Useful to control the playback speed.
		virtual void SetTimeScale(const float val) = 0;

		/// @brief Plays the loaded clip on TrackEntry 0.
		/// @param idx Clip index in mLoadedClips.
		/// @param isRepeated Should the animation be looped?
		virtual void SetAnimation(const int idx, const bool isRepeated) = 0;

		/// @brief Turns the skin on/off.
		/// @param idx Skin index. This will be used to bit-shift the skin combination.
		virtual void ToggleSkin(const size_t idx) = 0;

		/// @brief Returns the refernce to the loaded spine animation clips.
		virtual spine::Vector<spine::Animation*>& LoadedClips() = 0;

		/// @brief FTSpineAnimation uses SpineMesh, not Mesh!
		virtual Common::FTDS::DynamicArray<Mesh*>* Meshes() = delete;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		virtual ~ISpineAnimation() override = 0;
	};

	/// @brief Maximum number of skin that can be loaded to this instance.
	constexpr size_t MAX_SKIN_COUNT = 20;

	namespace ChunkKey
	{
		namespace FTSpineAnimation
		{
			constexpr const char* FT_SPINE_ANIMATION = "FTSpineAnimation";
			constexpr const char* JSON_KEY			 = "JSON Key";
			constexpr const char* ATLAS_KEY			 = "Atlas Key";
			constexpr const char* SKIN_COMBINATION	 = "Skin Combination";
		} // namespace FTSpineAnimation
	} // namespace ChunkKey
} // namespace Graphics