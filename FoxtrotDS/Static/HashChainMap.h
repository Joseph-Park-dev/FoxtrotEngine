#pragma once
#include "Dynamic/RecordNode.h"
#include "Array.h"
#include "Dynamic/DynamicArray.h"
#include "Debugging/DebugFuncs.h"

#include <stdint.h>
#include <string.h>

namespace FTDS
{
	inline uint64_t GenerateHash_fnv1a_64(const char* str)
	{
		const uint64_t FNV_PRIME	= 0x100000001b3;
		const uint64_t OFFSET_BASIS = 0xcbf29ce484222325;

		uint64_t hash = OFFSET_BASIS;

		while (*str)
		{
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
} // namespace FTDS

namespace FTDS
{
	template <typename TYPE>
	class HashChainMap : public FTDS::Array<FTDS::RecordNode<TYPE>*>
	{
	public:
		void Insert(FTDS::String key, TYPE value)
		{
			// HashChainMap uses FTDS::Array,
			// the number of slots will not be dynamic,
			// whereas the linked list inside a slot will be.
			assert(0 < this->Capacity());

			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key.C_Str(), this->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
			{
				// Is there any nodes with the same key?
				if (p->Equal(key))
				{
					// Abort insertion.
					FTDS::String msg("Duplicated key: ");
					msg.Append(key);
					Debug::LogError(__LINE__, __FILE__, msg.C_Str());
					return;
				}
			}

			// Place the node as the 1st in the row.
			RecordNode<TYPE>* node = DBG_NEW RecordNode<TYPE>(key, value);
			node->SetLink(this->mData[hashVal]);
			this->mData[hashVal] = node;
			++mSize;
		}

		FTDS::RecordNode<TYPE>* At(FTDS::String& key)
		{
			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key.C_Str(), this->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
			{
				// Is there any nodes with the same key?
				if (p->Equal(key))
					return p;
			}

			Debug::LogError(__LINE__, __FILE__, (FTDS::String("Search Failed for key: ") + key).C_Str());
			return nullptr;
		}

		void Erase(const char* key)
		{
			RecordNode<TYPE>* node = At(key);
			delete node;
			node = nullptr;
			--mSize;
		}

	public:
		template <class UnaryOperation>
		void IterateAllNodes(
			UnaryOperation&& unaryOp)
		{
			for (size_t i = 0; i < this->Capacity(); ++i)
			{
				if (this->mData[i])
				{
					RecordNode<TYPE>* node = this->mData[i];
					while (node)
					{
						unaryOp(node);
						node = node->GetLink();
					}
				}
			}
		}

		template <class UnaryOperation>
		void IterateAllValues(
			UnaryOperation&& unaryOp)
		{
			if (this->mSize < 1)
				return;

			for (size_t i = 0; i < this->Capacity(); ++i)
			{
				if (this->mData)
				{
					RecordNode<TYPE>* node = this->mData[i];
					while (node != nullptr)
					{
						unaryOp(node->Value());
						node = node->GetLink();
					}
				}
			}
		}

		// Clear the linked nodes inside the array data.
		void Clear() override
		{
			if (mSize < 1)
				return;

			// Gather up all nodes which are not null.
			DynamicArray<RecordNode<TYPE>*> nodes;
			nodes.Reserve(mSize);

			// Iterate through all nodes, making them null.
			for (size_t i = 0; i < this->Capacity(); ++i)
			{
				if (this->mData[i])
				{
					RecordNode<TYPE>* node = this->mData[i];
					while (node)
					{
						RecordNode<TYPE>* curr = node;
						node = node->GetLink();
						delete curr;
						curr = nullptr;
					}
					this->mData[i] = nullptr;
				}
			}
			mSize = 0;
		}

	public:
		const size_t& GetSize() { return mSize; }
		bool		  IsEmpty() const { return mSize == 0; }

	public:
		HashChainMap()
			: FTDS::Array<RecordNode<TYPE>*>()
			, mSize(0)
		{
		}

		HashChainMap(size_t capacity)
			: FTDS::Array<RecordNode<TYPE>*>(capacity)
			, mSize(0)
		{
		}

		// Auto deletion of the RecordNodes inside each slot.
		~HashChainMap() override
		{
			Clear();
		}

	private:
		size_t mSize;
	};
} // namespace FTDS