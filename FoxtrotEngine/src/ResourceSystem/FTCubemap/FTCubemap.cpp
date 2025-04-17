#include "FTCubemap.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "Managers/ResourceManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
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

	for (Mesh* mesh : Meshes())
	{
		GetVCData().model = modelMat.Transpose();
		GetVCData().view = viewMat.Transpose();
		GetVCData().projection = projMat.Transpose();
	}
}

void FTCubemap::SetTexture(UINT texKey)
{
	if (texKey == ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		printf("ERROR: MeshRenderer::SetTexture() -> TexKey not assigned.\n");
		return;
	}
	FTBasicMeshGroup::SetTexture(texKey, ResourceManager::GetInstance()->GetCubeMapTexturesMap());

	if (!GetTexture())
		printf("ERROR: MeshRenderer::SetTexture() -> Cannot set texture %d, returning nullptr.\n", texKey);
}

void FTCubemap::Initialize(std::vector<FTMeshData>&& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	// Rendered face should be headed inward.
	for (FTMeshData& meshData : meshes)
		std::reverse(meshData.Indices.begin(), meshData.Indices.end());

	StandardMaterial* mat = static_cast<StandardMaterial*>(ResourceManager::GetInstance()->GetLoadedMaterial(1));
	Materials().push_back(mat);
	FTBasicMeshGroup::Initialize(std::move(meshes), device, context);
}

void FTCubemap::Render(FoxtrotRenderer* renderer)
{
	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	for (const Mesh* mesh : Meshes())
	{
		context->VSSetConstantBuffers(
			0, mesh->VertexConstantBuffers.size(), mesh->VertexConstantBuffers.data()->GetAddressOf());

		SetVertexShader(renderer->GetCubeMapVS());
		SetPixelShader(renderer->GetCubeMapPS());

		if (GetTexture())
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(GetTexture()->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(GetVertexShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		context->PSSetShader(GetPixelShader().Get(), 0, 0);

		if (!Materials().empty())
		{
			context->PSSetConstantBuffers(
				0, mesh->PixelConstantBuffers.size(), mesh->PixelConstantBuffers.data()->GetAddressOf());
		}

		context->IASetInputLayout(renderer->GetTextureInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->OMSetBlendState(renderer->GetBlendState().Get(), NULL, D3D11_DEFAULT_SAMPLE_MASK);

		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
}

void FTCubemap::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes)
{
	Meshes().reserve(meshes.size());
	for (const FTMeshData& meshData : meshes)
	{
		Mesh* newMesh = DBG_NEW Mesh;
		newMesh->VertexCount = UINT(meshData.Vertices.size());
		newMesh->IndexCount = UINT(meshData.Indices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, newMesh->VertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.Indices, newMesh->IndexBuffer);

		this->Meshes().push_back(newMesh);
	}
}

#ifdef FOXTROT_EDITOR
void FTCubemap::UpdateUI()
{
	static UINT key;
	FTEditorUtils::DisplayResSelection("Select Texture", ResourceManager::GetInstance()->GetCubeMapTexturesMap(), key);
	if (key != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		this->SetTexture(key);
}
#endif // FOXTROT_EDITOR