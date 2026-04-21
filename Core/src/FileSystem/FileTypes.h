// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// File types (extensions) that are used in Foxtrot Engine.
/// </summary>

#pragma once
namespace Core
{
	namespace FileTypes
	{
		// Essential Files
		constexpr const char* CHUNK		  = ".chunk";
		constexpr const char* GDPACK	  = ".gdpack";
		constexpr const char* PLUGIN_DATA = ".plgdat";

		constexpr const char* ALL_FILE_FORMATS = ".premade"
												 ","
												 ".png, .jpeg, .jpg, .dds"
												 ","
												 ".dds"
												 ","
												 ".tilemap"
												 ","
												 ".spritesheet"
												 ","
												 ".csv"
												 ","
												 ".json"
												 ","
												 ".txt, .atlas"
												 ","
												 ".fbx, .stl"
												 ","
												 ".spriteanim"
												 ","
												 ".spineanim"
												 ","
												 ".hlsl"
												 ","
												 ".mat"
												 ","
												 ".shadermeta";
		// FTResources
		constexpr const char* PREMADE		   = ".premade";
		constexpr const char* TEXTURE		   = ".png, .jpeg, .jpg, .dds";
		constexpr const char* DDS_TEXTURE	   = ".dds";
		constexpr const char* TILEMAP		   = ".tilemap";
		constexpr const char* JSON_SHEET	   = ".jsonsheet";
		constexpr const char* ATLAS_SHEET	   = ".atlassheet";
		constexpr const char* CSV			   = ".csv";
		constexpr const char* JSON			   = ".json";
		constexpr const char* TEXT			   = ".txt, .atlas";
		constexpr const char* MESH			   = ".fbx, .stl";
		constexpr const char* SPRITE_ANIMATION = ".spriteanim";
		constexpr const char* SPINE_ANIMATION  = ".spineanim";
		constexpr const char* SHADER		   = ".hlsl";
		constexpr const char* MATERIAL		   = ".ftmat";
		constexpr const char* SHADER_META	   = ".shadermeta";

		constexpr const char* VERTEX_SHADER	  = "VS";
		constexpr const char* GEOMETRY_SHADER = "PS";
		constexpr const char* PIXEL_SHADER	  = "PS";

		constexpr const char* FONT		= ".fnt";
		constexpr const char* FONT_META = ".fontmeta";

		namespace Sound
		{
			constexpr const char* WAV = ".wav";
		}

		constexpr const size_t MAX_RES_TYPE_COUNT = 17;
		constexpr const size_t MAX_SPEC_LENGTH	  = 50;

		/*inline void GetFileTypesSpec(
			FTDS::DynamicArray<COMDLG_FILTERSPEC*>* specArr,
			const wchar_t*							label,
			const char*								fileTypes)
		{
			COMDLG_FILTERSPEC* spec					= DBG_NEW COMDLG_FILTERSPEC();
			FTDS::String					  types = fileTypes;

			FTDS::DynamicArray<FTDS::String> splitted;

			FTDS::String buf = FTDS::String(fileTypes);
			types.Split(", ", splitted);
			if (splitted.IsEmpty())
				splitted.PushBack(types.C_Str());

			for (size_t i = 0; i < splitted.GetSize(); ++i)
			{
				FTDS::String val = "*";
				val.Append(splitted.At(i));
				if (i < splitted.GetSize() - 1)
					val.Append(";");

				types.Append(val);
			}

			wchar_t wStrSpec[MAX_SPEC_LENGTH];
			types.AssignToWStr(wStrSpec, MAX_SPEC_LENGTH);
			spec->pszName = label;
			spec->pszSpec = wStrSpec;

			specArr->PushBack(spec);
		}*/
	} // namespace FileTypes
} // namespace Core