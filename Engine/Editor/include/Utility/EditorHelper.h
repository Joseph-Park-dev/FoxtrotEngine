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
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (int&): reserved by this interface or unused by this implementation.
	inline void UPDATE_INT(const char*, int&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (int&): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (int): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (int): reserved by this interface or unused by this implementation.
	inline void UPDATE_CLAMPED_INT(const char*, int&, int, int) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (unsigned int&): reserved by this interface or unused by this implementation.
	inline void UPDATE_UNSIGNED_INT(const char*, unsigned int&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (float&): reserved by this interface or unused by this implementation.
	inline void UPDATE_FLOAT(const char*, float&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (bool&): reserved by this interface or unused by this implementation.
	inline void UPDATE_BOOL(const char*, bool&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (Math::FTVector2&): reserved by this interface or unused by this implementation.
	inline void UPDATE_VEC2(const char*, Math::FTVector2&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (Math::FTVector3&): reserved by this interface or unused by this implementation.
	inline void UPDATE_VEC3(const char*, Math::FTVector3&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (Math::FTVector4&): reserved by this interface or unused by this implementation.
	inline void UPDATE_VEC4(const char*, Math::FTVector4&) {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (Common::FTDS::String&): reserved by this interface or unused by this implementation.
	inline void UPDATE_STR(const char*, Common::FTDS::String&) {}
} // namespace Editor
#endif // FOXTROT_EDITOR
