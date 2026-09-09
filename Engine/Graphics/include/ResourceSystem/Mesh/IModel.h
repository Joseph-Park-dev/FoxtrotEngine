// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

#include "MeshConstantData.h"

namespace Core
{
	class Transform;
} // namespace Core

namespace Graphics
{
	class ICamera;
	class IRenderer;
	class FTMaterial;
	class D3D11PSO;
	struct Mesh;
	struct PointModelMat;

	struct ModelData :
		Common::ResourceData
	{
		/// @brief Initial horizontal direction the mesh is pointing toward.
		/// This is used to flip sprites when changing their direction.
		int FrontDir = 1;

		/// @brief Initial horizontal direction the mesh is pointing to.
		int Direction = 1;

		/// @brief X, Y, Z value that will be multiplied to the scale(3D transformation).
		/// Default is (1, 1, 1)
		Math::FTVector3 SizeScale = Math::FTVector3(1, 1, 1);

		/// @brief Meshes created from FTMeshData, ready to be rendered.
		Common::FTDS::DynamicArray<Mesh*> Meshes;

		/// @brief Vertex constant data.
		Graphics::PointModelMat VCData;
	};

	/// @brief A class holding Meshes created from a FTMeshData.
	/// This should be used in the MeshRenderer Component, and its derived Components.
	/// Other elements forming a 3D model, such as Material and UV Texture, will be
	/// combined with FTMeshGroup in those components.
	class IModel :
		public Common::IResource
	{
		////////////////
		/// Gameloop ///
		////////////////
	public:
		/// @brief Renders the entire meshes created, as a full model.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param pso Pipeline state object used for rendering.
		/// @param mat Matrix or material used by this operation.
		virtual void Render(
			Graphics::IRenderer* renderer,
			Core::Transform*	 transform,
			Graphics::ICamera*	 camInst,
			D3D11PSO*			 pso,
			FTMaterial*			 mat) = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IModel() override = 0;
	};

	namespace ChunkKey
	{
		namespace MeshGroup
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
		} // namespace MeshGroup
	} // namespace ChunkKey
} // namespace Graphics
