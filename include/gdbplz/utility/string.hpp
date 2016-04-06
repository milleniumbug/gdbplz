#ifndef WIERTLO_STRING_HPP_BE11054B57F4424B9B2CDA913290906C
#define WIERTLO_STRING_HPP_BE11054B57F4424B9B2CDA913290906C

#include <boost/numeric/conversion/cast.hpp>
#include <boost/utility/string_ref.hpp>
#include <type_traits>

namespace wiertlo
{
	namespace detail
	{
		boost::string_ref::size_type to_index(
			boost::string_ref in,
			std::make_signed<boost::string_ref::size_type>::type i)
		{
			return i >= 0 ?
				i :
				in.size() - static_cast<boost::string_ref::size_type>(-i);
		}
		
		boost::string_ref::size_type to_index(
			boost::string_ref in,
			boost::string_ref::size_type i)
		{
			static_cast<void>(in);
			return i;
		}
		
		template<
			typename T,
			typename std::enable_if<(
				std::is_integral<T>::value &&
				std::is_signed<T>::value &&
				!std::is_same<T, std::make_signed<boost::string_ref::size_type>::type>::value
			)>::type* = nullptr>
		boost::string_ref::size_type to_index(
			boost::string_ref in,
			T i)
		{
			return to_index(in, std::make_signed<boost::string_ref::size_type>::type(i));
		}
		
		template<
			typename T,
			typename std::enable_if<(
				std::is_integral<T>::value &&
				std::is_unsigned<T>::value &&
				!std::is_same<T, boost::string_ref::size_type>::value
			)>::type* = nullptr>
		boost::string_ref::size_type to_index(
			boost::string_ref in,
			T i)
		{
			return to_index(in, boost::string_ref::size_type(i));
		}
		
		inline
		boost::string_ref slice_impl(
			boost::string_ref in,
			boost::string_ref::size_type b,
			boost::string_ref::size_type e)
		{
			return b < in.size() ? in.substr(b, e-b) : in.substr(0,0);
		}
	}
	
	template<typename T>
	boost::string_ref slice(
		boost::string_ref in,
		T b)
	{
		return detail::slice_impl(in, detail::to_index(in, b), detail::to_index(in, in.size()));
	}
	
	template<typename T>
	boost::string_ref slice(
		boost::string_ref in,
		T b,
		T e)
	{
		return detail::slice_impl(in, detail::to_index(in, b), detail::to_index(in, e));
	}
	
	inline
	boost::string_ref trim(boost::string_ref in, boost::string_ref whitespace = " \r\n\t")
	{
		auto b = in.find_first_not_of(whitespace);
		auto e = in.find_last_not_of(whitespace);
		return slice(in, b, e+1);
	}
}

#endif