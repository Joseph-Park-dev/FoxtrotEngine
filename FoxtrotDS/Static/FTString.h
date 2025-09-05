#pragma once
#include <Dynamic/DynamicArray.h>

namespace FTDS
{
	char*  StrCat(const char* str1, const char* str2);
	size_t StrLen(const char* val);
	bool   StringEqual(const char* left, const char* right);

#define STRING_INPUT std::enable_if<std::is_same<T, const char*>::value, void>::type

	class String : public FTDS::DynamicArray<char>
	{
		////////////////////////////////
		/// String utility functions ///
		////////////////////////////////
	public:
		const int RFind(const char* target) const
		{
			size_t targetLen = FTDS::StrLen(target);
			if (!target || this->mLength < 1 || this->mLength < targetLen)
			{
				return -1; // Handle edge cases
			}

			for (int i = mLength - targetLen; 0 <= i; --i) // Start from end
			{
				FTDS::String query;
				this->SubStr(query, i, targetLen);
				if (FTDS::StringEqual(query.C_Str(), target))
				{
					return i; // Found last occurrence
				}
			}
			return -1; // Not found
		}

		int LFind(const char* target)
		{
			size_t targetLen = FTDS::StrLen(target);
			if (!target || this->mLength < 1 || this->mLength < targetLen)
			{
				return -1; // Handle edge cases
			}

			for (int i = 0; i < mLength - targetLen; ++i) // Start from the Beginning
			{
				FTDS::String query;
				this->SubStr(query, i, targetLen);
				if (query.Equal(std::move(target)))
					return i; // Found last occurrence
			}
			return -1; // Not found
		}

		void Append(const char* val)
		{
			size_t inputLength = FTDS::StrLen(val);
			size_t newCapacity = this->mLength + inputLength + 1;

			this->Reserve(newCapacity);
			memcpy_s(&this->mData[mLength], sizeof(char) * inputLength + 1, val, sizeof(char) * inputLength + 1);
			this->mLength += inputLength;
		}

		void Append(FTDS::String& val)
		{
			this->Append(val.C_Str());
		}

		void Append(const FTDS::String& val)
		{
			this->Append(val.C_Str());
		}

		void SubStr(FTDS::String& result, size_t start, size_t length) const
		{
			if (start >= mLength)
				return;

			char* str = DBG_NEW char[length + 1];
			for (size_t i = 0; i < length; ++i)
			{
				str[i] = this->mData[start + i];
			}
			str[length] = '\0';
			result.Assign(str);
			result.SetLength(length);
			delete[] str;
		}

		void SubStr(size_t start, size_t length)
		{
			if (start >= mLength)
				return;

			char* str = DBG_NEW char[length + 1];
			for (size_t i = 0; i < length; ++i)
			{
				str[i] = this->mData[start + i];
			}
			str[length] = '\0';
			this->Assign(str);
			mLength = length;
			delete[] str;
		}

		void Assign(const char* val)
		{
			size_t inputLength = StrLen(val);
			this->Reserve(inputLength + 1);

			strcpy_s(this->mData, sizeof(char) * inputLength + 1, val);
			this->mLength = inputLength;
		}

		void Assign(const FTDS::String& val)
		{
			Assign(val.C_Str());
		}

		template <typename T, typename... Args>
		typename STRING_INPUT Assign(T first, Args... rest)
		{
			this->Assign(first);
			((this->Append(rest), ...));
		}

		int ExtractUntilFirst(FTDS::String& result, const char* ch, bool trim = false)
		{
			int end = LFind(ch);
			if (end == -1)
				return end;
			SubStr(result, 0, end);

			if (trim)
			{
				size_t start = result.Length() + StrLen(ch);
				SubStr(*this, start, this->Length() - start);
			}
			return end;
		}

		void ExtractUntilLast(FTDS::String& result, const char* ch)
		{
			int end = RFind(ch);
			if (end == -1)
				return;
			SubStr(result, 0, end);
		}

		void ExtractFromLast(FTDS::String& result, const char* ch)
		{
			int end = RFind(ch);
			if (end == -1)
				return;
			SubStr(result, end + 1, mLength);
		}

		void ExtractFromLast(const char* ch)
		{
			int end = RFind(ch);
			if (end == -1)
				return;
			SubStr(end + 1, mLength);
		}

		void ExtractBracketedVal(FTDS::String& result, const char* left, const char* right)
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

		bool Contains(FTDS::String& value)
		{
			return -1 < RFind(value.C_Str());
		}

		bool Contains(const char* value)
		{
			return -1 < RFind(value);
		}

		inline void Split(const char* splitVal, FTDS::DynamicArray<FTDS::String>& result)
		{
			FTDS::String bufStr;
			while (-1 < ExtractUntilFirst(bufStr, splitVal, true))
			{
				result.PushBack(bufStr.C_Str());
				bufStr.Clear();
			}
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
			MultiByteToWideChar(CP_UTF8, 0, mData, -1, wstr, length);
		}

		const size_t Length() const { return mLength; }
		void		 SetLength(size_t len) { mLength = len; }

		const bool IsEmpty() const { return mLength == 0 || !mData; }

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

		void operator=(const FTDS::String& str)
		{
			this->Assign(str.C_Str());
		}

		FTDS::String operator+(const char* str)
		{
			FTDS::String result(mData);
			result.Append(str);
			return result;
		}

		FTDS::String operator+(const FTDS::String& str)
		{
			FTDS::String result(mData);
			result.Append(str);
			return result;
		}

		FTDS::String operator+=(const FTDS::String& str)
		{
			FTDS::String result(mData);
			result.Append(str);
			return result;
		}

	public:
		String()
			: FTDS::DynamicArray<char>()
			, mLength(0)
		{
		}

		String(char* val)
			: FTDS::DynamicArray<char>()
			, mLength(0)
		{
			Assign(val);
		}

		String(const char* val)
			: FTDS::DynamicArray<char>()
			, mLength(0)
		{
			Assign(val);
		}

		String(FTDS::String& val)
			: FTDS::DynamicArray<char>()
			, mLength(0)
		{
			Assign(val.C_Str());
		}

		String(const FTDS::String& val)
			: FTDS::DynamicArray<char>()
			, mLength(0)
		{
			Assign(val.C_Str());
		}

		String(size_t num, char val)
			: FTDS::DynamicArray<char>()
			, mLength(num)
		{
			Reserve(num + 1);
			for (size_t i = 0; i < this->mLength; ++i)
				this->mData[i] = val;
			this->mData[mLength] = '\0';
		}

		void Clear() override
		{
			FTDS::Array<char>::Clear();
			mLength = 0;
		}

	private:
		size_t mLength;
	};

	template <>
	inline void FTDS::Array<FTDS::String>::AllocateMem(size_t newCap)
	{
		// Create an array with renewed capacity.
		FTDS::String* newArr = DBG_NEW FTDS::String[newCap];

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
	inline void FTDS::DynamicArray<const char*>::PushBack(const char* value)
	{
		++mSize;
		if (this->mCapacity <= mSize)
		{
			// Grow the array by double.
			FTDS::Array<const char*>::AllocateMem(mSize * 2);
		}

		// Assign the value.
		this->At(mSize - 1) = value;
	}

	inline bool StrContains(const char* str, const char* val)
	{
		return -1 < FTDS::String(str).RFind(val);
	}

	inline bool StrContains(const char* str, FTDS::String& val)
	{
		int idx = FTDS::String(str).RFind(val.C_Str());
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

	inline bool operator==(const FTDS::String& lhs, const char* rhs)
	{
		return lhs.Equal(std::move(rhs));
	}

	inline bool operator==(const char* lhs, const FTDS::String& rhs)
	{
		return rhs.Equal(std::move(lhs));
	}

	inline bool operator==(const FTDS::String lhs, const FTDS::String& rhs)
	{
		return lhs.Equal(rhs.C_Str());
	}

	inline bool operator!=(const FTDS::String lhs, const FTDS::String& rhs)
	{
		return lhs.NotEqual(rhs.C_Str());
	}
} // namespace FTDS