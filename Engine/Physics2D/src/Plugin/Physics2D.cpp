// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Plugin/Physics2D.h"

#include "box2d/box2d.h"

#include "Plugin/Physics2DExports.h"
#include "Foxtrot/Core/ModuleApi.h"
#include "Plugin/IPlugin.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"
#include "Factory/IComponentFactory.h"
#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "Core/FTCore.h"

#include "Component/BoxCollider2D.h"
#include "Component/CircleCollider2D.h"
#include "Component/Rigidbody2D.h"
#include "Manager/SceneManager.h"
#include "Scene/Scene.h"

namespace Physics2D
{
	/// @brief Plugin that joins the engine lifecycle and steps a Box2D world.
	/// @note The module factory owns this instance. GetInstance returns that instance, or nullptr before it is created.
	class Physics2D : public Common::IPlugin
	{
	public:
		/// @brief Returns the plugin name used by the module host.
		/// @return Stable plugin name.
		const char* GetName() override;

		/// @brief Accepts a component for later plugin-managed processing.
		/// @param comp Component instance associated with the actor or plugin.
		void RegisterComponent(Common::IComponent* comp) override;

		/// @brief Writes persistent properties. This plugin stores none.
		/// @param ofs Output stream receiving the serialized data.
		void SaveProperties(std::ofstream& ofs) override;
		/// @brief Reads persistent properties. This plugin stores none.
		/// @param ifs Input stream positioned at the expected data.
		void LoadProperties(std::ifstream& ifs) override;

		/// @brief Initializes the services required before the runtime lifecycle begins.
		void Initialize() override;
		/// @brief Finishes setup after initialization.
		void Setup() override;
		/// @brief Dispatches input for the current frame.
		void ProcessInput() override;
		/// @brief Advances frame-dependent state.
		/// @param deltaTime Elapsed frame time in seconds.
		void Update(float deltaTime) override;
		/// @brief Runs the post-update lifecycle phase.
		/// @param deltaTime Elapsed frame time in seconds.
		void LateUpdate(float deltaTime) override;
		/// @brief Submits graphics work for the current frame.
		void Render() override;
		/// @brief Processes queued lifecycle and scene changes.
		void ProcessEvent() override;
		/// @brief Releases runtime services during engine shutdown.
		void ShutDown() override;

		/// @brief Creates the plugin and records it as the instance returned by GetInstance.
		/// @param name Name supplied by the module factory. The reported plugin name stays "Physics2D".
		explicit Physics2D(const char* name);
		/// @brief Clears the shared instance when it refers to this plugin.
		virtual ~Physics2D();

		/// @brief Returns the Box2D world owned by this plugin.
		/// @return World identifier. Invalid until Initialize runs.
		b2WorldId& GetCurrentWorldID();

		/// @brief Returns the plugin instance created by the module factory.
		/// @return Borrowed plugin pointer, or nullptr before the module is created.
		static Physics2D* GetInstance();

	protected:
		/// @brief Restores plugin manager state. This plugin stores none.
		/// @param ifs Input stream positioned at the expected data.
		void LoadManagerData(std::ifstream& ifs) override;
		/// @brief Restores plugin resource state. This plugin stores none.
		/// @param ifs Input stream positioned at the expected data.
		void LoadResourceData(std::ifstream& ifs) override;

	private:
		b2WorldId mWorldID;
		float	  mFixedTimeStep;
		int		  mSubStepCount;

		static Physics2D* mInstance;

	private:
		Common::FTDS::DynamicArray<Common::IComponent*>* mRegisteredComps;
	};

	Physics2D* Physics2D::mInstance = nullptr;

	Physics2D* GetPhysics2D()
	{
		return Physics2D::GetInstance();
	}

	b2WorldId& GetPhysics2DWorldID()
	{
		return GetPhysics2D()->GetCurrentWorldID();
	}

	/// @brief Returns the plugin name used by the module host.
	/// @return Stable plugin name.
	const char* Physics2D::GetName()
	{
		return "Physics2D";
	}

	/// @brief Accepts a component for later plugin-managed processing.
	/// @param comp Component instance associated with the actor or plugin.
	void Physics2D::RegisterComponent(Common::IComponent* comp)
	{
		mRegisteredComps->PushBack(comp);
	}

	/// @brief Writes persistent properties. This plugin stores none.
	/// @param ofs Output stream receiving the serialized data.
	void Physics2D::SaveProperties(std::ofstream& ofs)
	{
		(void)ofs;
	}

	/// @brief Reads persistent properties. This plugin stores none.
	/// @param ifs Input stream positioned at the expected data.
	void Physics2D::LoadProperties(std::ifstream& ifs)
	{
		(void)ifs;
	}

	/// @brief Initializes the services required before the runtime lifecycle begins.
	void Physics2D::Initialize()
	{
		const float pixelsPerMeter = 64.f / 1.8f;
		b2SetLengthUnitsPerMeter(pixelsPerMeter);

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity	= b2Vec2{ 0.0f, -9.8f * pixelsPerMeter };
		mWorldID			= b2CreateWorld(&worldDef);

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
			(*iter)->Initialize();
	}

	/// @brief Finishes setup after initialization.
	void Physics2D::Setup()
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
			(*iter)->Setup();
	}

	/// @brief Dispatches input for the current frame.
	void Physics2D::ProcessInput()
	{
	}

	/// @brief Advances frame-dependent state.
	/// @param deltaTime Elapsed frame time in seconds.
	void Physics2D::Update(float deltaTime)
	{
		if (!Core::FTCore::GetInstance()->GetIsUpdating())
			return;
		if (!b2World_IsValid(mWorldID))
			return;

		b2World_Step(mWorldID, mFixedTimeStep, mSubStepCount);

		if (!Core::FTCore::GetInstance()->GetIsUpdating())
			return;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->Update(deltaTime);
		}
	}

	/// @brief Runs the post-update lifecycle phase.
	/// @param deltaTime Elapsed frame time in seconds.
	void Physics2D::LateUpdate(float deltaTime)
	{
		if (!Core::FTCore::GetInstance()->GetIsUpdating())
			return;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->LateUpdate(deltaTime);
		}
	}

	/// @brief Submits graphics work for the current frame.
	void Physics2D::Render()
	{
	}

	/// @brief Processes queued lifecycle and scene changes.
	void Physics2D::ProcessEvent()
	{
	}

	/// @brief Releases runtime services during engine shutdown.
	void Physics2D::ShutDown()
	{
		b2DestroyWorld(mWorldID);
	}

	/// @brief Creates the plugin.
	/// @param name Name supplied by the module factory. The reported plugin name stays "Physics2D".
	Physics2D::Physics2D(const char* name)
		: mWorldID(b2_nullWorldId)
		, mFixedTimeStep(1.f / 60.f)
		, mSubStepCount(4)
		, mRegisteredComps(DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>)
	{
		(void)name;
		mInstance = this;
	}

	/// @brief Clears the shared instance when it refers to this plugin.
	Physics2D::~Physics2D()
	{
		if (mInstance == this)
			mInstance = nullptr;
	}

	/// @brief Returns the plugin instance created by the module factory.
	/// @return Borrowed plugin pointer, or nullptr before the module is created.
	Physics2D* Physics2D::GetInstance()
	{
		return mInstance;
	}

	/// @brief Returns the Box2D world owned by this plugin.
	/// @return World identifier. Invalid until Initialize runs.
	b2WorldId& Physics2D::GetCurrentWorldID()
	{
		return mWorldID;
	}

	/// @brief Restores plugin manager state. This plugin stores none.
	/// @param ifs Input stream positioned at the expected data.
	void Physics2D::LoadManagerData(std::ifstream& ifs)
	{
		(void)ifs;
	}

	/// @brief Restores plugin resource state. This plugin stores none.
	/// @param ifs Input stream positioned at the expected data.
	void Physics2D::LoadResourceData(std::ifstream& ifs)
	{
		(void)ifs;
	}
} // namespace Physics2D

FT_EXPORT void SetComponentFactory(Common::FTDS::HashMap<ComponentFactoryEntry>& entries) noexcept
{
	entries.Insert(Physics2D::ChunkKey::Rigidbody2D::NAME, CreateRegisteredComponent<Physics2D::Rigidbody2D>);
	entries.Insert(Physics2D::ChunkKey::BoxCollider2D::NAME, CreateRegisteredComponent<Physics2D::BoxCollider2D>);
	entries.Insert(Physics2D::ChunkKey::CircleCollider2D::NAME, CreateRegisteredComponent<Physics2D::CircleCollider2D>);
}

extern "C"
{
	PHYSICS2D_API Common::IPlugin* CreatePlugin(const char* name)
	{
		return DBG_NEW Physics2D::Physics2D(name);
	}

	/// @brief Constructs the requested component type for an actor.
	/// @param plugin Plugin supplying component or lifecycle services.
	/// @param actor Actor participating in this operation.
	/// @param name Name used to identify the requested object or interface.
	/// @return Created component instance or resource.
	PHYSICS2D_API Common::IComponent* CreateComponent(Common::IPlugin* plugin, Common::IActor* actor, Common::FTDS::String& name)
	{
		(void)plugin;
		Common::FTDS::HashMap<ComponentFactoryEntry> entries(32);
		SetComponentFactory(entries);
		Common::FTDS::Record<ComponentFactoryEntry>* record = entries.At(name);
		if (!record || !record->Value())
			return nullptr;
		return record->Value()(actor);
	}
}