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

#include "Static/FTString.h"

namespace Core
{
	class Actor;
	class Transform;
	class FoxtrotRenderer;
} // namespace Core

namespace D3D11
{
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
		virtual Core::FTDS::String GetName() const override { return "MeshRenderer"; }

		/////////////////////////
		// Game-loop functions //
		/////////////////////////
	public:
		virtual void Initialize() override;
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(Core::FoxtrotRenderer* renderer) override;

		/////////////////////////////////////
		// Instantiation-related functions //
		/////////////////////////////////////
	public:
		MeshRenderer(Core::Plugin* plugin, Core::Actor* owner, int updateOrder);
		virtual ~MeshRenderer() override;
		virtual void CloneTo(Core::Actor* actor) override;

		///////////////////////////////////////
		// Getters & Setters to the elements //
		///////////////////////////////////////
	protected:
		FTMeshGroup*		   GetMeshGroup() const;
		FTTexture*			   GetTexture() const;
		Core::FoxtrotRenderer* GetRenderer() const;
		FTVertexShader*		   GetVS() const;
		FTGeometryShader*	   GetGS() const;
		FTPixelShader*		   GetPS() const;

		FTMaterial* GetMaterial() const;

		void SetRenderer(Core::FoxtrotRenderer* renderer);
		void SetMeshGroup(FTMeshGroup* meshGroup);
		void SetTexture(FTTexture* tex);
		void SetVS(FTVertexShader* vs);
		void SetGS(FTGeometryShader* gs);
		void SetPS(FTPixelShader* ps);
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
		virtual void SaveProperties(std::ofstream& ofs);
		virtual void LoadProperties(std::ifstream& ifs);

		/////////////////////////////////////
		// FoxtrotEditor related functions //
		/////////////////////////////////////
#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorRender(FoxtrotRenderer* renderer) override;
		virtual void EditorUIUpdate() override;
#endif
	};
} // namespace D3D11