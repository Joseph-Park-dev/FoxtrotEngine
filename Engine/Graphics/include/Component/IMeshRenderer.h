// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Component related to render sprites, tilemaps, animations as meshes.
/// </summary>

#pragma once
#include "IComponent.h"


namespace InputSystem
{
	class IInputDevice;
}

namespace Graphics
{
	class IRenderer;
	class ICamera;
	class IMeshRenderer;
	class IMeshGroup;
	class ITexture;
	class IVertexShader;
	class IGeometryShader;
	class IPixelShader;
	class IMaterial;

	class IMeshRenderer :
		public Common::IComponent
	{
		///////////////////////////
		// Component information //
		///////////////////////////
	public:
		virtual const char* GetName() const = 0;

		/////////////////////////
		// Game-loop functions //
		/////////////////////////
	public:
		virtual void Initialize()													   = 0;
		virtual void Setup()														   = 0;
		virtual void ProcessInput(InputSystem::IInputDevice* inputDevice)			   = 0;
		virtual void Update(float deltaTime)										   = 0;
		virtual void LateUpdate(float deltaTime)									   = 0;
		virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;

		virtual void CloneTo(Common::IActor* actor) = 0;

		/////////////////////////////////////
		// Instantiation-related functions //
		/////////////////////////////////////
	public:
		virtual ~IMeshRenderer() = default;

		///////////////////////////////////////
		// Getters & Setters to the elements //
		///////////////////////////////////////
	protected:
		virtual IMeshGroup*		 GetMeshGroup() const = 0;
		virtual ITexture*		 GetTexture() const	  = 0;
		virtual IVertexShader*	 GetVS() const		  = 0;
		virtual IGeometryShader* GetGS() const		  = 0;
		virtual IPixelShader*	 GetPS() const		  = 0;
		virtual IMaterial*		 GetMaterial() const  = 0;

		virtual void SetMeshGroup(IMeshGroup* meshGroup) = 0;
		virtual void SetTexture(ITexture* tex)			 = 0;
		virtual void SetVS(IVertexShader* vs)			 = 0;
		virtual void SetGS(IGeometryShader* gs)			 = 0;
		virtual void SetPS(IPixelShader* ps)			 = 0;
		virtual void SetMaterial(IMaterial* mat)		 = 0;

		///////////////////////////////////
		// Save & Load related functions //
		///////////////////////////////////
	public:
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void LoadProperties(std::ifstream& ifs) = 0;
	};
} // namespace Graphics