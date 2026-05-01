#pragma once
#include "Component/MeshRenderer.h"

#include <spine/spine.h>

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	class FTSpineAnimation;

	class SpineAnimator :
		public MeshRenderer
	{
	public:
		virtual Core::FTDS::String GetName() const override { return "SpineAnimator"; }

	public:
		void Play(const int idx, bool isRepeated);

	public:
		void		 Initialize() override;
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(D3D11::D3D11Renderer* renderer) override;

	public:
		SpineAnimator(Core::Plugin* plugin, Core::Actor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual void CloneTo(Core::Actor* actor) override;

	private:
		float		   mTimeScale;
		spine::Physics mUsePhysics;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(D3D11::D3D11Renderer* renderer) override;
		virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace SpineAnimator
		{
			constexpr const char* LOADED_ANIM = "Loaded Anim";
		}
	} // namespace ChunkKey
} // namespace D3D11