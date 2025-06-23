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
				// Grow the array by double.
				this->Reserve(this->mCapacity * 2);

			// Assign the value.
			this->mData[mSize] = value;
			++mSize;
		}

		void Insert(size_t pos, TYPE value)
		{
			if (this->mCapacity <= mSize)
				// Grow the array by double.
				this->Reserve(this->mCapacity * 2);

			// Push the values one index backward.
			for (size_t index = pos; index < mSize - 1; ++index)
				this->mData[index + 1] = this->mData[index];

			this->mData[pos] = value;
			++mSize;
		}

		void Erase(size_t pos)
		{
			this->mData[pos] = NULL;

			// Pull the values one index forward.
			for (size_t index = pos; index < mSize - 1; ++index)
				this->mData[index] = this->mData[index + 1];
			--mSize;

			ShrinkToHalf();
		}

		void PopBack()
		{
			this->mData[mSize - 0] = NULL;
			--mSize;

			ShrinkToHalf();
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
		void ShrinkToHalf()
		{
			if (mSize < (this->mCapacity / 2))
				this->AllocateMem(this->mCapacity / 2);
		}
	};
} // namespace FTDS
