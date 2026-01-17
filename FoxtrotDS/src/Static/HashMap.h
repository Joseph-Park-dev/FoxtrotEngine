#pragma once
#include "Static/Array.h"

#include "Static/Record.h"
#include "Static/FTString.h"
#include "Math/FTMath.h"

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
	class HashMap : public FTDS::Array<FTDS::Record<TYPE>*>
	{
	public:
		void Insert(int key, TYPE value)
		{
			FTDS::String conv;
			ToString(conv, key);
			this->Insert(conv, value);
		}

		void Insert(FTDS::String key, TYPE value)
		{
			// HashChainMap uses FTDS::Array,
			// the number of slots will not be dynamic,
			// whereas the linked list inside a slot will be.
			assert(0 < this->Capacity());

			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key.C_Str(), this->Capacity());

			Record<TYPE>* node = new Record<TYPE>(key, value);
			if (this->mData[hashVal])
			{
				size_t pow2 = Math::NextPowerOf2(static_cast<int>(this->Capacity()));
				// Perform Quadratic probing
				for (size_t i = 1; i <= pow2; ++i)
				{
					size_t qIndex = (hashVal + (i + i * i) / 2) % pow2;

					if (qIndex >= this->Capacity())
						continue;

					if (this->mData[qIndex] == nullptr)
					{
						hashVal = qIndex;
						break;
					}
				}
			}
			if (this->mData[hashVal])
			{
				delete node;
				node = nullptr;
				Assert(!this->mData[hashVal], "HashMap is full");
				return;
			}
			this->mData[hashVal] = node;
			++mSize;
		}

		FTDS::Record<TYPE>* At(const int key)
		{
			FTDS::String conv;
			ToString(conv, key);
			return this->At(conv);
		}

		FTDS::Record<TYPE>* At(const FTDS::String&& key)
		{
			return this->At(key.C_Str());
		}

		FTDS::Record<TYPE>* At(const FTDS::String& key)
		{
			return this->At(key.C_Str());
		}

		FTDS::Record<TYPE>* At(const char*&& key)
		{
			assert(0 < this->Capacity());

			// Get Hash Value from HashFuntion()
			size_t hashVal = FTDS::HashFunction(key, this->Capacity());

			FTDS::Record<TYPE>* rec = this->mData[hashVal];
			if (rec)
				if (rec->Equal(key))
					return rec;

			size_t pow2 = Math::NextPowerOf2(static_cast<int>(this->Capacity()));
			// Perform Quadratic probing
			for (int i = 1; i <= pow2; ++i)
			{
				size_t qIndex = (hashVal + (i + i * i) / 2) % pow2;

				if (qIndex >= this->Capacity())
					continue;

				rec = this->mData[qIndex];
				if (rec)
					if (rec->Equal(key))
						return rec;
			}
			return nullptr;
		}

		void Erase(const char* key)
		{
			Record<TYPE>* node = At(key);
			delete node;
			node = nullptr;
			--mSize;
		}

	public:
		template <class UnaryOperation>
		void IterateAllNodes(
			UnaryOperation&& unaryOp)
		{
			if (this->mSize < 1)
				return;

			for (size_t i = 0; i < this->Capacity(); ++i)
			{
				if (this->mData[i])
				{
					unaryOp(this->mData[i]);
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
					if (this->mData[i])
						unaryOp(this->mData[i]->Value());
				}
			}
		}

		// Clear the linked nodes inside the array data.
		void Clear() override
		{
			if (mSize < 1)
				return;
			IterateAllNodes([&](Record<TYPE>*& node) {
				if (node)
				{
					delete node;
					node = nullptr;
				}

			});
			mSize = 0;
		}

	public:
		const size_t& GetSize() { return mSize; }
		bool		  IsEmpty() const { return mSize == 0; }
		bool		  IsFull() const { return this->mCapacity <= mSize; }

	public:
		HashMap()
			: FTDS::Array<Record<TYPE>*>()
			, mSize(0)
		{
		}

		HashMap(size_t capacity)
			: FTDS::Array<Record<TYPE>*>(capacity)
			, mSize(0)
		{
		}

		// Auto deletion of the RecordNodes inside each slot.
		~HashMap() override
		{
			Clear();
			delete this->mData;
			this->mData = nullptr;
		}

	private:
		size_t mSize;
	};
} // namespace FTDS

// For making an obsolete HashChainMap
//// Traverse throught the linked list inside a slot.
// for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
//{
//	// Is there any nodes with the same key?
//	if (p->Equal(key))
//	{
//		// Abort insertion.
//		FTDS::String msg("Duplicated key: ");
//		msg.Append(key);
//		Debug::LogError(__LINE__, __FILE__, msg.C_Str());
//		return;
//	}
// }