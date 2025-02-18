#include "FTSpineExtension.h"

#include <spine/spine.h>

namespace spine
{
	spine::SpineExtension* getDefaultExtension()
	{
		return new spine::DefaultSpineExtension();
	}
} // namespace spine