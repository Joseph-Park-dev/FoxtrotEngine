#pragma once
#include "Component/Component.h"

namespace Core
{
	class FTInputDevice;
} // namespace Core

namespace D3D11
{
	class D3D11Renderer;

	class D3D11Component :
		public Core::Component
	{
	public:
		virtual Core::FTDS::String GetName() const = 0;

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
	public:
		virtual void EditorUIUpdate();

	public:
		virtual void EditorRender(D3D11::D3D11Renderer* renderer);
#endif
	};
} // namespace D3D11