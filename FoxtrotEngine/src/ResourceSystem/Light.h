#pragma once
#include <directxtk/SimpleMath.h>

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/MaterialFileKeys.h"
#include "Math/FTMath.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

namespace LightKeys
{
	constexpr const char* NAME = "Light";
	constexpr const char* TYPE = "Type";

	namespace TypeStr
	{
		constexpr const char* NULL_OBJ	  = "Null";
		constexpr const char* DIRECTIONAL = "Directional Light";
		constexpr const char* POINT		  = "Point Light";
		constexpr const char* SPOT		  = "Spot Light";
	} // namespace TypeStr

	constexpr const char* STRENGTH		= "Strength";
	constexpr const char* FALLOFF_START = "Falloff Start";
	constexpr const char* FALLOFF_END	= "Falloff End";
	constexpr const char* DIRECTION		= "Direction";
	constexpr const char* POSITION		= "Position";
	constexpr const char* SPOT_POWER	= "Spot Power";
	constexpr const char* IS_ACTIVE		= "Is Active";
} // namespace LightKeys

struct Light
{
	enum TYPE
	{
		DIRECTIONAL,
		POINT,
		SPOT,

		END
	};

	FTVector3 Strength	   = FTVector3(1.0f);
	float	  FallOffStart = 0.0f;
	FTVector3 Direction	   = FTVector3(0.0f, 0.0f, 1.0f);
	float	  FallOffEnd   = 10.0f;
	FTVector3 Position	   = FTVector3(0.0f, 0.0f, -2.0f);
	float	  SpotPower	   = 1.0f;

	void SaveProperties(std::ofstream& ofs, TYPE& type, bool& isActive)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::LIGHT);

		FileIOHelper::SaveInt(ofs, LightKeys::TYPE, type);
		FileIOHelper::SaveBool(ofs, LightKeys::IS_ACTIVE, isActive);
		FileIOHelper::SaveVector3(ofs, LightKeys::STRENGTH, Strength);
		FileIOHelper::SaveFloat(ofs, LightKeys::FALLOFF_START, FallOffStart);
		FileIOHelper::SaveFloat(ofs, LightKeys::FALLOFF_END, FallOffEnd);
		FileIOHelper::SaveVector3(ofs, LightKeys::DIRECTION, Direction);
		FileIOHelper::SaveVector3(ofs, LightKeys::POSITION, Position);
		FileIOHelper::SaveFloat(ofs, LightKeys::SPOT_POWER, SpotPower);

		FileIOHelper::EndDataPackSave(ofs, ChunkKeys::LIGHT);
	}

	void LoadProperties(std::ifstream& ifs, TYPE& type, bool& isActive)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::LIGHT);

		FileIOHelper::LoadFloat(ifs, SpotPower);
		FileIOHelper::LoadVector3(ifs, Position);
		FileIOHelper::LoadVector3(ifs, Direction);
		FileIOHelper::LoadFloat(ifs, FallOffEnd);
		FileIOHelper::LoadFloat(ifs, FallOffStart);
		FileIOHelper::LoadVector3(ifs, Strength);
		FileIOHelper::LoadBool(ifs, isActive);

		int typeInt = TYPE::DIRECTIONAL;
		FileIOHelper::LoadInt(ifs, typeInt);
		type = (TYPE)typeInt;
	}
#ifdef FOXTROT_EDITOR
	void UpdateUI()
	{
		CommandHistory::GetInstance()->UpdateVector3Value(LightKeys::STRENGTH, Strength);
		CommandHistory::GetInstance()->UpdateFloatValue(LightKeys::FALLOFF_START, &FallOffStart);
		CommandHistory::GetInstance()->UpdateFloatValue(LightKeys::FALLOFF_END, &FallOffEnd);
		CommandHistory::GetInstance()->UpdateVector3Value(LightKeys::DIRECTION, Direction);
		CommandHistory::GetInstance()->UpdateVector3Value(LightKeys::POSITION, Position);
		CommandHistory::GetInstance()->UpdateFloatValue(LightKeys::SPOT_POWER, &SpotPower);
	}

#endif
};

#ifdef FOXTROT_EDITOR
namespace FTEditorUtils
{
	inline const char* TypeToString(Light::TYPE type)
	{
		switch (type)
		{
			case Light::TYPE::DIRECTIONAL:
				return LightKeys::TypeStr::DIRECTIONAL;
			case Light::TYPE::POINT:
				return LightKeys::TypeStr::POINT;
			case Light::TYPE::SPOT:
				return LightKeys::TypeStr::SPOT;
			default:
				return LightKeys::TypeStr::NULL_OBJ;
		}
	}
} // namespace FTEditorUtils
#endif // FOXTROT_EDITOR