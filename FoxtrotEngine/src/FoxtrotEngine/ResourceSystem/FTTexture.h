#pragma once
#include "FoxtrotEngine/ResourceSystem/FTResource.h"

#include <wrl.h> // ComPtr
#include <DirectXTex.h>

class FoxtrotRenderer;

class FTTexture :
    public FTResource
{
public:
    void CreateTexture(FoxtrotRenderer* renderer, const std::string filename);
    bool ReleaseTexture();

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView() { return mTextureResourceView; }
    void SetResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResView) 
    { mTextureResourceView = shaderResView; }

public:
    FTTexture()
        : FTResource()
    {}
    virtual ~FTTexture() override { ReleaseTexture(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       mSamplerState;

private:
    friend class ResourceManager;
};