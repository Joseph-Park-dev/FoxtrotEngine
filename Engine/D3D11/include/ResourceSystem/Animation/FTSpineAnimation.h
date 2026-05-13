// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Sprite/FTSprite.h"

#include <FTDS/Dynamic/DynamicArray.h>
#include <spine/spine.h>

namespace Core
{
	class Transform;
	class FTJSON;
	class FTText;
} // namespace Core

namespace D3D11
{
	using namespace Common;
	class Camera;
	class FTTexture;
	class FTPixelShader;
	struct SpineMesh;
	enum SPINE_ATTACHMENT_TYPE;

	/// @brief A FTResource that holds a Spine Animation.
	/// This will be registered to a SpineAnimator Component instance.
	class FTSpineAnimation :
		public FTSprite
	{
	public:
		static inline D3D11::ResType Type = D3D11::ResType::SPINE_ANIMATION;

	public:
		void Update(float deltaTime, spine::Physics physics);

		/// @brief Renders the entire meshes created, as a full model.
		void Render(
			D3D11Renderer*	 renderer,
			Core::Transform* transform,
			Camera*			 camInst,
			FTTexture*		 tex,
			FTVertexShader*	 vs,
			FTPixelShader*	 ps,
			FTMaterial*		 mat);

		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Gets the skin combination consists of shifted bits.
		const unsigned char GetSkinCombination() const;

		/// @brief Sets the skin conbination & updates the skin.
		void SetSkinCombination(const unsigned char skinCombi);

		/// @brief Sets timescale for the loaded clips.
		/// Useful to control the playback speed.
		void SetTimeScale(const float val);

		/// @brief Plays the loaded clip on TrackEntry 0.
		/// @param idx Clip index in mLoadedClips.
		/// @param isRepeated Should the animation be looped?
		void SetAnimation(const int idx, const bool isRepeated);

		/// @brief Turns the skin on/off.
		/// @param idx Skin index. This will be used to bit-shift the skin combination.
		void ToggleSkin(const size_t idx);

		/// @brief Returns the refernce to the loaded spine animation clips.
		spine::Vector<spine::Animation*>& LoadedClips();

		/// @brief FTSpineAnimation uses SpineMesh, not Mesh!
		Common::FTDS::DynamicArray<Mesh*>* Meshes() = delete;

	public:
		/// @brief Creates the spine animation.
		/// The file paths of the generic data will only be used.
		FTSpineAnimation(Common::FTResourceDef& resDef, D3D11Renderer* renderer);
		~FTSpineAnimation() override;

	private:
		/// @brief JSON path, which will be use to create spine anim.
		Common::FTDS::String* mJSONPath;

		/// @brief Atlas file path, which will be use to create spine anim.
		Common::FTDS::String* mAtlasPath;

		/// @brief Timescale for the loaded clips.
		float mTimeScale;

		/// @brief Loaded spine animation clips.
		spine::Vector<spine::Animation*> mLoadedClips;

		/// @brief Loaded spine skins.
		spine::Vector<spine::Skin*> mSkins;

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

	private:
		void Process(D3D11::D3D11Renderer* renderer);

		/// @brief Loads Spine Atlas & JSON, and creates a spine animation.
		/// @param device
		void InitializeSpinAnim(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/// @brief Used in InitializeSpineAnim(), creates meshes for the spine animation.
		/// @param order draw order for the current mesh.
		/// @param attachment attachment to acquire mesh properties.
		/// @param attachmentType This is either a SpineMesh or SpineRegion.
		void InitializeMeshes(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			int									  order,
			void*								  attachment,
			D3D11::SPINE_ATTACHMENT_TYPE		  attachmentType);

		/// @brief Updates the buffers for each spine meshes according to the vertices data.
		void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

		/// @brief Updates the skin according to the skin combination.
		void SetSkin();

#ifdef FOXTROT_EDITOR
	public:
		FTSpineAnimation(
			Common::FTResourceDef&	  resDef,
			D3D11Renderer*			  renderer,
			const Common::FTDS::String* jsonPath,
			const Common::FTDS::String* atlasPath);

	public:
		/// @see FTResource::UpdateUI()
		void UpdateUI() override;

	public:
		/// @see FTResource::AddRefCount()
		virtual void AddRefCount() override;

		/// @see FTResource::SubtractRefCount()
		virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
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
} // namespace D3D11