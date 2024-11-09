#include "Renderer/FoxtrotRenderer.h"

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr
#include <DirectXColors.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/VertexTypes.h>

#include "Core/TemplateFunctions.h"
#include "Actors/Transform.h"
#include "ResourceSystem/Vertex.h"
#include "Managers/KeyInputManager.h"
#include "Managers/SceneManager.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "EditorLayer.h"
#include "RenderTextureClass.h"
#endif // FOXTROT_EDITOR

FoxtrotRenderer* FoxtrotRenderer::CreateRenderer(HWND window, int width, int height)
{
	FoxtrotRenderer* ftRenderer = new FoxtrotRenderer();
	if (!ftRenderer->Initialize(window, width, height))
	{
		LogString("Failed to Initialize FT_Renderer");
		return nullptr;
	}
	return ftRenderer;
}

bool FoxtrotRenderer::Initialize(HWND window, int width, int height)
{
	mRenderWidth	= width;
	mRenderHeight	= height;

	DX::ThrowIfFailed(CreateDeviceAndContext(window));

	/*HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return false;*/

	DX::ThrowIfFailed(CreateRenderTargetView());

	DX::ThrowIfFailed(CreateRasterizerState());

	DX::ThrowIfFailed(CreateDepthBuffer());

	DX::ThrowIfFailed(CreateDepthStencilState());

	DX::ThrowIfFailed(CreateBlendState());
	mContext->OMSetBlendState(mBlendState.Get(), 0, D3D11_DEFAULT_SAMPLE_MASK);

	DX::ThrowIfFailed(CreateTextureSampler());

	DX::ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 + 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3 + 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!CreateVertexShaderAndInputLayout(
		VERTEX_SHADER_PATH, inputElements, mBasicVertexShader,
		mBasicInputLayout))
	{
		return false;
	}
	if (!CreatePixelShader(PIXEL_SHADER_PATH, mBasicPixelShader))
	{
		return false;
	}

	/*mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(),
	mDepthStencilView.Get());*/
	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	mContext->VSSetShader(mBasicVertexShader.Get(), 0, 0);

	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	mContext->PSSetShader(mBasicPixelShader.Get(), 0, 0);

	mContext->RSSetState(mSolidRasterizerState.Get());

#ifdef FOXTROT_EDITOR
	mRenderTexture = new RenderTextureClass();
	if (!mRenderTexture)
	{
		LogString("Error : FoxtrotRenderer Initialize - CreateRenderTexture failed.");
		return false;
	}
	mRenderTexture->InitializeTexture(mDevice, mRenderWidth, mRenderHeight);
#endif // FOXTROT_EDITOR
	return true;
}

void FoxtrotRenderer::DestroyRenderer(FoxtrotRenderer* renderer)
{
#ifdef FOXTROT_EDITOR
	// 렌더 텍스쳐 객체를 해제한다
	if (renderer->mRenderTexture)
	{
		delete renderer->mRenderTexture;
		renderer->mRenderTexture = 0;
	}
#endif // FOXTROT_EDITOR

	if (renderer == nullptr)
	{
		LogString("Renderer is already null");
		return;
	}
	else
	{
		delete renderer;
		renderer = nullptr;
	}
}

void FoxtrotRenderer::SwapChainPresent(UINT syncInterval, UINT flags)
{
	mSwapChain->Present(syncInterval, flags);
}

// Constant buffer data 업데이트 & 그 내용을 GPU 버퍼로 복사
// 이후 Render() 에서 Vertex shader를 실행시키게 되는데,
// 이때 Constant buffer data를 사용
// mContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
//void FoxtrotRenderer::UpdateConstantBufferData(Transform* transform)

void FoxtrotRenderer::RenderClear()
{
	float clearColor[4] = { 211.0f,211.0f,211.0f,1.0f };
	mContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	mContext->ClearDepthStencilView(mDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
}

void FoxtrotRenderer::ResizeWindow(UINT width, UINT height)
{
	if (mSwapChain) { // 처음 실행이 아닌지 확인
		// Create 되는 변수들 Release 
		mRenderTargetView.Reset();
		mDepthStencilBuffer.Reset();
		mDepthStencilView.Reset();
		mContext->Flush(); //요게 없으면 메모리가 훅 올라갑니다!

		mRenderWidth = (float)width;
		mRenderHeight = (float)height;
		mSwapChain->ResizeBuffers(0, // 현재 개수 유지
			mRenderWidth,
			mRenderHeight,
			DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
			0);
		CreateRenderTargetView();
		CreateDepthBuffer();
		SetViewport();
	}
}

void FoxtrotRenderer::SwitchFillMode()
{
	if (mFillMode == FillMode::WireFrame)
		mContext->RSSetState(mWireframeRasterizerState.Get());
	else if (mFillMode == FillMode::Solid)
		mContext->RSSetState(mSolidRasterizerState.Get());
}

HRESULT FoxtrotRenderer::CreateDeviceAndContext(HWND window)
{
	// 만약 그래픽스 카드 호환성 문제로 D3D11CreateDevice()가 실패하는 경우에는
	// D3D_DRIVER_TYPE_HARDWARE 대신 D3D_DRIVER_TYPE_WARP 사용해보세요
	// const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_WARP;
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	// 여기서 생성하는 것들
	// m_device, mContext, m_swapChain,
	// m_renderTargetView, m_screenViewport, m_rasterizerState

	// m_device와 mContext 생성

	mCreateDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	mCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	const D3D_FEATURE_LEVEL featureLevels[2] = {
		D3D_FEATURE_LEVEL_11_0, // 더 높은 버전이 먼저 오도록 설정
		D3D_FEATURE_LEVEL_9_3 };
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = S_OK;

	hr = D3D11CreateDevice(
		nullptr,                  // Specify nullptr to use the default adapter.
		driverType,               // Create a device using the hardware graphics driver.
		0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		mCreateDeviceFlags,        // Set debug and Direct2D compatibility flags.
		featureLevels,            // List of feature levels this app can support.
		ARRAYSIZE(featureLevels), // Size of the list above.
		D3D11_SDK_VERSION,		  // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		device.GetAddressOf(),	  // Returns the Direct3D device created.
		&featureLevel,			  // Returns feature level of device created.
		context.GetAddressOf()    // Returns the device immediate context.
	);
	if(FAILED(hr)){
		LogString("D3D11CreateDevice() failed.");
		return hr;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0){
		LogString("D3D Feature Level 11 is unsupported.");
		return E_FAIL;
	}

	// 참고: Immediate vs deferred context
	// A deferred context is primarily used for multithreading and is not necessary for a
	// single-threaded application.
	// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-intro#deferred-context

	// 4X MSAA 지원하는지 확인

	device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &mNumQualityLevels);
	if (mNumQualityLevels <= 0) {
		LogString("MSAA not supported.");
	}

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = mRenderWidth;               // set the back buffer width
	sd.BufferDesc.Height = mRenderHeight;             // set the back buffer height
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
	sd.BufferCount = 2;                                // Double-buffering
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
	sd.OutputWindow = window;                    // the window to be used
	sd.Windowed = TRUE;                                // windowed/full-screen mode
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//if (mNumQualityLevels > 0) {
	//	sd.SampleDesc.Count = 4; // how many multisamples
	//	sd.SampleDesc.Quality = mNumQualityLevels - 1;
	//}
	//else {
	//	sd.SampleDesc.Count = 1; // how many multisamples
	//	sd.SampleDesc.Quality = 0;
	//}
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	hr = device.As(&mDevice);
	if(FAILED(hr)){
		LogString("device.AS() failed.");
		return hr;
	}

	hr = context.As(&mContext);
	if(FAILED(hr)){
		LogString("context.As() failed.");
		return hr;
	}

	hr = D3D11CreateDeviceAndSwapChain(
		0, // Default adapter
		driverType,
		0, // No software device
		mCreateDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION,
		&sd, &mSwapChain, &mDevice, &featureLevel,
		&mContext);
	if(FAILED(hr)){
		LogString("D3D11CreateDeviceAndSwapChain() failed.");
		return hr;
	}
	return hr;
}

HRESULT FoxtrotRenderer::CreateRenderTargetView()
{
	mRenderTargetView.Reset();
	ComPtr<ID3D11Texture2D> backBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	if (backBuffer)
		return mDevice->CreateRenderTargetView(
			backBuffer.Get(), NULL, mRenderTargetView.GetAddressOf()
		);
}

HRESULT FoxtrotRenderer::CreateRasterizerState()
{
	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rastDescSolid;
	ZeroMemory(&rastDescSolid, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	//rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDescSolid.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDescSolid.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDescSolid.FrontCounterClockwise = false;

	D3D11_RASTERIZER_DESC rastDescWireFrame;
	ZeroMemory(&rastDescWireFrame, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	//rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDescWireFrame.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rastDescWireFrame.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDescWireFrame.FrontCounterClockwise = false;

	HRESULT solidResult = mDevice->CreateRasterizerState(&rastDescSolid, &mSolidRasterizerState);
	HRESULT wireResult = mDevice->CreateRasterizerState(&rastDescWireFrame, &mWireframeRasterizerState);
	
	return solidResult & wireResult;
}

void FoxtrotRenderer::SetViewport()
{
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = float(mRenderWidth);
	mScreenViewport.Height = float(mRenderHeight);
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	Camera::GetInstance()->CalcNDCRatio();
	mContext->RSSetViewports(1, &mScreenViewport);
}

HRESULT FoxtrotRenderer::CreateDepthBuffer()
{
	// Create depth buffer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = mRenderWidth;
	depthStencilBufferDesc.Height = mRenderHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//if (mNumQualityLevels > 0) {
	//	depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
	//	depthStencilBufferDesc.SampleDesc.Quality = mNumQualityLevels - 1;
	//}
	//else {
	//	depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
	//	depthStencilBufferDesc.SampleDesc.Quality = 0;
	//}
	depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&depthStencilBufferDesc, 0,
		mDepthStencilBuffer.GetAddressOf())))
	{
		LogString("CreateTexture2D() failed.");
		return E_FAIL;
	}
	if (FAILED(
		mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, mDepthStencilView.GetAddressOf())))
	{
		LogString("CreateDepthStencilView() failed.");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT FoxtrotRenderer::CreateDepthStencilState()
{
	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true; // false
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	return mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());
}

HRESULT FoxtrotRenderer::CreateBlendState()
{
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	return mDevice->CreateBlendState(&omDesc, mBlendState.GetAddressOf());
}

//bool FoxtrotRenderer::ImportTextures()
//{
//	if(!mWallTexture.CreateTexture("./Assets/Asteroid.png", mDevice))
//		return false;
//	return true;
//}

HRESULT FoxtrotRenderer::CreateTextureSampler()
{
	// FTTexture sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	return mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

bool FoxtrotRenderer::UpdateDepthBuffer(int width, int height)
{
	mDepthStencilBuffer.Reset();
	mDepthStencilView.Reset();

	// Create depth buffer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = width;
	depthStencilBufferDesc.Height = height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//if (mNumQualityLevels > 0) {
	//	depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
	//	depthStencilBufferDesc.SampleDesc.Quality = mNumQualityLevels - 1;
	//}
	//else {
	//	depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
	//	depthStencilBufferDesc.SampleDesc.Quality = 0;
	//}
	depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&depthStencilBufferDesc, 0,
		mDepthStencilBuffer.GetAddressOf())))
	{
		LogString("CreateTexture2D() failed.");
		return false;
	}
	if (FAILED(
		mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, mDepthStencilView.GetAddressOf())))
	{
		LogString("CreateDepthStencilView() failed.");
		return false;
	}
	return true;
}

void FoxtrotRenderer::CreateIndexBuffer(const std::vector<uint32_t>& indices,
	ComPtr<ID3D11Buffer>& m_indexBuffer) {
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
	bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
	bufferDesc.StructureByteStride = sizeof(uint16_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	mDevice->CreateBuffer(&bufferDesc, &indexBufferData,
		m_indexBuffer.GetAddressOf());
}

bool FoxtrotRenderer::CreateVertexShaderAndInputLayout(
	const std::wstring& filename,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
	ComPtr<ID3D11VertexShader>& vertexShader,
	ComPtr<ID3D11InputLayout>& inputLayout) {

	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	HRESULT createVS = mDevice->CreateVertexShader(shaderBlob->GetBufferPointer(),
							shaderBlob->GetBufferSize(), NULL,vertexShader.GetAddressOf());
	if (FAILED(createVS))
	{
		LogString("Create Vertex Shader failed");
		return false;
	}
	HRESULT createIptLayout = mDevice->CreateInputLayout(inputElements.data(),
		UINT(inputElements.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), inputLayout.GetAddressOf());
	if(FAILED(createIptLayout))
	{
		LogString("Create Input Layout failed");
		return false;
	}
	return true;
}

bool FoxtrotRenderer::CreatePixelShader(const std::wstring& filename,
	ComPtr<ID3D11PixelShader>& pixelShader) {
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, 0, "main", "ps_5_0",
		compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	HRESULT createPS = mDevice->CreatePixelShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		pixelShader.GetAddressOf());
	if (FAILED(createPS))
	{
		LogString("Create Pixel Shader failed");
		return false;
	}
	return true;
}

FoxtrotRenderer::FoxtrotRenderer()
	: mClearColor{0.0f,0.0f,0.0f,1.0f}
	, mFillMode(FillMode::Solid)
{}

#ifdef FOXTROT_EDITOR
void FoxtrotRenderer::RenderToTexture()
{
	mRenderTexture->DrawOnTexture(mContext, mRenderTargetView, mDepthStencilView, this);
}
#endif // FOXTROT_EDITOR

//void FoxtrotRenderer::DrawPrimitives()
//{
//	//PAINTSTRUCT ps;
//	//HDC hdc = BeginPaint(hwnd, &ps);
//	//for (size_t i = 0; i < mRegisteredPrimitive.size(); ++i)
//	//{
//	//	FTVector2 topLeft = mRegisteredPrimitive[i].first;
//	//	FTVector2 bottomRight = mRegisteredPrimitive[i].second;
//	//	HPEN pen = CreatePen(PS_DASH, 2, RGB(255, 0, 0));
//	//	SelectObject(hdc, GetStockObject(NULL_BRUSH));
//	//	(HPEN)SelectObject(hdc, pen);
//	//	Rectangle(hdc, (int)topLeft.x, (int)topLeft.y, (int)bottomRight.x, (int)bottomRight.y);
//	//	DeleteObject(pen);
//	//}
//	//EndPaint(hwnd, &ps);
//	//mRegisteredPrimitive.clear();
//	for (size_t i = 0; i < mRegisteredPrimitive.size(); ++i) {
//		DirectX::FXMMATRIX world = mRegisteredPrimitive[i]->basicVertexConstantBufferData.model;
//		DirectX::FXMMATRIX view = mRegisteredPrimitive[i]->basicVertexConstantBufferData.view;
//		DirectX::FXMMATRIX projection = mRegisteredPrimitive[i]->basicVertexConstantBufferData.projection;
//		mRegisteredPrimitive[i]->primitive->Draw(world, view, projection, DirectX::Colors::Red);
//	}
//}
//
//void FoxtrotRenderer::DrawRectangle(GeometricPrimitiveMesh* rect){
//	std::vector<GeometricPrimitiveMesh*>::iterator iter =
//		std::find(mRegisteredPrimitive.begin(), mRegisteredPrimitive.end(), rect);
//	if (iter == mRegisteredPrimitive.end()) {
//		mRegisteredPrimitive.push_back(rect);
//	}
//	else{
//		(*iter)->basicVertexConstantBufferData = rect->basicVertexConstantBufferData;
//	}
//}
 // _DEBUG