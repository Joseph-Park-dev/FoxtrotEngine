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

	mServices = { sizeof(mServices), this, [](void* self, const char* name) noexcept {
					return static_cast<ModuleHost*>(self)->Find(name);
				} };
	delete path;
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

	// Loading DLL
	auto	   e	= std::make_unique<Entry>();
	const auto path = mDir + file;
	e->Handle		= LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!e->Handle)
		throw std::runtime_error(std::string("Cannot load ") + expectedName + " (Win32 " + std::to_string(GetLastError()) + ")");

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
