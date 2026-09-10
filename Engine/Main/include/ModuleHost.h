#pragma once
#include "Foxtrot/Core/ModuleApi.h"
#include <Windows.h>
#include <memory>
#include <string>
#include <vector>

// Bootstrap code lives in the EXE, so it can unload the last DLL safely.
class ModuleHost
{
public:
	/// @brief Resolves the executable directory and initializes host services for DLL loading.
	/// @throws std::runtime_error If the executable directory cannot be resolved.
	ModuleHost();

	/// @brief Releases the resources managed by this instance during destruction.
	~ModuleHost();

	/// @brief Disables copying so the instance's managed state cannot be duplicated.
	/// @note Unnamed parameter (const ModuleHost&): reserved by this interface or unused by this implementation.
	ModuleHost(const ModuleHost&) = delete;

	/// @brief Disables assignment so managed instance state cannot be copied.
	/// @note Unnamed parameter (const ModuleHost&): reserved by this interface or unused by this implementation.
	/// @return No value; this overload is deleted and cannot be called.
	ModuleHost& operator=(const ModuleHost&) = delete;

	/// @brief Loads a DLL beside the executable, validates its module ABI, and initializes it once.
	/// @param file Source filename or module file to load.
	/// @param expectedName Module name required by the host's ABI validation.
	/// @return Borrowed module descriptor, valid until Shutdown() or host destruction.
	/// @pre file and expectedName must be valid null-terminated strings.
	/// @throws std::runtime_error If loading, ABI validation, or module initialization fails.
	const Foxtrot::ModuleAPI* Load(const wchar_t* file, const char* expectedName);

	/// @brief Looks up a previously loaded module by its exact name.
	/// @param name Name used to identify the requested object or interface.
	/// @return Borrowed descriptor, or nullptr for a null or unknown name.
	const Foxtrot::ModuleAPI* Find(const char* name) const noexcept;

	/// @brief Shuts down, destroys, and unloads modules in reverse load order.
	/// @note Invalidates all descriptors and service pointers obtained from these modules.
	void Shutdown() noexcept;

private:
	struct Entry
	{
		HMODULE			   Handle{};
		Foxtrot::ModuleAPI API{};
		bool			   Initialized{};
	};

	/// @brief Root directory path where the DLL is loaded from.
	std::wstring						mDir;
	std::vector<std::unique_ptr<Entry>> mEntries;
	Foxtrot::HostServices				mServices{};

private:
	bool IsInvalidAPI(Foxtrot::ModuleAPI& API, const char* name);
};