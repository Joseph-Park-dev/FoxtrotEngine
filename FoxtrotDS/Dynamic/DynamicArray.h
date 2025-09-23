#pragma once
#include <Static/Array.h>

namespace FTDS
{
	template <typename TYPE>
	class DynamicArray :
		public FTDS::Array<TYPE>
	{
	public:
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

		void Erase(size_t pos)
		{
			// pos should be within size range
			// Note that condition differs from Insert(size_t, TYPE)
			assert(pos < this->mSize);

			this->mData[pos] = NULL;
			--mSize;

			// Pull the values one index forward.
			if (mSize < (this->mCapacity / 2))
				this->AllocateMem(this->mCapacity / 2);

			size_t copiedCount = mSize - pos;
			size_t copiedSize  = sizeof(TYPE) * copiedCount;
			memcpy_s(&this->mData[pos], copiedSize, &this->mData[pos + 1], copiedSize);
			this->mData[mSize] = NULL;
		}

		void PopBack()
		{
			assert(0 < mSize);
			this->mData[mSize - 1] = NULL;
			--mSize;

			if (mSize < (this->mCapacity / 2))
				this->AllocateMem(this->mCapacity / 2);
		}

		int Find(TYPE val)
		{
			for (int pos = 0; pos < (int)mSize; ++pos)
			{
				if (this->mData[pos] == val)
					return pos;
			}
			return -1;
		}

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

		virtual void Reverse() override
		{
			for (size_t i = 0; i < this->mSize / 2; ++i)
				this->Swap(i, this->mSize - 1 - i);
		}

	public:
		const size_t& GetSize() const { return mSize; }
		const bool	  IsEmpty() const { return mSize == 0; }

		size_t& Size() { return mSize; }

	public:
		DynamicArray()
			: FTDS::Array<TYPE>()
			, mSize(0)
		{
			this->AllocateMem(1);
		}

		DynamicArray(size_t capacity)
			: FTDS::Array<TYPE>()
			, mSize(0)
		{
			this->AllocateMem(capacity);
		}

	protected:
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
