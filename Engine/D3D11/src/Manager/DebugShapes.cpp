// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/DebugShapes.h"

#include <wrl.h>
#include <d3d11.h>
#include <string>

#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "ResourceSystem/Shape/FTShape.h"
#include "FileSystem/NullKeys.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/ResourceManager.h"
#include "ResourceSystem/D3D11PSO.h"

#ifdef FOXTROT_EDITOR
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using Microsoft::WRL::ComPtr;

	constexpr const char* DEBUG_SHAPE_PSO = "DebugShapePSO";

	/// @brief Initializes the debug-geometry registry.
	/// @note Initializes the :DebugShapes base or delegates to its constructor.
	DebugShapes::DebugShapes()
		: mShapes(DBG_NEW Common::FTDS::DynamicArray<FTShape*>)
		, mRenderer(nullptr)
		, mPSO(nullptr)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	DebugShapes::~DebugShapes()
	{
		DeleteAll();
		delete mShapes;
	}

	D3D11_API FTRectangle* DebugShapes::AddRectangle()
	{
		FTRectangle* rect = DBG_NEW FTRectangle;
		if (mRenderer)
			rect->Initialize(mRenderer);
		AddShape(rect);
		return rect;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void DebugShapes::Initialize(D3D11Renderer* renderer)
	{
		if (renderer)
			mRenderer = renderer;

		ResourceManager* resources = ResourceManager::GetInstance();
		if (!resources)
			return;

		if (!mPSO)
			mPSO = resources->GetPSO(DEBUG_SHAPE_PSO);
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	void DebugShapes::Render(D3D11Renderer* renderer)
	{
		if (!renderer || !mShapes || mShapes->GetSize() < 1)
			return;

		if (!mPSO || !mRenderer)
			Initialize(renderer);

		mPSO->SetToPipeline(renderer->GetContext());

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
			(*iter)->Render(renderer);

		// DebugShapeVS writes COLOR after an extra POSITION. That matches DebugShapePS
		// only while DebugShapeGS is still bound, so clear it after the whole batch.
		renderer->GetContext()->GSSetShader(nullptr, nullptr, 0);
	}

	/// @brief Registers a shape for subsequent rendering.
	/// @param shape Shape to register, remove, or render.
	void DebugShapes::AddShape(FTShape* shape)
	{
		mShapes->PushBack(shape);
	}

	/// @brief Removes a shape from the managed rendering collection.
	/// @param shape Shape to register, remove, or render.
	void DebugShapes::RemoveShape(FTShape* shape)
	{
		int pos = mShapes->Find(shape);
		if (pos != Common::ChunkKey::NullVal::INVALID_IDX)
		{
			size_t	 idx   = static_cast<size_t>(pos);
			FTShape* shape = mShapes->At(idx);
			delete shape;
			shape = nullptr;
			mShapes->Erase(idx);
		}
	}

	/// @brief Deletes the scene's managed actors and clears its collections.
	void DebugShapes::DeleteAll()
	{
		if (mShapes->GetSize() < 1)
			return;

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
		{
			delete (*iter);
			(*iter) = nullptr;
		}
		mShapes->Clear();
	}

#ifdef FOXTROT_EDITOR

#endif // FOXTROT_EDITOR
} // namespace D3D11
namespace D3D11
{
	DebugShapes* DebugShapes::mInstance = nullptr;

	/// @brief Returns the shared instance used by this manager.
	/// @return Borrowed singleton pointer; do not delete it directly.
	/// @note Synchronize concurrent initialization, access, and destruction externally.
	DebugShapes* DebugShapes::GetInstance()
	{
		if (!mInstance)
			mInstance = DBG_NEW DebugShapes();
		return mInstance;
	}

	/// @brief Deletes the shared instance and clears the singleton pointer.
	/// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
	void DebugShapes::Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}
} // namespace D3D11
