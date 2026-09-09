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

	class FTShape
	{
	public:
		/// @brief Returns the vcdata used by this ftshape.
		/// @return Borrowed access to the vcdata.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API DebugVCData& GetVCData();
		/// @brief Returns the gscdata used by this ftshape.
		/// @return Borrowed access to the gscdata.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API DebugGCData& GetGSCData();
		/// @brief Returns the pixel constant data used by this ftshape.
		/// @return Borrowed access to the pixel constant data.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API DebugPCData& GetPixelConstantData();

		/// @brief Returns the mesh used by this ftshape.
		/// @return Borrowed access to the mesh.
		D3D11_API Mesh* GetMesh();
		/// @brief Returns the is active used by this ftshape.
		/// @return Current value of the is active flag.
		bool  GetIsActive() { return mIsActive; }

		/// @brief Updates the is active used by subsequent operations.
		/// @param val Replacement is active.
		void SetIsActive(bool val) { mIsActive = val; }

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API virtual void Initialize(D3D11Renderer* renderer);
		/// @brief Uploads vertex-shader constant data.
		/// @param model Model resource associated with this object.
		/// @param camInst Camera supplying the view and projection for this draw.
		D3D11_API void		 UpdateVC(Math::FTMatrix4& model, Core::ICamera* camInst);
		/// @brief Uploads geometry-shader constant data.
		/// @param camInst Camera supplying the view and projection for this draw.
		D3D11_API void		 UpdateGC(Core::ICamera* camInst);
		/// @brief Uploads pixel-shader constant data.
		D3D11_API virtual void UpdatePC();

		// This is for ShapeActors (e.g. SquareActor)
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void Render(D3D11Renderer* renderer);
		// This should be called in DebugShapes instance only once per frame.
		// You don't have to use this member function by yourself.
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param vertexShader Vertex shader used by the pipeline.
		/// @param pixelShader Pixel shader used by the pipeline.
		/// @param inputLayout Direct3D vertex input layout.
		D3D11_API void Render(
			D3D11Renderer*								renderer,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
			Microsoft::WRL::ComPtr<ID3D11PixelShader>&	pixelShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	inputLayout);

	public:
		/// @brief Initializes shape geometry and its rendering state.
		D3D11_API FTShape();
		/// @brief Releases the resources managed by this instance during destruction.
		D3D11_API virtual ~FTShape();

	protected:
		/// @brief Builds or binds the mesh resources required by the renderer.
		/// @param device Direct3D device used to create GPU resources.
		/// @param meshData CPU-side mesh vertices and indices.
		D3D11_API void InitializeMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTDebugMeshData&& meshData);

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
		/// @brief Allocates and initializes the GPU constant buffer used by this object.
		/// @param device Direct3D device used to create GPU resources.
		D3D11_API void InitializeConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device);
		/// @brief Uploads the current shader parameters to the constant buffers.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		D3D11_API void UpdateConstantBuffers(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

		/// @brief Recomputes the model transform used for rendering.
		/// @param pos Position or zero-based insertion index.
		/// @param rot Rotation used by the operation.
		/// @param size Number of elements or bytes required by the operation.
		D3D11_API void UpdateModelMatrix(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 size);
		/// @brief Recomputes the camera view matrix from its position and orientation.
		/// @param camInst Camera supplying the view and projection for this draw.
		D3D11_API void UpdateViewMatrix(Core::ICamera* camInst);
		/// @brief Recomputes the camera projection from its current lens and viewport settings.
		/// @param camInst Camera supplying the view and projection for this draw.
		D3D11_API void UpdateProjectionMatrix(Core::ICamera* camInst);
	};

	namespace ChunkKey
	{
		constexpr const char* FTSHAPE_IS_ACTIVE = "Is Active";
	}
} // namespace D3D11
