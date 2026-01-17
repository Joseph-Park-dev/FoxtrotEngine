#pragma once
#include "ArrayDS.h"

namespace FTDS
{
	template <typename TYPE>
	class ArrayList : public ArrayDS<TYPE>
	{
		using ArrayDS<TYPE>::mData;
		using ArrayDS<TYPE>::mSize;
		using ArrayDS<TYPE>::mCapacity;

	public:
		void Insert(int pos, TYPE element);
		void Remove(int pos);
		int	 GetEntry(int pos);
		bool Find(int item);
		void Replace(int pos, int e);

	public:
		ArrayList(void);
	};

	template <typename TYPE>
	inline void ArrayList<TYPE>::Insert(int pos, TYPE element)
	{
	}

	template <typename TYPE>
	inline void ArrayList<TYPE>::Remove(int pos)
	{
	}

	template <typename TYPE>
	inline int ArrayList<TYPE>::GetEntry(int pos)
	{
		return 0;
	}

	template <typename TYPE>
	inline bool ArrayList<TYPE>::Find(int item)
	{
		return false;
	}

	template <typename TYPE>
	inline void ArrayList<TYPE>::Replace(int pos, int e)
	{
	}

} // namespace FTDS