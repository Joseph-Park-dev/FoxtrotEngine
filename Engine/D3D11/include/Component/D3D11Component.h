#pragma once
#include "Component/Component.h"

#include "FTDS/Static/FTString.h"

namespace Core
{
	class FTInputDevice;
} // namespace Core

namespace D3D11
{
	using namespace Common;
	class D3D11Renderer;
	class Camera;

	class D3D11Component :
		public Core::Component
	{
	public:
		virtual Common::FTDS::String GetName() const = 0;

	public:
		virtual void Initialize();
		virtual void Setup();
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) = 0;
		virtual void Update(float deltaTime)						= 0;
		virtual void LateUpdate(float deltaTime)					= 0;
		virtual void Render(D3D11::D3D11Renderer* renderer)			= 0;
		virtual void CloneTo(Core::Actor* actor)					= 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		D3D11Component(Core::Plugin* plugin, Core::Actor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~D3D11Component();

#ifdef FOXTROT_EDITOR
		virtual void EditorRender(D3D11::D3D11Renderer* renderer, D3D11::Camera* camInst) = 0;
		virtual void EditorUIUpdate() override;

#endif // FOXTROT_EDITOR
	};
} // namespace D3D11