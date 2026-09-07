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
#pragma once

#include "Debugging/DebugFuncs.h"
#include "Component/IComponent.h"
#include "FTDS/Static/FTString.h"

namespace Common
{
	class Entity;
	class IActor;

	class IPlugin
	{
	public:
		virtual void RegisterComponent(Common::IComponent* comp) = 0;

	public:
		////////////////////////////////
		////// Data Loading Phase //////
		////////////////////////////////
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void LoadProperties(std::ifstream& ifs) = 0;

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
		virtual void* QueryInterface(const char*) noexcept { return nullptr; }
		virtual ~IPlugin() = default;

	protected:
		virtual void LoadManagerData(std::ifstream& ifs)  = 0;
		virtual void LoadResourceData(std::ifstream& ifs) = 0;
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
} // namespace Common