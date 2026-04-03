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
#include "Component/Component.h"

#include "Static/FTString.h"

class Actor;
class Transform;

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
		public Component
	{
		///////////////////////////
		// Component information //
		///////////////////////////
	public:
		virtual FTDS::String GetName() const override { return "MeshRenderer"; }

		/////////////////////////
		// Game-loop functions //
		/////////////////////////
	public:
		virtual void Initialize() override;
		virtual void Render(FoxtrotRenderer* renderer) override;

		/////////////////////////////////////
		// Instantiation-related functions //
		/////////////////////////////////////
	public:
		MeshRenderer(Plugin* plugin, Actor* owner, int updateOrder);
		virtual ~MeshRenderer() override;
		virtual void CloneTo(Actor* actor) override;

		///////////////////////////////////////
		// Getters & Setters to the elements //
		///////////////////////////////////////
	protected:
		FTMeshGroup*	  GetMeshGroup() const;
		FTTexture*		  GetTexture() const;
		FoxtrotRenderer*  GetRenderer() const;
		FTVertexShader*	  GetVS() const;
		FTGeometryShader* GetGS() const;
		FTPixelShader*	  GetPS() const;

		FTMaterial* GetMaterial() const;

		void SetRenderer(FoxtrotRenderer* renderer);
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
		FoxtrotRenderer*  mRenderer;
		FTMeshGroup*	  mMeshGroup;
		FTTexture*		  mTexture;
		FTVertexShader*	  mVS;
		FTGeometryShader* mGS;
		FTPixelShader*	  mPS;
		FTMaterial*		  mMaterial;
		int				  mFrontDir;

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