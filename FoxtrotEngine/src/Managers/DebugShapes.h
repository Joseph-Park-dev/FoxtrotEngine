// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton that manages debug shapes for components
/// e.g. Collider2DComponent
/// </summary>

#pragma once
#include "Core/SingletonMacro.h"
#include "ResourceSystem/FTShape.h"
#include "Renderer/FoxtrotRenderer.h"

class FTRectangle;

// This provides a context to render shapes as DebugShapes
class DebugShapes
{
	SINGLETON(DebugShapes)

public:
	// Adds the created shape to the std::vector.
	void AddShape(FTShape* shape);

	// To delete an individual shape, this member function can be called.
	// Do not delete debug shapes manually in Components.
	void RemoveShape(FTShape* shape);

	// The debug shapes is batch-deleted automatically in this member function.
	// Do not delete debug shapes manually in Components.
	void DeleteAll();

public:
	ComPtr<ID3D11VertexShader>&	  GetVS();
	ComPtr<ID3D11GeometryShader>& GetGSSquare();
	ComPtr<ID3D11PixelShader>&	  GetPS();
	ComPtr<ID3D11InputLayout>&	  GetInputLayout();

public:
	void Initialize(FoxtrotRenderer* renderer);

	// Batch renders shapes.
	// It is not necessary to render shapes manually from the Components
	void Render(FoxtrotRenderer* renderer);

private:
	std::vector<FTShape*> mShapes;
	std::wstring		  mVSPath;
	std::wstring		  mGSPath;
	std::wstring		  mPSPath;

private:
	ComPtr<ID3D11VertexShader>	 mVS;
	ComPtr<ID3D11GeometryShader> mGSSquare;
	ComPtr<ID3D11PixelShader>	 mPS;
	ComPtr<ID3D11InputLayout>	 mInputLayout;

private:
	void CreateShaders(ComPtr<ID3D11Device>& device);

#ifdef FOXTROT_EDITOR
public:
	FTRectangle* GetCameraRect();
	void		 SetCameraRect(FTRectangle* rect);
	void		 RenderCamRect(FoxtrotRenderer* renderer);

private:
	FTRectangle* mCamRect;
#endif // FOXTROT_EDITOR
};