#pragma once
#include "../Dynamic/DynamicArray.h"

#include <Windows.h>
#include <climits>

#include <Debugging/DebugMemAlloc.h>

namespace Common
{
	namespace FTDS
	{
		/// @brief Allocates a buffer containing the concatenation of two null-terminated strings.
		/// @param str1 First null-terminated string.
		/// @param str2 Second null-terminated string.
		/// @return New character buffer owned by the caller; release it with delete[]. Returns nullptr if either input is null.
		char*  StrCat(const char* str1, const char* str2);
		/// @brief Counts characters before the terminating null character.
		/// @param val Value to assign, insert, or process.
		/// @return Character count, excluding the null terminator.
		size_t StrLen(const char* val);
		/// @brief Compares characters until the first difference or the end of the left string.
		/// @param left Left operand or opening delimiter.
		/// @param right Right operand or closing delimiter.
		/// @return True when the right string ends at the stopping position.
		/// @note The current comparison also accepts a right string that is a prefix of the left string.
		bool   StringEqual(const char* left, const char* right);

#define STRING_INPUT std::enable_if<std::is_same<T, const char*>::value, void>::type

		class String : public Common::FTDS::DynamicArray<char>
		{
			////////////////////////////////
			/// String utility functions ///
			////////////////////////////////
		public:
			/// @brief Appends an element and grows the backing storage when necessary.
			/// @param value Value to assign, insert, or process.
			void PushBack(char value) override
			{
				Common::FTDS::DynamicArray<char>::PopBack();
				Common::FTDS::DynamicArray<char>::PushBack(value);
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}

			/// @brief Searches for the final occurrence of a substring.
			/// @param target Target object or value sought by the operation.
			/// @return Zero-based match position, or -1 when the delimiter is absent.
			const int RFind(const char* target) const
			{
				if (!target)
					return -1;
				size_t targetLen = FTDS::StrLen(target);
				if (!target || this->GetLength() < 1 || this->GetLength() < targetLen)
				{
					return -1; // Handle edge cases
				}

				int i = static_cast<int>(GetLength() - targetLen);
				for (; 0 <= i; --i) // Start from end
				{
					Common::FTDS::String query;
					this->SubStr(query, i, targetLen);
					if (Common::FTDS::StringEqual(query.C_Str(), target))
					{
						return static_cast<int>(i); // Found occurrence
					}
				}
				return -1; // Not found
			}

			/// @brief Searches for the first occurrence of a substring.
			/// @param target Target object or value sought by the operation.
			/// @return Zero-based match position, or -1 when the delimiter is absent.
			int LFind(const char* target)
			{
				if (!target)
					return -1;
				size_t targetLen = FTDS::StrLen(target);
				if (!target || this->GetLength() < 1 || this->GetLength() < targetLen)
				{
					return -1; // Handle edge cases
				}

				for (size_t i = 0; i <= GetLength() - targetLen && i <= INT_MAX; ++i) // Start from the Beginning
				{
					Common::FTDS::String query;
					this->SubStr(query, i, targetLen);
					if (query.Equal(std::move(target)))
						return static_cast<int>(i); // Found occurrence
				}
				return -1; // Not found
			}

			/// @brief Appends characters to the string and retains null termination.
			/// @param val Value to assign, insert, or process.
			void Append(const char* val)
			{
				size_t inputLength = FTDS::StrLen(val);
				size_t newCapacity = this->GetLength() + inputLength + 1;

				this->Reserve(newCapacity);
				memcpy_s(&this->mData[GetLength()], sizeof(char) * inputLength + 1, val, sizeof(char) * inputLength + 1);
				this->Size() += inputLength;
			}

			/// @brief Appends characters to the string and retains null termination.
			/// @param val Value to assign, insert, or process.
			void Append(Common::FTDS::String& val)
			{
				this->Append(val.C_Str());
			}

			/// @brief Appends characters to the string and retains null termination.
			/// @param val Value to assign, insert, or process.
			void Append(const Common::FTDS::String& val)
			{
				this->Append(val.C_Str());
			}

			/// @brief Copies the requested character range, leaving the destination unchanged when the start is outside the string.
			/// @param result Receives the extracted or computed result.
			/// @param start First character index or delimiter marking truncation.
			/// @param length Number of characters or elements in the range.
			void SubStr(Common::FTDS::String& result, size_t start, size_t length) const
			{
				if (start >= GetLength())
					return;

				char* str = DBG_NEW char[length + 1];
				for (size_t i = 0; i < length; ++i)
				{
					str[i] = this->mData[start + i];
				}
				str[length] = '\0';
				result.Assign(str);
				delete[] str;
			}

			/// @brief Copies the requested character range, leaving the destination unchanged when the start is outside the string.
			/// @param start First character index or delimiter marking truncation.
			/// @param length Number of characters or elements in the range.
			void SubStr(size_t start, size_t length)
			{
				if (start >= GetLength())
					return;

				char* str = DBG_NEW char[length + 1];
				for (size_t i = 0; i < length; ++i)
				{
					str[i] = this->mData[start + i];
				}
				str[length] = '\0';
				this->Assign(str);
				delete[] str;
			}

			/// @brief Replaces the stored contents with the supplied source values.
			/// @param val Value to assign, insert, or process.
			void Assign(const char* val)
			{
				size_t inputLength = StrLen(val);
				this->Reserve(inputLength + 1);

				strcpy_s(this->mData, sizeof(char) * inputLength + 1, val);
				this->Size() = inputLength + 1;
			}

			/// @brief Replaces the stored contents with the supplied source values.
			/// @param val Value to assign, insert, or process.
			void Assign(const Common::FTDS::String& val)
			{
				Assign(val.C_Str());
			}

			template <typename T, typename... Args>
			/// @brief Replaces the stored contents with the supplied source values.
			/// @param first Initial input value or first operand.
			/// @param rest Additional values appended after the initial value.
			/// @return Replaces the stored contents with the supplied source values.
			typename STRING_INPUT Assign(T first, Args... rest)
			{
				this->Assign(first);
				((this->Append(rest), ...));
			}

			/// @brief Extracts the prefix before the first delimiter, optionally consuming that prefix and delimiter.
			/// @param result Receives the extracted or computed result.
			/// @param ch Delimiter text to search for.
			/// @param trim Whether the consumed prefix and delimiter are removed from this string.
			/// @return Zero-based match position, or -1 when the delimiter is absent.
			int ExtractUntilFirst(Common::FTDS::String& result, const char* ch, bool trim = false)
			{
				int end = LFind(ch);
				if (end == -1)
					return end;
				SubStr(result, 0, end);

				if (trim)
				{
					size_t start = result.GetLength() + StrLen(ch);
					SubStr(*this, start, this->GetLength() - start);
				}
				return end;
			}

			/// @brief Extracts the prefix before the final delimiter.
			/// @param result Receives the extracted or computed result.
			/// @param ch Delimiter text to search for.
			void ExtractUntilLast(Common::FTDS::String& result, const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(result, 0, end);
			}

			/// @brief Extracts characters following the final delimiter.
			/// @param result Receives the extracted or computed result.
			/// @param ch Delimiter text to search for.
			/// @note Extraction advances one character past the delimiter's starting position.
			void ExtractFromLast(Common::FTDS::String& result, const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(result, end + 1, GetLength());
			}

			/// @brief Extracts characters following the final delimiter.
			/// @param ch Delimiter text to search for.
			/// @note Extraction advances one character past the delimiter's starting position.
			void ExtractFromLast(const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(end + 1, GetLength());
			}

			/// @brief Extracts the characters between the first left marker and the last right marker.
			/// @param result Receives the extracted or computed result.
			/// @param left Left operand or opening delimiter.
			/// @param right Right operand or closing delimiter.
			void ExtractBracketedVal(Common::FTDS::String& result, const char* left, const char* right)
			{
				int begin = LFind(left);
				int end	  = RFind(right);
				if (begin != -1 && end != -1)
					SubStr(result, begin + 1, end - begin - 1);
			}

			/// @brief Compares stored characters or a record key with the supplied text.
			/// @param right Right operand or closing delimiter.
			/// @return True when the compared values match; otherwise false.
			bool Equal(const char* right) const
			{
				return StringEqual(this->C_Str(), right);
			}

			/// @brief Tests whether the supplied text differs from the stored characters.
			/// @param right Right operand or closing delimiter.
			/// @return True when the supplied text differs from the stored characters; otherwise false.
			bool NotEqual(const char* right) const
			{
				return !StringEqual(this->C_Str(), right);
			}

			/// @brief Searches the stored text for an occurrence of the supplied substring.
			/// @param value Value to assign, insert, or process.
			/// @return True if the operation succeeds or the tested condition holds; otherwise false.
			bool Contains(Common::FTDS::String& value)
			{
				return -1 < RFind(value.C_Str());
			}

			/// @brief Searches the stored text for an occurrence of the supplied substring.
			/// @param value Value to assign, insert, or process.
			/// @return True if the operation succeeds or the tested condition holds; otherwise false.
			bool Contains(const char* value)
			{
				return -1 < RFind(value);
			}

			/// @brief Consumes delimiter-terminated prefixes and appends them to the output array.
			/// @param splitVal Delimiter separating the output substrings.
			/// @param result Receives the extracted or computed result.
			/// @note A final segment without a trailing delimiter is not appended.
			inline void Split(const char* splitVal, Common::FTDS::DynamicArray<Common::FTDS::String>& result)
			{
				Common::FTDS::String bufStr;
				while (-1 < ExtractUntilFirst(bufStr, splitVal, true))
				{
					result.PushBack(bufStr.C_Str());
					bufStr.Clear();
				}
			}

			/// @brief Reorders the traversable elements from last to first in place.
			void Reverse() override
			{
				for (size_t i = 0; i < this->GetLength() / 2; ++i)
					this->Swap(i, this->GetLength() - 1 - i);
			}

			/////////////////////////
			/// String Properties ///
			/////////////////////////
		public:
			/// @brief Exposes the string's null-terminated character buffer.
			/// @return Borrowed null-terminated buffer, invalidated by storage changes or destruction.
			const char* C_Str() const
			{
				return this->mData;
			}

			/// @brief Allocates a UTF-16 conversion of the stored UTF-8 string.
			/// @return New UTF-16 buffer; the caller must release it with delete[].
			const wchar_t* WC_Str() const
			{
				int		 size_needed = MultiByteToWideChar(CP_UTF8, 0, mData, -1, nullptr, 0);
				wchar_t* wstr		 = DBG_NEW wchar_t[size_needed];
				MultiByteToWideChar(CP_UTF8, 0, mData, -1, wstr, size_needed);
				return wstr; // Remember to delete[] wstr after use
			}

			/// @brief Converts stored UTF-8 text into the caller's UTF-16 buffer.
			/// @param wstr Writable UTF-16 character buffer.
			/// @param length Number of characters or elements in the range.
			void AssignToWStr(wchar_t* wstr, size_t length)
			{
				MultiByteToWideChar(CP_UTF8, 0, mData, -1, wstr, static_cast<int>(length));
			}

			/// @brief Returns the length used by this string.
			/// @return Current length.
			const size_t GetLength() const { return this->GetSize() - 1; }

			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			const bool IsEmpty() const { return GetLength() == 0 || !mData; }

			// Print this string on CMD.
			/// @brief Writes the stored text followed by a newline to standard output.
			void CMDPrint()
			{
				printf("%s \n", this->mData);
			}

			/// @brief Copies the supplied value into this object.
			/// ///////////////////////
			/// Operator Overloads ///
			/// ///////////////////////
			/// @param str Text used by the operation.
			void operator=(const char* str)
			{
				this->Assign(str);
			}

			/// @brief Copies the supplied value into this object.
			/// @param str Text used by the operation.
			void operator=(const Common::FTDS::String& str)
			{
				this->Assign(str.C_Str());
			}

			/// @brief Builds concatenated text without modifying the original string.
			/// @param str Text used by the operation.
			/// @return Value produced by the overloaded operation.
			Common::FTDS::String operator+(const char* str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

			/// @brief Builds concatenated text without modifying the original string.
			/// @param str Text used by the operation.
			/// @return Value produced by the overloaded operation.
			Common::FTDS::String operator+(const Common::FTDS::String& str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

			/// @brief Produces concatenated text without assigning it back to this string.
			/// @param str Text used by the operation.
			/// @return New concatenated string.
			Common::FTDS::String operator+=(const Common::FTDS::String& str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

		public:
			/// @brief Initializes a null-terminated character sequence.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String()
				: Common::FTDS::DynamicArray<char>()
			{
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}

			/// @brief Initializes a null-terminated character sequence.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String(char* val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val);
			}

			/// @brief Initializes a null-terminated character sequence.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String(const char* val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val);
			}

			/// @brief Initializes a null-terminated character sequence.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String(Common::FTDS::String& val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val.C_Str());
			}

			/// @brief Initializes a null-terminated character sequence.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String(const Common::FTDS::String& val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val.C_Str());
			}

			/// @brief Initializes a null-terminated character sequence.
			/// @param num Number of entries or repeated characters.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the :DynamicArray<char> base or delegates to its constructor.
			String(size_t num, char val)
				: Common::FTDS::DynamicArray<char>()
			{
				Reserve(num + 1);
				this->Size() = num + 1;

				for (size_t i = 0; i < this->GetLength(); ++i)
					this->mData[i] = val;
				this->mData[GetLength()] = '\0';
			}

			/// @brief Resets the logical contents and releases or reinitializes storage as defined by the container.
			void Clear() override
			{
				FTDS::Array<char>::Clear();
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}
		};

		/// @brief Reallocates backing storage and updates the capacity used by this container.
		/// @param newCap New number of backing-storage slots.
		/// @note Reallocation invalidates pointers, references, and iterators into the old storage.
		template <>
		inline void FTDS::Array<Common::FTDS::String>::AllocateMem(size_t newCap)
		{
			// Create an array with renewed capacity.
			Common::FTDS::String* newArr = DBG_NEW Common::FTDS::String[newCap];

			// Copy previous data.
			for (size_t i = 0; i < mCapacity; ++i)
				if (!this->mData[i].IsEmpty())
					newArr[i].Assign(this->mData[i]);

			delete[] mData;

			// Set new array as current data.
			mData = newArr;
			// Set new capacity.
			mCapacity = newCap;
		}

		/// @brief Appends an element and grows the backing storage when necessary.
		/// @param value Value to assign, insert, or process.
		template <>
		inline void Common::FTDS::DynamicArray<const char*>::PushBack(const char* value)
		{
			assert(value);

			++mSize;
			if (this->mCapacity <= mSize)
			{
				// Grow the array by double.
				this->AllocateMem(mSize * 2);
			}

			// Assign the value.
			this->At(mSize - 1) = value;
		}

		/// @brief Appends an element and grows the backing storage when necessary.
		/// @param value Value to assign, insert, or process.
		template <>
		inline void Common::FTDS::DynamicArray<Common::FTDS::String*>::PushBack(Common::FTDS::String* value)
		{
			assert(value);

			++mSize;
			if (this->mCapacity <= mSize)
			{
				// Grow the array by double.
				Common::FTDS::DynamicArray<Common::FTDS::String*>::AllocateMem(mSize * 2);
			}

			this->mData;

			// Assign the value.
			this->At(mSize - 1) = value;
		}

		/// @brief Searches stored entries for the supplied key or value.
		/// @param value Value to assign, insert, or process.
		/// @return Borrowed matching entry, or nullptr when the search does not find one.
		template <>
		inline int Common::FTDS::DynamicArray<Common::FTDS::String*>::Find(Common::FTDS::String* value)
		{
			assert(this->mData);

			for (size_t pos = 0; pos < mSize; ++pos)
			{
				if (value->Equal(this->mData[pos]->C_Str()))
					return (int)pos;
			}
			return -1;
		}

		/// @brief Tests whether a null-terminated string contains the requested substring.
		/// @param str Text used by the operation.
		/// @param val Value to assign, insert, or process.
		/// @return True when a null-terminated string contains the requested substring; otherwise false.
		inline bool StrContains(const char* str, const char* val)
		{
			return -1 < Common::FTDS::String(str).RFind(val);
		}

		/// @brief Tests whether a null-terminated string contains the requested substring.
		/// @param str Text used by the operation.
		/// @param val Value to assign, insert, or process.
		/// @return True when a null-terminated string contains the requested substring; otherwise false.
		inline bool StrContains(const char* str, Common::FTDS::String& val)
		{
			int idx = Common::FTDS::String(str).RFind(val.C_Str());
			return -1 < idx;
		}

		/// @brief Counts characters before the terminating null character.
		/// @param str Text used by the operation.
		/// @return Character count, excluding the null terminator.
		inline size_t StrLen(const char* str)
		{
			size_t length = 0;
			while (str[length] != '\0')
				++length;
			return length;
		}

		/// @brief Compares characters until the first difference or the end of the left string.
		/// @param left Left operand or opening delimiter.
		/// @param right Right operand or closing delimiter.
		/// @return True when the right string ends at the stopping position.
		/// @note The current comparison also accepts a right string that is a prefix of the left string.
		inline bool StringEqual(const char* left, const char* right)
		{
			// The size of two strings are not equal.
			size_t i = 0;
			while (left[i] != '\0')
			{
				if (left[i] != right[i])
					break;
				++i;
			}
			return right[i] == '\0';
		}

		/// @brief Allocates a buffer containing the concatenation of two null-terminated strings.
		/// @param str1 First null-terminated string.
		/// @param str2 Second null-terminated string.
		/// @return New character buffer owned by the caller; release it with delete[]. Returns nullptr if either input is null.
		inline char* StrCat(const char* str1, const char* str2)
		{
			if (!str1 || !str2)
				return nullptr; // Handle null pointers

			// Calculate lengths
			size_t len1 = 0, len2 = 0;
			len1 = FTDS::StrLen(str1);
			len2 = FTDS::StrLen(str2);

			// Allocate memory for the concatenated string (+1 for null terminator)
			char* result = DBG_NEW char[len1 + len2 + 1];

			// Copy str1 into result
			for (size_t i = 0; i < len1; ++i)
			{
				result[i] = str1[i];
			}

			// Append str2 to result
			for (size_t i = 0; i < len2; ++i)
			{
				result[len1 + i] = str2[i];
			}

			result[len1 + len2] = '\0'; // Null-terminate the new string

			return result;
		}

		/// @brief Converts the supplied value to the text representation used by the engine.
		/// @param str Text used by the operation.
		/// @param val Value to assign, insert, or process.
		inline void ToString(Common::FTDS::String& str, int val)
		{
			if (val == 0)
			{
				str.PushBack(static_cast<char>(48));
				return;
			}

			while (val > 0)
			{
				int digit = val % 10; // Get last digit
				val /= 10;			  // Remove last digit
				str.PushBack(static_cast<char>(digit + 48));
			}
			str.Reverse();
		}

		/// @brief Compares the stored value or iterator position with the operand.
		/// @param lhs Left comparison or arithmetic operand.
		/// @param rhs Right comparison or arithmetic operand.
		/// @return True when the compared values match; otherwise false.
		inline bool operator==(const Common::FTDS::String& lhs, const char* rhs)
		{
			return lhs.Equal(std::move(rhs));
		}

		/// @brief Compares the stored value or iterator position with the operand.
		/// @param lhs Left comparison or arithmetic operand.
		/// @param rhs Right comparison or arithmetic operand.
		/// @return True when the compared values match; otherwise false.
		inline bool operator==(const char* lhs, const Common::FTDS::String& rhs)
		{
			return rhs.Equal(std::move(lhs));
		}

		/// @brief Compares the stored value or iterator position with the operand.
		/// @param lhs Left comparison or arithmetic operand.
		/// @param rhs Right comparison or arithmetic operand.
		/// @return True when the compared values match; otherwise false.
		inline bool operator==(const Common::FTDS::String& lhs, const Common::FTDS::String& rhs)
		{
			return lhs.Equal(rhs.C_Str());
		}

		/// @brief Tests whether the stored value or iterator position differs from the operand.
		/// @param lhs Left comparison or arithmetic operand.
		/// @param rhs Right comparison or arithmetic operand.
		/// @return True when the stored value or iterator position differs from the operand; otherwise false.
		inline bool operator!=(const Common::FTDS::String& lhs, const Common::FTDS::String& rhs)
		{
			return lhs.NotEqual(rhs.C_Str());
		}
	} // namespace FTDS
} // namespace Common
