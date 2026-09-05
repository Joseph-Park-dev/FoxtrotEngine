#pragma once

#ifdef FOXTROT_EDITOR
namespace Math
{
	class FTVector2;
	class FTVector3;
	class FTVector4;
} // namespace Math

namespace Common
{
	namespace FTDS
	{
		class String;
	}
} // namespace Common

namespace Editor
{
	inline void UPDATE_INT(const char*, int&) {}
	inline void UPDATE_CLAMPED_INT(const char*, int&, int, int) {}
	inline void UPDATE_UNSIGNED_INT(const char*, unsigned int&) {}
	inline void UPDATE_FLOAT(const char*, float&) {}
	inline void UPDATE_BOOL(const char*, bool&) {}
	inline void UPDATE_VEC2(const char*, Math::FTVector2&) {}
	inline void UPDATE_VEC3(const char*, Math::FTVector3&) {}
	inline void UPDATE_VEC4(const char*, Math::FTVector4&) {}
	inline void UPDATE_STR(const char*, Common::FTDS::String&) {}
} // namespace Editor
#endif // FOXTROT_EDITOR
