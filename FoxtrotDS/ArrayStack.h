#pragma once
#include "DSLogs.h"

constexpr size_t MAX_STACK_SIZE = 50;

template <typename TYPE>
class ArrayStack
{
public:
	void Push(TYPE element);
	void Pop();
	void Peek();

public:
	bool IsEmpty();
	bool IsFull();

public:
	ArrayStack();
	~ArrayStack();

private:
	int top;
	TYPE data[MAX_STACK_SIZE];
};

template<typename TYPE>
inline void ArrayStack<TYPE>::Push(TYPE element)
{
	assert(!IsFull()); // Stack is full.
	data[++top] = element;
		
}

template<typename TYPE>
inline void ArrayStack<TYPE>::Pop()
{
	assert
	if(IsEmpty()) LogError("Stack is empty")
}

template<typename TYPE>
inline void ArrayStack<TYPE>::Peek()
{
}

template<typename TYPE>
inline bool ArrayStack<TYPE>::IsEmpty()
{
	return false;
}

template<typename TYPE>
inline bool ArrayStack<TYPE>::IsFull()
{
	return false;
}

template<typename TYPE>
inline ArrayStack<TYPE>::ArrayStack()
{
}

template<typename TYPE>
inline ArrayStack<TYPE>::~ArrayStack()
{
}
