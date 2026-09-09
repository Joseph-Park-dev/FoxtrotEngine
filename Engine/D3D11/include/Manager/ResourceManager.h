#pragma once
#include "Manager/ResourceManagerBase.h"

#include "Utility/SingletonMacro.h"
#include "ResourceSystem/ResourcePack.h"

namespace D3D11
{
	class FTSpriteAnimation;
	class FTSpineAnimation;
	class FTFont;
	class FTMaterial;
	class FTMeshGroup;
	class FTVertexShader;
	class FTGeometryShader;
	class FTPixelShader;
	class FTSprite;
	class FTTileMap;
	class D3D11PSO;
	class D3D11Renderer;

	class ResourceManager
	{
		SINGLETON(ResourceManager)
	public:
		/// @brief Creates or loads the resources needed before scene content can be used.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		void LoadDefaultResources(D3D11::D3D11Renderer* renderer);
		/// @brief Restores resource entries from a .chunk data pack.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param renderer Renderer providing the graphics device and current render state.
		void LoadResourcesFromChunk(std::ifstream& ifs, void* renderer);

	public:
		/// @brief Returns the sprite animation used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the sprite animation.
		FTSpriteAnimation* GetSpriteAnimation(Common::FTDS::String& key);
		/// @brief Returns the spine animation used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the spine animation.
		FTSpineAnimation*  GetSpineAnimation(Common::FTDS::String& key);
		/// @brief Returns the font used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the font.
		FTFont*			   GetFont(Common::FTDS::String& key);
		/// @brief Returns the material used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the material.
		FTMaterial*		   GetMaterial(Common::FTDS::String& key);
		/// @brief Returns the mesh group used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the mesh group.
		FTMeshGroup*	   GetMeshGroup(Common::FTDS::String& key);
		/// @brief Returns the vertex shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the vertex shader.
		FTVertexShader*	   GetVertexShader(Common::FTDS::String& key);
		/// @brief Returns the geometry shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the geometry shader.
		FTGeometryShader*  GetGeometryShader(Common::FTDS::String& key);
		/// @brief Returns the pixel shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the pixel shader.
		FTPixelShader*	   GetPixelShader(Common::FTDS::String& key);
		/// @brief Returns the sprite used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the sprite.
		FTSprite*		   GetSprite(Common::FTDS::String& key);
		/// @brief Returns the tile map used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the tile map.
		FTTileMap*		   GetTileMap(Common::FTDS::String& key);
		/// @brief Returns the pso used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the pso.
		D3D11PSO*		   GetPSO(Common::FTDS::String& key);

		/// @brief Returns the sprite animation used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the sprite animation.
		FTSpriteAnimation* GetSpriteAnimation(const char* key);
		/// @brief Returns the spine animation used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the spine animation.
		FTSpineAnimation*  GetSpineAnimation(const char* key);
		/// @brief Returns the font used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the font.
		FTFont*			   GetFont(const char* key);
		/// @brief Returns the material used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the material.
		FTMaterial*		   GetMaterial(const char* key);
		/// @brief Returns the mesh group used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the mesh group.
		FTMeshGroup*	   GetMeshGroup(const char* key);
		/// @brief Returns the vertex shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the vertex shader.
		FTVertexShader*	   GetVertexShader(const char* key);
		/// @brief Returns the geometry shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the geometry shader.
		FTGeometryShader*  GetGeometryShader(const char* key);
		/// @brief Returns the pixel shader used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the pixel shader.
		FTPixelShader*	   GetPixelShader(const char* key);
		/// @brief Returns the sprite used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the sprite.
		FTSprite*		   GetSprite(const char* key);
		/// @brief Returns the tile map used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the tile map.
		FTTileMap*		   GetTileMap(const char* key);
		/// @brief Returns the pso used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the pso.
		D3D11PSO*		   GetPSO(const char* key);

	public:
		/// @brief Returns the sprite animations used by this resource manager.
		/// @return Borrowed access to the sprite animations.
		Common::ResourcePack<FTSpriteAnimation>* GetSpriteAnimations();
		/// @brief Returns the spine animations used by this resource manager.
		/// @return Borrowed access to the spine animations.
		Common::ResourcePack<FTSpineAnimation>*	 GetSpineAnimations();
		/// @brief Returns the fonts used by this resource manager.
		/// @return Borrowed access to the fonts.
		Common::ResourcePack<FTFont>*			 GetFonts();
		/// @brief Returns the materials used by this resource manager.
		/// @return Borrowed access to the materials.
		Common::ResourcePack<FTMaterial>*		 GetMaterials();
		/// @brief Returns the mesh groups used by this resource manager.
		/// @return Borrowed access to the mesh groups.
		Common::ResourcePack<FTMeshGroup>*		 GetMeshGroups();
		/// @brief Returns the vertex shaders used by this resource manager.
		/// @return Borrowed access to the vertex shaders.
		Common::ResourcePack<FTVertexShader>*	 GetVertexShaders();
		/// @brief Returns the geometry shaders used by this resource manager.
		/// @return Borrowed access to the geometry shaders.
		Common::ResourcePack<FTGeometryShader>*	 GetGeometryShaders();
		/// @brief Returns the pixel shaders used by this resource manager.
		/// @return Borrowed access to the pixel shaders.
		Common::ResourcePack<FTPixelShader>*	 GetPixelShaders();
		/// @brief Returns the sprites used by this resource manager.
		/// @return Borrowed access to the sprites.
		Common::ResourcePack<FTSprite>*			 GetSprites();
		/// @brief Returns the tile maps used by this resource manager.
		/// @return Borrowed access to the tile maps.
		Common::ResourcePack<FTTileMap>*		 GetTileMaps();
		/// @brief Returns the psos used by this resource manager.
		/// @return Borrowed access to the psos.
		Common::ResourcePack<D3D11PSO>*			 GetPSOs();

	private:
		Common::ResourcePack<FTSpriteAnimation>* mSpriteAnimations;
		Common::ResourcePack<FTSpineAnimation>*	 mSpineAnimations;
		Common::ResourcePack<FTFont>*			 mFonts;
		Common::ResourcePack<FTMaterial>*		 mMaterials;
		Common::ResourcePack<FTMeshGroup>*		 mMeshGroups;
		Common::ResourcePack<FTVertexShader>*	 mVertexShaders;
		Common::ResourcePack<FTGeometryShader>*	 mGeometryShaders;
		Common::ResourcePack<FTPixelShader>*	 mPixelShaders;
		Common::ResourcePack<FTSprite>*			 mSprites;
		Common::ResourcePack<FTTileMap>*		 mTileMaps;
		Common::ResourcePack<D3D11PSO>*			 mPSOs;
	};
} // namespace D3D11
