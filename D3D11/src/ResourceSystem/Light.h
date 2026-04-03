#pragma once
#include <directxtk/SimpleMath.h>

#include "FileSystem/FileIOHelper.h"
#include "Math/FTMath.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	namespace LightKey
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
	} // namespace LightKey

	namespace ChunkKey
	{
		constexpr const char* LIGHT = "Light";
	}

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
			FileIOHelper::BeginDataPackSave(ofs, ChunkKey::LIGHT);

			FileIOHelper::SaveInt(ofs, LightKey::TYPE, type);
			FileIOHelper::SaveBool(ofs, LightKey::IS_ACTIVE, isActive);
			FileIOHelper::SaveVector3(ofs, LightKey::STRENGTH, Strength);
			FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_START, FallOffStart);
			FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_END, FallOffEnd);
			FileIOHelper::SaveVector3(ofs, LightKey::DIRECTION, Direction);
			FileIOHelper::SaveVector3(ofs, LightKey::POSITION, Position);
			FileIOHelper::SaveFloat(ofs, LightKey::SPOT_POWER, SpotPower);

			FileIOHelper::EndDataPackSave(ofs, ChunkKey::LIGHT);
		}

		void LoadProperties(std::ifstream& ifs, TYPE& type, bool& isActive)
		{
			FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::LIGHT);

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
			CommandHistory::GetInstance()->UpdateVector3Value(LightKey::STRENGTH, Strength);
			CommandHistory::GetInstance()->UpdateFloatValue(LightKey::FALLOFF_START, FallOffStart);
			CommandHistory::GetInstance()->UpdateFloatValue(LightKey::FALLOFF_END, FallOffEnd);
			CommandHistory::GetInstance()->UpdateVector3Value(LightKey::DIRECTION, Direction);
			CommandHistory::GetInstance()->UpdateVector3Value(LightKey::POSITION, Position);
			CommandHistory::GetInstance()->UpdateFloatValue(LightKey::SPOT_POWER, SpotPower);
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
					return LightKey::TypeStr::DIRECTIONAL;
				case Light::TYPE::POINT:
					return LightKey::TypeStr::POINT;
				case Light::TYPE::SPOT:
					return LightKey::TypeStr::SPOT;
				default:
					return LightKey::TypeStr::NULL_OBJ;
			}
		}
	} // namespace FTEditorUtils
#endif // FOXTROT_EDITOR
} // namespace D3D11