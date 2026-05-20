#include "Component/D3D11Component.h"

#include "Plugin/IPlugin.h"
#include "Actor/IActor.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include <Utility/EditorHelper.h>
#endif // FOXTROT_EDITOR

namespace D3D11
{
	void D3D11::D3D11Component::Initialize()
	{
		mIsInitialized = true;
	}

	void D3D11::D3D11Component::Setup()
	{
		mIsSetup = true;
	}

	void D3D11::D3D11Component::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Core::ChunkKey::IS_ACTIVE, mIsActive);
		Common::FileIOHelper::SaveInt(ofs, Core::ChunkKey::UPDATE_ORDER, mUpdateOrder);
	}

	void D3D11::D3D11Component::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadInt(ifs, mUpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, mIsActive);
	}

	Core::IActor* D3D11Component::GetOwner()
	{
		return mOwner;
	}

	const int D3D11Component::GetUpdateOrder()
	{
		return mUpdateOrder;
	}

	const bool D3D11Component::GetIsInitialized() const
	{
		return mIsInitialized;
	}

	const bool D3D11Component::GetIsSetup() const
	{
		return mIsSetup;
	}

	const bool D3D11Component::GetIsActive() const
	{
		return mIsActive;
	}

	void D3D11Component::SetIsActive(bool isActive)
	{
		mIsActive = isActive;
	}

	D3D11::D3D11Component::D3D11Component(Core::IActor* owner, int updateOrder)
		: mOwner(owner)
		, mUpdateOrder(updateOrder)
		, mIsInitialized(false)
		, mIsSetup(false)
		, mIsActive(true)
	{
	}

	D3D11::D3D11Component::~D3D11Component()
	{
	}

#ifdef FOXTROT_EDITOR
	void D3D11Component::EditorUpdate(float deltaTime)
	{
	}
	void D3D11Component::EditorUIUpdate()
	{
		Editor::UPDATE_BOOL("Is Active", mIsActive);
	}
#endif
} // namespace D3D11