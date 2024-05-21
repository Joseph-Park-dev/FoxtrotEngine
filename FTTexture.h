#pragma once
#include "FTResource.h"

#include <wrl.h> // ComPtr

#include "FoxtrotRenderer.h"

class FTTexture :
    public FTResource
{
public:
    bool CreateTexture(FoxtrotRenderer* renderer);

public:
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          GetTexture()      { return mTexture; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetResourceView() { return mTextureResourceView; }

public:
    FTTexture()
        : FTResource()
    {}

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;

private:
    friend class ResourceManager;
};