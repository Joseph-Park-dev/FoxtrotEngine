#pragma once
#include "Components/TileMapRenderer.h"

#include "ResourceSystem/FTFontMap.h"
#include "ResourceSystem/TextAttribute.h"

class TextRenderer :
    public TileMapRenderer
{
public:
    virtual std::string GetName() const override;

public:
    virtual void Initialize (FTCore* coreInstance)      override;
    virtual void Render     (FoxtrotRenderer* renderer) override;
    virtual void CloneTo    (Actor* actor)              override;

public:
    TextRenderer(
        Actor* owner,
        int drawOrder = DEFAULT_DRAWORDER,
        int UpdateOrder = DEFAULT_UPDATEORDER
    );
    ~TextRenderer() override;

protected:
    virtual void InitializeTileMap() override;

private:
    std::wstring   mText;
    TextAttribute* mTextAttribute;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;
};

