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
	struct SpineVCData;
	struct SpinePSData;
	enum SPINE_ATTACHMENT_TYPE;

	/// @brief A FTResource that holds a Spine Animation.
	/// This will be registered to a SpineAnimator Component instance.
	class FTSpineAnimation :
		public FTSprite
	{
	public:
		static inline D3D11::ResType Type = D3D11::ResType::SPINE_ANIMATION;

	public:
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		/// @param physics Physics service used by the operation.
		void Update(float deltaTime, spine::Physics physics);

		/// @brief Uploads model, view, and projection for the spine vertex shader.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param mat Matrix or material used by this operation.
		/// @param gcDataCount Number of geometry-shader constant-data entries.
		/// @param meshIndex Index of the mesh within the group.
		virtual void UpdateConstantBuffers(
			Core::IRenderer* renderer,
			Core::Transform* transform,
			Core::ICamera*	 camInst,
			FTMaterial*		 mat,
			const size_t	 gcDataCount = 1,
			const int		 meshIndex	 = 0) override;

		/// @brief Renders the entire meshes created, as a full model.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param pso Pipeline state object used for rendering.
		/// @param mat Matrix or material used by this operation.
		void Render(
			Core::IRenderer* renderer,
			Core::Transform* transform,
			Core::ICamera*	 camInst,
			D3D11PSO*		 pso,
			FTMaterial*		 mat) override;

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
		/// @brief Gets the skin combination consists of shifted bits.
		/// @return Current skin combination.
		const unsigned int GetSkinCombination() const;

		/// @brief Sets the skin conbination & updates the skin.
		/// @param skinCombi Replacement skin combination.
		void SetSkinCombination(const unsigned int skinCombi);

		/// @brief Sets timescale for the loaded clips.
		/// Useful to control the playback speed.
		/// @param val Replacement time scale.
		void SetTimeScale(const float val);

		/// @brief Plays the loaded clip on TrackEntry 0.
		/// @param idx Clip index in mLoadedClips.
		/// @param isRepeated Should the animation be looped?
		void SetAnimation(const int idx, const bool isRepeated);

		/// @brief Turns the skin on/off.
		/// @param idx Skin index. This will be used to bit-shift the skin combination.
		void ToggleSkin(const size_t idx);

		/// @brief Returns the refernce to the loaded spine animation clips.
		/// @return Borrowed access to the loaded clips.
		/// @note Changes through the returned reference affect this object's stored state.
		spine::Vector<spine::Animation*>& LoadedClips();

		/// @brief FTSpineAnimation uses SpineMesh, not Mesh!
		/// @return No value; this overload is deleted and cannot be called.
		Common::FTDS::DynamicArray<Mesh*>* Meshes() = delete;

	public:
		/// @brief Loads a .spineanim file from the resource path.
		/// The file is not an image, so sprite texture setup is skipped.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTSpineAnimation(Common::FTResourceDef& resDef, void* renderer);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTSpineAnimation() override;

	private:
		/// @brief JSON path, which will be use to create spine anim.
		Common::FTDS::String* mJSONPath;

		/// @brief Atlas file path, which will be use to create spine anim.
		Common::FTDS::String* mAtlasPath;

		/// @brief Timescale for the loaded clips.
		float mTimeScale;

		/// @brief When true, the animation starts facing right. Steering replaces this after movement begins.
		bool mInitRightward;

		/// @brief Loaded spine animation clips.
		spine::Vector<spine::Animation*> mLoadedClips;

		/// @brief Loaded spine skins.
		spine::Vector<spine::Skin*> mSkins;

		/// @brief Defines how the skins are combined. One bit per skin, up to MAX_SKIN_COUNT.
		unsigned int mSkinCombination;

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

		/// @brief Model, view, and projection uploaded to the spine vertex shader.
		SpineVCData* mSpineVCData;

		/// @brief GPU copy of mSpineVCData, bound at vertex-shader register b0.
		Microsoft::WRL::ComPtr<ID3D11Buffer> mSpineVCBuf;

		/// @brief Pixel constants matching Texture2DSpinePS.
		SpinePSData* mSpinePSData;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mSpinePSBuf;

	private:
		/// @brief Applies this object's processing step.
		/// @param renderer Renderer providing the graphics device and current render state.
		void Process(void* renderer);

		/// @brief Loads Spine Atlas & JSON, and creates a spine animation.
		/// @param device
		void InitializeSpinAnim(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/// @brief Allocates the spine vertex constant buffer that holds view and projection.
		/// @param device Direct3D device used to create GPU resources.
		virtual void InitializeConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

		/// @brief Returns the mesh for a slot, creating or growing it to fit the current attachment.
		/// @param device Direct3D device used to create GPU resources.
		/// @param slotIndex Stable slot index from SlotData.
		/// @param attachmentType Spine attachment category.
		/// @param vertexCount Vertices required by the current attachment.
		/// @param indexCount Indices required by the current attachment.
		/// @return Mesh stored at slotIndex, or null when the attachment has no vertices.
		SpineMesh* EnsureSlotMesh(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			int									  slotIndex,
			D3D11::SPINE_ATTACHMENT_TYPE		  attachmentType,
			size_t								  vertexCount,
			size_t								  indexCount);

		/// @brief Updates the buffers for each spine meshes according to the vertices data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		void UpdateBuffers(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

		/// @brief Updates the skin according to the skin combination.
		void SetSkin();

		/// @brief Sets the facing direction from mInitRightward before steering takes over.
		void ApplyInitialDirection();

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Initializes Spine skeleton, atlas, and animation playback resources.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param jsonPath Path to the JSON resource description.
		/// @param atlasPath Path to the Spine atlas file.
		FTSpineAnimation(
			Common::FTResourceDef&		resDef,
			Core::IRenderer*			renderer,
			const Common::FTDS::String* jsonPath,
			const Common::FTDS::String* atlasPath);

	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		/// @see FTResource::UpdateUI()
		void UpdateUI() override;

	public:
		/// @brief Increments the resource metadata's reference count when metadata exists.
		/// @see FTResource::AddRefCount()
		virtual void AddRefCount() override;

		/// @brief Decrements the resource metadata's reference count when metadata exists.
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
			constexpr const char* INIT_RIGHTWARD	 = "Init Rightward";
		} // namespace FTSpineAnimation
	} // namespace ChunkKey
} // namespace D3D11
