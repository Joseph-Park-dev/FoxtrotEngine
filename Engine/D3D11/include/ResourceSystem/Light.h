#pragma once
#include "FileSystem/FileIOHelper.h"
#include "FTMath.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "Utility/EditorHelper.h"
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

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @param type Resource or component type identifier.
		/// @param isActive Whether the object participates in runtime processing.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs, TYPE& type, bool& isActive)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::LIGHT);

			Common::FileIOHelper::SaveInt(ofs, LightKey::TYPE, type);
			Common::FileIOHelper::SaveBool(ofs, LightKey::IS_ACTIVE, isActive);
			Common::FileIOHelper::SaveVector3(ofs, LightKey::STRENGTH, Strength);
			Common::FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_START, FallOffStart);
			Common::FileIOHelper::SaveFloat(ofs, LightKey::FALLOFF_END, FallOffEnd);
			Common::FileIOHelper::SaveVector3(ofs, LightKey::DIRECTION, Direction);
			Common::FileIOHelper::SaveVector3(ofs, LightKey::POSITION, Position);
			Common::FileIOHelper::SaveFloat(ofs, LightKey::SPOT_POWER, SpotPower);

			Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::LIGHT);
		}

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param type Resource or component type identifier.
		/// @param isActive Whether the object participates in runtime processing.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs, TYPE& type, bool& isActive)
		{
			Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::LIGHT);

			Common::FileIOHelper::LoadFloat(ifs, SpotPower);
			Common::FileIOHelper::LoadVector3(ifs, Position);
			Common::FileIOHelper::LoadVector3(ifs, Direction);
			Common::FileIOHelper::LoadFloat(ifs, FallOffEnd);
			Common::FileIOHelper::LoadFloat(ifs, FallOffStart);
			Common::FileIOHelper::LoadVector3(ifs, Strength);
			Common::FileIOHelper::LoadBool(ifs, isActive);

			int typeInt = TYPE::DIRECTIONAL;
			Common::FileIOHelper::LoadInt(ifs, typeInt);
			type = (TYPE)typeInt;
		}
#ifdef FOXTROT_EDITOR
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI()
		{
			::Editor::UPDATE_VEC3(LightKey::STRENGTH, Strength);
			::Editor::UPDATE_FLOAT(LightKey::FALLOFF_START, FallOffStart);
			::Editor::UPDATE_FLOAT(LightKey::FALLOFF_END, FallOffEnd);
			::Editor::UPDATE_VEC3(LightKey::DIRECTION, Direction);
			::Editor::UPDATE_VEC3(LightKey::POSITION, Position);
			::Editor::UPDATE_FLOAT(LightKey::SPOT_POWER, SpotPower);
		}

#endif
	};

#ifdef FOXTROT_EDITOR
	namespace Editor
	{
		/// @brief Converts a resource or component type identifier to its display text.
		/// @param type Resource or component type identifier.
		/// @return Converts a resource or component type identifier to its display text.
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
