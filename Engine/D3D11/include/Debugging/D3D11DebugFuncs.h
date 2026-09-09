#pragma once
#include <exception>
#include <Windows.h>
#include <stdio.h>

namespace D3D11
{
	// Exception handling for D3D11
	namespace DX
	{
		// Helper class for COM exceptions
		class com_exception : public std::exception
		{
		public:
			/// @brief Initializes the failed HRESULT retained for diagnostic reporting.
			/// @param hr HRESULT returned by the Direct3D operation.
			/// @note Initializes the result base or delegates to its constructor.
			com_exception(HRESULT hr)
				: result(hr) {}

			/// @brief Exposes the diagnostic text associated with this exception.
			/// @return Borrowed pointer to a shared diagnostic buffer.
			/// @note A subsequent call overwrites the buffer; concurrent calls require external synchronization.
			const char* what() const noexcept override
			{
				static char s_str[64] = {};
				sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
				return s_str;
			}

		private:
			HRESULT result;
		};

		// Helper utility converts D3D API failures into exceptions.
		/// @brief Raises an exception when a Direct3D operation reports failure.
		/// @param hr HRESULT returned by the Direct3D operation.
		/// @throws com_exception If hr indicates failure.
		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				printf("%s", com_exception(hr).what());
				throw com_exception(hr);
			}
		}
	} // namespace DX
} // namespace D3D11
