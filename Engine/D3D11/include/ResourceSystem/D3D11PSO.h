#pragma once
#include "ResourceSystem//D3D11Resource.h"

#include "wrl.h"

#include "ResourceSystem/Material/FTMaterial.h"

namespace D3D11
{
	class FTVertexShader;
	class FTGeometryShader;
	class FTPixelShader;

	struct PSODef : Common::FTResourceDef
	{
		FTVertexShader*	  VS;
		FTGeometryShader* GS;
		FTPixelShader*	  PS;

		Microsoft::WRL::ComPtr<ID3D11BlendState>		BS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSS;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	RS;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>		SS;
		// Microsoft::WRL::ComPtr<ID3D11HullShader>		mHullShader;
		// Microsoft::WRL::ComPtr<ID3D11DomainShader>	mDomainShader;

		float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		UINT  StencilRef	 = 0;

		D3D11_PRIMITIVE_TOPOLOGY PrimTopology =
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	class D3D11PSO : public D3D11Resource
	{
	public:
		static D3D11::ResType Type;

	public:
		// void operator=(const D3D11PSO& pso);
		/// @brief Tests the valid condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		bool IsValid();

	public:
		/// @brief Updates the to pipeline used by subsequent operations.
		/// @param context Replacement to pipeline.
		void SetToPipeline(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

	public:
		/// @brief Initializes the shader stages and state used by a rendering pipeline.
		/// @param resDef Resource definition containing the filename and source path.
		D3D11PSO(PSODef& resDef);

	private:
		D3D11::FTVertexShader*	 mVS;
		D3D11::FTGeometryShader* mGS;
		D3D11::FTPixelShader*	 mPS;

		Microsoft::WRL::ComPtr<ID3D11BlendState>		mBS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDSS;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>	mRS;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>		mSS;
		// Microsoft::WRL::ComPtr<ID3D11HullShader>		mHullShader;
		// Microsoft::WRL::ComPtr<ID3D11DomainShader>	mDomainShader;

		float mBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		UINT  mStencilRef	  = 0;

		D3D11_PRIMITIVE_TOPOLOGY mPrimTopology =
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		bool mWireFramed;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI() override;

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace D3D11PSO
		{
			constexpr const char* D3D11PSO = "D3D11PSO";
		}
	} // namespace ChunkKey
} // namespace D3D11
