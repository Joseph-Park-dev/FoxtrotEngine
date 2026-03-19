// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the EDITOR_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// EDITOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef EDITOR_EXPORTS
#define EDITOR_API __declspec(dllexport)
#else
#define EDITOR_API __declspec(dllimport)
#endif

#include "Plugin/Plugin.h"

// This class is exported from the dll
class EDITOR_API Editor : public Plugin
{
public:
	virtual void Initialize() override;
	virtual void Setup() override;

	// Gameloop functions.
	virtual void ProcessInput(FTInputDevice* inputDevice) override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

	virtual void Clear() override;

public:
	Editor();
	// TODO: add your methods here.
};

extern EDITOR_API int Editor;

EDITOR_API int Editor(void);
