#pragma once
#include "Core/SingletonMacro.h"
#include "ResourceSystem/FTShape.h"
#include "Renderer/FoxtrotRenderer.h"

// This provides a context to render shapes as DebugGeometries
class DebugGeometries
{
	SINGLETON(DebugGeometries);

public:
	void Initialize(FoxtrotRenderer* renderer);
	void Render(FoxtrotRenderer* renderer);

public:
	void RemoveShape(FTShape* shape);
	void DeleteAll();

private:
	std::vector<FTShape*>	mShapes;

	std::wstring			mVertexShaderPath;
	std::wstring			mPixelShaderPath;

private:
	ComPtr<ID3D11VertexShader>	mVertexShader;
	ComPtr<ID3D11PixelShader>	mPixelShader;
	ComPtr<ID3D11InputLayout>	mInputLayout;

private:
	void AddShape(FTShape* shape);
	void CreateShaders(ComPtr<ID3D11Device>& device);

	friend class FTRectangle;
};