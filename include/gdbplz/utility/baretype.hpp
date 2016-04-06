#ifndef WIERTLO_BARETYPE_HPP_8C742FB656DB43B68ECBF3ED8769F33C
#define WIERTLO_BARETYPE_HPP_8C742FB656DB43B68ECBF3ED8769F33C

#include <type_traits>

namespace wiertlo
{
	namespace detail
	{
		template<typename T>
		using RemoveCv = typename std::remove_cv<T>::type;
		
		template<typename T>
		using RemoveReference = typename std::remove_reference<T>::type;
	}
}

#define WIERTLO_BARETYPE(...) wiertlo::detail::RemoveCv<wiertlo::detail::RemoveReference<decltype(__VA_ARGS__)>>

#endif