// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
///
/// </summary>

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
class FTMeshGroup : public FTResource
{
public:
	/// @brief Receives meshData and creates the mesh ready for rendering.
	/// @param meshData A single meshData generated from GeometryGenerator.
	void Initialize(
		FTMeshData*					 meshData,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	/// @brief Receives meshData and creates the mesh ready for rendering.
	/// @param meshData An array of meshData mostly read from a supported 3D file such as .fbx
	/// @see GeometryGenerator::ReadFile()
	void Initialize(
		FTDS::DynamicArray<FTMeshData*>&& meshData,
		ComPtr<ID3D11Device>&			  device,
		ComPtr<ID3D11DeviceContext>&	  context);

	/// @brief Renders the entire meshes created, as a full model.
	virtual void Render(
		FoxtrotRenderer* renderer,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat);

	/// @brief Renders a single mesh from created meshes. Useful for SpriteAnimation.
	/// @param meshIndex Index of a mesh to be rendered.
	virtual void Render(
		int				 meshIndex,
		FoxtrotRenderer* renderer,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat);

public:
	FTMeshGroup(FTResourceDef& resDef);
	virtual ~FTMeshGroup();

protected:
	/// @brief Creates a mesh from a single meshData.
	virtual void	InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData* meshData);

	/// @brief Creates the meshes from the array of meshData.
	/// This is usually called when importing a 3D model file.
	virtual void	InitializeMeshes(ComPtr<ID3D11Device>& device, FTDS::DynamicArray<FTMeshData*>&& meshDataArr);

	/// @brief Creates constant buffers such as Vertex Constant Buffers.
	virtual void	InitializeConstantBuffers(ComPtr<ID3D11Device>& device);

	/// @brief Create texture sampler.
	/// @todo Consider moving this to D3D11Utils class.
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);

	/// @brief Reads a file, takes the FTMeshData, creates the meshes.
	virtual void Process(FoxtrotRenderer* renderer) override;

	/// @brief Delete all created meshes.
	void Clear();

protected:
	/// @brief Get meshes ready to be rendered.
	FTDS::DynamicArray<Mesh*>* Meshes();

	ComPtr<ID3D11SamplerState>& GetSamplerState();

	/// @brief Get vertex constant buffer.
	ComPtr<ID3D11Buffer>&		GetVCBuf();

private:
	/// @brief Horizontal direction this mesh is pointing to.
	/// This is used to flip sprites when changing their direction.
	int mDirection;

	/// @brief Meshes created from FTMeshData, ready to be rendered.
	FTDS::DynamicArray<Mesh*>* mMeshes;
	ComPtr<ID3D11SamplerState> mSamplerState;

	/// @brief Vertex constant buffer which will applied to all meshes.
	ComPtr<ID3D11Buffer> mVCBuf;

	/// @brief Vertex constant data.
	BasicVCData			 mVCData;

private:
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
};

namespace ChunkKey
{
	namespace FTMeshGroup
	{
		constexpr const char* MESH_KEY	  = "Mesh Key";
		constexpr const char* TEXTURE_KEY = "Texture Key";
		constexpr const char* SHADER_KEY  = "Shader Key";
		constexpr const char* VS_KEY	  = "Vertex Shader Key";
		constexpr const char* PS_KEY	  = "Pixel Shader Key";
		constexpr const char* MAT_KEY	  = "Material Key";

		constexpr const char* DRAW_TEXTURE = "Draw Texture";
		constexpr const char* DRAW_NORMALS = "Draw Normals";
	} // namespace FTMeshGroup
} // namespace ChunkKey