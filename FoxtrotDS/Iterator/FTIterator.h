#pragma once
namespace FTDS
{
	template <typename TYPE>
	class FTIterator
	{
		virtual TYPE& operator*() = 0;
		virtual TYPE* operator++() = 0;
		virtual TYPE* operator++(int) = 0;
	};
}