#include "Components/UIs/TextRenderer.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "ResourceSystem/FTFont/FTFont.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Managers/ResourceManager.h"

#include "FileSystem/BufferSizes.h"

#ifdef FOXTROT_EDITOR
	#include "DirectoryHelper.h"
	#include "EditorUtils.h"
#endif

FTDS::String TextRenderer::GetName() const
{
	return "TextRenderer";
}

void TextRenderer::UpdateText(FTDS::String&& text, FoxtrotRenderer* renderer)
{
	if (mText.NotEqual(text.C_Str()))
	{
		static_cast<FTFont*>(GetMeshGroup())->AddText(renderer->GetDevice(), text);
		mText.Assign(text);
	}
}

void TextRenderer::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		FTVector3 pos	= GetOwner()->GetTransform()->GetWorldPosition();
		FTVector3 scale = GetOwner()->GetTransform()->GetWorldScale();
		static_cast<FTFont*>(GetMeshGroup())->UpdateTextVertices(mText, renderer, pos, scale, FTVector2::Zero, FTVector3::Zero, 1.f);
		static_cast<FTFont*>(GetMeshGroup())->Render(mText, renderer, GetOwner()->GetTransform(), Camera::GetInstance(), GetTexture(), GetVS(), GetPS(), GetMaterial());
	}
}

void TextRenderer::CloneTo(Actor* actor)
{
	TextRenderer* newComp = DBG_NEW TextRenderer(actor, GetUpdateOrder());
	newComp->mText		  = this->mText;
}

TextRenderer::TextRenderer(Actor* owner, int UpdateOrder)
	: MeshRenderer(owner, UpdateOrder)
{
}

void TextRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
}

void TextRenderer::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void TextRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	this->Render(renderer);
}

void TextRenderer::EditorUIUpdate()
{
	MeshRenderer::EditorUIUpdate();

	static FTDS::String text;
	CommandHistory::GetInstance()->UpdateStringValue("Text", text);
	ImGui::SameLine();
	if (ImGui::Button("Update"))
		UpdateText(std::move(text), GetRenderer());

	static FTFont* font = nullptr;
	FTEditorUtils::DisplayResSelection("Select Font", ResourceManager::GetInstance()->GetFonts(), font);

	if (font)
	{
		SetMeshGroup(font);
		SetTexture(font->GetFontImage());
	}

	if (GetMeshGroup())
		static_cast<FTFont*>(GetMeshGroup())->UpdateUI();
}

//
// void TextRenderer::UpdateTextAttribute()
//{
//	float rotation	= mTextAttribute->Rotation;
//	float offset[2] = { mTextAttribute->Offset.x, mTextAttribute->Offset.y };
//	float origin[2] = { mTextAttribute->Origin.x, mTextAttribute->Origin.y };
//	float scale[2]	= { mTextAttribute->Scale.x, mTextAttribute->Scale.y };
//	float color[4]	= { mTextAttribute->Color.x, mTextAttribute->Color.y, mTextAttribute->Color.z, mTextAttribute->Color.w };
//
//	ImGui::DragFloat2(ChunkKey::TextRenderer::TEXT_OFFSET, offset, FLOATMOD_SPEED);
//	ImGui::DragFloat2(ChunkKey::TextRenderer::TEXT_ORIGIN, origin, FLOATMOD_SPEED);
//	ImGui::DragFloat2(ChunkKey::TextRenderer::TEXT_SCALE, scale, FLOATMOD_SPEED);
//	ImGui::ColorPicker4(ChunkKey::TextRenderer::TEXT_COLOR, color, FLOATMOD_SPEED);
//
//	mTextAttribute->Rotation = rotation;
//	mTextAttribute->Offset	 = DirectX::SimpleMath::Vector2(offset[0], offset[1]);
//	mTextAttribute->Origin	 = DirectX::SimpleMath::Vector2(origin[0], origin[1]);
//	mTextAttribute->Scale	 = DirectX::SimpleMath::Vector2(scale[0], scale[1]);
//	mTextAttribute->Color	 = DirectX::SimpleMath::Vector4(color[0], color[1], color[2], color[3]);
//}
#endif // FOXTROT_EDITOR