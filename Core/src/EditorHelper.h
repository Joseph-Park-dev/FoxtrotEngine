#pragma once
#ifdef FOXTROT_EDITOR
	#include <CommandHistory.h>

namespace Editor
{
	extern CORE_API UPDATE_INT_VAL			gUpdateIntValueFunc;
	extern CORE_API UPDATE_CLAMPED_INT_VAL	gUpdateClampedIntValueFunc;
	extern CORE_API UPDATE_UNSIGNED_INT_VAL gUpdateUnsignedIntValueFunc;
	extern CORE_API UPDATE_FLOAT_VAL		gUpdateFloatValueFunc;
	extern CORE_API UPDATE_BOOL_VAL			gUpdateBoolValueFunc;
	extern CORE_API UPDATE_VEC2_VAL			gUpdateVector2ValueFunc;
	extern CORE_API UPDATE_VEC3_VAL			gUpdateVector3ValueFunc;
	extern CORE_API UPDATE_VEC4_VAL			gUpdateVector4ValueFunc;
	extern CORE_API UPDATE_STR_VAL			gUpdateStringValueFunc;

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