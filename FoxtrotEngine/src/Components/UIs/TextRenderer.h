#pragma once
#include "Components/TileMapRenderer.h"

#include <directxtk/SpriteFont.h>

#include "ResourceSystem/FTFontMap.h"
#include "ResourceSystem/TextAttribute.h"

class TextRenderer :
	public Component
{
public:
	virtual std::string GetName() const override;

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
	std::string	   mText;
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
	constexpr const char* TEXT	 = "Text";
	constexpr const char* ORIGIN = "Origin";
	constexpr const char* COLOR = "Color";
} // namespace ChunkKey