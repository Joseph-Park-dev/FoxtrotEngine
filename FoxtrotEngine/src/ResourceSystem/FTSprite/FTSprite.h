#pragma once
#include "ResourceSystem/FTMeshGroup.h"

#include <wrl.h>

struct PointVPMat;
class FTGeometryShader;
class FTPixelShader;
class FoxtrotRenderer;

class FTSprite :
	public FTMeshGroup
{
public:
	virtual void Render(
		FoxtrotRenderer*  renderer,
		Transform*		  transform,
		Camera*			  camInst,
		FTVertexShader*	  vs,
		FTGeometryShader* gs,
		FTPixelShader*	  ps,
		FTMaterial*		  mat);

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

	/// @param gcDataCount Number of GS constant data in the buffer.
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		Transform*					 transform,
		Camera*						 camInst,
		FTMaterial*					 mat,
		const int					 frontDir	 = 0,
		const size_t				 gcDataCount = 1);

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
