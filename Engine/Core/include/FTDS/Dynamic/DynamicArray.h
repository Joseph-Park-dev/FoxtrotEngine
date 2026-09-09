#pragma once
#include "../Static/Array.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class DynamicArray :
			public FTDS::Array<TYPE>
		{
		public:
			/// @brief Creates an iterator at the beginning of the underlying storage.
			/// @return Iterator to the first traversable element.
			FTDS::FTIteratorArray<TYPE> Begin() override { return FTDS::FTIteratorArray<TYPE>(this->mData); }
			/// @brief Creates the sentinel iterator immediately after the traversable range.
			/// @return Past-the-end iterator; do not dereference it.
			FTDS::FTIteratorArray<TYPE> End() override { return FTDS::FTIteratorArray<TYPE>(&this->mData[mSize]); }

		public:
			/// @brief Invokes the callback for each element in the array's traversable range.
			/// @param unaryOp Callback invoked for each visited entry.
			template <class Func>
			void IterateArray(Func&& unaryOp)
			{
				for (size_t i = 0; i < this->mSize; ++i)
					unaryOp(this->mData[i]);
			}

			/// @brief Appends an element and grows the backing storage when necessary.
			/// @param value Value to assign, insert, or process.
			virtual void PushBack(TYPE value)
			{
				++mSize;
				if (this->mCapacity < mSize)
				{
					// Grow the array by double.
					this->AllocateMem(this->mCapacity * 2);
				}

				// Assign the value.
				this->mData[mSize - 1] = value;
			}

			/// @brief Copies logical elements from the source into this container's storage.
			/// @param from Source container whose values are copied.
			void Copy(Common::FTDS::DynamicArray<TYPE>& from)
			{
				this->Reserve(from.GetSize());

				size_t copySize = sizeof(TYPE) * from.GetSize();
				memcpy_s(this->mData, copySize, from.mData, copySize);

				this->mSize = from.GetSize();
			}

			/// @brief Adds a keyed or positioned element to the container.
			/// @param pos Position or zero-based insertion index.
			/// @param value Value to assign, insert, or process.
			void Insert(size_t pos, TYPE value)
			{
				// pos should be within size range
				assert(pos <= this->mSize);

				++mSize;

				if (this->mCapacity < mSize)
					// Grow the array by double.
					this->AllocateMem(this->mCapacity * 2);

				size_t copiedCount = mSize - pos - 1;
				size_t copiedSize  = sizeof(TYPE) * copiedCount;
				memcpy_s(&this->mData[pos + 1], copiedSize, &this->mData[pos], copiedSize);

				this->mData[pos] = value;
			}

			/// @brief Removes the element selected by the supplied index or key.
			/// @param pos Position or zero-based insertion index.
			void Erase(size_t pos)
			{
				// pos should be within size range
				// Note that condition differs from Insert(size_t, TYPE)
				assert(pos < this->mSize);

				this->mData[pos] = NULL;

				// Pull the values one index forward.
				if (mSize < (this->mCapacity / 2))
					this->AllocateMem(this->mCapacity / 2);

				if (0 < mSize)
				{
					--mSize;
					size_t copiedCount = mSize - pos;
					size_t copiedSize  = sizeof(TYPE) * copiedCount;
					memcpy_s(&this->mData[pos], copiedSize, &this->mData[pos + 1], copiedSize);
					this->mData[mSize] = NULL;
				}
			}

			/// @brief Removes the last logical element and adjusts the backing capacity when required.
			void PopBack()
			{
				assert(0 < mSize);
				this->mData[mSize - 1] = NULL;
				--mSize;

				if (mSize < (this->mCapacity / 2))
					this->AllocateMem(this->mCapacity / 2);
			}

			/// @brief Searches stored entries for the supplied key or value.
			/// @param val Value to assign, insert, or process.
			/// @return Index of the matching entry, or -1 when no entry matches.
			int Find(TYPE val)
			{
				for (size_t pos = 0; pos < mSize; ++pos)
				{
					if (this->mData[pos] == val)
						return (int)pos;
				}
				return -1;
			}

			/// @brief Replaces the stored contents with the supplied source values.
			/// @param other Source object or comparison operand.
			void Assign(DynamicArray<TYPE>* other)
			{
				this->Reserve(other->GetSize());
				size_t i = 0;
				other->IterateArray([&](TYPE t) {
					this->At(i) = t;
					++i;
				});
				this->mSize = other->GetSize();
			}

			/// @brief Reorders the traversable elements from last to first in place.
			virtual void Reverse() override
			{
				for (size_t i = 0; i < this->mSize / 2; ++i)
					this->Swap(i, this->mSize - 1 - i);
			}

		public:
			/// @brief Returns the size used by this dynamic array.
			/// @return Borrowed access to the size.
			const size_t& GetSize() const { return mSize; }
			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			const bool	  IsEmpty() const { return mSize == 0; }

			/// @brief Exposes the element count used to track occupied storage.
			/// @return Borrowed access to the size.
			/// @note Changes through the returned reference affect this object's stored state.
			size_t& Size() { return mSize; }

		public:
			/// @brief Initializes empty growable storage and its logical element count.
			/// @note Initializes the :Array<TYPE> base or delegates to its constructor.
			DynamicArray()
				: FTDS::Array<TYPE>()
				, mSize(0)
			{
				this->Reserve(1);
			}

			/// @brief Initializes empty growable storage and its logical element count.
			/// @param capacity Initial number of element slots to allocate.
			/// @note Initializes the :Array<TYPE> base or delegates to its constructor.
			DynamicArray(size_t capacity)
				: FTDS::Array<TYPE>()
				, mSize(0)
			{
				this->Reserve(capacity);
			}

			/// @brief Resets the logical contents and releases or reinitializes storage as defined by the container.
			void Clear() override
			{
				mSize = 0;
				FTDS::Array<TYPE>::Clear();
			}

		protected:
			/// @brief Reallocates backing storage and updates the capacity used by this container.
			/// @param newCap New number of backing-storage slots.
			/// @note Reallocation invalidates pointers, references, and iterators into the old storage.
			virtual void AllocateMem(size_t newCap) override
			{
				// Create an array with renewed capacity.
				this->mData = static_cast<TYPE*>(realloc(this->mData, sizeof(TYPE) * newCap));
				memset(&this->mData[mSize], NULL, sizeof(TYPE) * (newCap - mSize));
				// Set new capacity.
				this->mCapacity = newCap;
			}

		private:
			size_t mSize;

			// private:
			//	/// <summary>
			//	/// Allocate memeory, copying the values from pos
			//	/// </summary>
			//	/// <param name="newCap"> : Capacity of the newly allocated array</param>
			//	/// <param name="pos"> : Index position to start copying the value</param>
			//	/// <param name="offsetDest"> : Offset from the destination position</param>
			//	/// <param name="offsetSrc"> : Offset from the source position</param>
			//	void AllocateMem(size_t newCap, size_t pos, size_t offsetDest, size_t offsetSrc)
			//	{
			//		// Create an array with renewed capacity.
			//		TYPE* newArr = DBG_NEW TYPE[newCap];
			//		memset(newArr, NULL, sizeof(TYPE) * newCap);

			//		// Copy the data in front of the pos,
			//		// which is not included in the copied size.
			//		size_t initSize = sizeof(TYPE) * pos;
			//		memcpy_s(newArr, initSize, this->mData, initSize);

			//		// Set new capacity.
			//		this->mCapacity = newCap;

			//		assert(this->mData);
			//		assert(pos <= this->mCapacity);

			//		// Calculate memory size to be copied.
			//		size_t copiedSize = sizeof(TYPE) * (mSize - pos);

			//		// Copy previous data.
			//		memcpy_s(
			//			&newArr[pos + offsetDest],
			//			copiedSize,
			//			&this->mData[pos + offsetSrc],
			//			copiedSize);
			//		delete[] this->mData;

			//		// Set new array as current data.
			//		this->mData = newArr;
			//	}
			//
		};
	} // namespace FTDS
} // namespace Common
