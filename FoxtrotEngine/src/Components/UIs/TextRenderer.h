#pragma once
#include "Components/TileMapRenderer.h"

#include <directxtk/SpriteFont.h>

#include "ResourceSystem/FTFontMap.h"
#include "ResourceSystem/TextAttribute.h"

class TextRenderer :
	public Component
{
public:
	virtual FTDS::String GetName() const override;

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	TextRenderer(
		Actor* owner,
		int	   UpdateOrder);
	~TextRenderer() override;
	virtual void CloneTo(Actor* actor) override;

private:
	FTDS::String	   mText;
	TextAttribute* mTextAttribute;

	DirectX::SpriteBatch* spriteBatch;
	DirectX::SpriteFont*  spriteFont;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;

	void UpdateText();
	void UpdateTextAttribute();
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
	}
} // namespace ChunkKey