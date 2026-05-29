#pragma once
#include "Component/MeshRenderer.h"

#include <spine/spine.h>

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	using namespace Common;
	class FTSpineAnimation;

	class SpineAnimator :
		public MeshRenderer
	{
	public:
		virtual const char* GetName() override
		{
			return "SpineAnimator";
		}

	public:
		void Play(const int idx, bool isRepeated);

	public:
		void		 Initialize() override;
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

	public:
		SpineAnimator(Core::IActor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual void CloneTo(Core::IActor* actor) override;

	private:
		float		   mTimeScale;
		spine::Physics mUsePhysics;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst) override;
#endif // FOXTROT_EDITOR
	
	};

	namespace ChunkKey
	{
		namespace SpineAnimator
		{
			constexpr const char* NAME		  = "SpineAnimator";
			constexpr const char* LOADED_ANIM = "Loaded Anim";
		} // namespace SpineAnimator
	} // namespace ChunkKey
} // namespace D3D11