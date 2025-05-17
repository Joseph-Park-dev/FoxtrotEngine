#pragma once
#include "Dynamic/RecordNode.h"
#include "Array.h"
#include "Debugging/DebugFuncs.h"

#include <stdint.h>
#include <string.h>

namespace FTDS
{
	inline uint64_t GenerateHash_fnv1a_64(const char* str) {
		const uint64_t FNV_PRIME = 0x100000001b3;
		const uint64_t OFFSET_BASIS = 0xcbf29ce484222325;

		uint64_t hash = OFFSET_BASIS;

		while (*str) {
			hash ^= (uint8_t)*str++;
			hash *= FNV_PRIME;
		}

		return hash;
	}

	// Simple HashFunction using modular operator.
	inline size_t HashFunction(const char* key, size_t arrSize)
	{
		return GenerateHash_fnv1a_64(key) % arrSize;
	}

	// Sum the ASCII code numbers from the alphabets of the key.
	inline size_t Transform(const char* key)
	{
		size_t number = 0;
		while (*key)
			number += (*key++);
		return number;
	}
}

namespace FTDS
{
	template <typename TYPE>
	class HashChainMap
		: public FTDS::Array<FTDS::RecordNode<TYPE>*>
	{
	public:
		void Insert(const char* key, TYPE value)
		{
			// HashChainMap uses FTDS::Array,
			// the number of slots will not be dynamic, 
			// whereas the linked list inside a slot will be. 
			assert(0 < this->Capacity());

			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key, this->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
			{
				// Is there any nodes with the same key?
				if (p->Equal(key))
				{
					// Abort insertion.
					std::string msg = std::string("Duplicated key") + key;
					Debug::LogError(__LINE__, __FILE__, msg);
					return;
				}
			}

			// Place the node as the 1st in the row.
			RecordNode<TYPE>* node = DBG_NEW RecordNode<TYPE>(key, value);
			node->SetLink(this->mData[hashVal]);
			this->mData[hashVal] = node;
		}

		TYPE At(const char* key)
		{
			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key, this->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
			{
				// Is there any nodes with the same key?
				if (p->Equal(key))
					return p->Value();
			}

			Debug::LogError(__LINE__, __FILE__, std::string("Search Failed for key: ") + key);
			return NULL;
		}

		void Erase(const char* key)
		{
			RecordNode<TYPE>* node = At(key);
			delete node;
			node = nullptr;
		}

	public:
		HashChainMap()
			: FTDS::Array<RecordNode<TYPE>*>()
		{
		}

		HashChainMap(size_t capacity)
			: FTDS::Array<RecordNode<TYPE>*>(capacity)
		{
		}

		// Auto deletion of the RecordNodes inside each slot.
		~HashChainMap()
		{
			for (size_t i=0; i < this->Capacity(); ++i)
			{
				if (this->mData[i])
				{
					RecordNode<TYPE>* p = this->mData[i];
					while (p != nullptr)
					{
						RecordNode<TYPE>* temp = p;
						p = p->GetLink();
						delete temp;
						temp = nullptr;
					}
				}
			}
		}
	};
} // namespace FTDS