// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_STRING_H
#define INCLUDE_KMMQTT_STL_STRING_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <cstring>
#include <string>

namespace kmMqtt
{
	namespace kmStd
	{
		template <
			typename CharT,
			typename Traits = ::std::char_traits<CharT>,
			typename Allocator = StdAllocator<CharT>>
		using basic_string = ::std::basic_string<CharT, Traits, Allocator>;

		using string = basic_string<char>;
		using wstring = basic_string<wchar_t>;

		inline string to_string(int value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(long value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(long long value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(unsigned value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(unsigned long value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(unsigned long long value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(float value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(double value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline string to_string(long double value)
		{
			return string{ ::std::to_string(value).c_str() };
		}

		inline int strcmp(const char* lhs, const char* rhs) noexcept
		{
			return ::strcmp(lhs, rhs);
		}
	}

	inline kmStd::string operator+(const char* lhs, const kmStd::string& rhs)
	{
		kmStd::string result{ lhs == nullptr ? "" : lhs };
		result += rhs;
		return result;
	}

	inline kmStd::string operator+(char lhs, const kmStd::string& rhs)
	{
		kmStd::string result;
		result += lhs;
		result += rhs;
		return result;
	}
}

#endif // INCLUDE_KMMQTT_STL_STRING_H
