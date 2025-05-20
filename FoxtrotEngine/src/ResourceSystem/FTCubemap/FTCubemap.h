#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

class FTCubemap :
	public FTBasicMeshGroup
{
public:
	virtual void CalcVCData(Camera* camInst);

public:
	virtual void Initialize(
		std::vector<FTMeshData>&& meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) override;

	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11ShaderResourceView>& GetDiffuseResView();
	ComPtr<ID3D11ShaderResourceView>& GetSpecularResView();

	void SetDiffuseTexture(FTDS::String& key);
	void SetSpecularTexture(FTDS::String& key);

protected:
	virtual void InitializeMeshes(
		ComPtr<ID3D11Device>& device, 
		std::vector<FTMeshData>& meshes) override;

private:
	ComPtr<ID3D11ShaderResourceView> mDiffuseResView;
	ComPtr<ID3D11ShaderResourceView> mSpecularResView;

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
}