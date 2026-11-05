#include "ModuleHost.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include <cstring>
#include <cstdio>
#include <stdexcept>

namespace
{
	bool IsPinnedModule(const char* name)
	{
		return !name || std::strcmp(name, "Core") == 0 || std::strcmp(name, "EngineRuntime") == 0 || std::strcmp(name, "Editor") == 0 || std::strcmp(name, "FoxtrotD3D11") == 0 || std::strcmp(name, "D3D11") == 0;
	}

	bool IsAbsoluteModulePath(const wchar_t* file)
	{
		if (!file || !file[0])
			return false;
		if (file[0] == L'\\' || file[0] == L'/')
			return true;
		const wchar_t drive	 = file[0];
		const bool	   letter = (drive >= L'A' && drive <= L'Z') || (drive >= L'a' && drive <= L'z');
		return letter && file[1] == L':';
	}
}

/// @brief Resolves the executable directory and initializes host services for DLL loading.
/// @note Initializes the :ModuleHost base or delegates to its constructor.
/// @throws std::runtime_error If the executable directory cannot be resolved.
ModuleHost::ModuleHost()
{
	// Resolves directory path
	auto path = std::make_unique<wchar_t[]>(32768);
	DWORD	 count = GetModuleFileNameW(nullptr, path.get(), 32768);
	if (!count || count == 32768)
		throw std::runtime_error("Cannot resolve executable directory");
	mDir.assign(path.get(), count);
	mDir.resize(mDir.find_last_of(L"\\/") + 1);

	mServices = { sizeof(mServices), this, [](void* self, const char* name) noexcept {
					return static_cast<ModuleHost*>(self)->Find(name);
				} };
}

/// @brief Releases the resources managed by this instance during destruction.
ModuleHost::~ModuleHost() { Shutdown(); }

/// @brief Looks up a previously loaded module by its exact name.
/// @param name Name used to identify the requested object or interface.
/// @return Borrowed descriptor, or nullptr for a null or unknown name.
const Foxtrot::ModuleAPI* ModuleHost::Find(const char* name) const noexcept
{
	if (!name)
		return nullptr;

	// Find entry with same ModuleAPI name.
	for (const auto& e : mEntries)
		if (e->API.Name && std::strcmp(name, e->API.Name) == 0)
			return &e->API;
	return nullptr;
}

/// @brief Loads a DLL beside the executable, validates its module ABI, and initializes it once.
/// @param file Source filename or module file to load.
/// @param expectedName Module name required by the host's ABI validation.
/// @return Borrowed module descriptor, valid until Shutdown() or host destruction.
/// @pre file and expectedName must be valid null-terminated strings.
/// @throws std::runtime_error If loading, ABI validation, or module initialization fails.
const Foxtrot::ModuleAPI* ModuleHost::Load(const wchar_t* file, const char* expectedName)
{
	if (auto existing = Find(expectedName))
		return existing;

	// An absolute path is a project plugin. A relative path is a DLL beside the executable.
	auto			   e	= std::make_unique<Entry>();
	const std::wstring path = IsAbsoluteModulePath(file) ? std::wstring(file) : (mDir + file);
	e->File					= path;
	e->Handle				= LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!e->Handle)
		throw std::runtime_error(std::string("Cannot load ") + expectedName + " (Win32 " + std::to_string(GetLastError()) + ")");

	// Initializing Module as an entry, feeding API data.
	try
	{
		// Getting a function ptr that fetches API from the module.
		auto get = reinterpret_cast<Foxtrot::GetModuleAPI>(GetProcAddress(e->Handle, "FtGetModuleAPI"));

		// Assigning API values into the entry.
		if (!get || get(Foxtrot::ModuleAbi, sizeof(e->API), &e->API) != Foxtrot::Status::Ok)
			throw std::runtime_error("Missing or incompatible module entry point");

		if (IsInvalidAPI(e->API, expectedName))
			throw std::runtime_error("Invalid module API descriptor");

		// Initializing entry.
		e->Initialized = true; // Shutdown must tolerate partial initialization.
		if (e->API.Initialize(e->API.Instance, &mServices) != Foxtrot::Status::Ok)
			throw std::runtime_error(std::string("Initialization failed: ") + expectedName);

		// Finalizing DLL loading.
		std::fprintf(stderr, "Loaded %s\n", expectedName);
		mEntries.push_back(std::move(e));
		return &mEntries.back()->API;
	}
	catch (...)
	{
		if (e->Initialized)
			e->API.Shutdown(e->API.Instance);
		if (e->API.Destroy)
			e->API.Destroy(e->API.Instance);
		FreeLibrary(e->Handle);
		throw;
	}
}
/// @brief Unloads one hosted plugin DLL and loads it again.
/// @param expectedName Module name required by the host's ABI validation.
/// @param fileOverride Absolute or host-relative DLL path. Null keeps the path used for the previous load.
/// @return Ok when the module was attached again. InvalidArgument when this host did not load it. Busy when the module must stay mapped. Failed when reload did not complete.
Foxtrot::Status ModuleHost::Reload(const char* expectedName, const wchar_t* fileOverride)
{
	if (!expectedName || !*expectedName)
		return Foxtrot::Status::InvalidArgument;
	if (IsPinnedModule(expectedName))
		return Foxtrot::Status::Busy;

	size_t index = mEntries.size();
	for (size_t i = 0; i < mEntries.size(); ++i)
	{
		if (mEntries[i]->API.Name && std::strcmp(mEntries[i]->API.Name, expectedName) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == mEntries.size())
		return Foxtrot::Status::InvalidArgument;

	const Foxtrot::ModuleAPI* runtime = Find("EngineRuntime");
	if (!runtime || !runtime->Query)
		return Foxtrot::Status::Failed;
	auto* control = static_cast<Foxtrot::RuntimeControl*>(runtime->Query(runtime->Instance, "RuntimeControl"));
	if (!control || !control->Detach || !control->Attach)
		return Foxtrot::Status::Failed;

	const std::wstring	   file		   = (fileOverride && fileOverride[0]) ? std::wstring(fileOverride) : mEntries[index]->File;
	const Foxtrot::ModuleAPI api	   = mEntries[index]->API;
	const bool			   initialized = mEntries[index]->Initialized;
	const HMODULE		   handle	   = mEntries[index]->Handle;

	if (control->Detach(expectedName) != Foxtrot::Status::Ok)
		return Foxtrot::Status::Failed;

	if (initialized && api.Shutdown)
		api.Shutdown(api.Instance);
	if (api.Destroy)
		api.Destroy(api.Instance);
	FreeLibrary(handle);
	mEntries.erase(mEntries.begin() + static_cast<std::ptrdiff_t>(index));

	try
	{
		Load(file.c_str(), expectedName);
	}
	catch (const std::exception& error)
	{
		std::fprintf(stderr, "Reload failed for %s: %s\n", expectedName, error.what());
		return Foxtrot::Status::Failed;
	}
	catch (...)
	{
		std::fprintf(stderr, "Reload failed for %s\n", expectedName);
		return Foxtrot::Status::Failed;
	}

	const Foxtrot::ModuleAPI* module = Find(expectedName);
	if (!module || !module->Query)
		return Foxtrot::Status::Failed;
	void* plugin = module->Query(module->Instance, "LegacyPlugin");
	if (!plugin || control->Attach(expectedName, plugin) != Foxtrot::Status::Ok)
		return Foxtrot::Status::Failed;

	std::fprintf(stderr, "Reloaded %s\n", expectedName);
	return Foxtrot::Status::Ok;
}

/// @brief Unloads one hosted plugin DLL and leaves it unmapped.
/// @param expectedName Module name required by the host's ABI validation.
/// @return Ok when the module was released. InvalidArgument when this host did not load it. Busy when the module must stay mapped. Failed when unload did not complete.
Foxtrot::Status ModuleHost::Unload(const char* expectedName)
{
	if (!expectedName || !*expectedName)
		return Foxtrot::Status::InvalidArgument;
	if (IsPinnedModule(expectedName))
		return Foxtrot::Status::Busy;

	size_t index = mEntries.size();
	for (size_t i = 0; i < mEntries.size(); ++i)
	{
		if (mEntries[i]->API.Name && std::strcmp(mEntries[i]->API.Name, expectedName) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == mEntries.size())
		return Foxtrot::Status::InvalidArgument;

	const Foxtrot::ModuleAPI* runtime = Find("EngineRuntime");
	if (!runtime || !runtime->Query)
		return Foxtrot::Status::Failed;
	auto* control = static_cast<Foxtrot::RuntimeControl*>(runtime->Query(runtime->Instance, "RuntimeControl"));
	if (!control || !control->Detach)
		return Foxtrot::Status::Failed;

	const Foxtrot::ModuleAPI api		  = mEntries[index]->API;
	const bool				 initialized = mEntries[index]->Initialized;
	const HMODULE			 handle		  = mEntries[index]->Handle;

	if (control->Detach(expectedName) != Foxtrot::Status::Ok)
		return Foxtrot::Status::Failed;

	if (initialized && api.Shutdown)
		api.Shutdown(api.Instance);
	if (api.Destroy)
		api.Destroy(api.Instance);
	FreeLibrary(handle);
	mEntries.erase(mEntries.begin() + static_cast<std::ptrdiff_t>(index));

	std::fprintf(stderr, "Unloaded %s\n", expectedName);
	return Foxtrot::Status::Ok;
}

/// @brief Shuts down, destroys, and unloads modules in reverse load order.
/// @note Invalidates all descriptors and service pointers obtained from these modules.
void ModuleHost::Shutdown() noexcept
{
	while (!mEntries.empty())
	{
		auto e = std::move(mEntries.back());
		mEntries.pop_back();
		if (e->Initialized)
			e->API.Shutdown(e->API.Instance);
		e->API.Destroy(e->API.Instance);
		std::fprintf(stderr, "Unloading %s\n", e->API.Name);
		FreeLibrary(e->Handle);
	}
}

bool ModuleHost::IsInvalidAPI(Foxtrot::ModuleAPI& API, const char* name)
{
	return API.Size != sizeof(API) || API.Abi != Foxtrot::ModuleAbi ||
		API.Build != Foxtrot::BuildAbi || API.PointerBytes != sizeof(void*) ||
		!API.Name || std::strcmp(API.Name, name) != 0 ||
		!API.Initialize || !API.Shutdown || !API.Destroy || !API.Query;
}
