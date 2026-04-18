#pragma once
#include "Core/Singleton.h"

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
