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

namespace D3D11
{
	using namespace Common;
	void ResourceManager::LoadDefaultResources()
	{
		// Common::FTDS::HashMap<FTVertexShader*>	 VS;
		// Common::FTDS::HashMap<FTGeometryShader*> GS;
		// Common::FTDS::HashMap<FTPixelShader*>	 PS;

		// FTResourceDef resDef{
		//	::ChunkKey::NullVal::NULL_OBJECT, ::ChunkKey::NullVal::NULL_OBJECT
		// };

		/////////////////////////////////
		/////// Vertex Shader Setup /////
		/////////////////////////////////

		// VS.Reserve(5);

		//// 2D sprite animation vertex shader
		// Common::FTDS::String vsPath = Path::Resource::SHADERS_2D;
		// resDef.FileName		= "SpriteVS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// VS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTVertexShader(resDef, mRenderer));

		//// 2D text renderer vertex shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "TextRenderer2DVS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// VS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTVertexShader(resDef, mRenderer));

		//// 2D debug shape vertex shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "DebugShapeVS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// VS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTVertexShader(resDef, mRenderer));

		//// 2D texture vertex shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "TextureVS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// VS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTVertexShader(resDef, mRenderer));

		//// 2D spine animation vertex shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "TextureVSSpine.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// VS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTVertexShader(resDef, mRenderer));

		////////////////////////////////
		/////// Pixel Shader Setup /////
		////////////////////////////////

		// PS.Reserve(6);

		//// 2D sprite animation pixel shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "SpritePS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		//// 2D text renderer pixel shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "TextRenderer2DPS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		//// 2D texture pixel shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "Texture2DPS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		//// 2D spine animation pixel shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "Texture2DSpinePS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		//// 2D debug shape pixel shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "DebugShapePS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		//// 2D texture pixel shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "TexturePS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// PS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTPixelShader(resDef, mRenderer));

		///////////////////////////////////
		/////// Geometry Shader Setup /////
		///////////////////////////////////

		// GS.Reserve(2);

		//// 2D sprite animation geometry shader
		// vsPath			= Path::Resource::SHADERS_2D;
		// resDef.FileName = "SpriteGS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// GS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTGeometryShader(resDef, mRenderer));

		//// 2D debug shape geometry shader
		// vsPath			= Path::Resource::SHADERS;
		// resDef.FileName = "DebugShapeGS.hlsl";
		// vsPath.Append(resDef.FileName);
		// resDef.RelativePath = vsPath.C_Str();

		// GS.Insert(
		//	resDef.FileName,
		//	DBG_NEW FTGeometryShader(resDef, mRenderer));

		////////////////////////////////
		/////// PSO Elements Setup /////
		////////////////////////////////

		// ComPtr<ID3D11BlendState> spriteBS;

		// D3D11_BLEND_DESC blendDesc = {};
		// ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		// blendDesc.AlphaToCoverageEnable	 = FALSE;
		// blendDesc.IndependentBlendEnable = FALSE;

		// D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
		// rtBlendDesc.BlendEnable						= TRUE;
		// rtBlendDesc.SrcBlend						= D3D11_BLEND_ONE;
		// rtBlendDesc.DestBlend						= D3D11_BLEND_INV_SRC_ALPHA;
		// rtBlendDesc.BlendOp							= D3D11_BLEND_OP_ADD;
		// rtBlendDesc.SrcBlendAlpha					= D3D11_BLEND_ONE;
		// rtBlendDesc.DestBlendAlpha					= D3D11_BLEND_INV_SRC_ALPHA;
		// rtBlendDesc.BlendOpAlpha					= D3D11_BLEND_OP_ADD;
		// rtBlendDesc.RenderTargetWriteMask			= D3D11_COLOR_WRITE_ENABLE_ALL;

		// DX::ThrowIfFailed(mRenderer->GetDevice()->CreateBlendState(&blendDesc, spriteBS.GetAddressOf()));

		// ComPtr<ID3D11DepthStencilState> spriteDSS;

		// D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		// ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		// depthStencilDesc.DepthEnable	= false; // false
		// depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		// depthStencilDesc.DepthFunc		= D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		// DX::ThrowIfFailed(mRenderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, spriteDSS.GetAddressOf()));

		// mRS->Reserve(2);
		// ComPtr<ID3D11RasterizerState> SolidRS;
		// ComPtr<ID3D11RasterizerState> WireframeRS;

		// D3D11_RASTERIZER_DESC rastDesc;
		// ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
		//// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		// rastDesc.FillMode			   = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		// rastDesc.CullMode			   = D3D11_CULL_MODE::D3D11_CULL_NONE;
		// rastDesc.FrontCounterClockwise = false;
		// rastDesc.DepthClipEnable	   = true;
		// rastDesc.MultisampleEnable	   = true;

		// DX::ThrowIfFailed(mRenderer->GetDevice()->CreateRasterizerState(&rastDesc, SolidRS.GetAddressOf()));

		// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		// DX::ThrowIfFailed(mRenderer->GetDevice()->CreateRasterizerState(&rastDesc, WireframeRS.GetAddressOf()));

		// mRS->At(0) = SolidRS;
		// mRS->At(1) = WireframeRS;

		// D3D11_SAMPLER_DESC sampDesc;
		// ZeroMemory(&sampDesc, sizeof(sampDesc));
		// sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		// sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		// sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		// sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		// sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		// sampDesc.MinLOD			= 0;
		// sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

		//// Create the Sample State
		// mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

		/////////////////////////////
		/////// Assembling PSOs /////
		/////////////////////////////
		// PSODef psoDef;
		//{
		//	psoDef.FileName		= "SpritePSO";
		//	psoDef.RelativePath = ::ChunkKey::NullVal::NULL_OBJECT;

		//	psoDef.VS			= VS.At("SpriteVS.hlsl")->Value();
		//	psoDef.GS			= GS.At("SpriteGS.hlsl")->Value();
		//	psoDef.PS			= PS.At("SpritePS.hlsl")->Value();
		//	psoDef.BS			= spriteBS;
		//	psoDef.DSS			= spriteDSS;
		//	psoDef.RS			= SolidRS;
		//	psoDef.PrimTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

		//	mPSOs->Insert(psoDef.FileName, DBG_NEW D3D11PSO(psoDef));
		//}

		//{
		//	psoDef.FileName		= "SpriteAnimPSO";
		//	psoDef.RelativePath = ::ChunkKey::NullVal::NULL_OBJECT;

		//	psoDef.VS			= VS.At("SpriteVS.hlsl")->Value();
		//	psoDef.GS			= GS.At("SpriteGS.hlsl")->Value();
		//	psoDef.PS			= PS.At("SpritePS.hlsl")->Value();
		//	psoDef.BS			= spriteBS;
		//	psoDef.DSS			= spriteDSS;
		//	psoDef.RS			= SolidRS;
		//	psoDef.PrimTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

		//	mPSOs->Insert(psoDef.FileName, DBG_NEW D3D11PSO(psoDef));
		//}
	}

	FTSpriteAnimation* ResourceManager::GetSpriteAnimation(Common::FTDS::String& key)
	{
		return mSpriteAnimations->GetResource(key);
	}

	FTSpineAnimation* ResourceManager::GetSpineAnimation(Common::FTDS::String& key)
	{
		return mSpineAnimations->GetResource(key);
	}

	FTFont* ResourceManager::GetFont(Common::FTDS::String& key)
	{
		return mFonts->GetResource(key);
	}

	FTMaterial* ResourceManager::GetMaterial(Common::FTDS::String& key)
	{
		return mMaterials->GetResource(key);
	}

	FTMeshGroup* ResourceManager::GetMeshGroup(Common::FTDS::String& key)
	{
		return mMeshGroups->GetResource(key);
	}

	FTVertexShader* ResourceManager::GetVertexShader(Common::FTDS::String& key)
	{
		return mVertexShaders->GetResource(key);
	}

	FTGeometryShader* ResourceManager::GetGeometryShader(Common::FTDS::String& key)
	{
		return mGeometryShaders->GetResource(key);
	}

	FTPixelShader* ResourceManager::GetPixelShader(Common::FTDS::String& key)
	{
		return mPixelShaders->GetResource(key);
	}

	FTSprite* ResourceManager::GetSprite(Common::FTDS::String& key)
	{
		return mSprites->GetResource(key);
	}

	FTTileMap* ResourceManager::GetTileMap(Common::FTDS::String& key)
	{
		return mTileMaps->GetResource(key);
	}

	D3D11PSO* ResourceManager::GetPSO(Common::FTDS::String& key)
	{
		return mPSOs->GetResource(key);
	}

	FTSpriteAnimation* ResourceManager::GetSpriteAnimation(const char* key)
	{
		return mSpriteAnimations->GetResource(key);
	}

	FTSpineAnimation* ResourceManager::GetSpineAnimation(const char* key)
	{
		return mSpineAnimations->GetResource(key);
	}

	FTFont* ResourceManager::GetFont(const char* key)
	{
		return mFonts->GetResource(key);
	}

	FTMaterial* ResourceManager::GetMaterial(const char* key)
	{
		return mMaterials->GetResource(key);
	}

	FTMeshGroup* ResourceManager::GetMeshGroup(const char* key)
	{
		return mMeshGroups->GetResource(key);
	}

	FTVertexShader* ResourceManager::GetVertexShader(const char* key)
	{
		return mVertexShaders->GetResource(key);
	}

	FTGeometryShader* ResourceManager::GetGeometryShader(const char* key)
	{
		return mGeometryShaders->GetResource(key);
	}

	FTPixelShader* ResourceManager::GetPixelShader(const char* key)
	{
		return mPixelShaders->GetResource(key);
	}

	FTSprite* ResourceManager::GetSprite(const char* key)
	{
		return mSprites->GetResource(key);
	}

	FTTileMap* ResourceManager::GetTileMap(const char* key)
	{
		return mTileMaps->GetResource(key);
	}

	D3D11PSO* ResourceManager::GetPSO(const char* key)
	{
		return mPSOs->GetResource(key);
	}

	Common::ResourcePack<FTSpriteAnimation>* ResourceManager::GetSpriteAnimations()
	{
		return mSpriteAnimations;
	}

	Common::ResourcePack<FTSpineAnimation>* ResourceManager::GetSpineAnimations()
	{
		return mSpineAnimations;
	}

	Common::ResourcePack<FTFont>* ResourceManager::GetFonts()
	{
		return mFonts;
	}

	Common::ResourcePack<FTMaterial>* ResourceManager::GetMaterials()
	{
		return mMaterials;
	}

	Common::ResourcePack<FTMeshGroup>* ResourceManager::GetMeshGroups()
	{
		return mMeshGroups;
	}

	Common::ResourcePack<FTVertexShader>* ResourceManager::GetVertexShaders()
	{
		return mVertexShaders;
	}

	Common::ResourcePack<FTGeometryShader>* ResourceManager::GetGeometryShaders()
	{
		return mGeometryShaders;
	}

	Common::ResourcePack<FTPixelShader>* ResourceManager::GetPixelShaders()
	{
		return mPixelShaders;
	}

	Common::ResourcePack<FTSprite>* ResourceManager::GetSprites()
	{
		return mSprites;
	}

	Common::ResourcePack<FTTileMap>* ResourceManager::GetTileMaps()
	{
		return mTileMaps;
	}

	Common::ResourcePack<D3D11PSO>* ResourceManager::GetPSOs()
	{
		return mPSOs;
	}

	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
	}
} // namespace D3D11