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
#include "Components/Component.h"

#include "Static/FTString.h"

class FTBasicMeshGroup;
class FTCore;
class Actor;
class Camera;
class Transform;
class FTTexture;
class FTMaterial;
struct Mesh;
struct FTMeshData;

#define CHECK_RENDERER(RendererInst) \
	if (!RendererInst)               \
		ImGui::Text("Renderer is not loaded");

class MeshRenderer :
	public Component
{
	///////////////////////////
	// Component information //
	///////////////////////////
public:
	virtual std::string GetName() const override { return "MeshRenderer"; }

	/////////////////////////
	// Game-loop functions //
	/////////////////////////
public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

	/////////////////////////////////////
	// Instantiation-related functions //
	/////////////////////////////////////
public:
	MeshRenderer(Actor* owner, int updateOrder);
	virtual ~MeshRenderer() override;
	virtual void CloneTo(Actor* actor) override;

	/////////////////////////////////
	// Keys to select the elements //
	/////////////////////////////////
protected:
	FTDS::String& GetMeshKey();
	FTDS::String& GetTexKey();

	void SetMeshKey(FTDS::String& key);
	void SetMeshKey(const char* key);
	void SetTexKey(FTDS::String& key);
	void SetTexKey(const char* key);

	///////////////////////////////////////
	// Getters & Setters to the elements //
	///////////////////////////////////////
protected:
	FTBasicMeshGroup* GetMeshGroup() const;
	FTTexture*		  GetTexture() const;
	FoxtrotRenderer*  GetRenderer() const;

	void SetRenderer(FoxtrotRenderer* renderer);
	void SetMeshGroup(FTBasicMeshGroup* meshGroup);
	void SetMaterials();

	std::vector<FTDS::String>& MaterialKeys();

	////////////////////////
	// Element operations //
	////////////////////////
protected:
	virtual bool InitializeMesh();
	bool		 InitializeMesh(FTDS::String& key);
	virtual void UpdateMesh(Transform* transform, Camera* camInst);

	//////////////////////////////////////////
	// Component elements					//
	// These will be read from .chunk file. //
	//////////////////////////////////////////
private:
	// Identifiers for the object in the Resource Map from the ResourceManager instance.
	FTDS::String			 mMeshKey;
	FTDS::String			 mTexKey;
	std::vector<FTDS::String> mMaterialKeys;

	////////////////////////////////////////
	// Component elements				  //
	// These will be set in Initialize()  //
	////////////////////////////////////////
private:
	FoxtrotRenderer*  mRenderer;
	FTBasicMeshGroup* mMeshGroup;

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

protected:
	virtual void OnConfirmUpdate() override;
	void		 OnResetTexture();
	void		 UpdateSprite();
	void		 UpdateSprite(FTDS::String& key);
	void		 UpdateMaterial();
	void		 AddModel();
#endif
};

namespace ChunkKey
{
	constexpr const char* MESH_KEY		 = "Mesh Key";
	constexpr const char* TEXTURE_KEY	 = "Texture Key";
	constexpr const char* MATERIAL_KEYS	 = "Material Key";
	constexpr const char* MATERIAL_COUNT = "Count";
} // namespace ChunkKey