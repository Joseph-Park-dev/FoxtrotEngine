#pragma once
#include "ResourceSystem/Mesh/FTMeshGroup.h"

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	class FTCubemap :
		public FTMeshGroup
	{
	public:


		// public:
		//	virtual void Initialize(
		//		std::vector<FTMeshData>&& meshes,
		//		ComPtr<ID3D11Device>& device,
		//		ComPtr<ID3D11DeviceContext>& context) override;
		//
		//	virtual void Render(FoxtrotRenderer* renderer) override;

	public:
		/// @brief Returns the diffuse res view used by this ftcubemap.
		/// @return Borrowed access to the diffuse res view.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDiffuseResView();
		/// @brief Returns the specular res view used by this ftcubemap.
		/// @return Borrowed access to the specular res view.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSpecularResView();

		/// @brief Updates the diffuse texture used by subsequent operations.
		/// @param key Replacement diffuse texture.
		void SetDiffuseTexture(Common::FTDS::String& key);
		/// @brief Updates the specular texture used by subsequent operations.
		/// @param key Replacement specular texture.
		void SetSpecularTexture(Common::FTDS::String& key);

		// protected:
		//	virtual void InitializeMeshes(
		//		ComPtr<ID3D11Device>& device,
		//		std::vector<FTMeshData>& meshes) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDiffuseResView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSpecularResView;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
#ifdef FOXTROT_EDITOR
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
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
