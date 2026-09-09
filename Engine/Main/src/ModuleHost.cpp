#include "ModuleHost.h"
#include <cstring>
#include <cstdio>
#include <stdexcept>

/// @brief Resolves the executable directory and initializes host services for DLL loading.
/// @note Initializes the :ModuleHost base or delegates to its constructor.
/// @throws std::runtime_error If the executable directory cannot be resolved.
ModuleHost::ModuleHost()
{
	// Resolves directory path
	wchar_t* path  = new wchar_t[32768];
	DWORD	 count = GetModuleFileNameW(nullptr, path, 32768);
	if (!count || count == 32768)
		throw std::runtime_error("Cannot resolve executable directory");
	mDir.assign(path, count);
	mDir.resize(mDir.find_last_of(L"\\/") + 1);

	services = { sizeof(services), this, [](void* self, const char* name) noexcept {
					return static_cast<ModuleHost*>(self)->Find(name);
				} };
	delete path;
}
/// @brief Releases the resources managed by this instance during destruction.
ModuleHost::~ModuleHost() { Shutdown(); }

/// @brief Looks up a previously loaded module by its exact name.
/// @param name Name used to identify the requested object or interface.
/// @return Borrowed descriptor, or nullptr for a null or unknown name.
const Foxtrot::ModuleApi* ModuleHost::Find(const char* name) const noexcept
{
	if (!name)
		return nullptr;

	// Find entry with same ModuleAPI name.
	for (const auto& e : entries)
		if (e->api.name && std::strcmp(name, e->api.name) == 0)
			return &e->api;
	return nullptr;
}
/// @brief Loads a DLL beside the executable, validates its module ABI, and initializes it once.
/// @param file Source filename or module file to load.
/// @param expectedName Module name required by the host's ABI validation.
/// @return Borrowed module descriptor, valid until Shutdown() or host destruction.
/// @pre file and expectedName must be valid null-terminated strings.
/// @throws std::runtime_error If loading, ABI validation, or module initialization fails.
const Foxtrot::ModuleApi* ModuleHost::Load(const wchar_t* file, const char* expectedName)
{
	if (auto existing = Find(expectedName))
		return existing;

	// Load DLL
	auto	   e	= std::make_unique<Entry>();
	const auto path = mDir + file;
	e->handle		= LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!e->handle)
		throw std::runtime_error(std::string("Cannot load ") + expectedName + " (Win32 " + std::to_string(GetLastError()) + ")");

	try
	{
		auto get = reinterpret_cast<Foxtrot::GetModuleApi>(GetProcAddress(e->handle, "FtGetModuleApi"));
		if (!get || get(Foxtrot::ModuleAbi, sizeof(e->api), &e->api) != Foxtrot::Status::Ok)
			throw std::runtime_error("Missing or incompatible module entry point");
		if (e->api.size != sizeof(e->api) || e->api.abi != Foxtrot::ModuleAbi ||
			e->api.build != Foxtrot::BuildAbi || e->api.pointerBytes != sizeof(void*) ||
			!e->api.name || std::strcmp(e->api.name, expectedName) != 0 ||
			!e->api.initialize || !e->api.shutdown || !e->api.destroy || !e->api.query)
			throw std::runtime_error("Invalid module API descriptor");
		e->initialized = true; // Shutdown must tolerate partial initialization.
		if (e->api.initialize(e->api.instance, &services) != Foxtrot::Status::Ok)
			throw std::runtime_error(std::string("Initialization failed: ") + expectedName);
		std::fprintf(stderr, "Loaded %s\n", expectedName);
		entries.push_back(std::move(e));
		return &entries.back()->api;
	}
	catch (...)
	{
		if (e->initialized)
			e->api.shutdown(e->api.instance);
		if (e->api.destroy)
			e->api.destroy(e->api.instance);
		FreeLibrary(e->handle);
		throw;
	}
}
/// @brief Shuts down, destroys, and unloads modules in reverse load order.
/// @note Invalidates all descriptors and service pointers obtained from these modules.
void ModuleHost::Shutdown() noexcept
{
	while (!entries.empty())
	{
		auto e = std::move(entries.back());
		entries.pop_back();
		if (e->initialized)
			e->api.shutdown(e->api.instance);
		e->api.destroy(e->api.instance);
		std::fprintf(stderr, "Unloading %s\n", e->api.name);
		FreeLibrary(e->handle);
	}
}
