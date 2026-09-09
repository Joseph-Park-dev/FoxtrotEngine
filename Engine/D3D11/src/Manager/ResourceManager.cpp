#include "ResourceSystem/Material/StandardMaterial.h"
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

/**
 * @file
 * ResourceManager.cpp
 *
 * The ResourceManager class is responsible for loading, storing,
 * and managing the lifetime of all game assets and resources,
 * such as textures, shaders, materials, sounds, and more.
 *
 * Resources are loaded from .chunk files (containing
 * serialized resources) and stored in hash maps for quick
 * lookup by other engine systems.
 *
 * Note: This file contains platform-specific code for Windows
 * (e.g., Direct3D 11) and is tightly coupled with the Foxtrot
 * engine's rendering and file I/O subsystems.
 */

#include "Manager/ResourceManager.h"

#include "ResourceSystem/D3D11Resource.h"
#include "FTDS/Static/FTString.h"
#include "FileSystem/NullKeys.h"
#include "Entity/Entity.h"
#include "Renderer/D3D11Renderer.h"
#include "Debugging/D3D11DebugFuncs.h"

#include "ResourceSystem/SupportedResources.h"

namespace D3D11
{
	constexpr const char* SHADERS	 = ".\\FoxtrotEngine\\Assets\\Shaders\\";
	constexpr const char* SHADERS_2D = ".\\FoxtrotEngine\\Assets\\Shaders\\2D\\";
	using namespace Common;
	using Microsoft::WRL::ComPtr;

	/// @brief Creates or loads the resources needed before scene content can be used.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
	void ResourceManager::LoadDefaultResources(D3D11::D3D11Renderer* renderer)
	{
        if (!mPSOs->GetResMap()->IsEmpty()) return;
        // The packs own default shaders; PSOs only borrow them.
        auto& VS = *mVertexShaders->GetResMap();
        auto& GS = *mGeometryShaders->GetResMap();
        auto& PS = *mPixelShaders->GetResMap();

		FTResourceDef resDef{
			Common::ChunkKey::NullVal::NULL_OBJECT, Common::ChunkKey::NullVal::NULL_OBJECT
		};

		///////////////////////////////
		///// Vertex Shader Setup /////
		///////////////////////////////

		VS.Reserve(5);

		// 2D sprite animation vertex shader
		Common::FTDS::String vsPath = SHADERS_2D;
		resDef.FileName				= "SpriteVS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		VS.Insert(
			resDef.FileName,
			DBG_NEW FTVertexShader(resDef, renderer));

		// 2D text renderer vertex shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "TextRenderer2DVS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		VS.Insert(
			resDef.FileName,
			DBG_NEW FTVertexShader(resDef, renderer));

		// 2D debug shape vertex shader
		vsPath			= SHADERS;
		resDef.FileName = "DebugShapeVS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		VS.Insert(
			resDef.FileName,
			DBG_NEW FTVertexShader(resDef, renderer));

		// 2D texture vertex shader
		vsPath			= SHADERS;
		resDef.FileName = "TextureVS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		VS.Insert(
			resDef.FileName,
			DBG_NEW FTVertexShader(resDef, renderer));

		// 2D spine animation vertex shader
		vsPath			= SHADERS;
		resDef.FileName = "TextureVSSpine.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		VS.Insert(
			resDef.FileName,
			DBG_NEW FTVertexShader(resDef, renderer));

		//////////////////////////////
		///// Pixel Shader Setup /////
		//////////////////////////////

		PS.Reserve(6);

		// 2D sprite animation pixel shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "SpritePS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		// 2D text renderer pixel shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "TextRenderer2DPS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		// 2D texture pixel shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "Texture2DPS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		// 2D spine animation pixel shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "Texture2DSpinePS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		// 2D debug shape pixel shader
		vsPath			= SHADERS;
		resDef.FileName = "DebugShapePS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		// 2D texture pixel shader
		vsPath			= SHADERS;
		resDef.FileName = "TexturePS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		PS.Insert(
			resDef.FileName,
			DBG_NEW FTPixelShader(resDef, renderer));

		/////////////////////////////////
		///// Geometry Shader Setup /////
		/////////////////////////////////

		GS.Reserve(2);

		// 2D sprite animation geometry shader
		vsPath			= SHADERS_2D;
		resDef.FileName = "SpriteGS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		GS.Insert(
			resDef.FileName,
			DBG_NEW FTGeometryShader(resDef, renderer));

		// 2D debug shape geometry shader
		vsPath			= SHADERS;
		resDef.FileName = "DebugShapeGS.hlsl";
		vsPath.Append(resDef.FileName);
		resDef.Path = vsPath.C_Str();

		GS.Insert(
			resDef.FileName,
			DBG_NEW FTGeometryShader(resDef, renderer));

		//////////////////////////////
		///// PSO Elements Setup /////
		//////////////////////////////

		ComPtr<ID3D11BlendState> spriteBS;

		D3D11_BLEND_DESC blendDesc = {};
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		blendDesc.AlphaToCoverageEnable	 = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;

		D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
		rtBlendDesc.BlendEnable						= TRUE;
		rtBlendDesc.SrcBlend						= D3D11_BLEND_ONE;
		rtBlendDesc.DestBlend						= D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOp							= D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha					= D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha					= D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOpAlpha					= D3D11_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask			= D3D11_COLOR_WRITE_ENABLE_ALL;

		DX::ThrowIfFailed(renderer->GetDevice()->CreateBlendState(&blendDesc, spriteBS.GetAddressOf()));

		ComPtr<ID3D11DepthStencilState> spriteDSS;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthStencilDesc.DepthEnable	= false; // false
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc		= D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		DX::ThrowIfFailed(renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, spriteDSS.GetAddressOf()));

		ComPtr<ID3D11RasterizerState> SolidRS;
		ComPtr<ID3D11RasterizerState> WireframeRS;

		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
															  // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rastDesc.FillMode			   = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rastDesc.CullMode			   = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthClipEnable	   = true;
		rastDesc.MultisampleEnable	   = true;

		DX::ThrowIfFailed(renderer->GetDevice()->CreateRasterizerState(&rastDesc, SolidRS.GetAddressOf()));

		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		DX::ThrowIfFailed(renderer->GetDevice()->CreateRasterizerState(&rastDesc, WireframeRS.GetAddressOf()));

		Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
		D3D11_SAMPLER_DESC						   sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD			= 0;
		sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

		// Create the Sample State
		renderer->GetDevice()->CreateSamplerState(&sampDesc, SamplerState.GetAddressOf());

		///////////////////////////
		///// Assembling PSOs /////
		///////////////////////////
		PSODef psoDef;
		{
			psoDef.FileName = "SpritePSO";
			psoDef.Path		= Common::ChunkKey::NullVal::NULL_OBJECT;

			psoDef.VS			= VS.At("SpriteVS.hlsl")->Value();
			psoDef.GS			= GS.At("SpriteGS.hlsl")->Value();
			psoDef.PS			= PS.At("SpritePS.hlsl")->Value();
			psoDef.BS			= spriteBS;
			psoDef.DSS			= spriteDSS;
			psoDef.RS			= SolidRS;
			psoDef.SS			= SamplerState;
			psoDef.PrimTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

			mPSOs->AddResource(psoDef.FileName, DBG_NEW D3D11PSO(psoDef));
		}

		{
			psoDef.FileName = "SpriteAnimPSO";
			psoDef.Path		= Common::ChunkKey::NullVal::NULL_OBJECT;

			psoDef.VS			= VS.At("SpriteVS.hlsl")->Value();
			psoDef.GS			= GS.At("SpriteGS.hlsl")->Value();
			psoDef.PS			= PS.At("SpritePS.hlsl")->Value();
			psoDef.BS			= spriteBS;
			psoDef.DSS			= spriteDSS;
			psoDef.RS			= SolidRS;
			psoDef.SS			= SamplerState;
			psoDef.PrimTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

			mPSOs->AddResource(psoDef.FileName, DBG_NEW D3D11PSO(psoDef));
		}
	}

	/// @brief Restores resource entries from a .chunk data pack.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param renderer Renderer providing the graphics device and current render state.
	void ResourceManager::LoadResourcesFromChunk(std::ifstream& ifs, void* renderer)
	{
		mSpriteAnimations->LoadResourcesFromChunk(ifs, renderer, [](Common::FTResourceDef& def, void* context) -> FTSpriteAnimation* { return new FTSpriteAnimation(def, static_cast<D3D11Renderer*>(context)); });
		mSpineAnimations->LoadResourcesFromChunk(ifs, renderer, [](Common::FTResourceDef& def, void* context) -> FTSpineAnimation* { return new FTSpineAnimation(def, static_cast<D3D11Renderer*>(context)); });
		mFonts->LoadResourcesFromChunk(ifs, renderer, [](Common::FTResourceDef& def, void* context) -> FTFont* { return new FTFont(def, static_cast<D3D11Renderer*>(context)); });
		mMaterials->LoadResourcesFromChunk(ifs, renderer, [](Common::FTResourceDef& def, void* context) -> FTMaterial* { return new StandardMaterial(def, static_cast<D3D11Renderer*>(context)); });
		mMeshGroups->LoadResourcesFromChunk(ifs, renderer);
		mVertexShaders->LoadResourcesFromChunk(ifs, renderer);
		mGeometryShaders->LoadResourcesFromChunk(ifs, renderer);
		mPixelShaders->LoadResourcesFromChunk(ifs, renderer);
		mSprites->LoadResourcesFromChunk(ifs, renderer);
		mTileMaps->LoadResourcesFromChunk(ifs);
		//mPSOs->LoadResourcesFromChunk(ifs, renderer);
	}

	/// @brief Returns the sprite animation used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the sprite animation.
	FTSpriteAnimation* ResourceManager::GetSpriteAnimation(Common::FTDS::String& key)
	{
		return mSpriteAnimations->GetResource(key);
	}

	/// @brief Returns the spine animation used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the spine animation.
	FTSpineAnimation* ResourceManager::GetSpineAnimation(Common::FTDS::String& key)
	{
		return mSpineAnimations->GetResource(key);
	}

	/// @brief Returns the font used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the font.
	FTFont* ResourceManager::GetFont(Common::FTDS::String& key)
	{
		return mFonts->GetResource(key);
	}

	/// @brief Returns the material used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the material.
	FTMaterial* ResourceManager::GetMaterial(Common::FTDS::String& key)
	{
		return mMaterials->GetResource(key);
	}

	/// @brief Returns the mesh group used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the mesh group.
	FTMeshGroup* ResourceManager::GetMeshGroup(Common::FTDS::String& key)
	{
		return mMeshGroups->GetResource(key);
	}

	/// @brief Returns the vertex shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the vertex shader.
	FTVertexShader* ResourceManager::GetVertexShader(Common::FTDS::String& key)
	{
		return mVertexShaders->GetResource(key);
	}

	/// @brief Returns the geometry shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the geometry shader.
	FTGeometryShader* ResourceManager::GetGeometryShader(Common::FTDS::String& key)
	{
		return mGeometryShaders->GetResource(key);
	}

	/// @brief Returns the pixel shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the pixel shader.
	FTPixelShader* ResourceManager::GetPixelShader(Common::FTDS::String& key)
	{
		return mPixelShaders->GetResource(key);
	}

	/// @brief Returns the sprite used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the sprite.
	FTSprite* ResourceManager::GetSprite(Common::FTDS::String& key)
	{
		return mSprites->GetResource(key);
	}

	/// @brief Returns the tile map used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the tile map.
	FTTileMap* ResourceManager::GetTileMap(Common::FTDS::String& key)
	{
		return mTileMaps->GetResource(key);
	}

	/// @brief Returns the pso used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the pso.
	D3D11PSO* ResourceManager::GetPSO(Common::FTDS::String& key)
	{
		return mPSOs->GetResource(key);
	}

	/// @brief Returns the sprite animation used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the sprite animation.
	FTSpriteAnimation* ResourceManager::GetSpriteAnimation(const char* key)
	{
		return mSpriteAnimations->GetResource(key);
	}

	/// @brief Returns the spine animation used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the spine animation.
	FTSpineAnimation* ResourceManager::GetSpineAnimation(const char* key)
	{
		return mSpineAnimations->GetResource(key);
	}

	/// @brief Returns the font used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the font.
	FTFont* ResourceManager::GetFont(const char* key)
	{
		return mFonts->GetResource(key);
	}

	/// @brief Returns the material used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the material.
	FTMaterial* ResourceManager::GetMaterial(const char* key)
	{
		return mMaterials->GetResource(key);
	}

	/// @brief Returns the mesh group used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the mesh group.
	FTMeshGroup* ResourceManager::GetMeshGroup(const char* key)
	{
		return mMeshGroups->GetResource(key);
	}

	/// @brief Returns the vertex shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the vertex shader.
	FTVertexShader* ResourceManager::GetVertexShader(const char* key)
	{
		return mVertexShaders->GetResource(key);
	}

	/// @brief Returns the geometry shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the geometry shader.
	FTGeometryShader* ResourceManager::GetGeometryShader(const char* key)
	{
		return mGeometryShaders->GetResource(key);
	}

	/// @brief Returns the pixel shader used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the pixel shader.
	FTPixelShader* ResourceManager::GetPixelShader(const char* key)
	{
		return mPixelShaders->GetResource(key);
	}

	/// @brief Returns the sprite used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the sprite.
	FTSprite* ResourceManager::GetSprite(const char* key)
	{
		return mSprites->GetResource(key);
	}

	/// @brief Returns the tile map used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the tile map.
	FTTileMap* ResourceManager::GetTileMap(const char* key)
	{
		return mTileMaps->GetResource(key);
	}

	/// @brief Returns the pso used by this resource manager.
	/// @param key Lookup key identifying the stored entry.
	/// @return Borrowed access to the pso.
	D3D11PSO* ResourceManager::GetPSO(const char* key)
	{
		return mPSOs->GetResource(key);
	}

	/// @brief Returns the sprite animations used by this resource manager.
	/// @return Borrowed access to the sprite animations.
	Common::ResourcePack<FTSpriteAnimation>* ResourceManager::GetSpriteAnimations()
	{
		return mSpriteAnimations;
	}

	/// @brief Returns the spine animations used by this resource manager.
	/// @return Borrowed access to the spine animations.
	Common::ResourcePack<FTSpineAnimation>* ResourceManager::GetSpineAnimations()
	{
		return mSpineAnimations;
	}

	/// @brief Returns the fonts used by this resource manager.
	/// @return Borrowed access to the fonts.
	Common::ResourcePack<FTFont>* ResourceManager::GetFonts()
	{
		return mFonts;
	}

	/// @brief Returns the materials used by this resource manager.
	/// @return Borrowed access to the materials.
	Common::ResourcePack<FTMaterial>* ResourceManager::GetMaterials()
	{
		return mMaterials;
	}

	/// @brief Returns the mesh groups used by this resource manager.
	/// @return Borrowed access to the mesh groups.
	Common::ResourcePack<FTMeshGroup>* ResourceManager::GetMeshGroups()
	{
		return mMeshGroups;
	}

	/// @brief Returns the vertex shaders used by this resource manager.
	/// @return Borrowed access to the vertex shaders.
	Common::ResourcePack<FTVertexShader>* ResourceManager::GetVertexShaders()
	{
		return mVertexShaders;
	}

	/// @brief Returns the geometry shaders used by this resource manager.
	/// @return Borrowed access to the geometry shaders.
	Common::ResourcePack<FTGeometryShader>* ResourceManager::GetGeometryShaders()
	{
		return mGeometryShaders;
	}

	/// @brief Returns the pixel shaders used by this resource manager.
	/// @return Borrowed access to the pixel shaders.
	Common::ResourcePack<FTPixelShader>* ResourceManager::GetPixelShaders()
	{
		return mPixelShaders;
	}

	/// @brief Returns the sprites used by this resource manager.
	/// @return Borrowed access to the sprites.
	Common::ResourcePack<FTSprite>* ResourceManager::GetSprites()
	{
		return mSprites;
	}

	/// @brief Returns the tile maps used by this resource manager.
	/// @return Borrowed access to the tile maps.
	Common::ResourcePack<FTTileMap>* ResourceManager::GetTileMaps()
	{
		return mTileMaps;
	}

	/// @brief Returns the psos used by this resource manager.
	/// @return Borrowed access to the psos.
	Common::ResourcePack<D3D11PSO>* ResourceManager::GetPSOs()
	{
		return mPSOs;
	}

	/// @brief Initializes the resource packs owned by the manager.
	/// @note Initializes the :ResourceManager base or delegates to its constructor.
	ResourceManager::ResourceManager()
	: mSpriteAnimations(new Common::ResourcePack<FTSpriteAnimation>(16))
	, mSpineAnimations(new Common::ResourcePack<FTSpineAnimation>(16))
	, mFonts(new Common::ResourcePack<FTFont>(16))
	, mMaterials(new Common::ResourcePack<FTMaterial>(16))
	, mMeshGroups(new Common::ResourcePack<FTMeshGroup>(16))
	, mVertexShaders(new Common::ResourcePack<FTVertexShader>(16))
	, mGeometryShaders(new Common::ResourcePack<FTGeometryShader>(16))
	, mPixelShaders(new Common::ResourcePack<FTPixelShader>(16))
	, mSprites(new Common::ResourcePack<FTSprite>(16))
	, mTileMaps(new Common::ResourcePack<FTTileMap>(16))
	, mPSOs(new Common::ResourcePack<D3D11PSO>(16))
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	ResourceManager::~ResourceManager()
	{
		delete mSpriteAnimations;
		delete mSpineAnimations;
		delete mTileMaps;
		delete mFonts;
		delete mMeshGroups;
		delete mMaterials;
		delete mPSOs;
		delete mVertexShaders;
		delete mGeometryShaders;
		delete mPixelShaders;
		delete mSprites;
	}
} // namespace D3D11
