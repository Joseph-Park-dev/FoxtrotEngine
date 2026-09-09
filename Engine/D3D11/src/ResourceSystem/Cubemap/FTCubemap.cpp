#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "ResourceSystem/Cubemap/FTCubemap.h"

#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "Manager/ResourceManager.h"
#include "Renderer/IRenderer.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif

namespace D3D11
{

	// void FTCubemap::Initialize(std::vector<FTMeshData>&& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	//{
	//	// Rendered face should be headed inward.
	//	for (FTMeshData& meshData : meshes)
	//		std::reverse(meshData.Indices.begin(), meshData.Indices.end());
	//
	//	//FTMeshGroup::Initialize(std::move(meshes), device, context);
	//
	//	std::vector<Common::FTDS::String> matKey = { ChunkKey::STANDARD_MAT };
	// }

	// void FTCubemap::Render(FoxtrotRenderer* renderer)
	//{
	//	UINT						 stride	 = sizeof(Vertex);
	//	UINT						 offset	 = 0;
	//	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	// for (const Mesh* mesh : Meshes())
	//{
	//	context->VSSetConstantBuffers(
	//		0, mesh->VertexConstantBuffers.size(), mesh->VertexConstantBuffers.data()->GetAddressOf());

	//	/*SetVertexShader(renderer->GetCubeMapVS());
	//	SetPixelShader(renderer->GetCubeMapPS());*/

	//	if (mDiffuseResView && mSpecularResView)
	//	{
	//		ID3D11ShaderResourceView* resViews[2] = {
	//			mDiffuseResView.Get(),
	//			mSpecularResView.Get()
	//		};
	//		context->PSSetShaderResources(0, 2, resViews);
	//	}

	//	context->VSSetShader(GetVertexShader()->GetShader().Get(), 0, 0);
	//	context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
	//	context->PSSetShader(GetPixelShader()->GetShader().Get(), 0, 0);

	//	if (!Materials().empty())
	//	{
	//		context->PSSetConstantBuffers(
	//			0, mesh->PixelConstantBuffers.size(), mesh->PixelConstantBuffers.data()->GetAddressOf());
	//	}

	//	context->IASetInputLayout(renderer->GetTextureInputLayout().Get());
	//	context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	//	context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	context->OMSetBlendState(renderer->GetBlendState().Get(), NULL, D3D11_DEFAULT_SAMPLE_MASK);

	//	context->DrawIndexed(mesh->IndexCount, 0, 0);
	//}
	//}

	/// @brief Returns the diffuse res view used by this ftcubemap.
	/// @return Borrowed access to the diffuse res view.
	/// @note Changes through the returned reference affect this object's stored state.
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& FTCubemap::GetDiffuseResView() { return mDiffuseResView; }
	/// @brief Returns the specular res view used by this ftcubemap.
	/// @return Borrowed access to the specular res view.
	/// @note Changes through the returned reference affect this object's stored state.
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& FTCubemap::GetSpecularResView() { return mSpecularResView; }

	/// @brief Updates the diffuse texture used by subsequent operations.
	/// @param key Replacement diffuse texture.
	void FTCubemap::SetDiffuseTexture(Common::FTDS::String& key)
	{
		auto* sprite = ResourceManager::GetInstance()->GetSprite(key);
		FTTexture* tex = sprite ? sprite->GetTexture() : nullptr;
		mDiffuseResView = tex ? tex->GetSRV() : nullptr;
	}

	/// @brief Updates the specular texture used by subsequent operations.
	/// @param key Replacement specular texture.
	void FTCubemap::SetSpecularTexture(Common::FTDS::String& key)
	{
		auto* sprite = ResourceManager::GetInstance()->GetSprite(key);
		FTTexture* tex = sprite ? sprite->GetTexture() : nullptr;
		mSpecularResView = tex ? tex->GetSRV() : nullptr;
	}

	// void FTCubemap::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes)
	//{
	//	//Meshes().reserve(meshes.size());
	//	for (const FTMeshData& meshData : meshes)
	//	{
	//		Mesh* newMesh = DBG_NEW Mesh;
	//		newMesh->VertexCount = UINT(meshData.Vertices.size());
	//		newMesh->IndexCount = UINT(meshData.Indices.size());
	//
	//		D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, newMesh->VertexBuffer);
	//		D3D11Utils::CreateIndexBuffer(device, meshData.Indices, newMesh->IndexBuffer);
	//
	//		//this->Meshes().push_back(newMesh);
	//	}
	// }

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void FTCubemap::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CubeMap::FTCubeMap);
		FTMeshGroup::SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::CubeMap::FTCubeMap);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void FTCubemap::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs);
		FTMeshGroup::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	void FTCubemap::UpdateUI()
	{
		/*static const char* diffuseKey;
		FTEditorUtils::DisplayResSelection("Select Diffuse Texture", EditorResourceManager::GetInstance()->GetTexturesMap(), diffuseKey);
		if (diffuseKey != ::ChunkKey::NullVal::NULL_OBJECT)
			this->SetDiffuseTexture(diffuseKey);

		static const char* specularKey;
		FTEditorUtils::DisplayResSelection("Select Specular Texture", EditorResourceManager::GetInstance()->GetTexturesMap(), specularKey);
		if (specularKey != ::ChunkKey::NullVal::NULL_OBJECT)
			this->SetSpecularTexture(specularKey);*/
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
