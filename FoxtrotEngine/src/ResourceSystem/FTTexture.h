#pragma once
#include "ResourceSystem/FTResource.h"

#include <wrl.h> // ComPtr
#include <DirectXTex.h>

#include "Math/FTMath.h"

class FoxtrotRenderer;

#define TEXTURE_FORMAT_SUPPORTED ".png, .jpeg"

class FTTexture :
    public FTResource
{
public:
    bool CreateTexture(FoxtrotRenderer* renderer, const std::string filename);
    bool ReleaseTexture();

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetResourceView() { return mTextureResourceView; }
    void SetResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResView) 
    { mTextureResourceView = shaderResView; }

    Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture() { return mTexture; }

    const FTVector2 GetTexScale() const { return FTVector2(mTexWidth, mTexHeight); }
    int	GetTexWidth()  const { return mTexWidth; }
    int	GetTexHeight() const { return mTexHeight; }

    void SetTexWidth(int texWidth) { mTexWidth = texWidth; }
    void SetTexHeight(int texHeight) { mTexHeight = texHeight; }

    int& GetTexWidthRef()  { return mTexWidth; }
    int& GetTexHeightRef() { return mTexHeight; }

public:
    FTTexture()
        : FTResource()
        , mTexWidth(0)
        , mTexHeight(0)
    {}
    virtual ~FTTexture() override 
    { 
        ReleaseTexture(); 
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       mSamplerState;

    int mTexWidth;
    int mTexHeight;

	friend class ResourceManager;

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs)           override;

#ifdef FOXTROT_EDITOR
public:
    virtual void UpdateUI()         override;
#endif // FOXTROT_EDITOR
};