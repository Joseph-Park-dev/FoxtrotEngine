#pragma once
#include "ResourceSystem/FTMeshGroup.h"

#include <wrl.h>

struct PointVPMat;
class FTGeometryShader;
class FTPixelShader;
class FoxtrotRenderer;
class D3D11PSO;

class FTSprite :
	public FTMeshGroup
{
public:
	/// @param meshIndex Index of an elements from the sliced spritesheet.
	void UpdateConstantBuffers(
		FoxtrotRenderer* renderer,
		Transform*		 transform,
		Camera*			 camInst,
		FTMaterial*		 mat,
		const size_t	 gcDataCount = 1,
		const int		 meshIndex = 0);

	virtual void Render(
		FoxtrotRenderer* renderer,
		Transform*		 transform,
		Camera*			 camInst,
		D3D11PSO*		 pso,
		FTMaterial*		 mat) override;

public:
	FTTexture*	  GetTexture() const;
	SpriteGCData* GetGCSpriteData() const;

	void SetTexture(FTTexture* texture);
	void SetGCSpriteData(SpriteGCData* data);

public:
	FTSprite(FTResourceDef& resDef, FoxtrotRenderer* renderer);
	/// @brief Constructor skips FTSprite::Process() if isAnim is true.
	/// @param isAnim Is this constructor called from FTSpriteAnimation?
	FTSprite(FTResourceDef& resDef, FoxtrotRenderer* renderer, bool isAnim);
	virtual ~FTSprite();

protected:
	virtual void InitializeConstantBuffers(ComPtr<ID3D11Device>& device) override;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetGCMatBuf();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetGCSpriteBuf();

private:
	PointVPMat*							 mGCMatData;
	SpriteGCData*						 mGCSpriteData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGCMatBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGCSpriteBuf;

	FTTexture* mTexture;

private:
	void Process(FoxtrotRenderer* renderer, SpriteVertex* vertices, size_t verticesCount);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

public:
	virtual void AddRefCount() override;
	virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
};