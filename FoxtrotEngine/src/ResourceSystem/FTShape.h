// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class that renders basic geometries.
/// </summary>

#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "Math/FTMath.h"
#include "ResourceSystem/MeshConstantData.h"

struct Mesh;
struct FTMeshData;
class FoxtrotRenderer;
class Transform;
class Camera;
class FTRectArea;
struct FTDebugMeshData;
struct DebugGCData;

using namespace Microsoft::WRL;

class FTShape
{
public:
	DebugVCData& GetVCData();
	DebugGCData& GetGSCData();
	DebugPCData& GetPixelConstantData();

	Mesh* GetMesh();
	bool  GetIsActive() { return mIsActive; }

	void SetIsActive(bool val) { mIsActive = val; }

public:
	virtual void Initialize(FoxtrotRenderer* renderer);
	void		 UpdateVC(Matrix& model, Camera* camInst);
	void		 UpdateGC(Camera* camInst);
	virtual void UpdatePC();

	// This is for ShapeActors (e.g. SquareActor)
	void Render(FoxtrotRenderer* renderer);
	// This should be called in DebugShapes instance only once per frame.
	// You don't have to use this member function by yourself.
	void Render(
		FoxtrotRenderer*			renderer,
		ComPtr<ID3D11VertexShader>& vertexShader,
		ComPtr<ID3D11PixelShader>&	pixelShader,
		ComPtr<ID3D11InputLayout>&	inputLayout);

public:
	FTShape();
	virtual ~FTShape();

protected:
	void InitializeMesh(ComPtr<ID3D11Device>& device, FTDebugMeshData&& meshData);

private:
	Mesh* mMesh;
	bool  mIsActive;

private:
	DebugVCData mVSCData;
	DebugGCData mGSCData;
	DebugPCData mPSCData;

	ComPtr<ID3D11Buffer> mVSCBuf;
	ComPtr<ID3D11Buffer> mGSCBuf;
	ComPtr<ID3D11Buffer> mPSCBuf;

private:
	void InitializeConstantBuffer(ComPtr<ID3D11Device>& device);
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	void UpdateModelMatrix(FTVector3 pos, FTVector3 rot, FTVector3 size);
	void UpdateViewMatrix(Camera* camInst);
	void UpdateProjectionMatrix(Camera* camInst);
};

namespace ChunkKey
{
	constexpr const char* FTSHAPE_IS_ACTIVE = "Is Active";
}