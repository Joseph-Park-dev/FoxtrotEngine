#include "Plugin/ModuleAdapter.h"
extern "C" Common::IPlugin* CreatePlugin(const char*);
static Common::IPlugin* Create() { return CreatePlugin("D3D11"); }
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleApi(uint32_t abi, uint32_t bytes, Foxtrot::ModuleApi* out) noexcept
{ return Foxtrot::MakePluginApi(abi, bytes, out, "D3D11", Create); }
