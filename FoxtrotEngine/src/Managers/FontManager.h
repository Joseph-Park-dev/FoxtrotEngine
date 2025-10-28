#pragma once
#include "Core/SingletonMacro.h"

class FoxtrotRenderer;

class FontManager
{
	SINGLETON(FontManager);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);

private:
	void CreateFontMetaGUI();

#endif
};
