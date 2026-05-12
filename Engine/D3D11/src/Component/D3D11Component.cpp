#include "Component/D3D11Component.h"

#include "Plugin/IPlugin.h"
#include "Actor/Actor.h"

namespace D3D11
{
	void D3D11::D3D11Component::Initialize()
	{
		Core::Component::Initialize();
	}

	void D3D11::D3D11Component::Setup()
	{
		Core::Component::Setup();
	}

	void D3D11::D3D11Component::SaveProperties(std::ofstream& ofs)
	{
		Core::Component::SaveProperties(ofs);
	}

	void D3D11::D3D11Component::LoadProperties(std::ifstream& ifs)
	{
		Core::Component::LoadProperties(ifs);
	}

	D3D11::D3D11Component::D3D11Component(Core::Plugin* plugin, Core::Actor* owner, int updateOrder)
	{
		Create(plugin, owner, updateOrder);
	}

	D3D11::D3D11Component::~D3D11Component()
	{
	}
} // namespace D3D11