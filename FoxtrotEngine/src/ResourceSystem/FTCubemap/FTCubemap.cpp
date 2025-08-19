#include "FTCubemap.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "Managers/ResourceManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
#include "EditorResourceManager.h"
#endif

void FTCubemap::CalcVCData(Camera* camInst)
{
	// Model Transformation
	Matrix modelMat = Matrix();

	// View Transformation
	Matrix&& viewMat = camInst->GetViewRow();
	Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

	// Project Transformation
	Matrix&& projMat = std::move(camInst->GetProjRow());
}

//void FTCubemap::Initialize(std::vector<FTMeshData>&& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
//{
//	// Rendered face should be headed inward.
//	for (FTMeshData& meshData : meshes)
//		std::reverse(meshData.Indices.begin(), meshData.Indices.end());
//
//	//FTBasicMeshGroup::Initialize(std::move(meshes), device, context);
//
//	std::vector<FTDS::String> matKey = { ChunkKey::STANDARD_MAT };
//}

//void FTCubemap::Render(FoxtrotRenderer* renderer)
//{
//	UINT						 stride	 = sizeof(Vertex);
//	UINT						 offset	 = 0;
//	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	//for (const Mesh* mesh : Meshes())
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

ComPtr<ID3D11ShaderResourceView>& FTCubemap::GetDiffuseResView() { return mDiffuseResView; }
ComPtr<ID3D11ShaderResourceView>& FTCubemap::GetSpecularResView() { return mSpecularResView; }

void FTCubemap::SetDiffuseTexture(FTDS::String& key)
{
	FTTexture* tex = ResourceManager::GetInstance()->GetLoadedTexture(key);
	mDiffuseResView = tex->GetResourceView();
}

void FTCubemap::SetSpecularTexture(FTDS::String& key)
{
	FTTexture* tex = ResourceManager::GetInstance()->GetLoadedTexture(key);
	mSpecularResView = tex->GetResourceView();
}

//void FTCubemap::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes)
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
//}

void FTCubemap::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CubeMap::FTCubeMap);
	FTBasicMeshGroup::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::CubeMap::FTCubeMap);
}

void FTCubemap::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs);
	FTBasicMeshGroup::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTCubemap::UpdateUI()
{
	/*static const char* diffuseKey;
	FTEditorUtils::DisplayResSelection("Select Diffuse Texture", EditorResourceManager::GetInstance()->GetTexturesMap(), diffuseKey);
	if (diffuseKey != ChunkKey::NullVal::NULL_OBJECT)
		this->SetDiffuseTexture(diffuseKey);

	static const char* specularKey;
	FTEditorUtils::DisplayResSelection("Select Specular Texture", EditorResourceManager::GetInstance()->GetTexturesMap(), specularKey);
	if (specularKey != ChunkKey::NullVal::NULL_OBJECT)
		this->SetSpecularTexture(specularKey);*/
}
#endif // FOXTROT_EDITOR