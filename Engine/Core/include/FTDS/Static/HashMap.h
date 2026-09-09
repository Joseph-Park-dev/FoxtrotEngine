#pragma once
#include "FTDS/Static/Array.h"

#include "FTDS/Static/Record.h"
#include "FTDS/Static/FTString.h"
#include "Debugging/DebugFuncs.h"
#include "FTMath.h"

namespace Common
{
	namespace FTDS
	{
		/// @brief Computes a 64-bit FNV-1a hash of null-terminated text.
		/// @param str Text used by the operation.
		/// @return 64-bit FNV-1a hash value.
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
		/// @brief Maps a string hash into the available table slots.
		/// @param key Lookup key identifying the stored entry.
		/// @param arrSize Number of available hash-table slots; must be nonzero.
		/// @return Slot index in [0, arrSize).
		/// @pre arrSize must be nonzero.
		inline size_t HashFunction(const char* key, size_t arrSize)
		{
			return GenerateHash_fnv1a_64(key) % arrSize;
		}

		// Sum the ASCII code numbers from the alphabets of the key.
		/// @brief Computes a numeric key from the character codes in the supplied text.
		/// @param key Lookup key identifying the stored entry.
		/// @return A numeric key from the character codes in the supplied text.
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
			/// @brief Adds a keyed or positioned element to the container.
			/// @param key Lookup key identifying the stored entry.
			/// @param value Value to assign, insert, or process.
			void Insert(int key, TYPE value)
			{
				Common::FTDS::String conv;
				ToString(conv, key);
				this->Insert(conv, value);
			}

			/// @brief Adds a keyed or positioned element to the container.
			/// @param key Lookup key identifying the stored entry.
			/// @param value Value to assign, insert, or process.
			void Insert(Common::FTDS::String key, TYPE value)
			{
				// HashChainMap uses FTDS::Array,
				// the number of slots will not be dynamic,
				// whereas the linked list inside a slot will be.
				assert(0 < this->Capacity());

				// Get Hash Value from HashFuntion()
				size_t hashVal = FTDS::HashFunction(key.C_Str(), this->Capacity());

				Record<TYPE>* node = DBG_NEW Record<TYPE>(key, value);
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
					Common::Debug::LogError(__LINE__, __FILE__, "HashMap is full");
					delete node;
					node = nullptr;
					return;
				}
				this->mData[hashVal] = node;
				++mSize;
			}

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed record for the key, or nullptr when the lookup fails.
			FTDS::Record<TYPE>* At(const int key)
			{
				Common::FTDS::String conv;
				ToString(conv, key);
				return this->At(conv);
			}

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed record for the key, or nullptr when the lookup fails.
			FTDS::Record<TYPE>* At(const Common::FTDS::String&& key)
			{
				return this->At(key.C_Str());
			}

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed record for the key, or nullptr when the lookup fails.
			FTDS::Record<TYPE>* At(const Common::FTDS::String& key)
			{
				return this->At(key.C_Str());
			}

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed record for the key, or nullptr when the lookup fails.
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
				for (size_t i = 1; i <= pow2; ++i)
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

			/// @brief Removes the element selected by the supplied index or key.
			/// @param key Lookup key identifying the stored entry.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			void Erase(const char* key)
			{
				Record<TYPE>* node = At(key);
				delete node;
				node = nullptr;
				--mSize;
			}

		public:
			/// @brief Invokes the callback on occupied record slots.
			/// @param unaryOp Callback invoked for each visited entry.
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

			/// @brief Invokes the callback on values in occupied record slots.
			/// @param unaryOp Callback invoked for each visited entry.
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
			/// @brief Resets the logical contents and releases or reinitializes storage as defined by the container.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
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
			/// @brief Returns the size used by this hash map.
			/// @return Borrowed access to the size.
			const size_t& GetSize() { return mSize; }
			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			bool		  IsEmpty() const { return mSize == 0; }
			/// @brief Tests whether occupied storage has reached the capacity.
			/// @return True when occupied storage has reached the capacity; otherwise false.
			bool		  IsFull() const { return this->mCapacity <= mSize; }

		public:
			/// @brief Initializes an empty keyed record table.
			HashMap()
				: FTDS::Array<Record<TYPE>*>()
				, mSize(0)
			{
			}

			/// @brief Initializes an empty keyed record table.
			/// @param capacity Initial number of element slots to allocate.
			HashMap(size_t capacity)
				: FTDS::Array<Record<TYPE>*>(capacity)
				, mSize(0)
			{
			}

			// Auto deletion of the RecordNodes inside each slot.
			/// @brief Releases the resources managed by this instance during destruction.
			~HashMap() override
			{
				Clear();
			}

		private:
			size_t mSize;
		};

		/// @brief Deletes owned pointer entries and clears or releases the map.
		/// @param map Map whose pointer entries are processed.
		/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
		template <class TYPE>
		void Safe_Delete_Map(Common::FTDS::HashMap<TYPE*>*& map)
		{
			for (auto iter = map->Begin(); iter != map->End(); ++iter)
			{
				delete (*iter);
				*iter = nullptr;
			}
			delete map;
			map = nullptr;
		}
	} // namespace FTDS

	// For making an obsolete HashChainMap
	//// Traverse throught the linked list inside a slot.
	// for (RecordNode<TYPE>* p = this->mData[hashVal]; p != nullptr; p = p->GetLink())
	//{
	//	// Is there any nodes with the same key?
	//	if (p->Equal(key))
	//	{
	//		// Abort insertion.
	//		Common::FTDS::String msg("Duplicated key: ");
	//		msg.Append(key);
	//		Common::Debug::LogError(__LINE__, __FILE__, msg.C_Str());
	//		return;
	//	}
	// }
} // namespace Common
