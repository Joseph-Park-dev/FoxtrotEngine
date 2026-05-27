#pragma once
#ifdef FOXTROT_EDITOR
	#include "Utility/EditorHelper.h"
	#include "FileSystem/DLLPath.h"

Editor::UPDATE_INT_VAL			Editor::gUpdateIntValueFunc			= reinterpret_cast<UPDATE_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateBoolValue"));
Editor::UPDATE_CLAMPED_INT_VAL	Editor::gUpdateClampedIntValueFunc	= reinterpret_cast<UPDATE_CLAMPED_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateClampedIntValue"));
Editor::UPDATE_UNSIGNED_INT_VAL Editor::gUpdateUnsignedIntValueFunc = reinterpret_cast<UPDATE_UNSIGNED_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateUnsignedIntValue"));
Editor::UPDATE_FLOAT_VAL		Editor::gUpdateFloatValueFunc		= reinterpret_cast<UPDATE_FLOAT_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateFloatValue"));
Editor::UPDATE_BOOL_VAL			Editor::gUpdateBoolValueFunc		= reinterpret_cast<UPDATE_BOOL_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateBoolValue"));
Editor::UPDATE_VEC2_VAL			Editor::gUpdateVector2ValueFunc		= reinterpret_cast<UPDATE_VEC2_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateVector2Value"));
Editor::UPDATE_VEC3_VAL			Editor::gUpdateVector3ValueFunc		= reinterpret_cast<UPDATE_VEC3_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateVector3Value"));
Editor::UPDATE_VEC4_VAL			Editor::gUpdateVector4ValueFunc		= reinterpret_cast<UPDATE_VEC4_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateVector4Value"));
Editor::UPDATE_STR_VAL			Editor::gUpdateStringValueFunc		= reinterpret_cast<UPDATE_STR_VAL>(GetProcAddress(GetModuleHandleA(DLLPath::EDITOR), "UpdateStringValue"));

#endif // FOXTROT_EDITOR