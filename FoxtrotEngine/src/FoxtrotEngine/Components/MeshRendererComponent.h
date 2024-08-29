#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include <nlohmann/json.hpp>

#include "FoxtrotEngine/Core/FTCore.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
class FTBasicMeshGroup;
struct Mesh;
struct MeshData;

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
	virtual bool InitializeMesh(std::string key);
	virtual	void UpdateMesh(Transform* transform, Camera* cameraInstance);
			void UpdateBuffers();

public:
	virtual std::wstring GetName() const override { return L"MeshRendererComponent"; }

protected:
	FoxtrotRenderer*	GetRenderer	()	{ return mRenderer; }
	FTBasicMeshGroup*	GetMeshGroup()	{ return mMeshGroup; }
	std::string			GetMeshKey	()	{ return mMeshKey; }
	std::string			GetTexKey	()	{ return mTexKey; }

	void				SetMeshKey	(std::string key) { mMeshKey.assign(key); }
	void				SetTexKey	(std::string key) { mTexKey.assign(key); }
	void				SetTexture();

private:
	FoxtrotRenderer*	mRenderer;

	FTBasicMeshGroup*	mMeshGroup;
	// Identifier for the object in the Resource Map from the ResourceManager instance.
	std::string			mMeshKey;
	std::string			mTexKey;

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
#endif // DEBUG
};