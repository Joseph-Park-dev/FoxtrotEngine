#pragma once
#include <Static/Array.h>

namespace FTDS
{
	template <typename TYPE>
	class DynamicArray :
		public FTDS::Array<TYPE>
	{
	public:
		void PushBack(TYPE value)
		{
			if (this->mCapacity <= mSize)
			{
				// Grow the array by double.
				FTDS::Array<TYPE>::AllocateMem(this->mCapacity * 2);
			}

			// Assign the value.
			this->mData[mSize] = value;
			++mSize;
		}

		void Insert(size_t pos, TYPE value)
		{
			// pos should be within size range
			assert(pos <= this->mSize);

			if (this->mCapacity <= mSize)
				// Grow the array by double.
				this->AllocateMem(this->mCapacity * 2, pos, 1, 0);
			else
			{
				size_t copiedCount = mSize - pos;
				size_t copiedSize  = sizeof(TYPE) * copiedCount;
				memcpy_s(&this->mData[pos + 1], copiedSize, &this->mData[pos], copiedSize);
			}

			this->mData[pos] = value;
			++mSize;
		}

		void Erase(size_t pos)
		{
			// pos should be within size range
			// Note that condition differs from Insert(size_t, TYPE)
			assert(pos < this->mSize);

			this->mData[pos] = TYPE();
			--mSize;

			// Pull the values one index forward.
			if (mSize <= (this->mCapacity / 2))
				this->AllocateMem(this->mCapacity / 2, pos, 0, 1);
			else
			{
				size_t copiedCount = mSize - pos;
				size_t copiedSize  = sizeof(TYPE) * copiedCount;
				memcpy_s(&this->mData[pos], copiedSize, &this->mData[pos + 1], copiedSize);
			}
		}

		void PopBack()
		{
			this->mData[mSize - 1] = TYPE();
			--mSize;

			if (mSize < (this->mCapacity / 2))
				FTDS::Array<TYPE>::AllocateMem(this->mCapacity / 2);
		}

		int Find(TYPE val)
		{
			for (int pos = 0; pos < (int)mSize; ++pos)
			{
				if (this->mData[pos] == val)
					return pos;
				++pos;
			}
			return -1;
		}

	public:
		const size_t& GetSize() const { return mSize; }
		bool		  IsEmpty() { return mSize == 0; }

	public:
		DynamicArray()
			: FTDS::Array<TYPE>()
			, mSize(0)
		{
			this->Reserve(1);
		}

		DynamicArray(size_t capacity)
			: FTDS::Array<TYPE>(capacity)
			, mSize(0)
		{
		}

		virtual ~DynamicArray()
		{
		}

	private:
		size_t mSize;

	private:
		/// <summary>
		/// Allocate memeory, copying the values from pos
		/// </summary>
		/// <param name="newCap"> : Capacity of the newly allocated array</param>
		/// <param name="size"> : Current size of this DynamicArray</param>
		/// <param name="pos"> : Index position to start copying the value</param>
		/// <param name="offsetDest"> : Offset from the destination position</param>
		/// <param name="offsetSrc"> : Offset from the source position</param>
		void AllocateMem(size_t newCap, size_t pos, size_t offsetDest, size_t offsetSrc)
		{
			// Create an array with renewed capacity.
			TYPE* newArr = DBG_NEW TYPE[newCap];

			// Copy the data in front of the pos,
			// which is not included in the copied size.
			size_t initSize = sizeof(TYPE) * pos;
			memcpy_s(newArr, initSize, this->mData, initSize);

			// Set new capacity.
			this->mCapacity = newCap;

			assert(this->mData);
			assert(pos <= this->mCapacity);

			// Calculate memory size to be copied.
			size_t copiedSize = sizeof(TYPE) * (mSize - pos);

			// Copy previous data.
			memcpy_s(
				&newArr[pos + offsetDest],
				copiedSize,
				&this->mData[pos + offsetSrc],
				copiedSize);
			delete[] this->mData;

			// Set new array as current data.
			this->mData = newArr;
		}
	};
} // namespace FTDS
