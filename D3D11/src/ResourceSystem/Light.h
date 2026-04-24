#pragma once
#include "FileSystem/FileIOHelper.h"
#include "Math/FTMath.h"

#ifdef FOXTROT_EDITOR
	#include <CommandHistory.h>
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

		Math::FTVector3 Strength	 = Math::FTVector3(1.0f);
		float			FallOffStart = 0.0f;
		Math::FTVector3 Direction	 = Math::FTVector3(0.0f, 0.0f, 1.0f);
		float			FallOffEnd	 = 10.0f;
		Math::FTVector3 Position	 = Math::FTVector3(0.0f, 0.0f, -2.0f);
		float			SpotPower	 = 1.0f;

		void SaveProperties(std::ofstream& ofs, TYPE& type, bool& isActive)
		{
			Core::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::LIGHT);

			Core::FileIOHelper::SaveInt(ofs, LightKey::TYPE, type);
			Core::FileIOHelper::SaveBool(ofs, LightKey::IS_ACTIVE, isActive);
			Core::FileIOHelper::SaveVector3(ofs, LightKey::STRENGTH, Strength);
			Core::FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_START, FallOffStart);
			Core::FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_END, FallOffEnd);
			Core::FileIOHelper::SaveVector3(ofs, LightKey::DIRECTION, Direction);
			Core::FileIOHelper::SaveVector3(ofs, LightKey::POSITION, Position);
			Core::FileIOHelper::SaveFloat(ofs, LightKey::SPOT_POWER, SpotPower);

			Core::FileIOHelper::EndDataPackSave(ofs, ChunkKey::LIGHT);
		}

		void LoadProperties(std::ifstream& ifs, TYPE& type, bool& isActive)
		{
			Core::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::LIGHT);

			Core::FileIOHelper::LoadFloat(ifs, SpotPower);
			Core::FileIOHelper::LoadVector3(ifs, Position);
			Core::FileIOHelper::LoadVector3(ifs, Direction);
			Core::FileIOHelper::LoadFloat(ifs, FallOffEnd);
			Core::FileIOHelper::LoadFloat(ifs, FallOffStart);
			Core::FileIOHelper::LoadVector3(ifs, Strength);
			Core::FileIOHelper::LoadBool(ifs, isActive);

			int typeInt = TYPE::DIRECTIONAL;
			Core::FileIOHelper::LoadInt(ifs, typeInt);
			type = (TYPE)typeInt;
		}
#ifdef FOXTROT_EDITOR
		void UpdateUI()
		{
			Editor::CommandHistory::GetInstance()->UpdateVector3Value(LightKey::STRENGTH, Strength);
			Editor::CommandHistory::GetInstance()->UpdateFloatValue(LightKey::FALLOFF_START, FallOffStart);
			Editor::CommandHistory::GetInstance()->UpdateFloatValue(LightKey::FALLOFF_END, FallOffEnd);
			Editor::CommandHistory::GetInstance()->UpdateVector3Value(LightKey::DIRECTION, Direction);
			Editor::CommandHistory::GetInstance()->UpdateVector3Value(LightKey::POSITION, Position);
			Editor::CommandHistory::GetInstance()->UpdateFloatValue(LightKey::SPOT_POWER, SpotPower);
		}

#endif
	};

#ifdef FOXTROT_EDITOR
	namespace Editor
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
	} // namespace Editor
#endif // FOXTROT_EDITOR
} // namespace D3D11