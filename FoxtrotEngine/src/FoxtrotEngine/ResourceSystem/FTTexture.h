#pragma once
#include "FTResource.h"

#include <wrl.h> // ComPtr
#include <DirectXTex.h>

#include "FoxtrotRenderer.h"

class FTTexture :
    public FTResource
{
public:
    bool CreateTexture(FoxtrotRenderer* renderer);
    bool ReleaseTexture();

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView() { return mTextureResourceView; }
    DirectX::ScratchImage* GetScratchImage() { return mScratchImage; }
    void SetResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResView) 
    { mTextureResourceView = shaderResView; }

public:
    FTTexture()
        : FTResource()
        , mScratchImage{ new DirectX::ScratchImage }
    {}
    virtual ~FTTexture() override { ReleaseTexture(); }

private:
    DirectX::ScratchImage* mScratchImage;
    DirectX::TexMetadata mMetaData;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;

private:
    friend class ResourceManager;
};