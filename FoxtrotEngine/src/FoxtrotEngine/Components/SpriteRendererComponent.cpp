#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Scenes/Scene.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Physics/Bounds.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/D3D11Utils.h"

void SpriteRendererComponent::SetTexture()
{
	MeshRendererComponent::SetTexture();
	mTexWidth = GetMeshGroup()->GetTexture()->GetTexWidth();
	mTexHeight = GetMeshGroup()->GetTexture()->GetTexHeight();
}

void SpriteRendererComponent::UpdateTexture(FoxtrotRenderer* renderer, std::string fileName){
	//std::vector<Mesh**>& meshes = renderer->GetRenderPool();
	//std::vector<Mesh**>::iterator iter = std::find(meshes.begin(), meshes.end(), GetMeshArray());
	//if (iter != meshes.end())
	//{
	//	delete (*iter);
	//	meshes.erase(iter);

	//	//FTTexture* texture = (*iter)->texture;
	//	//ResourceManager::GetInstance()->UpdateTexture(renderer, texture, channels);
	//	//mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	//}
}

void SpriteRendererComponent::Initialize(FTCore* coreInstance){
	MeshRendererComponent::Initialize(coreInstance);
	InitializeMesh();
	//InitializeMesh(GeometryGenerator::MakeSquare(1.0f));
}


void SpriteRendererComponent::Update(float deltaTime){
	if(GetMeshGroup())
		MeshRendererComponent::Update(deltaTime);
}

void SpriteRendererComponent::Render(FoxtrotRenderer* renderer) {
	MeshRendererComponent::Render(renderer);
}

bool SpriteRendererComponent::InitializeMesh()
{
	MeshRendererComponent::InitializeMesh(MAPKEY_SPRITE_MESH);
	if (!GetMeshGroup()) {
		LogString("ERROR: SpriteRendererComponent::InitializeMesh() -> Mesh Init failed");
		return false;
	}
	return true;
}


//void SpriteRendererComponent::InitializeMesh()
//{
//	mMesh = new Mesh;
//	MeshData squareMeshData = GeometryGenerator::MakeSquare(mScale);
//	D3D11Utils::CreateVertexBuffer(
//		mRenderer->GetDevice(),
//		squareMeshData.vertices,
//		mMesh->vertexBuffer
//	);
//
//	mMesh->mIndexCount = UINT(squareMeshData.indices.size());
//	mMesh->mVertexCount = squareMeshData.vertices.size();
//
//	D3D11Utils::CreateIndexBuffer(
//		mRenderer->GetDevice(),
//		squareMeshData.indices,
//		mMesh->indexBuffer
//	);
//}

#ifdef _DEBUG
#include "FoxtrotEditor/CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

//void SpriteRendererComponent::SetTexture()
//{
//	//rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
//	//rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
//	//rect->w = mTextWidth;
//	//rect->h = mTexHeight;
//}

void SpriteRendererComponent::EditorUpdate(float deltaTime){
	Update(deltaTime);
}

void SpriteRendererComponent::EditorUIUpdate(){
	if (GetRenderer())
	{
		UpdateSprite();
		ImGui::SeparatorText("Sprite Size");
		UpdateScale();
	}
}

void SpriteRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{
	MeshRendererComponent::EditorRender(renderer);
}

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder)
	, mChannel(4)
	, mScale(1.f)
{}

SpriteRendererComponent::~SpriteRendererComponent()
{}

void SpriteRendererComponent::OnConfirmUpdate()
{
	if (ImGui::Button("Update")) {
		this->SetTexture();
	}
}

void SpriteRendererComponent::UpdateSprite(){
	std::string currentSprite = "No sprite has been assigned";
	if (!GetTexKey().empty())
		currentSprite = "Current sprite : \n" + 
			ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite")){
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog("SelectSprite", "Select Sprite", TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select Sprite");
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL, ImGuiWindowFlags_MenuBar)) {
		std::unordered_map<std::string, FTTexture*>& texturesMap = ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			std::string spriteKey = {};
			static int selected = -1;
		    int i = 0;
			for (auto iter = texturesMap.begin(); iter != texturesMap.end(); ++iter, ++i)
			{
				if (ImGui::Selectable((*iter).first.c_str(), selected == i))
				{
					spriteKey = (*iter).first;
					selected = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1) {
				SetTexKey(spriteKey);
				SetTexture();
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void SpriteRendererComponent::UpdateTexWidth(){
	//CommandHistory::GetInstance()->UpdateIntValue("Texture Width", &GetMeshArray()[0]->texture->GetTexWidthRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateTexHeight(){
	//CommandHistory::GetInstance()->UpdateIntValue("Texture Height", &GetMeshArray()[0]->texture->GetTexHeightRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateScale(){
	CommandHistory::GetInstance()->UpdateFloatValue("Texture Scale", &mScale, FLOATMOD_SPEED);
}

void SpriteRendererComponent::SaveProperties(nlohmann::ordered_json& out){
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["TextureWidth"], mTexWidth);
	FileIOHelper::AddScalarValue(out["TextureHeight"], mTexHeight);
	FileIOHelper::AddScalarValue(out["Channel"], mChannel);
	FileIOHelper::AddScalarValue(out["Scale"], mScale);
}

void SpriteRendererComponent::LoadProperties(std::ifstream& ifs){
	Component::LoadProperties(ifs);
	//GetMeshArray()[0]->texture->SetTexWidth(FileIOHelper::LoadFloat(ifs));
	//GetMeshArray()[0]->texture->SetTexHeight(FileIOHelper::LoadFloat(ifs));

	//FileIOHelper::LoadFTTexture(ifs, GetMesh()->texture);
}
#endif