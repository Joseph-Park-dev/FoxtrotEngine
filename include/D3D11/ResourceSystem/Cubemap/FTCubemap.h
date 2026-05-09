#pragma once
#include "ResourceSystem/Mesh/FTMeshGroup.h"

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	class FTCubemap :
		public FTMeshGroup
	{
	public:
		virtual void CalcVCData(Camera* camInst);

		// public:
		//	virtual void Initialize(
		//		std::vector<FTMeshData>&& meshes,
		//		ComPtr<ID3D11Device>& device,
		//		ComPtr<ID3D11DeviceContext>& context) override;
		//
		//	virtual void Render(FoxtrotRenderer* renderer) override;

	public:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDiffuseResView();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSpecularResView();

		void SetDiffuseTexture(Common::FTDS::String& key);
		void SetSpecularTexture(Common::FTDS::String& key);

		// protected:
		//	virtual void InitializeMeshes(
		//		ComPtr<ID3D11Device>& device,
		//		std::vector<FTMeshData>& meshes) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDiffuseResView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSpecularResView;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
#ifdef FOXTROT_EDITOR
		void UpdateUI();
#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace CubeMap
		{
			constexpr const char* FTCubeMap = "FTCubeMap";
		}
	} // namespace ChunkKey
} // namespace D3D11