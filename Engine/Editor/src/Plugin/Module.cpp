#include "Plugin/ModuleAdapter.h"
/// @brief Allocates the plugin implementation exported by this module.
/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
/// @return Created plugin instance or resource.
extern "C" Common::IPlugin* CreatePlugin(const char*);
/// @brief Constructs the requested object through the configured factory.
/// @return Created  instance or resource.
static Common::IPlugin* Create() { return CreatePlugin("Editor"); }
/// @brief Validates the requested ABI and publishes this module's lifecycle and service descriptor.
/// @param abi abi used by this operation.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @param out Receives the operation's output.
/// @return ABI status indicating success or the reason the descriptor could not be produced.
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleApi(uint32_t abi, uint32_t bytes, Foxtrot::ModuleApi* out) noexcept
{ return Foxtrot::MakePluginApi(abi, bytes, out, "Editor", Create); }
