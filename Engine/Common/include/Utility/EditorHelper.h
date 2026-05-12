#pragma once
#ifdef FOXTROT_EDITOR
	#include "Plugin/CoreExports.h"
	#include "CommandHistory.h"

namespace Editor
{
	extern __declspec(dllexport) UPDATE_INT_VAL			 gUpdateIntValueFunc;
	extern __declspec(dllexport) UPDATE_CLAMPED_INT_VAL	 gUpdateClampedIntValueFunc;
	extern __declspec(dllexport) UPDATE_UNSIGNED_INT_VAL gUpdateUnsignedIntValueFunc;
	extern __declspec(dllexport) UPDATE_FLOAT_VAL		 gUpdateFloatValueFunc;
	extern __declspec(dllexport) UPDATE_BOOL_VAL		 gUpdateBoolValueFunc;
	extern __declspec(dllexport) UPDATE_VEC2_VAL		 gUpdateVector2ValueFunc;
	extern __declspec(dllexport) UPDATE_VEC3_VAL		 gUpdateVector3ValueFunc;
	extern __declspec(dllexport) UPDATE_VEC4_VAL		 gUpdateVector4ValueFunc;
	extern __declspec(dllexport) UPDATE_STR_VAL			 gUpdateStringValueFunc;

	#define UPDATE_INT(label, ref) gUpdateIntValueFunc(label, ref);
	#define UPDATE_CLAMPED_INT(label, ref, minVal, maxVal) gUpdateClampedIntValueFunc(label, ref, minVal, maxVal);
	#define UPDATE_UNSIGNED_INT(label, ref) gUpdateUnsignedIntValueFunc(label, ref);
	#define UPDATE_FLOAT(label, ref) gUpdateFloatValueFunc(label, ref);
	#define UPDATE_BOOL(label, ref) gUpdateBoolValueFunc(label, ref);
	#define UPDATE_VEC2(label, ref) gUpdateVector2ValueFunc(label, ref);
	#define UPDATE_VEC3(label, ref) gUpdateVector3ValueFunc(label, ref);
	#define UPDATE_VEC4(label, ref) gUpdateVector4ValueFunc(label, ref);
	#define UPDATE_STR(label, ref) gUpdateStringValueFunc(label, ref);

} // namespace Editor
#endif // FOXTROT_EDITOR