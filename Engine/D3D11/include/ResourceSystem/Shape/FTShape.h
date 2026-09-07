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
#include "Plugin/D3D11Exports.h"
#include <d3d11.h>
#include <wrl.h>

#include "FTMath.h"
#include "../Mesh/MeshConstantData.h"

namespace Core
{
	class Transform;
} // namespace Core

namespace D3D11
{
	class FTRectArea;
	struct FTDebugMeshData;
	struct Mesh;
	struct FTMeshData;
	class D3D11Renderer;

	class D3D11_API FTShape
	{
	public:
		DebugVCData& GetVCData();
		DebugGCData& GetGSCData();
		DebugPCData& GetPixelConstantData();

		Mesh* GetMesh();
		bool  GetIsActive() { return mIsActive; }

		void SetIsActive(bool val) { mIsActive = val; }

	public:
		virtual void Initialize(D3D11Renderer* renderer);
		void		 UpdateVC(Math::FTMatrix4& model, Core::ICamera* camInst);
		void		 UpdateGC(Core::ICamera* camInst);
		virtual void UpdatePC();

		// This is for ShapeActors (e.g. SquareActor)
		void Render(D3D11Renderer* renderer);
		// This should be called in DebugShapes instance only once per frame.
		// You don't have to use this member function by yourself.
		void Render(
			D3D11Renderer*								renderer,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
			Microsoft::WRL::ComPtr<ID3D11PixelShader>&	pixelShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	inputLayout);

	public:
		FTShape();
		virtual ~FTShape();

	protected:
		void InitializeMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTDebugMeshData&& meshData);

	private:
		Mesh* mMesh;
		bool  mIsActive;

	private:
		DebugVCData mVSCData;
		DebugGCData mGSCData;
		DebugPCData mPSCData;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mGSCBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSCBuf;

	private:
		void InitializeConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device);
		void UpdateConstantBuffers(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

		void UpdateModelMatrix(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 size);
		void UpdateViewMatrix(Core::ICamera* camInst);
		void UpdateProjectionMatrix(Core::ICamera* camInst);
	};

	namespace ChunkKey
	{
		constexpr const char* FTSHAPE_IS_ACTIVE = "Is Active";
	}
} // namespace D3D11