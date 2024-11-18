#pragma once
#include "Components/Component.h"

#include <nlohmann/json.hpp>

#include "Core/FTCore.h"
#include "ResourceSystem/FTBasicMeshGroup.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
struct Mesh;
struct MeshData;

#define VALUE_NOT_ASSIGNED -1

#define CHECK_RENDERER(RendererInst) if(!RendererInst) ImGui::Text("Renderer is not loaded");

class MeshRendererComponent :
    public Component
{
public:
	virtual	void Initialize	(FTCore* coreInstance)		override;
	virtual void Update		(float deltaTime)			override;
	virtual void LateUpdate	(float deltaTime)			override {};
	virtual void Render		(FoxtrotRenderer* renderer) override;

protected:
	virtual bool InitializeMesh();
	virtual bool InitializeMesh(UINT key);
	virtual bool InitializeMesh(MeshData& meshData);
	virtual	void UpdateMesh(Transform* transform, Camera* cameraInstance);
	virtual void UpdateBuffers();

public:
	virtual std::string GetName() const override { return "MeshRendererComponent"; }

protected:
	FoxtrotRenderer*	GetRenderer	()	{ return mRenderer; }
	FTBasicMeshGroup*	GetMeshGroup()	{ return mMeshGroup; }
	UINT				GetMeshKey	()	{ return mMeshKey; }
	UINT				GetTexKey	()	{ return mTexKey; }
	FTTexture*			GetTexture	()	{ return mMeshGroup->GetTexture(); }

	void				SetMeshGroup(FTBasicMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
	void				SetMeshKey	(UINT key) { mMeshKey = key; }
	void				SetTexKey	(UINT key) { mTexKey = key; }
	virtual void		SetTexture();

private:
	FoxtrotRenderer*	mRenderer;
	FTBasicMeshGroup*	mMeshGroup;
	// Identifier for the object in the Resource Map from the ResourceManager instance.
	UINT				mMeshKey;
	UINT				mTexKey;

public:
			MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder);
	virtual ~MeshRendererComponent() override;

protected:
	void UpdateConstantBufferModel		(Transform* transform);
	void UpdateConstantBufferView		(Camera* camInst);
	void UpdateConstantBufferProjection	(Camera* camInst);

public:
	virtual void		 SaveProperties(std::ofstream& ofs);
	virtual void		 LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;
	virtual void LoadProperties(nlohmann::ordered_json& in)	 override;
	virtual void EditorUIUpdate() override;

protected:
	virtual void OnConfirmUpdate() override;
#endif
};