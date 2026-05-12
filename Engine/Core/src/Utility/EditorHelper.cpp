#pragma once
#ifdef FOXTROT_EDITOR
	#include "Utility/EditorHelper.h"

	#include "DLLData.h"

Editor::UPDATE_INT_VAL			Editor::gUpdateIntValueFunc			= reinterpret_cast<UPDATE_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateBoolValue"));
Editor::UPDATE_CLAMPED_INT_VAL	Editor::gUpdateClampedIntValueFunc	= reinterpret_cast<UPDATE_CLAMPED_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateClampedIntValue"));
Editor::UPDATE_UNSIGNED_INT_VAL Editor::gUpdateUnsignedIntValueFunc = reinterpret_cast<UPDATE_UNSIGNED_INT_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateUnsignedIntValue"));
Editor::UPDATE_FLOAT_VAL		Editor::gUpdateFloatValueFunc		= reinterpret_cast<UPDATE_FLOAT_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateFloatValue"));
Editor::UPDATE_BOOL_VAL			Editor::gUpdateBoolValueFunc		= reinterpret_cast<UPDATE_BOOL_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateBoolValue"));
Editor::UPDATE_VEC2_VAL			Editor::gUpdateVector2ValueFunc		= reinterpret_cast<UPDATE_VEC2_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateVector2Value"));
Editor::UPDATE_VEC3_VAL			Editor::gUpdateVector3ValueFunc		= reinterpret_cast<UPDATE_VEC3_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateVector3Value"));
Editor::UPDATE_VEC4_VAL			Editor::gUpdateVector4ValueFunc		= reinterpret_cast<UPDATE_VEC4_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateVector4Value"));
Editor::UPDATE_STR_VAL			Editor::gUpdateStringValueFunc		= reinterpret_cast<UPDATE_STR_VAL>(GetProcAddress(GetModuleHandleA(DLLPaths::EDITOR), "UpdateStringValue"));

#endif // FOXTROT_EDITOR