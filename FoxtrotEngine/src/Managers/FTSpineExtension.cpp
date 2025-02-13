#include "FTSpineExtension.h"

#include <spine/spine.h>
#include <spine/Extension.h>

#include "Debugging/DebugMemAlloc.h"

using namespace spine;

SpineExtension* spine::getDefaultExtension()
{
	return new DefaultSpineExtension();
}