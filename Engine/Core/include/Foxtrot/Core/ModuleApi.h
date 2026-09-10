#pragma once
#include <cstdint>

// Only fixed-layout values, borrowed byte ranges and opaque pointers cross the
// bootstrap boundary. Never retain an API after its DLL has been released.
#define FT_CALL __cdecl
#define FT_EXPORT extern "C" __declspec(dllexport)

namespace Foxtrot
{
	enum BuildConfig
	{
		Debug	= 1,
		Release = 2,
		Editor	= 3
	};

	constexpr uint32_t ModuleAbi = 1;
#if defined(FOXTROT_EDITOR)
	constexpr uint32_t BuildAbi = BuildConfig::Editor;
#elif defined(_DEBUG)
	constexpr uint32_t BuildAbi = BuildConfig::Debug;
#else
	constexpr uint32_t BuildAbi = BuildConfig::Release;
#endif

	enum class Status : int32_t
	{
		Ok,
		InvalidArgument,
		AbiMismatch,
		Failed,
		Busy
	};

	struct ModuleAPI;

	// Interface that the executable gives each DLL so the DLL can find other loaded modules
	struct HostServices
	{
		// The structure’s byte size, used for compatibility checking
		uint32_t Size;

		// An opaque pointer owned by the executable.
		void* Context;

		// A function pointer for looking up a module by name.
		const ModuleAPI*(FT_CALL* Find)(
			void*,
			const char*) noexcept;
	};

	/// @brief Standardized interface that every engine DLL exposes to the executable.
	/// Contains module metadata & func pointers.
	struct ModuleAPI
	{
		/*======================================
		//======== Compatibility fields ========
		======================================*/

		/// @brief Size of the ModuleAPI structure.
		/// It detects cases where the executable and DLL were compiled with different structure layouts.
		uint32_t Size;

		/// @brief Version of the module interface protocol: If the host and DLL use different ABI versions, loading is rejected.
		uint32_t Abi;

		/// @brief Build configuration.
		uint32_t Build;

		/// @brief Size of ponter in the DLL. This prevents mixing 32-bit and 64-bit modules.
		uint32_t PointerBytes;

		/*===========================================
		//======== Module identity and state ========
		===========================================*/

		/// @brief The module's stable name.
		const char* Name;

		/// @brief An opaque pointer to the module’s private state.
		/// The executable does not need to know the actual C++ type.
		void* Instance;

		/*=====================================
		//======== Lifecycle callbacks ========
		=====================================*/

		/// @brief Called after the DLL has been loaded and validated.
		/// It initializes the module and gives it access to other loaded modules through HostServices.
		Status (FT_CALL* Initialize)(
			void*,
			const HostServices*) noexcept;

		/// @brief Stops the module and releases runtime resources. The module object may still exist afterward.
		void(FT_CALL* Shutdown)(void*) noexcept;
		void(FT_CALL* Destroy)(void*) noexcept;
		void*(FT_CALL* Query)(void*, const char*) noexcept;
	};

	using GetModuleAPI = Status(FT_CALL*)(uint32_t, uint32_t, ModuleAPI*) noexcept;
} // namespace Foxtrot
