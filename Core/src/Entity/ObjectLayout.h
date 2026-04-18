#pragma once
#include "FTDS/Static/HashMap.h"

namespace Core
{
	// -------------------------------------------------------
	// Member type tags
	// -------------------------------------------------------
	enum class MemberType
	{
		DATA,
		FUNC_PTR,	  // stored free function pointer (last example)
		MEM_FUNC_PTR, // ← C++ member function pointer (this example)
		UNKNOWN
	};

	// -------------------------------------------------------
	// Raw storage for a member function pointer
	// Member function pointers are NOT regular pointers —
	// they can be 8–16 bytes depending on inheritance/virtual
	// We store them as a raw byte blob
	// -------------------------------------------------------
	struct MemberFuncStorage
	{
		unsigned char Bytes[16]; // large enough for any member func ptr
		size_t		  Size;		 // actual sizeof the stored mfp
	};

	// -------------------------------------------------------
	// Member descriptor — extended with mfp storage slot
	// -------------------------------------------------------
	struct MemberDesc
	{
		const char*		  Name;
		size_t			  Offset; // offsetof — for data members
		size_t			  Size;	  // sizeof
		MemberType		  Type;
		bool			  ReadOnly;
		MemberFuncStorage MFPtr; // ← stores the mfp blob (only for MemberFuncPtr)
	};

	// -------------------------------------------------------
	// Helper: build a MemberDesc for a member function pointer
	// Usage:
	//   MakeMemberFuncDesc("Heal", &Player::Heal)
	// -------------------------------------------------------
	template <typename MFP>
	MemberDesc MakeMemberFuncDesc(const char* name, MFP mfp)
	{
		MemberDesc desc = {};
		desc.Name		= name;
		desc.Offset		= 0; // not a data offset — mfp lives in mfpStorage
		desc.Size		= sizeof(MFP);
		desc.Type		= MemberType::MEM_FUNC_PTR;
		desc.ReadOnly	= true;

		// Store raw bytes of the member function pointer
		desc.MFPtr.Size = sizeof(MFP);
		memcpy(desc.MFPtr.Bytes, &mfp, sizeof(MFP));
		return desc;
	}

	// -------------------------------------------------------
	// Helper: retrieve a member function pointer by name
	// and call it on a concrete object instance
	//
	// Usage:
	//   auto fn = GetMemberFuncPtr<Player, void(int)>(layout, "Heal");
	//   if (fn) (player.*fn)(50);
	// -------------------------------------------------------
	template <typename ClassType, typename Signature>
	auto GetMemberFuncPtr(FTDS::HashMap<MemberDesc*>* members, const char* name)
		-> Signature ClassType::*
	{
		using MFP = Signature ClassType::*;

		const MemberDesc* m = members->At(name)->Value();
		if (!m)
		{
			fprintf(stderr, "[GetMemberFuncPtr] member '%s' not found\n", name);
			return nullptr;
		}

		if (m.Type != MemberType::MEM_FUNC_PTR)
		{
			fprintf(stderr, "[GetMemberFuncPtr] '%s' is not a MemberFuncPtr\n", name);
			return nullptr;
		}

		if (m.MFPtr.Size != sizeof(MFP))
		{
			fprintf(stderr, "[GetMemberFuncPtr] '%s' signature size mismatch"
							" (stored=%zu, requested=%zu)\n",
					name,
					m.MFPtr.Size,
					sizeof(MFP));
			return nullptr;
		}

		// Read the raw bytes back into the correct MFP type
		MFP mfp;
		memcpy(&mfp, m.MFPtr.Bytes, sizeof(MFP));
		return mfp;
	}

// -------------------------------------------------------
// Convenience macro — less verbose at call site
// Usage:
//   CALL_MEMBER_FUNC(layout, obj, Player, "Heal", void(int), 50);
// -------------------------------------------------------
#define CALL_MEMBER_FUNC(layout, obj, Class, name, Sig, ...)          \
	do                                                                \
	{                                                                 \
		auto _mfp = GetMemberFuncPtr<Class, Sig>(layout, name);       \
		if (_mfp)                                                     \
			(obj.*_mfp)(__VA_ARGS__);                                 \
		else                                                          \
			fprintf(stderr, "CALL_MEMBER_FUNC: '%s' failed\n", name); \
	}                                                                 \
	while (0)

	// -------------------------------------------------------
	// Function pointer types
	// -------------------------------------------------------
	typedef void* (*GetInstanceFn)();
	typedef void (*PrintStateFn)();

	// -------------------------------------------------------
	// Helper: find a member by name
	// -------------------------------------------------------
	inline const MemberDesc* FindMember(FTDS::HashMap<MemberDesc*>* members, const char* name)
	{
		return members->At(name)->Value();
	}

	// -------------------------------------------------------
	// Helper: get raw pointer to a member from object + descriptor
	// -------------------------------------------------------
	inline void* GetMemberPtr(void* obj, const MemberDesc* desc)
	{
		return (char*)obj + desc->Offset;
	}

// -------------------------------------------------------
// C API
// -------------------------------------------------------
#include "Plugin/CoreExports.h"

	CORE_API void*				 Plugin_GetInstance();
	CORE_API void				 Plugin_PrintState();
} // namespace Core