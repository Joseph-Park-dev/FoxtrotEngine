#include "ResourceSystem/D3D11PSO.h"

#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTGeometryShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "Manager/ResourceManager.h"

namespace D3D11
{
	ResType D3D11PSO::Type = ResType::D3D11_PSO;

	/// @brief Tests the valid condition for the current object.
	/// Returns false if any of the elements is null.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11PSO::IsValid()
	{
		return mVS && mGS && mPS && mBS && mDSS && mRS && mBlendFactor && mPrimTopology;
	}

	/// @brief Updates the to pipeline used by subsequent operations.
	/// Set PSO to the current context.
	/// This is called every frame inside the gameloop.
	/// @param context Replacement to pipeline.
	void D3D11PSO::SetToPipeline(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		context->VSSetShader(mVS->GetShader().Get(), 0, 0);
		context->IASetInputLayout(mVS->GetInputLayout().Get());
		context->GSSetShader(mGS->GetShader().Get(), 0, 0);
		context->PSSetShader(mPS->GetShader().Get(), 0, 0);

		context->OMSetBlendState(mBS.Get(), mBlendFactor, 0xffffffff);
		context->OMSetDepthStencilState(mDSS.Get(), mStencilRef);
		context->RSSetState(mRS.Get());
		context->PSSetSamplers(0, 1, mSS.GetAddressOf());

		context->IASetPrimitiveTopology(mPrimTopology);
	}

	/// @brief Initializes the shader stages and state used by a rendering pipeline.
	/// Initializes the PSO.
	/// Change of values after the initialization is NOT recommended.
	/// @param resDef Resource definition containing the filename and source path.
	/// @note Initializes the :D3D11PSO base or delegates to its constructor.
	D3D11PSO::D3D11PSO(PSODef& resDef)
		: D3D11::D3D11Resource(resDef)
		, mWireFramed(false)
	{
		mVS	 = resDef.VS;
		mGS	 = resDef.GS;
		mPS	 = resDef.PS;
		mBS	 = resDef.BS;
		mDSS = resDef.DSS;
		mRS	 = resDef.RS;
		mSS	 = resDef.SS;

		size_t blndFacSize = sizeof(float) * 4;
		memcpy_s(mBlendFactor, blndFacSize, resDef.BlendFactor, blndFacSize);

		mStencilRef	  = resDef.StencilRef;
		mPrimTopology = resDef.PrimTopology;
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void D3D11PSO::UpdateUI()
	{
		ImGui::SeparatorText("Vertex Shader");
		mVS->UpdateUI();
		Editor::UPDATE_BOOL("Wireframe", mWireFramed);

		//! mWireFramed ? mRS = ResourceManager::GetInstance()->GetRS()->At(0) : mRS = ResourceManager::GetInstance()->GetRS()->At(1);
	}
#endif
} // namespace D3D11
