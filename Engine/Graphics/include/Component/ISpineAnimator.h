#pragma once
#include "IMeshRenderer.h"

namespace Graphics
{
	class ISpineAnimator :
		public IMeshRenderer
	{
	public:
		/// @brief Starts or selects animation playback.
		/// @param idx Zero-based element index.
		/// @param isRepeated Whether playback repeats after its final frame.
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
