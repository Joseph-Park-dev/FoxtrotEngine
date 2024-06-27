#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr
#include <directxtk/PrimitiveBatch.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"

#ifdef _DEBUG
#include "FoxtrotEditor/EditorLayer.h"
#include "FoxtrotEditor/EditorCamera2D.h"
#include "FoxtrotEditor/RenderTextureClass.h"
#endif // _DEBUG


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
	mRenderWidth = width;
	mRenderHeight = height;
	GetAspectRatio();
	/*mClearColor[0] = 217.f / 255.f;
	mClearColor[1] = 216.f / 255.f;
	mClearColor[2] = 212.f / 255.f;
	mClearColor[3] = 1.0f;*/

	mClearColor[0] = 0.0f;
	mClearColor[1] = 0.0f;
	mClearColor[2] = 0.0f;
	mClearColor[3] = 1.0f; 

	if (!CreateDeviceAndContext(window))
	{
		LogString("Create Device and Context failed");
		LogString("Create SwapChain failed");
		return false;
	}

	/*HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return false;*/

	if (!CreateRenderTargetView())
	{
		LogString("Create Render Target View failed.");
		return false;
	}

	if (!CreateRasterizerState())
	{
		LogString("Create Rasterizer State failed.");
		return false;
	}

	if (!CreateDepthBuffer())
	{
		LogString("Create Depth Buffer failed.");
		return false;
	}

	if (!CreateDepthStencilState())
	{
		LogString("Create Depth Stencil State failed.");
		return false;
	}

	if (!CreateBlendState())
	{
		LogString("Create Blend State failed.");
		return false;
	}
	mContext->OMSetBlendState(mBlendState.Get(), 0, D3D11_DEFAULT_SAMPLE_MASK);

	if (!CreateTextureSampler())
	{
		LogString("Create Sampler State failed.");
		return false;
	}

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		LogString("FoxtrotRenderer::CoInitilizeEX() -> Failed.");
		return false;
	}
		// error

	SetViewport();

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
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

#ifdef _DEBUG
	if (!CreateRenderTexture(mRenderWidth, mRenderHeight))
	{
		LogString("Error : FoxtrotRenderer Initialize - CreateRenderTexture failed.");
		return false;
	}
#endif // _DEBUG
	return true;
}

void FoxtrotRenderer::DestroyRenderer(FoxtrotRenderer* renderer)
{
#ifdef _DEBUG
	// 렌더 텍스쳐 객체를 해제한다
	if (renderer->mRenderTexture)
	{
		renderer->mRenderTexture->Shutdown();
		delete renderer->mRenderTexture;
		renderer->mRenderTexture = 0;
	}
#endif // _DEBUG
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

void FoxtrotRenderer::Render()
{
	// 전체 장면을 먼제 텍스쳐로 렌더링 합니다
	// 
	// 왜 여기로? => Pixel shader에게 텍스처와 셈플러 두가지를 넘겨주기 위함
	// RS: Rasterizer stage
	// OM: Output-Merger stage
	// VS: Vertex Shader
	// PS: Pixel Shader
	// IA: Input-Assembler stage

	/*mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(),
		mDepthStencilView.Get());*/
	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	mContext->VSSetShader(mBasicVertexShader.Get(), 0, 0);

	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	mContext->PSSetShader(mBasicPixelShader.Get(), 0, 0);

	mContext->RSSetState(mRasterizerState.Get());

	/*if (m_drawAsWire) {
		mContext->RSSetState(m_wireRasterizerSate.Get());
	}
	else {
		mContext->RSSetState(m_solidRasterizerSate.Get());
	}*/

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 버텍스/인덱스 버퍼 설정
	for (const Mesh* mesh : mMeshes) {
		mContext->VSSetConstantBuffers(
			0, 1, mesh->vertexConstantBuffer.GetAddressOf());

		if(mesh->texture != nullptr)
			mContext->PSSetShaderResources(
				0, 1, mesh->texture->GetResourceView().GetAddressOf()
			);

		mContext->PSSetConstantBuffers(
			0, 1, mesh->pixelConstantBuffer.GetAddressOf());

		mContext->IASetInputLayout(mBasicInputLayout.Get());
		mContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
			&stride, &offset);
		mContext->IASetIndexBuffer(mesh->indexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, 0);
		mContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mContext->DrawIndexed(mesh->indexCount, 0, 0);
	}
}

// Constant buffer data 업데이트 & 그 내용을 GPU 버퍼로 복사
// 이후 Render() 에서 Vertex shader를 실행시키게 되는데,
// 이때 Constant buffer data를 사용
// mContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
//void FoxtrotRenderer::UpdateConstantBufferData(Transform* transform)
//{
//	
//}

void FoxtrotRenderer::RenderClear()
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
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

void FoxtrotRenderer::ResizeSceneViewport(UINT width, UINT height)
{
	if (mSwapChain) { // 처음 실행이 아닌지 확인
		// Create 되는 변수들 Release 
		mRenderTargetView.Reset();
		mDepthStencilBuffer.Reset();
		mDepthStencilView.Reset();
		mContext->Flush(); //요게 없으면 메모리가 훅 올라갑니다!

		mRenderWidth = width;
		mRenderHeight = height;
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

void FoxtrotRenderer::RemoveMesh(Mesh* mesh)
{
	std::vector<Mesh*>::iterator iter = std::find(mMeshes.begin(), mMeshes.end(), mesh);
	if (iter != mMeshes.end())
	{
		delete (*iter);
		mMeshes.erase(iter);
		mMeshes.shrink_to_fit();
	}
}

bool FoxtrotRenderer::CreateDeviceAndContext(HWND window)
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

	if (FAILED(D3D11CreateDevice(
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
	)))
	{
		LogString("D3D11CreateDevice() failed.");
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		LogString("D3D Feature Level 11 is unsupported.");
		return false;
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

	if (FAILED(device.As(&mDevice))) {
		LogString("device.AS() failed.");
		return false;
	}
	if (FAILED(context.As(&mContext))) {
		LogString("context.As() failed.");
		return false;
	}

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		0, // Default adapter
		driverType,
		0, // No software device
		mCreateDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION,
		&sd, &mSwapChain, &mDevice, &featureLevel,
		&mContext)))
	{
		LogString("D3D11CreateDeviceAndSwapChain() failed.");
		return false;
	}
	return true;
}

bool FoxtrotRenderer::CreateRenderTargetView()
{
	mRenderTargetView.Reset();
	ComPtr<ID3D11Texture2D> backBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	if (backBuffer) {
		mDevice->CreateRenderTargetView(backBuffer.Get(), NULL, mRenderTargetView.GetAddressOf());
		return true;
	}
	else
		return false;
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
	mContext->RSSetViewports(1, &mScreenViewport);
}

bool FoxtrotRenderer::CreateRasterizerState()
{
	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	//rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;

	if (FAILED(mDevice->CreateRasterizerState(&rastDesc, &mRasterizerState)))
		return false;
	return true;
}

bool FoxtrotRenderer::CreateDepthBuffer()
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

bool FoxtrotRenderer::CreateDepthStencilState()
{
	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true; // false
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	if (FAILED(mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf())))
	{
		LogString("CreateDepthStencilState() failed.");
		return false;
	}
	return true;
}

bool FoxtrotRenderer::CreateBlendState()
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

	if (FAILED(mDevice->CreateBlendState(&omDesc, mBlendState.GetAddressOf())))
	{
		LogString("Error - FoxtrotRenderer::Initialize, Failed to CreateBlendState()");
		return false;
	}
	return true;
}

//bool FoxtrotRenderer::ImportTextures()
//{
//	if(!mWallTexture.CreateTexture("./Assets/Asteroid.png", mDevice))
//		return false;
//	return true;
//}

bool FoxtrotRenderer::CreateTextureSampler()
{
	// FTTexture sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	if (FAILED(mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf())))
		return false;
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

void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
	if (FAILED(hr)) {
		// 파일이 없을 경우
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
			LogString("File not found.");
		}

		// 에러 메시지가 있으면 출력
		if (errorBlob) {
			std::cout << "Shader compile error\n"
				<< (char*)errorBlob->GetBufferPointer() << std::endl;
		}
	}
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
{}

#ifdef _DEBUG
bool FoxtrotRenderer::CreateRenderTexture(int width, int height)
{
	mRenderTexture = new RenderTextureClass();
	// 렌더링 텍스처 객체를 초기화한다
	bool result = mRenderTexture->Initialize(mDevice.Get(), width, height, mNumQualityLevels);
	if (!result)
	{
		return false;
	}
}

bool FoxtrotRenderer::UpdateRenderTexture(int width, int height)
{
	bool result = mRenderTexture->Update(mDevice.Get(), width, height, mNumQualityLevels);
	if (!result)
	{
		return false;
	}
}

void FoxtrotRenderer::RenderToTexture()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다
	mContext->OMSetRenderTargets(1, mRenderTexture->GetRenderTargetView().GetAddressOf(), mRenderTexture->GetDepthStencilView().Get());

	// 렌더링을 텍스처에 지웁니다
	mRenderTexture->ClearRenderTarget(mContext.Get(), mRenderTexture->GetDepthStencilView().Get(), mClearColor);

	// 이제 장면을 렌더링하면 백 버퍼 대신 텍스처로 렌더링됩니다
	Render();

	//// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다
	mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	//return true;
}

void FoxtrotRenderer::DrawPrimitives(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	for (size_t i = 0; i < mRegisteredPrimitive.size(); ++i)
	{
		FTVector2 topLeft = mRegisteredPrimitive[i].first;
		FTVector2 bottomRight = mRegisteredPrimitive[i].second;
		HPEN pen = CreatePen(PS_DASH, 2, RGB(255, 0, 0));
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		(HPEN)SelectObject(hdc, pen);
		Rectangle(hdc, (int)topLeft.x, (int)topLeft.y, (int)bottomRight.x, (int)bottomRight.y);
		DeleteObject(pen);
	}
	EndPaint(hwnd, &ps);
	mRegisteredPrimitive.clear();
}

void FoxtrotRenderer::DrawRectangle(FTVector2 topLeft, FTVector2 bottomRight)
{
	std::pair<FTVector2, FTVector2> pair = { topLeft, bottomRight };
	mRegisteredPrimitive.push_back(pair);
}
#endif // _DEBUG