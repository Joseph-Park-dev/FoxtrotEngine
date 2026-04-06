// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/D3D11Resource.h"

#include <wrl.h>
#include <d3d11.h>

#include "Math/FTMath.h"

namespace Core
{
	class Transform;
}

namespace D3D11
{
	class FTMaterial;
	class FTVertexShader;
	class D3D11Renderer;
	class FTGeometryShader;
	class FTPixelShader;
	class D3D11PSO;
	class Camera;
	struct Mesh;
	struct FTMeshData;
	struct PointModelMat;

	/// @brief A class holding Meshes created from a FTMeshData.
	/// This should be used in the MeshRenderer Component, and its derived Components.
	/// Other elements forming a 3D model, such as Material and UV Texture, will be
	/// combined with FTMeshGroup in those components.
	class FTMeshGroup :
		public D3D11Resource
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Renders the entire meshes created, as a full model.
		virtual void Render(
			D3D11Renderer*	 renderer,
			Core::Transform* transform,
			Camera*			 camInst,
			D3D11PSO*		 pso,
			FTMaterial*		 mat);

		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override {};

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override {};

	public:
		/// @brief Set scale value to be multiplied with the scale (3D transformation).
		void SetSizeScale(const Math::FTVector3 scale);

		/// @brief Returns the initial front horizontal direction.
		const int GetFrontDir() const;

		/// @brief Set as the model is initially facing the right side of the screen?
		/// @param val If this is true, it means yes.
		void SetRightIsFront(bool val);

		/// @brief Returns meshes ready to be rendered.
		Core::FTDS::DynamicArray<Mesh*>* Meshes();

	public:
		/// @brief Relative path is used for importing 3D files.
		// This is unused until 3D feature is implemented.
		// FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer);
		FTMeshGroup(Core::FTResourceDef& resDef, D3D11Renderer* renderer, FTMeshData* meshData = nullptr);
		virtual ~FTMeshGroup();

	protected:
		/// @brief Reads a file which returns the FTMeshData, and creates the meshes.
		// This is unused untill 3D feature is implemented.
		// virtual void Process(FoxtrotRenderer* renderer) override;

		/// @brief Creates constant buffers such as Vertex Constant Buffers.
		virtual void InitializeConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/// @brief Create texture sampler.
		/// @todo Consider moving this to D3D11Utils class.
		HRESULT CreateTextureSampler(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/// @brief Updates the constant buffers right before rendering.
		/// @param transform Transformation of the mesh, usually of the Actor.
		/// @param camInst Any camera instance in the .chunk.
		/// @param mat Material applied to this mesh model.
		/// @param frontDir Initial front direction the model is facing toward.
		/// @todo If the engine targets for 2D games, remove the inverse transpose calculation.
		virtual void UpdateConstantBuffers(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			Core::Transform*							 transform,
			Camera*										 camInst,
			FTMaterial*									 mat,
			const int									 frontDir = 0);

		/// @brief Delete all created meshes.
		void Clear();

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState();

		/// @brief Returns vertex constant buffer.
		Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVCBuf();

		PointModelMat* GetVCData();

		/// @brief Returns the size scale.
		const Math::FTVector3& GetSizeScale() const;

		/// @brief Returns the mesh direction.
		const int GetDirection() const;

		/// @brief Set the mesh direction.
		void SetDirection(int dir);

	private:
		/// @brief Initial horizontal direction the mesh is pointing toward.
		/// This is used to flip sprites when changing their direction.
		int mFrontDir;

		/// @brief Initial horizontal direction the mesh is pointing to.
		int mDirection;

		/// @brief X, Y, Z value that will be multiplied to the scale(3D transformation).
		/// Default is (1, 1, 1)
		Math::FTVector3 mSizeScale;

		/// @brief Meshes created from FTMeshData, ready to be rendered.
		Core::FTDS::DynamicArray<Mesh*>*		   mMeshes;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;

		/// @brief Vertex constant buffer which will applied to all meshes.
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVCBuf;

		/// @brief Vertex constant data.
		PointModelMat* mVCData;

	private:
		/// @brief Takes a FTMeshData (usually from GeometryGenerator), and creates the meshes.
		void Process(D3D11Renderer* renderer, FTMeshData* meshData);

		/// @brief Creates a mesh from a single meshData.
		void InitializeMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTMeshData* meshData);

		/// @brief Creates the meshes from the array of meshData.
		/// This is usually called when importing a 3D model file.
		void InitializeMeshes(Microsoft::WRL::ComPtr<ID3D11Device>& device, Core::FTDS::DynamicArray<FTMeshData*>&& meshDataArr);

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
			constexpr const char* FRONT_DIR		= "Front Direction";
			constexpr const char* SIZE_SCALE	= "Size Scale";

			constexpr const char* DRAW_TEXTURE = "Draw Texture";
			constexpr const char* DRAW_NORMALS = "Draw Normals";
		} // namespace FTMeshGroup
	} // namespace ChunkKey
} // namespace D3D11