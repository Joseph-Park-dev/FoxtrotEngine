#pragma once
#include "Component/Graphics/IMeshRenderer.h"

namespace Core
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
} // namespace D3D11