#pragma once
#include "RecordNode.h"
#include "Array.h"
#include "Debugging/DebugFuncs.h"

namespace FTDS
{
	// Sum the ASCII code numbers from the alphabets of the key.
	inline size_t Transform(const char* key)
	{
		size_t number = 0;
		while (*key)
			number += (*key++);
		return number;
	}

	// Simple HashFunction using modular operator.
	inline size_t HashFunction(const char* key, size_t arrSize)
	{
		return Transform(key) % arrSize;
	}
}

namespace FTDS
{
	template <typename TYPE>
	class HashChainMap
	{
	public:
		void Insert(const char* key, TYPE value)
		{
			// HashChainMap uses FTDS::Array,
			// the number of slots will not be dynamic, 
			// whereas the linked list inside a slot will be. 
			assert(0 < mDataArray->Capacity());

			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key, mDataArray->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = mDataArray->Data()[hashVal]; p != nullptr; p = p->GetLink())
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
			node->SetLink(mDataArray->Data()[hashVal]);
			mDataArray->Data()[hashVal] = node;
		}

		TYPE At(const char* key)
		{
			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key, mDataArray->Capacity());

			// Traverse throught the linked list inside a slot.
			for (RecordNode<TYPE>* p = mDataArray->Data()[hashVal]; p != nullptr; p = p->GetLink())
			{
				// Is there any nodes with the same key?
				if (p->Equal(key))
					return p->Value();
			}

			Debug::LogError(__LINE__, __FILE__, std::string("Search Failed for key: ") + key);
			return NULL;
		}

	public:
		HashChainMap()
			: mDataArray(DBG_NEW FTDS::Array<RecordNode<TYPE>*>)
		{
		}

		HashChainMap(size_t capacity)
			: mDataArray(DBG_NEW FTDS::Array<RecordNode<TYPE>*>(capacity))
		{
		}

		// Auto deletion of the RecordNodes inside each slot.
		~HashChainMap()
		{
			for (size_t i=0; i < mDataArray->Capacity(); ++i)
			{
				if (mDataArray->Data()[i])
				{
					RecordNode<TYPE>* p = mDataArray->Data()[i];
					while (p != nullptr)
					{
						RecordNode<TYPE>* temp = p;
						p = p->GetLink();
						delete temp;
						temp = nullptr;
					}
				}
			}
			delete mDataArray;
		}

	private:
		FTDS::Array<RecordNode<TYPE>*>* mDataArray;
	};
} // namespace FTDS