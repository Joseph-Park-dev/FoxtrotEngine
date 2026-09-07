// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Compatibility aliases for the Core-to-Graphics/InputSystem interface migration.
/// Existing modules still name types as Core::IRenderer, Core::ICamera, etc.
/// </summary>
#pragma once

#include "Renderer/IRenderer.h"
#include "Renderer/IWindow.h"
#include "Renderer/ICamera.h"
#include "Component/ISpriteRenderer.h"
#include "Component/IAnimator.h"
#include "Component/IMeshRenderer.h"
#include "IInputDevice.h"
#include "Plugin/IPlugin.h"
#include "Actor/IActor.h"
#include "Actor/ActorGroup.h"
#include "Component/IComponent.h"
#include "ResourceSystem/FTResource.h"

namespace Common
{
	enum class ActorState;
} // namespace Common

namespace Core
{
	using IRenderer		  = Graphics::IRenderer;
	using IWindow		  = Graphics::IWindow;
	using ICamera		  = Graphics::ICamera;
	using ISpriteRenderer = Graphics::ISpriteRenderer;
	using IAnimator		  = Graphics::IAnimator;
	using IMeshRenderer	  = Graphics::IMeshRenderer;
	using IInputDevice	  = InputSystem::IInputDevice;
	using IPlugin		  = Common::IPlugin;
	using IActor		  = Common::IActor;
	using IComponent	  = Common::IComponent;
	using ActorGroup	  = Common::ActorGroup;
	using ActorGroupUtil  = Common::ActorGroupUtil;
	using ActorState	  = Common::ActorState;
	using FillMode		  = Graphics::FillMode;
	using Viewtype		  = Graphics::Viewtype;
	using MOUSE			  = InputSystem::MOUSE;
	using KEYBOARD		  = InputSystem::KEYBOARD;
	using ButtonState	  = InputSystem::ButtonState;
	using ButtonInput	  = InputSystem::ButtonInput;
	using FTResourceDef	  = Common::FTResourceDef;

	namespace DefaultVal = Common::DefaultVal;
} // namespace Core
