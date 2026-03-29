#include "Plugin/Plugin.h"

#include "Plugin/PluginKey.h"
#include "FTCore.h"
#include "FTDS/Static/HashMap.h"
#include "Renderer/FTRectArea.h"

class FoxtrotEditor :
	public Plugin
{
public:
	virtual void SaveProperties() override;
	virtual void LoadProperties() override;

public:
	void Initialize() override;

public:
	FoxtrotEditor(FTCore* base);
	~FoxtrotEditor() override;

private:
};

void FoxtrotEditor::Initialize()
{
	FTRectArea* rndArea = DBG_NEW FTRectArea(0.f, 0.f, 500.f, 500.f);
	gBase->CallFunc<CREATE_D3D11_WINDOW>(PluginKey::D3D11::D3D11, PluginKey::D3D11::CREATE_D3D11_WINDOW, "Hello", 500, 500, rndArea);
}

void FoxtrotEditor::SaveProperties()
{
}

void FoxtrotEditor::LoadProperties()
{
}

FoxtrotEditor::FoxtrotEditor(FTCore* base)
	: Plugin(base)
{
}

FoxtrotEditor::~FoxtrotEditor()
{
}
