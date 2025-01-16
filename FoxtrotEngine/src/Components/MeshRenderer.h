// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base Component to render sprites, tilemaps, animations as meshes.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "Core/FTCore.h"
#include "ResourceSystem/FTBasicMeshGroup.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
struct Mesh;
struct MeshData;

#define CHECK_RENDERER(RendererInst) if(!RendererInst) ImGui::Text("Renderer is not loaded");

class MeshRendererComponent :
    public Component
{
public:
	virtual	void Initialize	(FTCore* coreInstance)		override;
	virtual void Update		(float deltaTime)			override;
	virtual void Render		(FoxtrotRenderer* renderer) override;

	virtual void CloneTo	(Actor* actor)				override;

protected:
	virtual bool InitializeMesh	();
			bool InitializeMesh	(UINT key);
			bool InitializeMesh	(MeshData& meshData);

			bool SetTexture		();
	virtual	void UpdateMesh		(Transform* transform, Camera* cameraInstance);
	virtual void UpdateBuffers	();

public:
	virtual std::string GetName() const override { return "MeshRendererComponent"; }

protected:
	FoxtrotRenderer*	GetRenderer	()	{ return mRenderer; }
	FTBasicMeshGroup*	GetMeshGroup()	{ return mMeshGroup; }
	UINT				GetMeshKey	()	{ return mMeshKey; }
	UINT				GetTexKey	()	{ return mTexKey; }
	FTTexture*			GetTexture	()	{ return mTexture; }

	void				SetRenderer	(FoxtrotRenderer* renderer) { mRenderer = renderer; }
	void				SetMeshGroup(FTBasicMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
	void				SetMeshKey	(UINT key) { mMeshKey = key; }
	void				SetTexKey	(UINT key) { mTexKey = key; }

private:
	// Identifier for the object in the Resource Map from the ResourceManager instance.
	// These will be read from .chunk file.
	UINT				mMeshKey;
	UINT				mTexKey;

	// These will be set in Initialize() member function.
	FoxtrotRenderer*	mRenderer;
	FTBasicMeshGroup*	mMeshGroup;
	FTTexture*			mTexture;

public:
			 MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder);
	virtual ~MeshRendererComponent() override;

protected:
	void UpdateConstantBufferModel		(Transform* transform);
	void UpdateConstantBufferView		(Camera* camInst);
	void UpdateConstantBufferProjection	(Camera* camInst);

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;

protected:
	virtual void OnConfirmUpdate() override;
#endif
};