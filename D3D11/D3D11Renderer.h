// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the D3D11RENDERER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// D3D11RENDERER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef D3D11RENDERER_EXPORTS
#define D3D11RENDERER_API __declspec(dllexport)
#else
#define D3D11RENDERER_API __declspec(dllimport)
#endif

// This class is exported from the dll
class D3D11RENDERER_API CD3D11Renderer {
public:
	CD3D11Renderer(void);
	// TODO: add your methods here.
};

extern D3D11RENDERER_API int nD3D11Renderer;

D3D11RENDERER_API int fnD3D11Renderer(void);
