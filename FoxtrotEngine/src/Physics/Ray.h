#pragma once
#include "Math/FTMath.h"

namespace Physics
{
	struct Ray
	{
		Ray(FTVector2 orig, FTVector2 end, ACTOR_GROUP group)
			:origin(orig), endPoint(end), groupFilter(group)
		{}
		FTVector2	origin;
		FTVector2	endPoint;
		ACTOR_GROUP groupFilter;
		float		mass;
		FTVector3	origin2Point;
		FTVector3	relativePosition;
		FTVector3	localInertia;
	};
}