#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include "FoxtrotEngine/Core/FTCore.h"

class Actor;
class Camera;
class Transform;
class FTTexture;
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
	virtual void InitializeMesh(std::vector<MeshData>&& meshDataVec);
	virtual void InitializeMesh(MeshData&& meshData);
	virtual	void UpdateMeshArray(Transform* transform, Camera* cameraInstance);
			void RenderMeshArray(FoxtrotRenderer* renderer);

public:
	virtual std::wstring GetName() const override { return L"MeshRendererComponent"; }

protected:
	FoxtrotRenderer*	GetRenderer()	{ return mRenderer; }
	Mesh**				GetMeshArray()	{ return mMeshArr; }
	std::string			GetKey()		{ return mKey; }
	size_t				GetMeshCount()	{ return GetArrayLength(mMeshArr); }

	void				SetKey		(std::string key) { mKey = key; }
	void				SetTexture	(FTTexture* texture);

private:
	FoxtrotRenderer*	mRenderer;
	Mesh**				mMeshArr;

	// Identifier for the object in the Resource Map from the ResourceManager instance.
	std::string      mKey;

public:
	MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder);
	virtual ~MeshRendererComponent() override;

protected:
	void UpdateConstantBufferModel		(Mesh* mesh, Transform* transform);
	void UpdateConstantBufferView		(Mesh* mesh, Camera* camInst);
	void UpdateConstantBufferProjection	(Mesh* mesh, Camera* camInst);

private:
	void AddCube();
	void AddMesh(MeshData&& meshData, size_t index);

	bool IsMeshEmpty();

#ifdef _DEBUG
	//This section will be omitted from Release mode!
public:
	//virtual void SaveProperties(std::ofstream& ofs) override;
	//virtual void LoadProperties(std::ifstream& ifs) override;

	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorUIUpdate() override;
#endif // DEBUG
};