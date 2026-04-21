#include "D3D11PSO.h"

#include "Shader/FTVertexShader.h"
#include "Shader/FTGeometryShader.h"
#include "Shader/FTPixelShader.h"
#include "Manager/ResourceManager.h"

#ifdef FOXTROT_EDITOR
#include 

#endif

namespace D3D11
{
	ResType D3D11PSO::Type = ResType::D3D11_PSO;

	/// <summary>
	/// Returns false if any of the elements is null.
	/// </summary>
	bool D3D11PSO::IsValid()
	{
		return mVS && mGS && mPS && mBS && mDSS && mRS && mBlendFactor && mPrimTopology;
	}

	/// <summary>
	/// Set PSO to the current context.
	/// This is called every frame inside the gameloop.
	/// </summary>
	void D3D11PSO::SetToPipeline(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		context->VSSetShader(mVS->GetShader().Get(), 0, 0);
		context->IASetInputLayout(mVS->GetInputLayout().Get());
		context->GSSetShader(mGS->GetShader().Get(), 0, 0);
		context->PSSetShader(mPS->GetShader().Get(), 0, 0);

		context->OMSetBlendState(mBS.Get(), mBlendFactor, 0xffffffff);
		context->OMSetDepthStencilState(mDSS.Get(), mStencilRef);
		context->RSSetState(mRS.Get());

		context->IASetPrimitiveTopology(mPrimTopology);
	}

	/// <summary>
	/// Initializes the PSO.
	/// Change of values after the initialization is NOT recommended.
	/// </summary>
	D3D11PSO::D3D11PSO(PSODef& resDef)
		: mWireFramed(false)
	{
		mVS	 = resDef.VS;
		mGS	 = resDef.GS;
		mPS	 = resDef.PS;
		mBS	 = resDef.BS;
		mDSS = resDef.DSS;
		mRS	 = resDef.RS;

		size_t blndFacSize = sizeof(float) * 4;
		memcpy_s(mBlendFactor, blndFacSize, resDef.BlendFactor, blndFacSize);

		mStencilRef	  = resDef.StencilRef;
		mPrimTopology = resDef.PrimTopology;
	}

#ifdef FOXTROT_EDITOR
	void D3D11PSO::UpdateUI()
	{
		ImGui::SeparatorText("Vertex Shader");
		mVS->UpdateUI();
		CommandHistory::GetInstance()->UpdateBoolValue("Wireframe", mWireFramed);

		!mWireFramed ? mRS = ResourceManager::GetInstance()->GetRS()->At(0) : mRS = ResourceManager::GetInstance()->GetRS()->At(1);
	}
#endif
} // namespace D3D11