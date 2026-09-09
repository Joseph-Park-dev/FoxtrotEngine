#pragma once
#include "Plugin/CoreExports.h"
#include "Utility/SingletonMacro.h"

#include "Component/IComponent.h"
#include "FTDS/Static/FTString.h"
#include "FTDS/Static/HashMap.h"

namespace Core
{
	class CORE_API FTCore
	{
		SINGLETON(FTCore)

	public:
		//////////////////////////////////
		////// Initialization Phase //////
		//////////////////////////////////
		// clang-format off
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		virtual bool Initialize();
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		virtual void Setup();
		/// @brief Runs input, update, rendering, and event processing until the engine stops.
		virtual void RunLoop();
        /// @brief Runs the engine loop for a bounded number of frames.
        /// @param maxFrames Maximum number of frames to run.
        void RunFrames(unsigned int maxFrames);
		// clang-format on

		///////////////////////
		////// Game Loop //////
		///////////////////////
		// clang-format off
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		virtual void ProcessInput();
		/// @brief Advances frame-dependent state using the current time step.
		virtual void Update();
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		virtual void LateUpdate();
		/// @brief Submits this object's graphics work for the current frame.
		virtual void Render();
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		virtual void ProcessEvent();
		// clang-format on

		/// @brief Releases runtime services and resources during engine shutdown.
		/// ////////////////////////////
		/// /// Termination Phase //////
		/// ////////////////////////////
		virtual void ShutDown();

	public:
		/// @brief Updates the is running used by subsequent operations.
		/// @param val Replacement is running.
		void SetIsRunning(bool val) { mIsRunning = val; }
		/// @brief Updates the is updating used by subsequent operations.
		/// @param val Replacement is updating.
		void SetIsUpdating(bool val) { mIsUpdating = val; }

	private:
		Common::FTDS::String* mGameDataPath;

		bool mIsRunning;
		bool mIsUpdating;

	private:
		/// @brief Reads the configured game data needed to initialize runtime content.
		void LoadGameData();
	};
} // namespace Core
