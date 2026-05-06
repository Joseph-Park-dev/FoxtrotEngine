#include "SpriteAnimMat.h"

#include <d3d11.h>

#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "TemplateFunctions.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Manager/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "DirectoryHelper.h"
	#include "CommandHistory.h"
	#include "EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;
	using Microsoft::WRL::ComPtr;
	ResType SpriteAnimMat::Type = ResType::MATERIAL;

	void SpriteAnimMat::CreatePixelConstBuffer(ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateConstantBuffer(device, *mData, GetPCBuf());
	}

	void SpriteAnimMat::UpdateBuffer(ComPtr<ID3D11DeviceContext>& context)
	{
		D3D11Utils::UpdateBuffer(context, *mData, GetPCBuf());
	}

	void SpriteAnimMat::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SpriteAnimMat::SPRITE_ANIM_MAT);

		FileIOHelper::LoadVector4(ifs, mData->Color);
		FileIOHelper::LoadBool(ifs, mData->UseTexture);
	}

	SpriteAnimMat::SpriteAnimMat(Core::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTMaterial(resDef)
		, mData(DBG_NEW SpriteAnimMatData)
	{
		CreatePixelConstBuffer(renderer->GetDevice());
	}

	SpriteAnimMat::~SpriteAnimMat()
	{
		delete mData;
	}

	void SpriteAnimMat::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SpriteAnimMat::SPRITE_ANIM_MAT);

		FileIOHelper::SaveBool(ofs, ChunkKey::SpriteAnimMat::USE_TEXTURE, mData->UseTexture);
		FileIOHelper::SaveVector4(ofs, ChunkKey::SpriteAnimMat::COLOR, mData->Color);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::SpriteAnimMat::SPRITE_ANIM_MAT);
	}

#ifdef FOXTROT_EDITOR
	void SpriteAnimMat::UpdateUI()
	{
		Editor::UPDATE_VEC4("Color", mData->Color);
		Editor::UPDATE_BOOL("Use Texture", mData->UseTexture);
	}
#endif
} // namespace D3D11