#ifndef WIERTLO_STRONG_TYPEDEF_HPP_7F0E6155B19442ADB4A13F8E9C63DEC3
#define WIERTLO_STRONG_TYPEDEF_HPP_7F0E6155B19442ADB4A13F8E9C63DEC3

#include <utility>
#include <type_traits>

namespace wiertlo
{
	namespace detail
	{
		// https://rmf.io/cxx11/is_related/
		// https://rmf.io/cxx11/type-traits-galore/
		template <typename T>
		using Bare = typename std::remove_cv<
			typename std::remove_reference<T>::type
		>::type;
		
		template <typename T, typename... U>
		struct is_related : std::false_type {};
		
		template <typename T, typename U>
		struct is_related<T, U> : std::is_same<Bare<T>, Bare<U>> {};
		
		template <bool B, typename...>
		struct dependent_bool_type : std::integral_constant<bool, B> {};
		
		template <bool B, typename... T>
		using Bool = typename dependent_bool_type<B, T...>::type;
		
		template <typename If, typename Then, typename Else>
		using Conditional = typename std::conditional<If::value, Then, Else>::type;
		
		template <typename... T>
		struct all : Bool<true> {};
		template <typename Head, typename... Tail>
		struct all<Head, Tail...> : Conditional<
			Head,
			all<Tail...>,
			Bool<false>>
		{};
	}
	
	template<typename T, typename Tag>
	class strong_typedef
	{
	private:
		T value;
	public:
		// TODO: is_related fix to avoid calling copy constructors
		template<typename... Args, typename std::enable_if<
			!detail::is_related<strong_typedef<T, Tag>, Args...>::value
		>::type* = nullptr>
		strong_typedef(Args&&... args) :
			value(std::forward<Args>(args)...)	
		{
			
		}
		
		T get() const
		{
			return value;
		}
		
		explicit operator T() const
		{
			return get();
		}
	};
}

#define WIERTLO_STRONG_TYPEDEF(what, ...) struct what : wiertlo::strong_typedef<__VA_ARGS__, what> { using wiertlo::strong_typedef<__VA_ARGS__, what>::strong_typedef; }

#endif