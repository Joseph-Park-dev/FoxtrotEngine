#pragma once
#include "Utility/SingletonMacro.h"

#include "Component/IComponent.h"
#include "FTDS/Static/FTString.h"
#include "FTDS/Static/HashMap.h"

namespace Core
{
	class FTCore
	{
		SINGLETON(FTCore)

	public:
		//////////////////////////////////
		////// Initialization Phase //////
		//////////////////////////////////
		// clang-format off
		virtual void Initialize();
		virtual void Setup();
		// clang-format on

		///////////////////////
		////// Game Loop //////
		///////////////////////
		// clang-format off
		virtual void ProcessInput();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();
		virtual void ProcessEvent();
		// clang-format on

		///////////////////////////////
		////// Termination Phase //////
		///////////////////////////////
		virtual void ShutDown();

	public:
		void SetIsRunning(bool val) { mIsRunning = val; }
		void SetIsUpdating(bool val) { mIsUpdating = val; }

	private:
		Common::FTDS::String* mGameDataPath;

		bool mIsRunning;
		bool mIsUpdating;

	private:
		void LoadGameData();
	};
} // namespace Core