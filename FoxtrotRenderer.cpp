#include "FoxtrotRenderer.h"

#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr

#include "TemplateFunctions.h"

auto MakeSquare() {
	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;
	using DirectX::SimpleMath::Vector4;

	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords; // 텍스춰 좌표

	const float scale = 1.0f;

	// 앞면
	positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	// FTTexture Coordinates (Direct3D 9)
	// https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	std::vector<Vertex> vertices;
	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v;
		v.position = positions[i];
		v.color = colors[i];
		v.texcoord = texcoords[i];
		vertices.push_back(v);
	}
	std::vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return std::tuple{ vertices, indices };
}

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

void FoxtrotRenderer::DestroyRenderer(FoxtrotRenderer* renderer)
{
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

void FoxtrotRenderer::Update()
{
	using namespace DirectX;

	// 모델의 변환
	mConstantBufferData.model =
		DirectX::SimpleMath::Matrix::CreateScale(1.5f) *
		DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	mConstantBufferData.model = mConstantBufferData.model.Transpose();

	// 시점 변환
	// m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus,
	// m_viewUp);
	mConstantBufferData.view =
		XMMatrixLookToLH(mViewEyePos, mViewEyeDir, mViewUp);
	mConstantBufferData.view = mConstantBufferData.view.Transpose();

	// 프로젝션
	// m_aspect = AppBase::GetAspectRatio(); // <- GUI에서 조절
	if (m_usePerspectiveProjection) {
		mConstantBufferData.projection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(mProjFovAngleY), mAspect, mNearZ, mFarZ);
	}
	else {
		mConstantBufferData.projection = XMMatrixOrthographicOffCenterLH(
			-mAspect, mAspect, -1.0f, 1.0f, mNearZ, mFarZ);
	}
	mConstantBufferData.projection =
		mConstantBufferData.projection.Transpose();

	// Constant를 CPU에서 GPU로 복사
	UpdateBuffer(mConstantBufferData, mConstantBuffer);

	UpdateBuffer(mPixelShaderConstantBufferData,
		mPixelShaderConstantBuffer);
}

void FoxtrotRenderer::Render()
{
	// 왜 여기로? => Pixel shader에게 텍스처와 셈플러 두가지를 넘겨주기 위함
	// RS: Rasterizer stage
	// OM: Output-Merger stage
	// VS: Vertex Shader
	// PS: Pixel Shader
	// IA: Input-Assembler stage

	mContext->RSSetViewports(1, &mScreenViewport);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	mContext->ClearDepthStencilView(mDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	// 비교: Depth Buffer를 사용하지 않는 경우
	// m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
	// nullptr);
	// Render target 지정 가능 -> 어떤 메모리에 렌더링을 할지를 지정 가능
	// Render target 여러개도 가능 (여러개의 Render target, or 여러개의 Pass(단계))
	mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(),
		mDepthStencilView.Get());
	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	// 어떤 쉐이더를 사용할지 설정
	mContext->VSSetShader(mColorVertexShader.Get(), 0, 0);
	mContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[1] = {
		mWallTexture.mTextureResourceView.Get()
	};

	mContext->PSSetShaderResources(0, 1, pixelResources);
	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	mContext->PSSetConstantBuffers(0, 1,
		mPixelShaderConstantBuffer.GetAddressOf());
	mContext->PSSetShader(mColorPixelShader.Get(), 0, 0);
	mContext->RSSetState(mRasterizerState.Get());

	// 버텍스/인덱스 버퍼 설정
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetInputLayout(mColorInputLayout.Get());
	mContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride,
		&offset);
	mContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mContext->DrawIndexed(mIndexCount, 0, 0);
}

void FoxtrotRenderer::RenderClear(const float clearColor[4])
{
	mContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	mContext->ClearDepthStencilView(mDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
}

bool FoxtrotRenderer::Initialize(HWND window, int width, int height)
{
	// 만약 그래픽스 카드 호환성 문제로 D3D11CreateDevice()가 실패하는 경우에는
	// D3D_DRIVER_TYPE_HARDWARE 대신 D3D_DRIVER_TYPE_WARP 사용해보세요
	// const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_WARP;
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	// 여기서 생성하는 것들
	// m_device, m_context, m_swapChain,
	// m_renderTargetView, m_screenViewport, m_rasterizerState

	// m_device와 m_context 생성

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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
		createDeviceFlags,        // Set debug and Direct2D compatibility flags.
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
	UINT numQualityLevels;
	device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
	if (numQualityLevels <= 0) {
		LogString("MSAA not supported.");
	}

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = width;               // set the back buffer width
	sd.BufferDesc.Height = height;             // set the back buffer height
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
	sd.BufferCount = 2;                                // Double-buffering
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
	sd.OutputWindow = window;                    // the window to be used
	sd.Windowed = TRUE;                                // windowed/full-screen mode
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (numQualityLevels > 0) {
		sd.SampleDesc.Count = 4; // how many multisamples
		sd.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		sd.SampleDesc.Count = 1; // how many multisamples
		sd.SampleDesc.Quality = 0;
	}

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
		createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION,
		&sd, &mSwapChain, &mDevice, &featureLevel,
		&mContext)))
	{
		LogString("D3D11CreateDeviceAndSwapChain() failed.");
		return false;
	}

	// CreateRenderTarget
	ID3D11Texture2D* pBackBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRenderTargetView);
		pBackBuffer->Release();
	}
	else {
		LogString("CreateRenderTargetView() failed.");
		return false;
	}
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = float(width);
	mScreenViewport.Height = float(height);
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	mContext->RSSetViewports(1, &mScreenViewport);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;

	mDevice->CreateRasterizerState(&rastDesc, &mRasterizerState);

	// Create depth buffer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = width;
	depthStencilBufferDesc.Height = height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (numQualityLevels > 0) {
		depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
		depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&depthStencilBufferDesc, 0,
		mDepthStencilBuffer.GetAddressOf())))
	{
		LogString("CreateTexture2D() failed.");
	}
	if (FAILED(
		mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, &mDepthStencilView)))
	{
		LogString("CreateDepthStencilView() failed.");
	}

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true; // false
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	if (FAILED(mDevice->CreateDepthStencilState(&depthStencilDesc,mDepthStencilState.GetAddressOf())))
	{
		LogString("CreateDepthStencilState() failed.");
	}

	mWallTexture.CreateTexture("./Assets/Asteroid.png", mDevice);

	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc,sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable =true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mDevice->CreateBlendState(&omDesc, mBlendState.GetAddressOf());
	mContext->OMSetBlendState(mBlendState.Get(), 0, 0xffffffff);

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
	mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

	// Geometry 정의
	auto [vertices, indices] = MakeSquare();

	// 버텍스 버퍼 만들기
	CreateVertexBuffer<Vertex>(vertices, mVertexBuffer);

	// 인덱스 버퍼 만들기
	mIndexCount = UINT(indices.size());

	CreateIndexBuffer(indices, mIndexBuffer);

	// ConstantBuffer 만들기
	mConstantBufferData.model = DirectX::SimpleMath::Matrix();
	mConstantBufferData.view = DirectX::SimpleMath::Matrix();
	mConstantBufferData.projection = DirectX::SimpleMath::Matrix();

	CreateConstantBuffer(mConstantBufferData, mConstantBuffer);

	CreateConstantBuffer(mPixelShaderConstantBufferData,
		mPixelShaderConstantBuffer);

	// 쉐이더 만들기

	// Input-layout objects describe how vertex buffer data is streamed into the
	// IA(Input-Assembler) pipeline stage.
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout

	// Input-Assembler Stage
	// The purpose of the input-assembler stage is to read primitive data
	// (points, lines and/or triangles) from user-filled buffers and assemble
	// the data into primitives that will be used by the other pipeline stages.
	// https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-input-assembler-stage

	// D3D11_INPUT_ELEMENT_DESC structure(d3d11.h)
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc

	// Semantics
	// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	CreateVertexShaderAndInputLayout(
		L"ColorVertexShader.hlsl", inputElements, mColorVertexShader,
		mColorInputLayout);

	CreatePixelShader(L"ColorPixelShader.hlsl", mColorPixelShader);

	return true;
}

void FoxtrotRenderer::CreateIndexBuffer(const std::vector<uint16_t>& indices,
	ComPtr<ID3D11Buffer>& m_indexBuffer) {
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
	bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
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

void FoxtrotRenderer::CreateVertexShaderAndInputLayout(
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
	HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, 0, "main", "vs_5_0",
		compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	mDevice->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&vertexShader);

	mDevice->CreateInputLayout(inputElements.data(),
		UINT(inputElements.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), &inputLayout);
}

void FoxtrotRenderer::CreatePixelShader(const std::wstring& filename,
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

	mDevice->CreatePixelShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&pixelShader);
}

FoxtrotRenderer::FoxtrotRenderer()
{}