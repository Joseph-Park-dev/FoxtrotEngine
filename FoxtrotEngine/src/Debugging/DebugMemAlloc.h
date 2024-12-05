#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef FOXTROT_EDITOR
#define DBG_NEW new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif // DEBUG