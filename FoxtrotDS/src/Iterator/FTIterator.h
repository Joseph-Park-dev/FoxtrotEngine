#pragma once
namespace FTDS
{
	template <typename TYPE>
	class FTIterator
	{
		virtual TYPE& operator*()	  = 0;
		virtual TYPE* operator++()	  = 0;
	};

	template <typename TYPE>
	class FTIteratorArray : public FTIterator<TYPE>
	{
	public:
		FTIteratorArray(TYPE* array)
			: mPtr(array)
			, mPos(0)
		{
		}

		TYPE& operator*() override { return *mPtr; }
		TYPE* operator++() override
		{
			++mPtr;
			++mPos;
			return mPtr;
		};

		bool operator==(FTIteratorArray&& rhs) { return mPtr == rhs.mPtr; }
		bool operator!=(FTIteratorArray&& rhs) { return mPtr != rhs.mPtr; }

		size_t IterPos() { return mPos; }

	private:
		TYPE* mPtr;
		size_t mPos;
	};
} // namespace FTDS