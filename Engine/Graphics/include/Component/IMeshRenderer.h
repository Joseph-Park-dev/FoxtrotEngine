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
#include "Component/IComponent.h"


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
		/// @brief Returns the name used by this imesh renderer.
		/// @return Borrowed access to the name.
		virtual const char* GetName() const = 0;

		/////////////////////////
		// Game-loop functions //
		/////////////////////////
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize()													   = 0;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup()														   = 0;
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(InputSystem::IInputDevice* inputDevice)			   = 0;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime)										   = 0;
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime)									   = 0;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;

		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Common::IActor* actor) = 0;

		/////////////////////////////////////
		// Instantiation-related functions //
		/////////////////////////////////////
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IMeshRenderer() = default;

		///////////////////////////////////////
		// Getters & Setters to the elements //
		///////////////////////////////////////
	protected:
		/// @brief Returns the mesh group used by this imesh renderer.
		/// @return Borrowed access to the mesh group.
		virtual IMeshGroup*		 GetMeshGroup() const = 0;
		/// @brief Returns the texture used by this imesh renderer.
		/// @return Borrowed access to the texture.
		virtual ITexture*		 GetTexture() const	  = 0;
		/// @brief Returns the vs used by this imesh renderer.
		/// @return Borrowed access to the vs.
		virtual IVertexShader*	 GetVS() const		  = 0;
		/// @brief Returns the gs used by this imesh renderer.
		/// @return Borrowed access to the gs.
		virtual IGeometryShader* GetGS() const		  = 0;
		/// @brief Returns the ps used by this imesh renderer.
		/// @return Borrowed access to the ps.
		virtual IPixelShader*	 GetPS() const		  = 0;
		/// @brief Returns the material used by this imesh renderer.
		/// @return Borrowed access to the material.
		virtual IMaterial*		 GetMaterial() const  = 0;

		/// @brief Updates the mesh group used by subsequent operations.
		/// @param meshGroup Replacement mesh group.
		virtual void SetMeshGroup(IMeshGroup* meshGroup) = 0;
		/// @brief Updates the texture used by subsequent operations.
		/// @param tex Replacement texture.
		virtual void SetTexture(ITexture* tex)			 = 0;
		/// @brief Updates the vs used by subsequent operations.
		/// @param vs Replacement vs.
		virtual void SetVS(IVertexShader* vs)			 = 0;
		/// @brief Updates the gs used by subsequent operations.
		/// @param gs Replacement gs.
		virtual void SetGS(IGeometryShader* gs)			 = 0;
		/// @brief Updates the ps used by subsequent operations.
		/// @param ps Replacement ps.
		virtual void SetPS(IPixelShader* ps)			 = 0;
		/// @brief Updates the material used by subsequent operations.
		/// @param mat Replacement material.
		virtual void SetMaterial(IMaterial* mat)		 = 0;

		///////////////////////////////////
		// Save & Load related functions //
		///////////////////////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;
	};
} // namespace Graphics
