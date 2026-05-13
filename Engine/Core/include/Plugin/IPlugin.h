// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Interface for all Plugins.
/// A direct instantiation of this class is strictly prohibited.
/// </summary>

#include <Windows.h>
#include <assert.h>

#include "Debugging/DebugFuncs.h"

#pragma once
namespace Core
{
	class Entity;

	class IPlugin
	{
	public:
		virtual Core::Entity* GetEntity(const char* name) = 0;

	public:
		//////////////////////////////////
		////// Initialization Phase //////
		//////////////////////////////////
		virtual void Initialize() = 0;
		virtual void Setup()	  = 0;

		///////////////////////
		////// Game Loop //////
		///////////////////////
		virtual void ProcessInput()				 = 0;
		virtual void Update(float deltaTime)	 = 0;
		virtual void LateUpdate(float deltaTime) = 0;
		virtual void Render()					 = 0;
		virtual void ProcessEvent()				 = 0;

		///////////////////////////////
		////// Termination Phase //////
		///////////////////////////////
		virtual void ShutDown() = 0;

	public:
		virtual ~IPlugin() {};
	};

	namespace ChunkKey
	{
		namespace Plugin
		{
			constexpr const char* PLUGIN_DATA		= "Plugin Data";
			constexpr const char* DLL_PATH			= "DLL Path";
			constexpr const char* COMP_CONSTRUCTORS = "Comp Constructors";
			constexpr const char* MANAGER_DATA		= "Manager Data";
		} // namespace Plugin
	} // namespace ChunkKey

	extern "C" __declspec(dllexport) IPlugin* CreatePlugin(const char* name);
} // namespace Core