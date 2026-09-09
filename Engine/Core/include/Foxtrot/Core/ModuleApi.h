#pragma once
#include <cstdint>

// Only fixed-layout values, borrowed byte ranges and opaque pointers cross the
// bootstrap boundary. Never retain an API after its DLL has been released.
#define FT_CALL __cdecl
#define FT_EXPORT extern "C" __declspec(dllexport)

namespace Foxtrot
{
	constexpr uint32_t ModuleAbi = 1;
#if defined(FOXTROT_EDITOR)
	constexpr uint32_t BuildAbi = 3;
#elif defined(_DEBUG)
	constexpr uint32_t BuildAbi = 1;
#else
	constexpr uint32_t BuildAbi = 2;
#endif

	enum class Status : int32_t
	{
		Ok,
		InvalidArgument,
		AbiMismatch,
		Failed,
		Busy
	};

	enum class BuildConfig
	{
		Debug	= 1,
		Release = 2,
		Editor	= 3
	};

	struct ModuleApi;

	// Interface that the executable gives each DLL so the DLL can find other loaded modules
	struct HostServices
	{
		// The structure’s byte size, used for compatibility checking
		uint32_t size;

		// An opaque pointer owned by the executable.
		void* context;

		// A function pointer for looking up a module by name.
		const ModuleApi*(FT_CALL* find)(
			void*,
			const char*) noexcept;
	};

	/// @brief Standardized interface that every engine DLL exposes to the executable.
	/// Contains module metadata & func pointers.
	struct ModuleApi
	{
		/*======================================
		//======== Compatibility fields ========
		======================================*/

		/// @brief Size of the ModuleApi structure.
		/// It detects cases where the executable and DLL were compiled with different structure layouts.
		uint32_t size;

		/// @brief Version of the module interface protocol: If the host and DLL use different ABI versions, loading is rejected.
		uint32_t abi;

		/// @brief Build configuration.
		BuildConfig build;

		/// @brief Size of ponter in the DLL. This prevents mixing 32-bit and 64-bit modules.
		uint32_t pointerBytes;

		/*===========================================
		//======== Module identity and state ========
		===========================================*/

		/// @brief The module's stable name.
		const char* name;

		/// @brief An opaque pointer to the module’s private state.
		/// The executable does not need to know the actual C++ type.
		void* instance;

		/*=====================================
		//======== Lifecycle callbacks ========
		=====================================*/

		/// @brief Called after the DLL has been loaded and validated. 
		/// It initializes the module and gives it access to other loaded modules through HostServices.
		/// @param initialize 
		/// @return 
		Status(FT_CALL* initialize)(void*, const HostServices*) noexcept;
		void(FT_CALL* shutdown)(void*) noexcept;
		void(FT_CALL* destroy)(void*) noexcept;
		void*(FT_CALL* query)(void*, const char*) noexcept;
	};

	using GetModuleApi = Status(FT_CALL*)(uint32_t, uint32_t, ModuleApi*) noexcept;
} // namespace Foxtrot
