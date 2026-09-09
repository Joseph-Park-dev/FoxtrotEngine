#pragma once

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class FTIterator
		{
			/// @brief Accesses the element at the iterator's current position.
			/// @return Reference to the current element.
			virtual TYPE& operator*()  = 0;
			/// @brief Advances the iterator to the next element.
			/// @return Pointer to the next element after advancing.
			virtual TYPE* operator++() = 0;
		};

		template <typename TYPE>
		class FTIteratorArray : public FTIterator<TYPE>
		{
		public:
			/// @brief Initializes an iterator pointing at borrowed array storage.
			/// @param array Contiguous array of elements.
			FTIteratorArray(TYPE* array)
				: mPtr(array)
			{
			}

			/// @brief Accesses the element at the iterator's current position.
			/// @return Reference to the current element.
			TYPE& operator*() override { return *mPtr; }
			/// @brief Advances the iterator to the next element.
			/// @return Pointer to the next element after advancing.
			TYPE* operator++() override
			{
				++mPtr;
				return mPtr;
			};

			/// @brief Compares the stored value or iterator position with the operand.
			/// @param rhs Right comparison or arithmetic operand.
			/// @return True when the compared values match; otherwise false.
			bool operator==(FTIteratorArray&& rhs) { return mPtr == rhs.mPtr; }
			/// @brief Tests whether the stored value or iterator position differs from the operand.
			/// @param rhs Right comparison or arithmetic operand.
			/// @return True when the stored value or iterator position differs from the operand; otherwise false.
			bool operator!=(FTIteratorArray&& rhs) { return mPtr != rhs.mPtr; }

		private:
			TYPE* mPtr;
		};
	} // namespace FTDS
} // namespace Common
