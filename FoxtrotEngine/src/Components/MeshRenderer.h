// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base  to render sprites, tilemaps, animations as meshes.
/// </summary>

#pragma once
#include "Components/Component.h"

#include <directxtk/SimpleMath.h>

#include "Core/FTCore.h"
#include "ResourceSystem/FTBasicMeshGroup.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
struct Mesh;
struct FTMeshData;

#define CHECK_RENDERER(RendererInst) \
	if (!RendererInst)               \
		ImGui::Text("Renderer is not loaded");

class MeshRenderer :
	public Component
{
public:
	virtual std::string GetName() const override { return "MeshRenderer"; }

public:
	FoxtrotRenderer*  GetRenderer() { return mRenderer; }
	FTBasicMeshGroup* GetMeshGroup() { return mMeshGroup; }
	UINT			  GetMeshKey() { return mMeshKey; }
	UINT			  GetTexKey() { return mTexKey; }
	FTTexture*		  GetTexture() { return mTexture; }

	void SetRenderer(FoxtrotRenderer* renderer) { mRenderer = renderer; }
	void SetMeshGroup(FTBasicMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
	void SetMeshKey(UINT key) { mMeshKey = key; }
	void SetTexKey(UINT key) { mTexKey = key; }

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

	virtual void CloneTo(Actor* actor) override;

public:
	MeshRenderer(Actor* owner, int updateOrder);
	virtual ~MeshRenderer() override;

protected:
	virtual bool InitializeMesh();
	bool		 InitializeMesh(UINT key);
	bool		 InitializeMesh(FTMeshData& meshData);
	bool		 InitializeMesh(std::vector<FTMeshData>& meshData);

	bool		 SetTexture();
	virtual void UpdateMesh(Transform* transform, Camera* camInst);
	virtual void UpdateBuffers();

	DirectX::SimpleMath::Matrix CalcModelMat(Transform* transform);

private:
	// Identifier for the object in the Resource Map from the ResourceManager instance.
	// These will be read from .chunk file.
	UINT mMeshKey;
	UINT mTexKey;

	// These will be set in Initialize() member function.
	FoxtrotRenderer*  mRenderer;
	FTBasicMeshGroup* mMeshGroup;
	FTTexture*		  mTexture;

protected:
	FTMaterial* mMaterial;

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorUIUpdate() override;

protected:
	virtual void OnConfirmUpdate() override;
	void		 OnResetTexture();
	void		 UpdateSprite();
	void		 UpdateSprite(UINT& key);

private:
	void AddCube();
	void AddPlane();
	void AddCylinder();
	void AddSphere();

	void Add3DModel();
#endif
};

namespace ChunkKey
{
	constexpr const char* MESH_KEY	  = "MeshKey";
	constexpr const char* TEXTURE_KEY = "TextureKey";
} // namespace ChunkKey