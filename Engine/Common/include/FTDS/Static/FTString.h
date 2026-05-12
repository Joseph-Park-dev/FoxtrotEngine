#pragma once
#include "../Dynamic/DynamicArray.h"

#include <Windows.h>

#include <Debugging/DebugMemAlloc.h>

namespace Common
{
	namespace FTDS
	{
		char*  StrCat(const char* str1, const char* str2);
		size_t StrLen(const char* val);
		bool   StringEqual(const char* left, const char* right);

#define STRING_INPUT std::enable_if<std::is_same<T, const char*>::value, void>::type

		class String : public Common::FTDS::DynamicArray<char>
		{
			////////////////////////////////
			/// String utility functions ///
			////////////////////////////////
		public:
			void PushBack(char value) override
			{
				Common::FTDS::DynamicArray<char>::PopBack();
				Common::FTDS::DynamicArray<char>::PushBack(value);
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}

			const int RFind(const char* target) const
			{
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
						return i; // Found last occurrence
					}
				}
				return -1; // Not found
			}

			int LFind(const char* target)
			{
				size_t targetLen = FTDS::StrLen(target);
				if (!target || this->GetLength() < 1 || this->GetLength() < targetLen)
				{
					return -1; // Handle edge cases
				}

				for (int i = 0; i < GetLength() - targetLen; ++i) // Start from the Beginning
				{
					Common::FTDS::String query;
					this->SubStr(query, i, targetLen);
					if (query.Equal(std::move(target)))
						return i; // Found last occurrence
				}
				return -1; // Not found
			}

			void Append(const char* val)
			{
				size_t inputLength = FTDS::StrLen(val);
				size_t newCapacity = this->GetLength() + inputLength + 1;

				this->Reserve(newCapacity);
				memcpy_s(&this->mData[GetLength()], sizeof(char) * inputLength + 1, val, sizeof(char) * inputLength + 1);
				this->Size() += inputLength;
			}

			void Append(Common::FTDS::String& val)
			{
				this->Append(val.C_Str());
			}

			void Append(const Common::FTDS::String& val)
			{
				this->Append(val.C_Str());
			}

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

			void Assign(const char* val)
			{
				size_t inputLength = StrLen(val);
				this->Reserve(inputLength + 1);

				strcpy_s(this->mData, sizeof(char) * inputLength + 1, val);
				this->Size() = inputLength + 1;
			}

			void Assign(const Common::FTDS::String& val)
			{
				Assign(val.C_Str());
			}

			template <typename T, typename... Args>
			typename STRING_INPUT Assign(T first, Args... rest)
			{
				this->Assign(first);
				((this->Append(rest), ...));
			}

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

			void ExtractUntilLast(Common::FTDS::String& result, const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(result, 0, end);
			}

			void ExtractFromLast(Common::FTDS::String& result, const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(result, end + 1, GetLength());
			}

			void ExtractFromLast(const char* ch)
			{
				int end = RFind(ch);
				if (end == -1)
					return;
				SubStr(end + 1, GetLength());
			}

			void ExtractBracketedVal(Common::FTDS::String& result, const char* left, const char* right)
			{
				int begin = LFind(left);
				int end	  = RFind(right);
				if (begin != -1 && end != -1)
					SubStr(result, begin + 1, end - begin - 1);
			}

			bool Equal(const char* right) const
			{
				return StringEqual(this->C_Str(), right);
			}

			bool NotEqual(const char* right) const
			{
				return !StringEqual(this->C_Str(), right);
			}

			bool Contains(Common::FTDS::String& value)
			{
				return -1 < RFind(value.C_Str());
			}

			bool Contains(const char* value)
			{
				return -1 < RFind(value);
			}

			inline void Split(const char* splitVal, Common::FTDS::DynamicArray<Common::FTDS::String>& result)
			{
				Common::FTDS::String bufStr;
				while (-1 < ExtractUntilFirst(bufStr, splitVal, true))
				{
					result.PushBack(bufStr.C_Str());
					bufStr.Clear();
				}
			}

			void Reverse() override
			{
				for (size_t i = 0; i < this->GetLength() / 2; ++i)
					this->Swap(i, this->GetLength() - 1 - i);
			}

			/////////////////////////
			/// String Properties ///
			/////////////////////////
		public:
			const char* C_Str() const
			{
				return this->mData;
			}

			const wchar_t* WC_Str() const
			{
				int		 size_needed = MultiByteToWideChar(CP_UTF8, 0, mData, -1, nullptr, 0);
				wchar_t* wstr		 = DBG_NEW wchar_t[size_needed];
				MultiByteToWideChar(CP_UTF8, 0, mData, -1, wstr, size_needed);
				return wstr; // Remember to delete[] wstr after use
			}

			void AssignToWStr(wchar_t* wstr, size_t length)
			{
				MultiByteToWideChar(CP_UTF8, 0, mData, -1, wstr, static_cast<int>(length));
			}

			const size_t GetLength() const { return this->GetSize() - 1; }

			const bool IsEmpty() const { return GetLength() == 0 || !mData; }

			// Print this string on CMD.
			void CMDPrint()
			{
				printf("%s \n", this->mData);
			}

			//////////////////////////
			/// Operator Overloads ///
			//////////////////////////
			void operator=(const char* str)
			{
				this->Assign(str);
			}

			void operator=(const Common::FTDS::String& str)
			{
				this->Assign(str.C_Str());
			}

			Common::FTDS::String operator+(const char* str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

			Common::FTDS::String operator+(const Common::FTDS::String& str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

			Common::FTDS::String operator+=(const Common::FTDS::String& str)
			{
				Common::FTDS::String result(mData);
				result.Append(str);
				return result;
			}

		public:
			String()
				: Common::FTDS::DynamicArray<char>()
			{
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}

			String(char* val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val);
			}

			String(const char* val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val);
			}

			String(Common::FTDS::String& val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val.C_Str());
			}

			String(const Common::FTDS::String& val)
				: Common::FTDS::DynamicArray<char>()
			{
				Assign(val.C_Str());
			}

			String(size_t num, char val)
				: Common::FTDS::DynamicArray<char>()
			{
				Reserve(num + 1);
				this->Size() = num + 1;

				for (size_t i = 0; i < this->GetLength(); ++i)
					this->mData[i] = val;
				this->mData[GetLength()] = '\0';
			}

			void Clear() override
			{
				FTDS::Array<char>::Clear();
				Common::FTDS::DynamicArray<char>::PushBack('\0');
			}
		};

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

		inline bool StrContains(const char* str, const char* val)
		{
			return -1 < Common::FTDS::String(str).RFind(val);
		}

		inline bool StrContains(const char* str, Common::FTDS::String& val)
		{
			int idx = Common::FTDS::String(str).RFind(val.C_Str());
			return -1 < idx;
		}

		inline size_t StrLen(const char* str)
		{
			size_t length = 0;
			while (str[length] != '\0')
				++length;
			return length;
		}

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

		inline bool operator==(const Common::FTDS::String& lhs, const char* rhs)
		{
			return lhs.Equal(std::move(rhs));
		}

		inline bool operator==(const char* lhs, const Common::FTDS::String& rhs)
		{
			return rhs.Equal(std::move(lhs));
		}

		inline bool operator==(const Common::FTDS::String& lhs, const Common::FTDS::String& rhs)
		{
			return lhs.Equal(rhs.C_Str());
		}

		inline bool operator!=(const Common::FTDS::String& lhs, const Common::FTDS::String& rhs)
		{
			return lhs.NotEqual(rhs.C_Str());
		}
	} // namespace FTDS
} // namespace Common