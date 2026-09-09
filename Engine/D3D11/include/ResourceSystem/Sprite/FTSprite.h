#pragma once
#include "ResourceSystem/Mesh/FTMeshGroup.h"


namespace D3D11
{
	struct PointVPMat;
	struct SpriteGCData;
	struct SpriteVertex;
	class FTGeometryShader;
	class FTPixelShader;
	class IRenderer;
	class D3D11PSO;
	class FTTexture;

	class FTSprite :
		public FTMeshGroup
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Uploads the current shader parameters to the constant buffers.
		/// @param meshIndex Index of an elements from the sliced spritesheet.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param mat Matrix or material used by this operation.
		/// @param gcDataCount Number of geometry-shader constant-data entries.
		void UpdateConstantBuffers(
			Core::IRenderer* renderer,
			Core::Transform* transform,
			Core::ICamera*	 camInst,
			FTMaterial*		 mat,
			const size_t	 gcDataCount = 1,
			const int		 meshIndex	 = 0);

		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param pso Pipeline state object used for rendering.
		/// @param mat Matrix or material used by this operation.
		virtual void Render(
			Core::IRenderer* renderer,
			Core::Transform* transform,
			Core::ICamera*	 camInst,
			D3D11PSO*		 pso,
			FTMaterial*		 mat) override;

	public:
		/// @brief Returns the texture used by this ftsprite.
		/// @return Borrowed access to the texture.
		FTTexture*	  GetTexture() const;
		/// @brief Returns the gcsprite data used by this ftsprite.
		/// @return Borrowed access to the gcsprite data.
		SpriteGCData* GetGCSpriteData() const;

		/// @brief Updates the texture used by subsequent operations.
		/// @param texture Replacement texture.
		void SetTexture(FTTexture* texture);
		/// @brief Updates the gcsprite data used by subsequent operations.
		/// @param data Replacement gcsprite data.
		void SetGCSpriteData(SpriteGCData* data);

	public:
		/// @brief Initializes sprite metadata and its mesh representation.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTSprite(Common::FTResourceDef& resDef, void* renderer);
		/// @brief Constructor skips FTSprite::Process() if isAnim is true.
		/// @param isAnim Is this constructor called from FTSpriteAnimation?
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTSprite(Common::FTResourceDef& resDef, void* renderer, bool isAnim);
		/// @brief Releases the resources managed by this instance during destruction.
		virtual ~FTSprite() override;

	protected:
		/// @brief Allocates and initializes the GPU constant buffers used by this object.
		/// @param device Direct3D device used to create GPU resources.
		virtual void InitializeConstantBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	protected:
		/// @brief Returns the gcmat buf used by this ftsprite.
		/// @return Borrowed access to the gcmat buf.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11Buffer>& GetGCMatBuf();
		/// @brief Returns the gcsprite buf used by this ftsprite.
		/// @return Borrowed access to the gcsprite buf.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11Buffer>& GetGCSpriteBuf();

	private:
		PointVPMat*							 mGCMatData;
		SpriteGCData*						 mGCSpriteData;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mGCMatBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mGCSpriteBuf;

		FTTexture* mTexture;

	private:
		/// @brief Applies this object's processing step.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param vertices Vertex data defining the mesh.
		/// @param verticesCount Number of vertices.
		void Process(Core::IRenderer* renderer, SpriteVertex* vertices, size_t verticesCount);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI() override;

	public:
		/// @brief Increments the resource metadata's reference count when metadata exists.
		virtual void AddRefCount() override;
		/// @brief Decrements the resource metadata's reference count when metadata exists.
		virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11
