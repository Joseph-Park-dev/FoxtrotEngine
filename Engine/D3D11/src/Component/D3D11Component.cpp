#include "Component/D3D11Component.h"

#include "Plugin/IPlugin.h"
#include "Actor/IActor.h"
#include "FileSystem/FileIOHelper.h"
#include "Component/Data/ComponentData.h"

#include "Component/Animator.h"
#include "Component/MeshRenderer.h"
#include "Component/SpineAnimator.h"
#include "Component/SpriteRenderer.h"
#include "Component/TileMapRenderer.h"

#ifdef FOXTROT_EDITOR
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	/// @brief Returns the owner used by this d3 d11 component.
	/// @return Borrowed access to the owner.
	Core::IActor* D3D11Component::GetOwner()
	{
		return mOwner;
	}

	D3D11PSO* D3D11Component::GetPSO() const
	{
		return mPSO;
	}

	void D3D11Component::SetPSO(D3D11PSO* pso)
	{
		mPSO = pso;
	}

	/// @brief Initializes component lifecycle state and its owning actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :D3D11Component base or delegates to its constructor.
	D3D11::D3D11Component::D3D11Component(Core::IActor* owner)
		: mOwner(owner)
		, mPSO(nullptr)
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	D3D11::D3D11Component::~D3D11Component()
	{
	}

	D3D11PSO*& D3D11Component::PSO()
	{
		return mPSO;
	}
} // namespace D3D11