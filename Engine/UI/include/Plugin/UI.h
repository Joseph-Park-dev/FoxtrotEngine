// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// UI plugin loaded by the engine module host.
/// </summary>
#pragma once

namespace Graphics
{
	class ICamera;
}

namespace InputSystem
{
	class IInputDevice;
}

namespace Common
{
	class IComponent;
}

namespace D3D11
{
	class FTFont;
}

namespace Common
{
	template <typename FTRESOURCE>
	class ResourcePack;
}

#ifdef FOXTROT_EDITOR
namespace Editor
{
	class ICommandHistory;

	/// @brief Resolves GetCMDHistory from Editor.dll. Safe to call more than once.
	void AcquireCommandHistory();

	/// @brief Returns the command history stored by AcquireCommandHistory.
	/// @return Borrowed command history. Null until AcquireCommandHistory has run.
	ICommandHistory* GetCommandHistory();
} // namespace Editor
#endif

namespace UI
{
	class UIPlugin;

	UIPlugin* GetUI();

	/// @brief Registers a component for plugin-managed lifecycle processing.
	void RegisterOwnedComponent(Common::IComponent* comp);
	/// @brief Removes a component from plugin-managed lifecycle processing.
	void UnregisterOwnedComponent(Common::IComponent* comp);

	/// @brief Returns the font pack owned by the UI plugin.
	/// @return Borrowed font pack. Null until the plugin is created.
	Common::ResourcePack<D3D11::FTFont>* GetFonts();

	/// @brief Returns the camera resolved during the latest UI input pass.
	Graphics::ICamera* GetUICamera();
	/// @brief Returns the input device resolved during the latest UI input pass.
	InputSystem::IInputDevice* GetUIInputDevice();
} // namespace UI
