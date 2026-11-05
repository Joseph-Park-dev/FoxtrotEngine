#pragma once
#include "Foxtrot/Core/ModuleApi.h"

namespace Foxtrot
{
	struct RuntimeControl
	{
		// Registers a plugin with the runtime’s PluginManager. The void* is cast to Common::IPlugin*.
		Status(FT_CALL* Attach)(const char*, void*) noexcept;

		// Calls FTCore::RunFrames(maxFrames) to execute the engine loop.
		Status(FT_CALL* Run)(uint32_t maxFrames) noexcept;

		// Deletes the current scene’s objects, releases world resources, and clears the running flag. Repeated calls are guarded.
		void(FT_CALL* Stop)() noexcept;

		// Removes a plugin from the runtime registry. Does not delete the plugin.
		Status(FT_CALL* Detach)(const char*) noexcept;
	};
} // namespace Foxtrot
