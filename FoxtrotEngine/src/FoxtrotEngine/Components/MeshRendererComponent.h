#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include "FoxtrotEngine/Core/FTCore.h"

class Actor;
class Camera;
class Transform;
struct Mesh;
struct MeshData;

class MeshRendererComponent :
    public Component
{
public:
	// core paramter
	// Editor Mode : FTCoreEditor
	// Game Mode   : FTCore
	virtual	void Initialize	(FTCore* coreInstance)		override {};
	virtual void Update		(float deltaTime)			override {};
	virtual void LateUpdate	(float deltaTime)			override {};
	virtual void Render		(FoxtrotRenderer* renderer) override {};

protected:
	virtual void InitializeMesh(FTCore* coreInstance, MeshData&& meshData);
			void UpdateMesh	(Transform* transform, Camera* cameraInstance);
			void RenderMesh	(FoxtrotRenderer* renderer);

protected:
	FoxtrotRenderer* GetRenderer()	{ return mRenderer; }
	Mesh*			 GetMesh()		{ return mMesh; }

private:
	FoxtrotRenderer* mRenderer;
	Mesh*			 mMesh;

public:
	MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder);
	virtual ~MeshRendererComponent() override;

private:
	void UpdateConstantBufferModel		(Mesh* mesh, Transform* transform);
	void UpdateConstantBufferView		(Mesh* mesh, Camera* camInst);
	void UpdateConstantBufferProjection	(Mesh* mesh, Camera* camInst);
};

