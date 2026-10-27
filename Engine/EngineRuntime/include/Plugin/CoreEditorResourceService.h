// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Editor service that loads and displays core assets (CSV, JSON, text).
/// </summary>
#pragma once
#include "Plugin/CoreExports.h"
#include "Plugin/IEditorResourceService.h"

#ifdef FOXTROT_EDITOR
namespace Common
{
	class IPlugin;
}

namespace Core
{
	class CoreEditorResourceService final :
		public IEditorResourceService
	{
	public:
		virtual bool LoadDefaultResources(
			const char*			 projDirectory,
			Graphics::IRenderer* renderer = nullptr) noexcept override;

		/// @brief Loads CSV, JSON, and text assets from the given directory.
		/// @param assetDirectory Directory containing the assets to load.
		/// @param renderer Unused. Core assets do not require a graphics device.
		/// @return True when the directory is valid and loading finishes.
		bool LoadAllResourcesInAsset(
			const char*			 assetDirectory,
			Graphics::IRenderer* renderer) noexcept override;

		/// @brief Draws the loaded core assets in the editor resource menu.
		void DisplayLoadedResources() override;
	};

	/// @brief Points this module's ImGui copy at the editor context.
	/// @param context ImGui context created by the editor, or nullptr on shutdown.
	CORE_API void SetEditorGuiContext(void* context);

	/// @brief Creates the core plugin that publishes this editor resource service.
	/// @return Plugin owned by the caller.
	Common::IPlugin* CreateCorePlugin();
} // namespace Core
#endif // FOXTROT_EDITOR
