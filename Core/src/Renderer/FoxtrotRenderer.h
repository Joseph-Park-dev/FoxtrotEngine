// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Main renderer from FoxtrotEngine.
/// Takes care of the rendering operations in DirectX 11 context.
/// </summary>

#pragma once
class Transform;
class ViewportRenderer;
class FTVector2;
class FTWindow;

enum class FillMode
{
	WireFrame,
	Solid
};

class FoxtrotRenderer
{
public:
	const float* GetClearColor() const;

	// FillMode related (Getters and Setters)
	void	 SwitchFillMode() const;
	FillMode GetFillMode() const;
	void	 SetFillMode(const FillMode mode);

private:
	float	 mClearColor[4];
	FillMode mFillMode;

	// Pixel color (RGBA) at the cursor position.
	uint8_t mCursorPosColor[4] = {
		0,
	};

public:
	FoxtrotRenderer();
	~FoxtrotRenderer();

private:
	// Intended to be used during initialization.
	bool Initialize(FTWindow* window, int renderWidth, int renderHeight);

	// ID3D11 Helper functions
	HRESULT CreateRasterizerState();
	HRESULT CreateDepthStencilState(ComPtr<ID3D11DepthStencilState>& dss, bool depthEnabled = true);
	HRESULT CreateBlendState();
	HRESULT CreateTextureSampler();

#ifdef FOXTROT_EDITOR
public:
	bool InitializeViewport(FTWindow* window, UINT posX, UINT posY, UINT width, UINT height);
	void RenderOnViewport();
	void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

public:
	ViewportRenderer* GetViewportRenderer();

private:
	ViewportRenderer* mViewportRenderer;
#endif // FOXTROT_EDITOR
};