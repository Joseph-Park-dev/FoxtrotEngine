#include "ResourceSystem/FTBasicMeshGroup.h"

#include <spine/spine.h>

class FTSpineAnimation : public FTBasicMeshGroup
{
public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

private:
	// Setup Pose Data (To be moved to ResourceManager)
	spine::String			   mAtlasPath;
	spine::String			   mJSONPath;
	spine::Atlas*			   mAtlas;
	spine::AnimationStateData* mAnimStateData;

	// InstanceData
	spine::Skeleton* mSkeleton;
};