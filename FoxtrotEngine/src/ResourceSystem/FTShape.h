#pragma once
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/MeshConstantData.h"
#include "Actors/Transform.h"
#include "Renderer/Camera.h"

class FTShape
{
public:
	FTShape();
	virtual ~FTShape();

public:
	BasicVertexConstantData& GetVertexConstantData() { return mVertexConstantData; }

public:
	virtual void Initialize(FoxtrotRenderer* renderer);

	// This should be included in components to EditorUpdate(float) 
			void Update(Transform* transform, Camera* cameraInst);
			void Update(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* cameraInst);

	// This is for ShapeActors (e.g. SquareActor)
			void Render(FoxtrotRenderer* renderer);
	// This should be called in DebugGeometries instance only once per frame.
	// You don't have to use this member function by yourself.
			void Render(
				FoxtrotRenderer* renderer, 
				ComPtr<ID3D11VertexShader>& vertexShader, 
				ComPtr<ID3D11PixelShader>& pixelShader, 
				ComPtr<ID3D11InputLayout>& inputLayout
			);

protected:
			void InitializeMesh(ComPtr<ID3D11Device>& device, MeshData&& meshData);

protected:
	virtual void UpdateConstantBufferModel(Transform* transform);
	virtual void UpdateConstantBufferModel(FTVector3 pos, FTVector3 rot, FTVector3 scale);
			void UpdateConstantBufferView(Camera* camInst);
			void UpdateConstantBufferProjection(Camera* camInst);

private:
	Mesh* mMesh;

private:
	BasicVertexConstantData mVertexConstantData;
	BasicPixelConstantData	mPixelConstantData;

	ComPtr<ID3D11Buffer>	mVertexConstantBuffer;
	ComPtr<ID3D11Buffer>	mPixelConstantBuffer;

private:
	void InitializeConstantBuffer(ComPtr<ID3D11Device>& device);
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
};

class FTRectangle : public FTShape
{
public:
	FTRectangle();

public:
	virtual void Initialize(FoxtrotRenderer* renderer) override;

protected:
	virtual void UpdateConstantBufferModel(FTVector3 pos, FTVector3 rot, FTVector3 scale) override;

private:
	FTVector2 mSize;
};