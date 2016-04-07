#ifndef WIERTLO_STRING_HPP_BE11054B57F4424B9B2CDA913290906C
#define WIERTLO_STRING_HPP_BE11054B57F4424B9B2CDA913290906C

#include <boost/utility/string_ref.hpp>
#include <type_traits>

namespace wiertlo
{
	
	namespace detail
	{
		// TODO: extend for more types
		typedef boost::string_ref StringRef;
		
		inline
		StringRef::size_type to_index(
			StringRef in,
			std::make_signed<StringRef::size_type>::type i)
		{
			return i >= 0 ?
				i :
				in.size() - static_cast<StringRef::size_type>(-i);
		}
		
		inline
		StringRef::size_type to_index(
			StringRef in,
			StringRef::size_type i)
		{
			static_cast<void>(in);
			return i;
		}
		
		template<
			typename T,
			typename std::enable_if<(
				std::is_integral<T>::value &&
				std::is_signed<T>::value &&
				!std::is_same<T, std::make_signed<StringRef::size_type>::type>::value
			)>::type* = nullptr>
		StringRef::size_type to_index(
			StringRef in,
			T i)
		{
			return to_index(in, std::make_signed<StringRef::size_type>::type(i));
		}
		
		template<
			typename T,
			typename std::enable_if<(
				std::is_integral<T>::value &&
				std::is_unsigned<T>::value &&
				!std::is_same<T, StringRef::size_type>::value
			)>::type* = nullptr>
		StringRef::size_type to_index(
			StringRef in,
			T i)
		{
			return to_index(in, StringRef::size_type(i));
		}
		
		inline
		StringRef slice_impl(
			StringRef in,
			StringRef::size_type b,
			StringRef::size_type e)
		{
			return b < in.size() ? in.substr(b, e-b) : in.substr(0,0);
		}
	}
	
	template<typename T>
	detail::StringRef slice(
		detail::StringRef in,
		T b)
	{
		return detail::slice_impl(in, detail::to_index(in, b), detail::to_index(in, in.size()));
	}
	
	template<typename T>
	detail::StringRef slice(
		detail::StringRef in,
		T b,
		T e)
	{
		return detail::slice_impl(in, detail::to_index(in, b), detail::to_index(in, e));
	}
	
	inline
	detail::StringRef trim(detail::StringRef in, detail::StringRef whitespace = " \r\n\t")
	{
		auto b = in.find_first_not_of(whitespace);
		auto e = in.find_last_not_of(whitespace);
		return slice(in, b, e+1);
	}
}

#endif