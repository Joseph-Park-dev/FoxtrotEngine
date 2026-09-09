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
#include "Component/D3D11Component.h"

#include "FTDS/Static/FTString.h"

namespace Core
{
	class Transform;
	class FoxtrotRenderer;
} // namespace Core

namespace D3D11
{
	using namespace Common;
	class FTTexture;
	class FTVertexShader;
	class FTGeometryShader;
	class FTPixelShader;
	class FTMaterial;
	struct Mesh;
	struct FTMeshData;
	class FTMeshGroup;
	class FTCore;
	class Camera;

	class MeshRenderer :
		public D3D11Component
	{
		///////////////////////////
		// Component information //
		///////////////////////////
	public:
		static inline const char* NAME = "MeshRenderer";
		/// @brief Returns the name used by this mesh renderer.
		/// @return Borrowed access to the name.
		virtual const char*		  GetName() override
		{
			return "MeshRenderer";
		}

		/////////////////////////
		// Game-loop functions //
		/////////////////////////
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() override;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup() override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) override;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

		/////////////////////////////////////
		// Instantiation-related functions //
		/////////////////////////////////////
	public:
		/// @brief Initializes mesh and material state for its owning actor.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		MeshRenderer(Core::IActor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~MeshRenderer() override;
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Core::IActor* actor) override;

		///////////////////////////////////////
		// Getters & Setters to the elements //
		///////////////////////////////////////
	protected:
		/// @brief Returns the mesh group used by this mesh renderer.
		/// @return Borrowed access to the mesh group.
		FTMeshGroup*		   GetMeshGroup() const;
		/// @brief Returns the texture used by this mesh renderer.
		/// @return Borrowed access to the texture.
		FTTexture*			   GetTexture() const;
		/// @brief Returns the renderer used by this mesh renderer.
		/// @return Borrowed access to the renderer.
		Core::FoxtrotRenderer* GetRenderer() const;
		/// @brief Returns the vs used by this mesh renderer.
		/// @return Borrowed access to the vs.
		FTVertexShader*		   GetVS() const;
		/// @brief Returns the gs used by this mesh renderer.
		/// @return Borrowed access to the gs.
		FTGeometryShader*	   GetGS() const;
		/// @brief Returns the ps used by this mesh renderer.
		/// @return Borrowed access to the ps.
		FTPixelShader*		   GetPS() const;

		/// @brief Returns the material used by this mesh renderer.
		/// @return Borrowed access to the material.
		FTMaterial* GetMaterial() const;

		/// @brief Updates the renderer used by subsequent operations.
		/// @param renderer Replacement renderer.
		void SetRenderer(Core::FoxtrotRenderer* renderer);
		/// @brief Updates the mesh group used by subsequent operations.
		/// @param meshGroup Replacement mesh group.
		void SetMeshGroup(FTMeshGroup* meshGroup);
		/// @brief Updates the texture used by subsequent operations.
		/// @param tex Replacement texture.
		void SetTexture(FTTexture* tex);
		/// @brief Updates the vs used by subsequent operations.
		/// @param vs Replacement vs.
		void SetVS(FTVertexShader* vs);
		/// @brief Updates the gs used by subsequent operations.
		/// @param gs Replacement gs.
		void SetGS(FTGeometryShader* gs);
		/// @brief Updates the ps used by subsequent operations.
		/// @param ps Replacement ps.
		void SetPS(FTPixelShader* ps);
		/// @brief Updates the material used by subsequent operations.
		/// @param mat Replacement material.
		void SetMaterial(FTMaterial* mat);

		////////////////////////////////////////
		// Component elements				  //
		// These will be set in Initialize()  //
		////////////////////////////////////////
	private:
		Core::FoxtrotRenderer* mRenderer;
		FTMeshGroup*		   mMeshGroup;
		FTTexture*			   mTexture;
		FTVertexShader*		   mVS;
		FTGeometryShader*	   mGS;
		FTPixelShader*		   mPS;
		FTMaterial*			   mMaterial;
		int					   mFrontDir;

		///////////////////////////////////
		// Save & Load related functions //
		///////////////////////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs);

		/////////////////////////////////////
		// FoxtrotEditor related functions //
		/////////////////////////////////////
#ifdef FOXTROT_EDITOR
	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst) override;

	protected:
		/// @brief Exposes the material used when drawing this object.
		/// @return Borrowed access to the material.
		/// @note Changes through the returned reference affect this object's stored state.
		FTMaterial*& Material();

#endif
	};

	namespace ChunkKey
	{
		namespace MeshRenderer
		{
			constexpr const char* NAME = "MeshRenderer";
		} // namespace MeshRenderer
	} // namespace ChunkKey

	#include "Plugin/D3D11Exports.h"
/// @brief Creates a mesh-rendering component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created mesh renderer instance or resource.
D3D11_API D3D11::MeshRenderer* CreateMeshRenderer(Core::IActor* actor);
} // namespace D3D11
