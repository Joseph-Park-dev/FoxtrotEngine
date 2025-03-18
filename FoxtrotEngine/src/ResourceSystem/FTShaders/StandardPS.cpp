#include "StandardPS.h"

#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "imgui.h"

	#include "FileSystem/BufferSizes.h"
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

StandardPS::StandardPS()
	: FTPixelShader()
	, mAmbient(FTVector3(0.1f))
	, mShininess(1.0f)
	, mDiffuse(FTVector3(0.5f))
	, mSpecular(FTVector3(0.5f))
{
}
//
//void StandardPS::SaveProperties(std::ofstream& ofs, UINT key)
//{
//	FileIOHelper::BeginDataPackSave(ofs, FileTypes::MATERIAL);
//
//	FTResource::SaveProperties(ofs, key);
//	FileIOHelper::SaveVector3(ofs, ChunkKey::Shader::AMBIENT, this->mAmbient);
//	FileIOHelper::SaveVector3(ofs, ChunkKey::Shader::SHININESS, this->mDiffuse);
//	FileIOHelper::SaveVector3(ofs, ChunkKey::Shader::DIFFUSE, this->mSpecular);
//	FileIOHelper::SaveFloat(ofs, ChunkKey::Shader::SPECULAR, this->mShininess);
//
//	FileIOHelper::EndDataPackSave(ofs, FileTypes::MATERIAL);
//}
//
//UINT StandardPS::LoadProperties(std::ifstream& ifs)
//{
//	FileIOHelper::BeginDataPackLoad(ifs, FileTypes::MATERIAL);
//
//	FileIOHelper::LoadVector3(ifs, this->mSpecular);
//	FileIOHelper::LoadVector3(ifs, this->mDiffuse);
//	FileIOHelper::LoadFloat(ifs, this->mShininess);
//	FileIOHelper::LoadVector3(ifs, this->mAmbient);
//
//	return FTResource::LoadProperties(ifs);
//}