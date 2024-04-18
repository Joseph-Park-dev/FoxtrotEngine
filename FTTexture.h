#pragma once
#include "FTResource.h"

#include <wrl.h> // ComPtr

#include "FoxtrotRenderer.h"

class FTTexture :
    public FTResource
{
private:
    //FTTexture();
    //~FTTexture() override;
    friend class ResourceManager;

public:
    void CreateTexture(const std::string filename, Microsoft::WRL::ComPtr<ID3D11Device> device);

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;
};