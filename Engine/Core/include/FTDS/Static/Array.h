#pragma once
#include <memory>
#include <cassert>

#include "../Iterator/FTIterator.h"

namespace Common
{
	namespace FTDS
	{
		/// <Note_on_deallocation>
		/// If an ArrayDS consist of dynamically allocated objects,
		/// (which means, it has variables created with "new" keyword)
		/// its data element must be deleted one by one to prevent memory leak.
		/// Clear() or the destructor won't free those objects automatically.
		/// </Note_on_deallocation>
		template <typename TYPE>
		class Array
		{
		public:
			/// @brief Accesses the indexed element without bounds checking.
			/// @param idx Zero-based element index.
			/// @return Reference to the indexed element.
			/// @pre The index must refer to an accessible element.
			TYPE& operator[](int idx) { return mData[idx]; }
			/// @brief Accesses the indexed element without bounds checking.
			/// @param idx Zero-based element index.
			/// @return Reference to the indexed element.
			/// @pre The index must refer to an accessible element.
			TYPE& operator[](size_t idx) { return mData[idx]; }

		public:
			/// @brief Creates an iterator at the beginning of the underlying storage.
			/// @return Iterator to the first traversable element.
			virtual FTDS::FTIteratorArray<TYPE> Begin() { return FTDS::FTIteratorArray<TYPE>(mData); }
			/// @brief Creates the sentinel iterator immediately after the traversable range.
			/// @return Past-the-end iterator; do not dereference it.
			virtual FTDS::FTIteratorArray<TYPE> End() { return FTDS::FTIteratorArray<TYPE>(&mData[mCapacity]); }

			// It is recommended to put null check to mPtr.
			/// @brief Invokes the callback for each element in the array's traversable range.
			/// @param unaryOp Callback invoked for each visited entry.
			template <class Func>
			void IterateArray(
				Func&& unaryOp)
			{
				for (size_t i = 0; i < mCapacity; ++i)
					unaryOp(mData[i]);
			}

			/// @brief Invokes the callback for each element in the array's traversable range.
			/// @param unaryOp Callback invoked for each visited entry.
			/// @param currPos Receives the index of the most recently visited element.
			template <class Func>
			void IterateArray(
				Func&& unaryOp, size_t& currPos)
			{
				currPos = 0;
				for (size_t i = 0; i < mCapacity; ++i)
				{
					unaryOp(mData[i]);
					currPos = i;
				}
			}

			/// @brief Exchanges the two indexed elements without changing the container's size.
			/// @param posLeft Index of the first element to exchange.
			/// @param posRight Index of the second element to exchange.
			void Swap(size_t posLeft, size_t posRight)
			{
				TYPE cache = this->mData[posLeft];
				// Replace the front value with back.
				this->mData[posLeft] = this->mData[posRight];
				// Replace the back value with front.
				this->mData[posRight] = cache;
			}

			/// @brief Reorders the traversable elements from last to first in place.
			virtual void Reverse()
			{
				for (size_t i = 0; i < this->mCapacity / 2; ++i)
					Swap(i, this->mCapacity - 1 - i);
			}

		public:
			// Re-allocate memory space when new capacity is bigger than current capacity
			/// @brief Grows storage when the requested capacity exceeds the current capacity.
			/// @param newCapacity Requested minimum number of backing-storage slots.
			/// @note Reallocation invalidates pointers, references, and iterators into the old storage.
			void Reserve(size_t newCapacity)
			{
				if (newCapacity <= mCapacity)
					return;
				AllocateMem(newCapacity);
			}

			// Clears the data, leaving the capacity unchanged.
			/// @brief Resets the logical contents and releases or reinitializes storage as defined by the container.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			virtual void Clear()
			{
				if (0 < mCapacity)
				{
					size_t newCap = mCapacity;
					mCapacity	  = 0;

					if (mData)
						free(mData);
					mData = nullptr;
					AllocateMem(newCap);
				}
			}

		public:
			// Gets the array which stores the data of the stack.
			// This can be used when freeing memory.
			// TYPE*	Data() { return mData; }

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param idx Zero-based element index.
			/// @return Reference to the selected element.
			/// @pre The index must refer to an accessible element.
			TYPE& At(size_t idx)
			{
				return mData[idx];
			}

			/// @brief Retrieves the element or record selected by an index or key.
			/// @param idx Zero-based element index.
			/// @return Reference to the selected element.
			/// @pre The index must refer to an accessible element.
			const TYPE& At(size_t idx) const { return mData[idx]; }

			/// @brief Reports the number of slots available in the backing allocation.
			/// @return Current capacity.
			size_t Capacity() { return mCapacity; }

		public:
			/// @brief Initializes empty contiguous storage with the requested capacity.
			Array()
				: mData(nullptr)
				, mCapacity(0)
			{
			}

			/// @brief Initializes empty contiguous storage with the requested capacity.
			/// @param capacity Initial number of element slots to allocate.
			Array(size_t capacity)
				: mData(nullptr)
				, mCapacity(0)
			{
				Reserve(capacity);
			}

			/// @brief Releases the resources managed by this instance during destruction.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			virtual ~Array()
			{
				free(mData);
				mData = nullptr;
			}

		public:
			/// @brief Exposes the stored payload for consumers of this resource or container.
			/// @return Borrowed access to the data.
			const TYPE* Data() const { return mData; }

		protected:
			/// @brief Reallocates backing storage and zeroes every slot, including previously stored contents.
			/// @param newCap New number of backing-storage slots.
			/// @note Reallocation invalidates pointers, references, and iterators into the old storage.
			virtual void AllocateMem(size_t newCap)
			{
				// Create an array with renewed capacity.
				mData = static_cast<TYPE*>(realloc(mData, sizeof(TYPE) * newCap));
				memset(mData, NULL, sizeof(TYPE) * newCap);
				// Set new capacity.
				mCapacity = newCap;
			}

			/// @brief Selects the smaller of two size values.
			/// @param a First operand.
			/// @param b Second operand.
			/// @return The smaller of two size values.
			size_t Min(size_t a, size_t b)
			{
				return a > b ? b : a;
			}

		protected:
			TYPE*  mData;
			size_t mCapacity;
		};
	} // namespace FTDS
} // namespace Common
