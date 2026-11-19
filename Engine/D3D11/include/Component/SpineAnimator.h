#pragma once
#include "Component/MeshRenderer.h"

#include <spine/spine.h>

#include "FTDS/Static/FTString.h"
#include "FTMath.h"

namespace D3D11
{
	using namespace Common;
	class FTSpineAnimation;

	class SpineAnimator :
		public MeshRenderer
	{
	public:
		static inline const char* NAME = "SpineAnimator";
		/// @brief Returns the name used by this spine animator.
		/// @return Borrowed access to the name.
		virtual const char* GetName() override
		{
			return "SpineAnimator";
		}

	public:
		/// @brief Starts or selects animation playback.
		/// @param idx Zero-based element index.
		/// @param isRepeated Whether playback repeats after its final frame.
		virtual void Play(const int idx, bool isRepeated);

		/// @brief World position of a skeleton bone, in the same space used to draw the animation.
		/// @param boneName Bone name in the loaded skeleton.
		/// @param outPosition Receives the bone origin in world units.
		/// @return True when the bone exists on the loaded animation.
		virtual bool GetBoneWorldPosition(const char* boneName, Math::FTVector3& outPosition);

		/// @brief Unit right axis of a skeleton bone, in the same space used to draw the animation.
		/// @param boneName Bone name in the loaded skeleton.
		/// @param outDirection Receives the bone's +X direction in world units.
		/// @return True when the bone exists and its right axis has length.
		virtual bool GetBoneWorldRight(const char* boneName, Math::FTVector3& outDirection);

		/// @brief Rotates a bone so its right axis, the skeleton +X direction, points at a world position.
		/// @param boneName Bone name in the loaded skeleton.
		/// @param worldTarget Point in the same world space used to draw the animation.
		/// @return True when the bone exists and the target is not on the bone.
		virtual bool AimBoneRight(const char* boneName, const Math::FTVector3& worldTarget);

		/// @brief Rotates driverBone so alignedBone's right axis points at a world position.
		/// @param driverBone Bone whose local rotation is changed.
		/// @param alignedBone Bone whose +X axis is aligned. Must be driverBone or a descendant.
		/// @param worldTarget Point in the same world space used to draw the animation.
		/// @param minRotation Lowest local rotation written to driverBone, in degrees.
		/// @param maxRotation Highest local rotation written to driverBone, in degrees.
		/// @return True when both bones exist, alignedBone hangs off driverBone, and the target is not on the driver pivot.
		virtual bool AimBoneRight(
			const char*			   driverBone,
			const char*			   alignedBone,
			const Math::FTVector3& worldTarget,
			float				   minRotation,
			float				   maxRotation);

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		void Initialize() override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) override;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

	public:
		/// @brief Initializes Spine animation playback state for its actor.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		SpineAnimator(Common::IActor* owner);
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Common::IActor* actor) override;

	private:
		float		   mTimeScale;
		spine::Physics mUsePhysics;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Advances state needed only while operating in the editor.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime) override;
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace SpineAnimator
		{
			constexpr const char* NAME		  = "SpineAnimator";
			constexpr const char* LOADED_ANIM = "Loaded Anim";
			constexpr const char* PSO		  = "SpineAnimPSO";
		} // namespace SpineAnimator
	} // namespace ChunkKey

#include "Plugin/D3D11Exports.h"
	/// @brief Creates a Spine animation component associated with an actor.
	/// @param actor Actor participating in this operation.
	/// @return Created spine animator instance or resource.
	D3D11_API D3D11::SpineAnimator* CreateSpineAnimator(Common::IActor* actor);
} // namespace D3D11
