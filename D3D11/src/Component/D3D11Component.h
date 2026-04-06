#pragma once
#include "Component/Component.h"

namespace Core
{
	class FTInputDevice;
	class FoxtrotRenderer;
} // namespace Core

namespace D3D11
{
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
		virtual void Render(Core::FoxtrotRenderer* renderer)		= 0;
		virtual void CloneTo(Core::Actor* actor)					= 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		D3D11Component(Core::Plugin* plugin, Core::Actor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~D3D11Component();
	};
} // namespace D3D11