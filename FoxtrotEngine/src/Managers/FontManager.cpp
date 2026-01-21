#include "FontManager.h"

#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTFont/FTFont.h"
#include "ResourceSystem/FTSprite/FTSprite.h"

#include "EditorUtils.h"

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

#ifdef FOXTROT_EDITOR
void FontManager::UpdateUI(bool* opened)
{
	if (!ImGui::Begin("Font Manager", opened))
	{
		ImGui::End();
	}
	else
	{
		CreateFontMetaGUI();
		ImGui::End();
	}
}

void FontManager::CreateFontMetaGUI()
{
	static FTSprite* tex;
	static FTFont*	 font;

	FTEditorUtils::DisplayResSelection("Select Font Image", ResourceManager::GetInstance()->GetSprites(), tex);
	FTEditorUtils::DisplayResSelection("Select Font Instance", ResourceManager::GetInstance()->GetFonts(), font);

	if (tex)
	{
		ImGui::Text("Selected Texture : %s", tex->GetFileName());
		tex->UpdateUI();
	}

	if (font)
		ImGui::Text("Selected Font : %s", font->GetFileName());

	if (ImGui::Button("Save Font Meta"))
	{
		font->SetFontImage(tex->GetTexture());

		FTDS::String metaPath = font->GetRelativePath();

		metaPath.SubStr(0, metaPath.RFind(FileTypes::FONT));
		metaPath.Append(FileTypes::FONT_META);

		std::ofstream ofs(metaPath.C_Str());
		font->SaveProperties(ofs);
		FileIOHelper::SaveBufferToFile(ofs);

		font->LoadFont(tex->GetTexture());
		tex	 = nullptr;
		font = nullptr;
	}
}
#endif
