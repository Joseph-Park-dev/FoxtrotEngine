// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all Components.
///
/// [Design philosophy]
/// The Component controls its elements in an upper level,
/// and the element has the related operation.
///
/// Means of access to the elements (Getters & Setters) are not to be
/// exposed. Make them protected, or minimize the cases of them being public.
/// </summary>

#pragma once
#include <iosfwd>

#include "Debugging/DebugMemAlloc.h"
#include "Actor/Actor.h"
#include "FileSystem/FileIOHelper.h"

namespace Core
{
	class Plugin;
	class Actor;
	class FoxtrotRenderer;
	class FTCore;
	class FTInputDevice;

	namespace DefaultVal
	{
		constexpr int UPDATE_ORDER = 100;
	}

	namespace FTDS
	{
		class String;
	}

	class Component
	{
	public:
		virtual FTDS::String GetName() const = 0;

	public:
		virtual void Initialize();
		virtual void Setup();
		virtual void ProcessInput(FTInputDevice* inputDevice) = 0;
		virtual void Update(float deltaTime)				  = 0;
		virtual void LateUpdate(float deltaTime)			  = 0;
		virtual void Render(FoxtrotRenderer* renderer)		  = 0;
		virtual void CloneTo(Actor* actor)					  = 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs);
		virtual void LoadProperties(std::ifstream& ifs);

	public:
		virtual Core::Actor* GetOwner() const { return mOwner; }
		const int			 GetUpdateOrder() const;
		virtual const bool	 GetIsInitialized() const { return mIsInitialized; }
		const bool			 GetIsSetup() const;
		const bool			 GetIsActive() const;

		void SetIsActive(bool isActive);

	protected:
		/// <summary>
		/// Creates a Component to be assigned to the Actor.
		/// </summary>
		void Create(
			Core::Plugin* plugin,
			Core::Actor*  owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER)
		{
			mPlugin		   = plugin;
			mOwner		   = owner;
			mUpdateOrder   = updateOrder;
			mIsInitialized = false;
			mIsSetup	   = false;
			mIsActive	   = false;

			mOwner->AddComponent(this);
		}

		virtual Plugin* GetPlugin();

	private:
		Core::Plugin* mPlugin	   = nullptr;
		Core::Actor*  mOwner	   = nullptr;
		int			  mUpdateOrder = Core::DefaultVal::UPDATE_ORDER;

	private:
		// This is turned as true as the Initialize(FTCore*) is executed.
		bool mIsInitialized = false;
		// This is turned as true as the Setup() is executed.
		bool mIsSetup = false;
		// The component is alive in the game loop
		// (During ProcessInput, Update, LateUpdate and Render)
		bool mIsActive = true;

	public:
		template <class T>
		static void Load(Actor* actor, std::ifstream& ifs)
		{
			/// Dynamically allocate actor of type T
			T* t = DBG_NEW T(actor, DefaultVal::UPDATE_ORDER);
			// Load Properties first -> then initialize with the loaded values.
			t->LoadProperties(ifs);
		}

#ifdef FOXTROT_EDITOR
	public:
		// Member functions for EditorElement objects.
		// Functions for editor specific tasks, which means
		// codes to be executed in the Editor, but not in the produced game.
		//
		// Renderer dependent components,
		// (such as MeshRendererComponent, SpriteRendererComponent, etc.)
		// usually use FTCore Update() & Render().
		virtual void EditorUpdate(float deltaTime) {};
		virtual void EditorRender(FoxtrotRenderer* renderer) {};

	public:
		virtual void EditorUIUpdate();

		void SetUpdateOrder(int updateOrder);

	public:
		template <class T>
		static void Create(Actor* actor, FTCore* coreInst)
		{
			// Dynamically allocate actor of type T
			T* t = DBG_NEW T(actor, DefaultVal::UPDATE_ORDER);
			// Call LoadProperties on DBG_NEW actor
			t->Initialize(coreInst);
		}

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		constexpr const char* COMPONENTS   = "Components";
		constexpr const char* UPDATE_ORDER = "UpdateOrder";
		constexpr const char* IS_ACTIVE	   = "Is Active";
	} // namespace ChunkKey

	extern "C" __declspec(dllexport) Component* Create(Plugin* plugin, Actor* actor, int updateOrder);

	void Component::Initialize()
	{
		mIsInitialized = true;
	}
	void Component::Setup()
	{
		mIsSetup = true;
	}

	void Component::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::SaveBool(ofs, ChunkKey::IS_ACTIVE, mIsActive);
		FileIOHelper::SaveInt(ofs, ChunkKey::UPDATE_ORDER, mUpdateOrder);
	}

	void Component::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::LoadInt(ifs, mUpdateOrder);
		FileIOHelper::LoadBool(ifs, mIsActive);
	}
} // namespace Core