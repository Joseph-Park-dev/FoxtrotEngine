// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <directxtk/SimpleMath.h>

#include "Renderer/D3D11Utils.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTResource.h"

class FTMaterial;
class FTVertexShader;
class FTMeshData;

/// @brief A class holding Meshes created from a FTMeshData.
/// This should be used in the MeshRenderer Component, and its derived Components.
/// Other elements forming a 3D model, such as Material and UV Texture, will be
/// combined with FTMeshGroup in those components.
class FTMeshGroup :
	public FTResource
{
public:
	/// @brief Renders the entire meshes created, as a full model.
	virtual void Render(
		FoxtrotRenderer* renderer,
		Transform*		 transform,
		Camera*			 camInst,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat);

public:
	/// @brief Set scale value to be multiplied with the scale (3D transformation).
	void SetSizeScale(const FTVector3 scale);

public:
	/// @brief Relative path is used for importing 3D files.
	FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer);
	FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer, FTMeshData* meshData);
	virtual ~FTMeshGroup();

protected:
	/// @brief Reads a file which returns the FTMeshData, and creates the meshes.
	virtual void Process(FoxtrotRenderer* renderer) override;

	/// @brief Receives an array of meshData and creates the mesh ready for rendering.
	/// @param meshData An array of meshData mostly read from a supported 3D file such as .fbx, or animation meshes
	/// @see GeometryGenerator::ReadFile()
	void Initialize(
		FTDS::DynamicArray<FTMeshData*>&& meshData,
		ComPtr<ID3D11Device>&			  device,
		ComPtr<ID3D11DeviceContext>&	  context);

	/// @brief Creates constant buffers such as Vertex Constant Buffers.
	void InitializeConstantBuffers(ComPtr<ID3D11Device>& device);

	/// @brief Create texture sampler.
	/// @todo Consider moving this to D3D11Utils class.
	HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);

	/// @brief Updates the constant buffers right before rendering.
	/// @param transform Transformation of the mesh, usually of the Actor.
	/// @param camInst Any camera instance in the .chunk.
	/// @param mat Material applied to this mesh model.
	/// @todo If the engine targets for 2D games, remove the inverse transpose calculation.
	virtual void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		Transform*					 transform,
		Camera*						 camInst,
		FTMaterial*					 mat);

	/// @brief Delete all created meshes.
	void Clear();

protected:
	/// @brief Returns meshes ready to be rendered.
	FTDS::DynamicArray<Mesh*>* Meshes();

	ComPtr<ID3D11SamplerState>& GetSamplerState();

	/// @brief Returns vertex constant buffer.
	ComPtr<ID3D11Buffer>& GetVCBuf();

	/// @brief Returns the size scale.
	const FTVector3& GetSizeScale() const;

private:
	/// @brief Horizontal direction this mesh is pointing to.
	/// This is used to flip sprites when changing their direction.
	int mDirection;

	/// @brief X, Y, Z value that will be multiplied to the scale(3D transformation).
	/// Default is (1, 1, 1)
	FTVector3 mSizeScale;

	/// @brief Meshes created from FTMeshData, ready to be rendered.
	FTDS::DynamicArray<Mesh*>* mMeshes;
	ComPtr<ID3D11SamplerState> mSamplerState;

	/// @brief Vertex constant buffer which will applied to all meshes.
	ComPtr<ID3D11Buffer> mVCBuf;

	/// @brief Vertex constant data.
	BasicVCData mVCData;

private:
	/// @brief Takes a FTMeshData (usually from GeometryGenerator), and creates the meshes.
	void Process(FoxtrotRenderer* renderer, FTMeshData* meshData);

	/// @brief Receives a meshData and creates the mesh ready for rendering.
	/// @param meshData A single meshData generated from GeometryGenerator.
	void Initialize(
		FTMeshData*					 meshData,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	/// @brief Creates a mesh from a single meshData.
	void InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData* meshData);

	/// @brief Creates the meshes from the array of meshData.
	/// This is usually called when importing a 3D model file.
	void InitializeMeshes(ComPtr<ID3D11Device>& device, FTDS::DynamicArray<FTMeshData*>&& meshDataArr);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();

#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace FTMeshGroup
	{
		constexpr const char* FT_MESH_GROUP = "FTMeshGroup";
		constexpr const char* MESH_KEY		= "Mesh Key";
		constexpr const char* TEXTURE_KEY	= "Texture Key";
		constexpr const char* SHADER_KEY	= "Shader Key";
		constexpr const char* VS_KEY		= "Vertex Shader Key";
		constexpr const char* PS_KEY		= "Pixel Shader Key";
		constexpr const char* MAT_KEY		= "Material Key";

		constexpr const char* DRAW_TEXTURE = "Draw Texture";
		constexpr const char* DRAW_NORMALS = "Draw Normals";
	} // namespace FTMeshGroup
} // namespace ChunkKey