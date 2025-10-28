#pragma once
#include "Components/TileMapRenderer.h"

#include <directxtk/SpriteFont.h>

#include "ResourceSystem/FTFontMap.h"
#include "ResourceSystem/TextAttribute.h"

class FTFont;

class TextRenderer :
	public MeshRenderer
{
public:
	virtual FTDS::String GetName() const override;

public:
	void UpdateText(FTDS::String&& text, FoxtrotRenderer* renderer);

public:
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	TextRenderer(
		Actor* owner,
		int	   UpdateOrder);
	virtual void CloneTo(Actor* actor) override;

private:
	FTDS::String mText;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	// void UpdateTextAttribute();
	void EditorRender(FoxtrotRenderer* renderer) override;
	void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace TextRenderer
	{
		constexpr const char* TEXT			= "Text";
		constexpr const char* TEXT_OFFSET	= "Offset";
		constexpr const char* TEXT_ROTATION = "Rotation";
		constexpr const char* TEXT_ORIGIN	= "Origin";
		constexpr const char* TEXT_SCALE	= "Scale";
		constexpr const char* TEXT_COLOR	= "Color";
	} // namespace TextRenderer
} // namespace ChunkKey