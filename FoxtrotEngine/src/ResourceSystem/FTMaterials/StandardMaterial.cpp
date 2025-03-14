#include "StandardMaterial.h"

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

template<typename SHADER>
void StandardMaterial<SHADER>::AssignData(MaterialData& matData)
{
	matData.Ambient = this->mAmbient;
	matData.Shininess = this->mShininess;
	matData.Diffuse = this->mDiffuse;
	matData.Specular = this->mSpecular;
}

template<typename SHADER>
void StandardMaterial<SHADER>::AssignNull(MaterialData& matData)
{
	matData.Ambient = FTVector3::Zero;
	matData.Shininess = 0.0f;
	matData.Diffuse = FTVector3::Zero;
	matData.Specular = FTVector3::Zero;
};

template<typename SHADER>
StandardMaterial<SHADER>::StandardMaterial()
	: FTMaterial()
	, mAmbient(FTVector3(0.1f))
	, mShininess(1.0f)
	, mDiffuse(FTVector3(0.5f))
	, mSpecular(FTVector3(0.5f))
{
}

template<typename SHADER>
void StandardMaterial<SHADER>::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave(ofs, FileTypes::MATERIAL);

	FTResource::SaveProperties(ofs, key);
	FileIOHelper::SaveVector3(ofs, ChunkKey::Material::AMBIENT, this->mAmbient);
	FileIOHelper::SaveVector3(ofs, ChunkKey::Material::SHININESS, this->mDiffuse);
	FileIOHelper::SaveVector3(ofs, ChunkKey::Material::DIFFUSE, this->mSpecular);
	FileIOHelper::SaveFloat(ofs, ChunkKey::Material::SPECULAR, this->mShininess);

	FileIOHelper::EndDataPackSave(ofs, FileTypes::MATERIAL);
}

template<typename SHADER>
UINT StandardMaterial<SHADER>::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, FileTypes::MATERIAL);

	FileIOHelper::LoadVector3(ifs, this->mSpecular);
	FileIOHelper::LoadVector3(ifs, this->mDiffuse);
	FileIOHelper::LoadFloat(ifs, this->mShininess);
	FileIOHelper::LoadVector3(ifs, this->mAmbient);

	return FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
template<typename SHADER>
void StandardMaterial<SHADER>::UpdateUI()
{
	ImGui::SeparatorText(ChunkKey::Material::TYPE);
	char name[BufferSize::STRING_BUFFER_SIZE];
	strcpy_s(name, BufferSize::STRING_BUFFER_SIZE, GetFileName().c_str());

	ImGui::InputText(ChunkKey::Material::NAME, name, BufferSize::STRING_BUFFER_SIZE);
	SetFileName(name);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::Material::AMBIENT, mAmbient);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKey::Material::SHININESS, mShininess);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::Material::DIFFUSE, mDiffuse);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::Material::SPECULAR, mSpecular);
	ImGui::Separator();
}
#endif
