#pragma once
#include "ResourceSystem/Mesh/FTMeshGroup.h"

namespace D3D11
{
	struct PointVPMat;
	struct SpriteGCData;
	struct SpriteVertex;
	class FTGeometryShader;
	class FTPixelShader;
	class D3D11Renderer;
	class D3D11PSO;
	class Camera;
	class FTTexture;

	class FTSprite :
		public FTMeshGroup
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @param meshIndex Index of an elements from the sliced spritesheet.
		void UpdateConstantBuffers(
			D3D11Renderer* renderer,
			Core::Transform* transform,
			Camera*		   camInst,
			FTMaterial*	   mat,
			const size_t   gcDataCount = 1,
			const int	   meshIndex   = 0);

		virtual void Render(
			D3D11Renderer* renderer,
			Core::Transform* transform,
			Camera*		   camInst,
			D3D11PSO*	   pso,
			FTMaterial*	   mat) override;

	public:
		FTTexture*	  GetTexture() const;
		SpriteGCData* GetGCSpriteData() const;

		void SetTexture(FTTexture* texture);
		void SetGCSpriteData(SpriteGCData* data);

	public:
		FTSprite(Common::FTResourceDef& resDef, D3D11Renderer* renderer);
		/// @brief Constructor skips FTSprite::Process() if isAnim is true.
		/// @param isAnim Is this constructor called from FTSpriteAnimation?
		FTSprite(Common::FTResourceDef& resDef, D3D11Renderer* renderer, bool isAnim);
		virtual ~FTSprite();

	protected:
		virtual void InitializeConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

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
		void Process(D3D11Renderer* renderer, SpriteVertex* vertices, size_t verticesCount);

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI() override;

	public:
		virtual void AddRefCount() override;
		virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11