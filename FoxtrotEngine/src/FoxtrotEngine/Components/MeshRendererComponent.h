#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include <nlohmann/json.hpp>

#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
struct Mesh;
struct MeshData;

#define VALUE_NOT_ASSIGNED -1

class MeshRendererComponent :
    public Component
{
public:
	// core paramter
	// Editor Mode : FTCoreEditor
	// Game Mode   : FTCore
	virtual	void Initialize	(FTCore* coreInstance)		override;
	virtual void Update		(float deltaTime)			override;
	virtual void LateUpdate	(float deltaTime)			override {};
	virtual void Render		(FoxtrotRenderer* renderer) override;

protected:
	virtual bool InitializeMesh();
	virtual bool InitializeMesh(UINT key);
	virtual bool InitializeMesh(MeshData meshData);
	virtual	void UpdateMesh(Transform* transform, Camera* cameraInstance);
	virtual void UpdateBuffers();

public:
	virtual std::wstring GetName() const override { return L"MeshRendererComponent"; }

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

#ifdef _DEBUG
	//This section will be omitted from Release mode!
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;
	//virtual void LoadProperties(std::ifstream& ifs) override;

	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;
	virtual void EditorUIUpdate() override;

protected:
	virtual void OnConfirmUpdate();
#endif // DEBUG
};