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

	class ResourceManager
	{
		SINGLETON(ResourceManager)
	public:
		void LoadDefaultResources(D3D11::D3D11Renderer* renderer);
		void LoadResourcesFromChunk(std::ifstream& ifs, void* renderer);

	public:
		FTSpriteAnimation* GetSpriteAnimation(Common::FTDS::String& key);
		FTSpineAnimation*  GetSpineAnimation(Common::FTDS::String& key);
		FTFont*			   GetFont(Common::FTDS::String& key);
		FTMaterial*		   GetMaterial(Common::FTDS::String& key);
		FTMeshGroup*	   GetMeshGroup(Common::FTDS::String& key);
		FTVertexShader*	   GetVertexShader(Common::FTDS::String& key);
		FTGeometryShader*  GetGeometryShader(Common::FTDS::String& key);
		FTPixelShader*	   GetPixelShader(Common::FTDS::String& key);
		FTSprite*		   GetSprite(Common::FTDS::String& key);
		FTTileMap*		   GetTileMap(Common::FTDS::String& key);
		D3D11PSO*		   GetPSO(Common::FTDS::String& key);

		FTSpriteAnimation* GetSpriteAnimation(const char* key);
		FTSpineAnimation*  GetSpineAnimation(const char* key);
		FTFont*			   GetFont(const char* key);
		FTMaterial*		   GetMaterial(const char* key);
		FTMeshGroup*	   GetMeshGroup(const char* key);
		FTVertexShader*	   GetVertexShader(const char* key);
		FTGeometryShader*  GetGeometryShader(const char* key);
		FTPixelShader*	   GetPixelShader(const char* key);
		FTSprite*		   GetSprite(const char* key);
		FTTileMap*		   GetTileMap(const char* key);
		D3D11PSO*		   GetPSO(const char* key);

	public:
		Common::ResourcePack<FTSpriteAnimation>* GetSpriteAnimations();
		Common::ResourcePack<FTSpineAnimation>*	 GetSpineAnimations();
		Common::ResourcePack<FTFont>*			 GetFonts();
		Common::ResourcePack<FTMaterial>*		 GetMaterials();
		Common::ResourcePack<FTMeshGroup>*		 GetMeshGroups();
		Common::ResourcePack<FTVertexShader>*	 GetVertexShaders();
		Common::ResourcePack<FTGeometryShader>*	 GetGeometryShaders();
		Common::ResourcePack<FTPixelShader>*	 GetPixelShaders();
		Common::ResourcePack<FTSprite>*			 GetSprites();
		Common::ResourcePack<FTTileMap>*		 GetTileMaps();
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