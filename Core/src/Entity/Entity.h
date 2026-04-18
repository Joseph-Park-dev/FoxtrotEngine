#pragma once
#include "Entity/ObjectLayout.h"

namespace Core
{
	class Entity
	{
	public:
		Core::FTDS::HashMap<MemberDesc*>* GetMembers() { return mMembers; };

	protected:
		virtual void RegisterMemberFuncs() = 0;
		virtual ~Entity() { delete mMembers; };

	protected:
		void Initialize(size_t memberCount) { mMembers->Reserve(memberCount); }

	private:
		Core::FTDS::HashMap<MemberDesc*>* mMembers = nullptr;
	};
} // namespace Core