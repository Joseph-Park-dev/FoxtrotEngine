#pragma once
#include "Math/FTMath.h"

namespace Physics
{
	struct Ray
	{
		Ray(FTVector2 orig, FTVector2 end, ActorGroup group)
			:origin(orig), endPoint(end), groupFilter(group)
		{}
		FTVector2	origin;
		FTVector2	endPoint;
		ActorGroup groupFilter;
		float		mass;
		FTVector3	origin2Point;
		FTVector3	relativePosition;
		FTVector3	localInertia;
	};
}