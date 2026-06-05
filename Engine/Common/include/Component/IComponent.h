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

namespace Common
{
	class IActor;
	class IInputDevice;
	class IRenderer;
	class ICamera;

	namespace DefaultVal
	{
		constexpr int UPDATE_ORDER = 100;
	}

	class IComponent
	{
		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		virtual void Initialize() = 0;
		virtual void Setup()	  = 0;

		////////////////
		/// Gameloop ///
		////////////////
	public:
		virtual void ProcessInput(IInputDevice* inputDevice)						   = 0;
		virtual void Update(float deltaTime)										   = 0;
		virtual void LateUpdate(float deltaTime)									   = 0;
		virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		virtual const char* GetName()				 = 0;
		virtual IActor*		GetOwner()				 = 0;
		virtual const int	GetUpdateOrder()		 = 0;
		virtual const bool	GetIsInitialized() const = 0;
		virtual const bool	GetIsSetup() const		 = 0;
		virtual const bool	GetIsActive() const		 = 0;

		virtual void SetIsActive(bool isActive) = 0;

		//////////////////////////////
		/// Constructors / Copying ///
		//////////////////////////////
	public:
		virtual void CloneTo(IActor* actor) = 0;

		//		///////////////////////////////
		//		/// Editor Member Functions ///
		//		///////////////////////////////
		// #ifdef FOXTROT_EDITOR
		//	public:
		//		virtual void EditorUpdate(float deltaTime)									 = 0;
		//		virtual void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;
		//		virtual void EditorUIUpdate()												 = 0;
		// #endif // FOXTROT_EDITOR
	};
} // namespace Common