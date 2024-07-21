#pragma once
#include "FoxtrotEngine/ResourceSystem/FTResource.h"

#include <wrl.h> // ComPtr
#include <DirectXTex.h>

class FoxtrotRenderer;

class FTTexture :
    public FTResource
{
public:
    bool CreateTexture(FoxtrotRenderer* renderer, const std::string filename);
    bool ReleaseTexture();

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView() { return mTextureResourceView; }
    void SetResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResView) 
    { mTextureResourceView = shaderResView; }

    int	GetTexWidth()  const { return mTexWidth; }
    int	GetTexHeight() const { return mTexHeight; }

    void SetTexWidth(int texWidth) { mTexWidth = texWidth; }
    void SetTexHeight(int texHeight) { mTexHeight = texHeight; }

    int& GetTexWidthRef()  { return mTexWidth; }
    int& GetTexHeightRef() { return mTexHeight; }

public:
    FTTexture()
        : FTResource()
    {}
    virtual ~FTTexture() override { ReleaseTexture(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       mSamplerState;

    int	mTexWidth;
    int	mTexHeight;

private:
    friend class ResourceManager;
};