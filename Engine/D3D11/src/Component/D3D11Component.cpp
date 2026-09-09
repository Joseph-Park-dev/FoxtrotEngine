#include "Component/D3D11Component.h"

#include "Plugin/IPlugin.h"
#include "Actor/IActor.h"
#include "FileSystem/FileIOHelper.h"
#include "Component/Data/ComponentData.h"

#ifdef FOXTROT_EDITOR
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void D3D11::D3D11Component::Initialize()
	{
		mIsInitialized = true;
	}

	/// @brief Updates the up used by subsequent operations.
	void D3D11::D3D11Component::Setup()
	{
		mIsSetup = true;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void D3D11::D3D11Component::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Common::ChunkKey::IS_ACTIVE, mIsActive);
		Common::FileIOHelper::SaveInt(ofs, Common::ChunkKey::UPDATE_ORDER, mUpdateOrder);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void D3D11::D3D11Component::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadInt(ifs, mUpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, mIsActive);
	}

	/// @brief Returns the owner used by this d3 d11 component.
	/// @return Borrowed access to the owner.
	Core::IActor* D3D11Component::GetOwner()
	{
		return mOwner;
	}

	/// @brief Returns the update order used by this d3 d11 component.
	/// @return Current update order.
	const int D3D11Component::GetUpdateOrder()
	{
		return mUpdateOrder;
	}

	/// @brief Returns the is initialized used by this d3 d11 component.
	/// @return Current value of the is initialized flag.
	const bool D3D11Component::GetIsInitialized() const
	{
		return mIsInitialized;
	}

	/// @brief Returns the is setup used by this d3 d11 component.
	/// @return Current value of the is setup flag.
	const bool D3D11Component::GetIsSetup() const
	{
		return mIsSetup;
	}

	/// @brief Returns the is active used by this d3 d11 component.
	/// @return Current value of the is active flag.
	const bool D3D11Component::GetIsActive() const
	{
		return mIsActive;
	}

	/// @brief Updates the is active used by subsequent operations.
	/// @param isActive Replacement is active.
	void D3D11Component::SetIsActive(bool isActive)
	{
		mIsActive = isActive;
	}

	/// @brief Initializes component lifecycle state and its owning actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :D3D11Component base or delegates to its constructor.
	D3D11::D3D11Component::D3D11Component(Core::IActor* owner, int updateOrder)
		: mOwner(owner)
		, mUpdateOrder(updateOrder)
		, mIsInitialized(false)
		, mIsSetup(false)
		, mIsActive(true)
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	D3D11::D3D11Component::~D3D11Component()
	{
	}

#ifdef FOXTROT_EDITOR
	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
	void D3D11Component::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		chInst->UpdateBoolValue("Is Active", mIsActive);
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
