#pragma once
#include "IMeshRenderer.h"

namespace Graphics
{
	class ISpineAnimator :
		public IMeshRenderer
	{
	public:
		virtual void Play(const int idx, bool isRepeated) = 0;
	};

	namespace ChunkKey
	{
		namespace SpineAnimator
		{
			constexpr const char* LOADED_ANIM = "Loaded Anim";
		}
	} // namespace ChunkKey
} // namespace Graphics